#include "big_int.h"

#include <utility>

mtmath::BigInt mtmath::BigInt::zeroConst = BigInt{};
mtmath::BigInt mtmath::BigInt::oneConst = BigInt{1};
mtmath::BigInt mtmath::BigInt::invalidConst = BigInt{INVALID, nullptr};

mtmath::BigInt::BigInt() { *this = zeroConst; }
mtmath::BigInt::BigInt(const mtmath::BigInt &other) = default;
mtmath::BigInt::BigInt(mtmath::BigInt &&other) noexcept : flags(other.flags), digits(std::move(other.digits)) {}
mtmath::BigInt::BigInt(uint8_t flags, std::shared_ptr<std::vector<uint8_t>> digits) : flags(flags), digits(std::move(digits)) {simplify();}

static char hex_char(uint8_t half_byte) {
  if (half_byte < 10) {
    return static_cast<char>('0' + static_cast<char>(half_byte));
  }
  return static_cast<char>('a' + static_cast<char>(half_byte - 10));
}

std::string mtmath::BigInt::to_string(int base) const {
  if (!is_valid()) {
    return "NaN";
  }

  if (is_zero()) {
    return "0";
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

  str.shrink_to_fit();
  return str;
}

void mtmath::BigInt::simplify(){
  if (!is_valid()) {
    *this = invalidConst;
    return;
  }

  for (size_t i = digits->size(); i > 0; --i) {
    if (digits->at(i - 1) != 0) {
      if (i != digits->size()) {
        digits->erase(digits->begin() + static_cast<std::vector<uint8_t>::difference_type>(i), digits->end());
        digits->shrink_to_fit();
      }
      if (digits->size() == 1 && digits->at(0) == 1) {
        *this = oneConst;
      }
      return;
    }
  }
  *this = zeroConst;
}

mtmath::BigInt mtmath::BigInt::operator-() const {
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

mtmath::BigInt mtmath::BigInt::operator+(const mtmath::BigInt &o) const noexcept {
  if (!is_valid() || !o.is_valid()) {
    BigInt r{};
    r.flags |= INVALID;
    return r;
  }
  if (flags == o.flags) {
    auto newDigits = std::make_shared<std::vector<uint8_t>>();
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

mtmath::BigInt mtmath::BigInt::operator-(const mtmath::BigInt &o) const noexcept {
  if (!is_valid() || !o.is_valid()) {
    BigInt r{};
    r.flags |= INVALID;
    return r;
  }

  if (flags == o.flags) {
    auto newDigits = std::make_shared<std::vector<uint8_t>>();
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

mtmath::BigInt &mtmath::BigInt::operator=(const mtmath::BigInt &other) = default;

mtmath::BigInt &mtmath::BigInt::operator=(mtmath::BigInt &&other) noexcept {
  std::swap(flags, other.flags);
  std::swap(digits, other.digits);
  return *this;
}

bool mtmath::BigInt::abs_less_than(const mtmath::BigInt &o) const noexcept {
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

mtmath::BigInt mtmath::BigInt::operator/(const mtmath::BigInt &denominator) const noexcept {
  auto [d, r] = divide(denominator);
  return d;
}

mtmath::BigInt mtmath::BigInt::operator%(const mtmath::BigInt &denominator) const noexcept {
  auto [d, r] = divide(denominator);
  return r;
}

mtmath::BigInt mtmath::BigInt::operator*(const mtmath::BigInt &o) const noexcept {
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


void mtmath::BigInt::compress(int base) {
  auto numerator = *digits;
  std::vector<uint8_t> result = *digits;
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

int mtmath::BigInt::compare(const mtmath::BigInt &o) const noexcept {
  if (!is_valid() || !o.is_valid()) {
    if (is_valid() != o.is_valid()) {
      return is_valid() ? 1 : -1;
    }
    else {
      return 0;
    }
  }
  else if (is_zero() && o.is_zero()) {
    return 0;
  }
  else if (is_negative() != o.is_negative()) {
    return is_negative() ? -1 : 1;
  }
  else if (digits->size() != o.digits->size()) {
    return digits->size() > o.digits->size() ? 1 : -1;
  }
  else {
    for (size_t i = 0; i < digits->size(); ++i) {
      auto index = digits->size() - i - 1;
      if (digits->at(index) != o.digits->at(index)) {
        return digits->at(index) < o.digits->at(index) ? -1 : 1;
      }
    }
  }
  return 0;
}

bool mtmath::BigInt::is_zero() const noexcept {
  return is_valid() && digits->empty();
}

bool mtmath::BigInt::is_valid() const noexcept {
  return !(flags & INVALID) && digits != nullptr;
}

bool mtmath::BigInt::is_negative() const noexcept {
  return flags & NEGATIVE;
}

mtmath::BigInt mtmath::BigInt::zero() {
  return zeroConst;
}

mtmath::BigInt mtmath::BigInt::one() {
  return oneConst;
}

mtmath::BigInt mtmath::BigInt::invalid() {
  return invalidConst;
}

mtmath::BigInt mtmath::BigInt::abs() const noexcept {
  auto copy = *this;
  copy.flags &= ~NEGATIVE;
  return copy;
}

mtmath::BigInt mtmath::BigInt::fresh() {
  BigInt r{};
  r.digits = std::make_shared<std::vector<uint8_t>>();
  return r;
}
std::tuple<mtmath::BigInt, mtmath::BigInt> mtmath::BigInt::divide(const mtmath::BigInt &denominator) const noexcept {
  // Handle invalid division case
  if (!is_valid() || !(denominator.is_valid()) || denominator.is_zero()) {
    return std::make_tuple(BigInt::invalid(), BigInt::invalid());
  }

  // Handle trivial cases
  auto denomAbs = denominator.abs();
  auto cmp = abs().compare(denomAbs);
  if (cmp < 0) {
    return std::make_tuple(BigInt::zero(), *this);
  }
  else if (cmp == 0) {
    auto res = BigInt::one();
    res.flags = flags ^ denominator.flags;
    return std::make_tuple(res, BigInt::zero());
  }
  else if (denomAbs == BigInt::oneConst) {
    auto res = *this;
    res.flags = flags ^ denominator.flags;
    return std::make_tuple(res, BigInt::zero());
  }

  // Get a fresh, new BigInt
  auto remainder = BigInt::fresh();
  auto quotient = BigInt::fresh();

  remainder.flags = flags ^ denominator.flags;
  quotient.flags = flags ^ denominator.flags;
  return std::make_tuple(remainder, quotient);
}
