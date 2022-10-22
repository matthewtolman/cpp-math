#include "big_int.h"

mtmath::BigInt::BigInt() : BigInt(0) {}
mtmath::BigInt::BigInt(const mtmath::BigInt &other) : negative(other.negative), digits(other.digits) {}
mtmath::BigInt::BigInt(mtmath::BigInt &&other) noexcept : negative(other.negative), digits(std::move(other.digits)) {}
mtmath::BigInt::BigInt(bool negative, std::shared_ptr<std::vector<uint8_t>> digits) : negative(negative), digits(digits) {}

static char hex_char(uint8_t half_byte) {
  if (half_byte < 10) {
    return '0' + half_byte;
  }
  return 'a' + (half_byte - 10);
}

std::string mtmath::BigInt::to_string() const {
  if (digits->size() == 0) {
    return "0";
  }
  std::string str;
  str.reserve((digits->size() * 2) + (negative ? 1 : 0));

  for (auto d : *digits) {
    uint8_t char1 = d & 0x0f;
    uint8_t char2 = (d & 0xf0) >> 4;
    str.push_back(hex_char(char1));
    str.push_back(hex_char(char2));
  }
  if (str[str.size() - 1] == '0') {
    str.erase(str.begin() + str.size() - 1);
  }
  str.push_back('x');
  str.push_back('0');
  if (negative) {
    str.push_back('-');
  }
  std::reverse(str.begin(), str.end());
  return str;
}

void mtmath::BigInt::simplify(){
  for (size_t i = digits->size(); i > 0; --i) {
    if (digits->at(i - 1) != 0) {
      if (i != digits->size()) {
        digits->erase(digits->begin() + i, digits->end());
        digits->shrink_to_fit();
      }
      return;
    }
  }
}

bool mtmath::BigInt::operator==(const mtmath::BigInt &o) const noexcept {
  if (negative != o.negative) {
    return false;
  }
  if (digits->size() != o.digits->size()) {
    return false;
  }
  for (size_t i = 0; i < digits->size(); ++i) {
    if (digits->at(i) != o.digits->at(i)) {
      return false;
    }
  }
  return true;
}

bool mtmath::BigInt::operator!=(const mtmath::BigInt &o) const noexcept {
  return !(*this == o);
}

mtmath::BigInt mtmath::BigInt::operator-() const {
  auto copy = *this;
  copy.negative = !copy.negative;
  return copy;
}

mtmath::BigInt mtmath::BigInt::operator+(const mtmath::BigInt &o) const noexcept {
  if (negative == o.negative) {
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
    newDigits->shrink_to_fit();
    return BigInt{negative, newDigits};
  }
  else if (negative) {
    return o - (this->operator-());
  }
  else {
    return this->operator-(-o);
  }
}

mtmath::BigInt mtmath::BigInt::operator-(const mtmath::BigInt &o) const noexcept {
  if (negative == o.negative) {
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
    newDigits->shrink_to_fit();
    auto res = BigInt{abs_less ? !negative : negative, newDigits};
    res.simplify();
    return res;
  }
  else {
    return this->operator+(-o);
  }
}

mtmath::BigInt &mtmath::BigInt::operator=(const mtmath::BigInt &other) = default;

mtmath::BigInt &mtmath::BigInt::operator=(mtmath::BigInt &&other) noexcept {
  std::swap(negative, other.negative);
  std::swap(digits, other.digits);
  return *this;
}

bool mtmath::BigInt::abs_less_than(const mtmath::BigInt &o) const noexcept {
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
mtmath::BigInt mtmath::BigInt::operator/(const mtmath::BigInt &o) const noexcept {
  return mtmath::BigInt();
}

void mtmath::BigInt::compress() {
  auto numerator = *digits;
  std::vector<uint8_t> result = *digits;
  digits->clear();
  result.clear();

  const auto divide_by = std::numeric_limits<uint8_t>::max() + 1;

  uint16_t divisor_remainder = 0;
  while (!numerator.empty()) {
    for (auto n : numerator) {
      divisor_remainder = divisor_remainder * 10 + n;
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
  digits->shrink_to_fit();
}
