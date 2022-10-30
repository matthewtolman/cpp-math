#include "big_int.h"

#include <utility>

static char hex_char(uint8_t half_byte) {
  if (half_byte < 10) {
    return static_cast<char>('0' + static_cast<char>(half_byte));
  }
  return static_cast<char>('a' + static_cast<char>(half_byte - 10));
}

void mtmath::BigInt::simplify(){
  if (!is_valid()) {
    flags = INVALID;
    digits.clear();
    return;
  }

  for (size_t i = digits.size(); i > 0; --i) {
    if (digits.at(i - 1) != 0) {
      if (i != digits.size()) {
        digits.erase(digits.begin() + static_cast<std::vector<uint8_t>::difference_type>(i), digits.end());
      }
      return;
    }
  }
  digits.clear();
}

std::strong_ordering mtmath::BigInt::operator<=>(const BigInt &o) const noexcept {
  if (!is_valid() || !o.is_valid()) {
    if (is_valid() != o.is_valid()) {
      return is_valid() ? std::strong_ordering::greater : std::strong_ordering::less;
    }
    else {
      return std::strong_ordering::equal;
    }
  }
  else if (is_zero() && o.is_zero()) {
    return std::strong_ordering::equal;
  }
  else if (is_negative() != o.is_negative()) {
    return is_negative() ? std::strong_ordering::less : std::strong_ordering::greater;
  }
  else {
    auto cmp = abs_compare(o);
    if (cmp < 0) {
      return std::strong_ordering::less;;
    }
    else if (cmp > 0) {
      return std::strong_ordering::greater;
    }
    else {
      return std::strong_ordering::equal;
    }
  }
}

std::tuple<mtmath::BigInt, mtmath::BigInt> mtmath::BigInt::divide(const BigInt &denominator) const noexcept {
  // Handle invalid division case
  if (!is_valid() || !(denominator.is_valid()) || denominator.is_zero()) {
    return std::make_tuple(BigInt::invalid(), BigInt::invalid());
  }

  // Handle trivial cases
  auto cmp = abs_compare(denominator);
  if (cmp < 0) {
    return std::make_tuple(*this, BigInt::zero());
  }
  else if (cmp == 0) {
    auto res = BigInt::one();
    res.flags = flags ^ denominator.flags;
    return std::make_tuple(BigInt::zero(), res);
  }
  else if (denominator.digits == ByteArray::from<uint8_t>(0x1U)) {
    auto res = *this;
    res.flags = flags ^ denominator.flags;
    return std::make_tuple(BigInt::zero(), res);
  }

  // Get a fresh, new BigInt
  auto remainder = BigInt{};
  auto quotient = BigInt{};

  remainder.flags = flags ^ denominator.flags;
  quotient.flags = flags ^ denominator.flags;
  auto numBits = digits.size() * 8;

  for (size_t i = 0; i < numBits; ++i) {
    auto n = numBits - i - 1;
    remainder = remainder << 1;
    auto numeratorBit = ((digits.at(n / 8) & (0x1U << (n % 8))) >> (n % 8)) & 0x1U;
    if (numeratorBit) {
      if (remainder.digits.empty()) {
        remainder.digits.emplace_back(0);
      }
      remainder.digits.at(0) |= 0x1;
    }
    if (remainder >= denominator) {
      remainder -= denominator;
      if (quotient.digits.size() <= n / 8) {
        quotient.digits.resize((n / 8) + 1);
      }
      quotient.digits.at(n / 8) |= 0x1 << (n % 8);
    }
  }

  remainder.digits.simplify();
  quotient.digits.simplify();
  return std::make_tuple(remainder, quotient);
}

mtmath::BigInt& mtmath::BigInt::operator+=(const mtmath::BigInt &o) noexcept {
  if (!is_valid() || !o.is_valid()) {
    flags |= INVALID;
    return *this;
  }
  if (flags == o.flags) {
    uint16_t buffer = 0;
    for (size_t index = 0; index < o.digits.size() || index < digits.size(); ++index) {
      auto left = index < digits.size() ? digits.at(index) : 0;
      auto right = index < o.digits.size() ? o.digits.at(index) : 0;
      buffer += left;
      buffer += right;
      auto digit = buffer % 256;
      if (index < digits.size()) {
        digits[index] = digit;
      }
      else {
        digits.emplace_back(digit);
      }
      buffer /= 256;
    }
    if (buffer) {
      auto digit = buffer % 256;
      if (o.digits.size() < digits.size()) {
        digits[o.digits.size()] = digit;
      }
      else {
        digits.emplace_back(digit);
      }
    }
    return *this;
  }
  else if (flags) {
    flags ^= NEGATIVE;
    *this -= o;
    flags ^= NEGATIVE;
    return *this;
  }
  else {
    return this->operator-=(-o);
  }
}

mtmath::BigInt& mtmath::BigInt::operator-=(const mtmath::BigInt &o) noexcept {
  if (!is_valid() || !o.is_valid()) {
    flags |= INVALID;
    return *this;
  }

  if (flags == o.flags) {
    auto newDigits = ByteArray{};
    newDigits.reserve(std::max(o.digits.size(), digits.size()));

    bool abs_less = abs_compare(o) < 0;
    const auto& bigger = abs_less ? o: *this;
    const auto& smaller = abs_less ? *this : o;

    int16_t borrow = 0;
    for (size_t index = 0; index < bigger.digits.size() || index < smaller.digits.size(); ++index) {
      auto left = static_cast<int16_t>(index < bigger.digits.size() ? bigger.digits.at(index) : 0);
      auto right = static_cast<int16_t>(index < smaller.digits.size() ? smaller.digits.at(index) : 0);
      auto num = left - right + borrow;
      borrow = 0;
      while (num < 0) {
        borrow -= 1;
        num += 256;
      }
      newDigits.emplace_back(static_cast<uint8_t>(num));
    }

    if (abs_less) {
      flags ^= NEGATIVE;
    }
    std::swap(digits, newDigits);
    simplify();
    return *this;
  }
  else {
    return this->operator+=(-o);
  }
}

mtmath::BigInt& mtmath::BigInt::operator/=(const mtmath::BigInt &o) noexcept {
  auto [r, q] = divide(o);
  *this = std::move(q);
  return *this;
}

mtmath::BigInt& mtmath::BigInt::operator%=(const mtmath::BigInt &o) noexcept {
  auto [r, q] = divide(o);
  *this = std::move(r);
  return *this;
}

mtmath::BigInt& mtmath::BigInt::operator*=(const mtmath::BigInt &o) noexcept {
  if (!is_valid() || !(o.is_valid())) {
    flags |= INVALID;
    return *this;
  }

  auto res = (to_immut() * o.to_immut()).to_mut();
  *this = std::move(res);
  return *this;
}

mtmath::BigInt& mtmath::BigInt::operator<<=(size_t i) {
  digits <<= i;
  return *this;
}

mtmath::BigInt& mtmath::BigInt::operator>>=(size_t i) {
  digits >>= i;
  return *this;
}

mtmath::immut::BigInt mtmath::BigInt::to_immut() const {
  return mtmath::immut::BigInt{flags, std::make_shared<ByteArray>(digits)};
}

std::string mtmath::BigInt::to_string(int base) const {
  if (!is_valid()) {
    return "NaN";
  }

  if (is_zero()) {
    return base == 16 ? "0x0" : "0";
  }

  return to_immut().to_string(base);
}

mtmath::BigInt mtmath::BigInt::operator-() const {
  if (!is_valid()) {
    return *this;
  }
  if (is_zero()) {
    return *this;
  }
  auto copy = *this;
  copy.flags ^= NEGATIVE;
  return copy;
}

void mtmath::BigInt::compress(int base) {
  std::vector<uint8_t> numerator;
  numerator.reserve(digits.size());
  std::copy(digits.begin(), digits.end(), std::back_inserter(numerator));
  std::vector<uint8_t> result;
  result.reserve(digits.size());
  digits.clear();
  result.clear();

  const auto divide_by = std::numeric_limits<uint8_t>::max() + 1;

  uint16_t divisor_remainder = 0;
  while (!numerator.empty()) {
    for (auto n : numerator) {
      divisor_remainder = divisor_remainder * base + n;
      result.push_back(divisor_remainder / divide_by);
      divisor_remainder %= divide_by;
    }

    bool leading_zero = true;
    result.erase(std::remove_if(result.begin(), result.end(), [&leading_zero](auto n) {
                   leading_zero = leading_zero && n == 0;
                   return leading_zero;
                 }), result.end());

    std::swap(result, numerator);
    result.clear();
    digits.emplace_back(divisor_remainder);
    divisor_remainder = 0;
  }

  simplify();
}
int mtmath::BigInt::abs_compare(const mtmath::BigInt &o) const noexcept {
  if (digits.size() != o.digits.size()) {
    return digits.size() > o.digits.size() ? 1 : -1;
  }
  else {
    for (size_t i = 0; i < digits.size(); ++i) {
      auto index = digits.size() - i - 1;
      if (digits.at(index) != o.digits.at(index)) {
        return digits.at(index) < o.digits.at(index) ? -1 : 1;
      }
    }
  }
  return 0;
}

mtmath::immut::BigInt mtmath::immut::BigInt::zeroConst = mtmath::immut::BigInt{};
mtmath::immut::BigInt mtmath::immut::BigInt::oneConst = mtmath::immut::BigInt{1};
mtmath::immut::BigInt mtmath::immut::BigInt::invalidConst = mtmath::immut::BigInt{INVALID, nullptr};

mtmath::immut::BigInt::BigInt() { *this = zeroConst; }
mtmath::immut::BigInt::BigInt(const mtmath::immut::BigInt &other) = default;
mtmath::immut::BigInt::BigInt(mtmath::immut::BigInt &&other) noexcept : flags(other.flags), digits(std::move(other.digits)) {}
mtmath::immut::BigInt::BigInt(uint8_t flags, std::shared_ptr<ByteArray> digits) : flags(flags), digits(std::move(digits)) {simplify();}

std::string mtmath::immut::BigInt::to_string(int base) const {
  if (!is_valid()) {
    return "NaN";
  }

  if (is_zero()) {
    return base == 16 ? "0x0" : "0";
  }

  std::string str;
  if (base == 16) {
    str.reserve((digits->size() * 2) + (is_negative() ? 1 : 0));

    for (auto d: *digits) {
      uint8_t char1 = d & 0x0f;
      uint8_t char2 = (d & 0xf0) >> 4;
      str.push_back(hex_char(char1));
      str.push_back(hex_char(char2));
    }
    if (str[str.size() - 1] == '0') {
      str.erase(str.begin() + static_cast<decltype(str)::difference_type>(str.size()) - 1);
    }
    str.push_back('x');
    str.push_back('0');
    if (is_negative()) {
      str.push_back('-');
    }
    std::reverse(str.begin(), str.end());
  }
  else {
    auto baseInt = mtmath::BigInt{base};
    str.reserve((digits->size()) + (is_negative() ? 1 : 0));
    auto cpy = abs_val().to_mut();
    while (!cpy.is_zero()) {
      auto [r, q] = cpy.divide(baseInt);
      auto digit = r.digits.as<uint8_t>();
      str.push_back(hex_char(digit));
      std::swap(cpy, q);
    }
    if (is_negative()) {
      str.push_back('-');
    }
    std::reverse(str.begin(), str.end());
  }

  str.shrink_to_fit();
  return str;
}

void mtmath::immut::BigInt::simplify(){
  if (!is_valid()) {
    *this = invalidConst;
    return;
  }

  for (size_t i = digits->size(); i > 0; --i) {
    if (digits->at(i - 1) != 0) {
      if (i != digits->size()) {
        digits->erase(digits->begin() + static_cast<std::vector<uint8_t>::difference_type>(i), digits->end());
      }
      if (digits->size() == 1 && digits->at(0) == 1) {
        *this = oneConst;
      }
      return;
    }
  }
  *this = zeroConst;
}

mtmath::immut::BigInt mtmath::immut::BigInt::operator-() const {
  if (!is_valid()) {
    return invalidConst;
  }
  if (is_zero()) {
    return zeroConst;
  }
  auto copy = *this;
  copy.flags ^= NEGATIVE;
  return copy;
}

mtmath::immut::BigInt mtmath::immut::BigInt::operator+(const mtmath::immut::BigInt &o) const noexcept {
  if (!is_valid() || !o.is_valid()) {
    BigInt r{};
    r.flags |= INVALID;
    return r;
  }
  if (flags == o.flags) {
    auto newDigits = std::make_shared<ByteArray>();
    newDigits->reserve(std::max(o.digits->size(), digits->size()) + 1);
    uint16_t buffer = 0;
    for (size_t index = 0; index < o.digits->size() || index < digits->size(); ++index) {
      auto left = index < digits->size() ? digits->at(index) : 0;
      auto right = index < o.digits->size() ? o.digits->at(index) : 0;
      buffer += left;
      buffer += right;
      newDigits->emplace_back(buffer % 256);
      buffer /= 256;
    }
    if (buffer) {
      newDigits->emplace_back(buffer);
    }
    auto res = BigInt{flags, newDigits};
    res.simplify();
    return res;
  }
  else if (flags) {
    return o - (this->operator-());
  }
  else {
    return this->operator-(-o);
  }
}

mtmath::immut::BigInt mtmath::immut::BigInt::operator-(const mtmath::immut::BigInt &o) const noexcept {
  if (!is_valid() || !o.is_valid()) {
    BigInt r{};
    r.flags |= INVALID;
    return r;
  }

  if (flags == o.flags) {
    auto newDigits = std::make_shared<ByteArray>();
    newDigits->reserve(std::max(o.digits->size(), digits->size()));

    bool abs_less = this->abs_less_than(o);
    const auto& bigger = abs_less ? o: *this;
    const auto& smaller = abs_less ? *this : o;

    int16_t borrow = 0;
    for (size_t index = 0; index < bigger.digits->size() || index < smaller.digits->size(); ++index) {
      auto left = static_cast<int16_t>(index < bigger.digits->size() ? bigger.digits->at(index) : 0);
      auto right = static_cast<int16_t>(index < smaller.digits->size() ? smaller.digits->at(index) : 0);
      auto num = left - right + borrow;
      borrow = 0;
      while (num < 0) {
        borrow -= 1;
        num += 256;
      }
      newDigits->emplace_back(static_cast<uint8_t>(num));
    }

    auto res = BigInt{static_cast<uint8_t>(abs_less ? flags ^ NEGATIVE : flags), newDigits};
    res.simplify();
    return res;
  }
  else {
    return this->operator+(-o);
  }
}

mtmath::immut::BigInt &mtmath::immut::BigInt::operator=(const mtmath::immut::BigInt &other) = default;

mtmath::immut::BigInt &mtmath::immut::BigInt::operator=(mtmath::immut::BigInt &&other) noexcept {
  std::swap(flags, other.flags);
  std::swap(digits, other.digits);
  return *this;
}

bool mtmath::immut::BigInt::abs_less_than(const mtmath::immut::BigInt &o) const noexcept {
  if (!is_valid() || !o.is_valid()) {
    return !o.is_valid() && is_valid();
  }

  if (digits->size() != o.digits->size()) {
    return digits->size() < o.digits->size();
  }
  for (size_t i = digits->size(); i > 0; --i) {
    if (digits->at(i - 1) != o.digits->at(i - 1)) {
      return digits->at(i - 1) < o.digits->at(i - 1);
    }
  }
  return false;
}

mtmath::immut::BigInt mtmath::immut::BigInt::operator/(const mtmath::immut::BigInt &denominator) const noexcept {
  auto [r, q] = divide(denominator);
  return q;
}

mtmath::immut::BigInt mtmath::immut::BigInt::operator%(const mtmath::immut::BigInt &denominator) const noexcept {
  auto [r, q] = divide(denominator);
  return r;
}

mtmath::immut::BigInt mtmath::immut::BigInt::operator*(const mtmath::immut::BigInt &o) const noexcept {
  if (!is_valid() || !(o.is_valid())) {
    BigInt r{};
    r.flags |= INVALID;
    return r;
  }

  auto result = BigInt::fresh();

  result.flags = flags ^ o.flags;

  // TODO: Adjust to use Karatsuba for very big integers

  for (size_t m = 0; m < digits->size(); ++m) {
    uint32_t carryOver = 0;
    for (size_t n = 0; n < o.digits->size(); ++n) {
      auto resIndex = m + n;
      auto res = static_cast<uint32_t>(digits->at(m)) * static_cast<uint32_t>(o.digits->at(n)) + carryOver;
      if (resIndex < result.digits->size()) {
        result.digits->at(resIndex) += res % 256;
      }
      else {
        result.digits->emplace_back(res % 256);
      }
      carryOver = res / 256;
    }

    if (carryOver) {
      auto resIndex = m + o.digits->size();
      if (resIndex < result.digits->size()) {
        result.digits->at(resIndex) += carryOver;
      } else {
        result.digits->emplace_back(carryOver);
      }
    }
  }

  result.simplify();
  return result;
}

void mtmath::immut::BigInt::compress(int base) {
  std::vector<uint8_t> numerator;
  numerator.reserve(digits->size());
  std::copy(digits->begin(), digits->end(), std::back_inserter(numerator));
  std::vector<uint8_t> result;
  result.reserve(digits->size());
  digits->clear();
  result.clear();

  const auto divide_by = std::numeric_limits<uint8_t>::max() + 1;

  uint16_t divisor_remainder = 0;
  while (!numerator.empty()) {
    for (auto n : numerator) {
      divisor_remainder = divisor_remainder * base + n;
      result.push_back(divisor_remainder / divide_by);
      divisor_remainder %= divide_by;
    }

    bool leading_zero = true;
    result.erase(std::remove_if(result.begin(), result.end(), [&leading_zero](auto n) {
      leading_zero = leading_zero && n == 0;
      return leading_zero;
    }), result.end());

    std::swap(result, numerator);
    result.clear();
    digits->emplace_back(divisor_remainder);
    divisor_remainder = 0;
  }

  simplify();
}

std::strong_ordering mtmath::immut::BigInt::operator<=>(const mtmath::immut::BigInt &o) const noexcept {
  if (!is_valid() || !o.is_valid()) {
    if (is_valid() != o.is_valid()) {
      return is_valid() ? std::strong_ordering::greater : std::strong_ordering::less;
    }
    else {
      return std::strong_ordering::equal;
    }
  }
  else if (is_zero() && o.is_zero()) {
    return std::strong_ordering::equal;
  }
  else if (is_negative() != o.is_negative()) {
    return is_negative() ? std::strong_ordering::less : std::strong_ordering::greater;
  }
  else if (digits->size() != o.digits->size()) {
    return digits->size() > o.digits->size() ? std::strong_ordering::greater : std::strong_ordering::less;
  }
  else {
    for (size_t i = 0; i < digits->size(); ++i) {
      auto index = digits->size() - i - 1;
      if (digits->at(index) != o.digits->at(index)) {
        return digits->at(index) < o.digits->at(index) ? std::strong_ordering::less : std::strong_ordering::greater;
      }
    }
  }
  return std::strong_ordering::equal;
}

bool mtmath::immut::BigInt::is_zero() const noexcept {
  return is_valid() && digits->empty();
}

bool mtmath::immut::BigInt::is_valid() const noexcept {
  return !(flags & INVALID) && digits != nullptr;
}

bool mtmath::immut::BigInt::is_negative() const noexcept {
  return flags & NEGATIVE;
}

mtmath::immut::BigInt mtmath::immut::BigInt::zero() {
  return zeroConst;
}

mtmath::immut::BigInt mtmath::immut::BigInt::one() {
  return oneConst;
}

mtmath::immut::BigInt mtmath::immut::BigInt::invalid() {
  return invalidConst;
}

mtmath::immut::BigInt mtmath::immut::BigInt::abs() const noexcept {
  auto copy = *this;
  copy.flags &= ~NEGATIVE;
  return copy;
}

mtmath::immut::BigInt mtmath::immut::BigInt::fresh() {
  BigInt r{};
  r.digits = std::make_shared<ByteArray>();
  return r;
}

std::tuple<mtmath::immut::BigInt, mtmath::immut::BigInt> mtmath::immut::BigInt::divide(const mtmath::immut::BigInt &denominator) const noexcept {
  auto [r, q] = to_mut().divide(denominator.to_mut());
  return std::make_tuple(r.to_immut(), q.to_immut());
}

mtmath::immut::BigInt mtmath::immut::BigInt::operator<<(size_t i) const noexcept {
  mtmath::immut::BigInt res;
  res.digits = std::make_shared<ByteArray>(digits->operator<<(i));
  return res;
}

mtmath::immut::BigInt mtmath::immut::BigInt::operator>>(size_t i) const noexcept {
  mtmath::immut::BigInt res;
  res.digits = std::make_shared<ByteArray>(digits->operator>>(i));
  return res;
}

mtmath::BigInt mtmath::immut::BigInt::to_mut() const {
  return mtmath::BigInt{flags, *digits};
}
