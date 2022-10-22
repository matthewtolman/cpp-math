#include "big_int.h"

mtmath::BigInt::BigInt() : BigInt(0) {}
mtmath::BigInt::BigInt(const mtmath::BigInt &other) : negative(other.negative), digits(other.digits) {}
mtmath::BigInt::BigInt(mtmath::BigInt &&other) noexcept : negative(other.negative), digits(std::move(other.digits)) {}
mtmath::BigInt::BigInt(bool negative, std::shared_ptr<std::vector<uint8_t>> digits) : negative(negative), digits(digits) {}

std::string mtmath::BigInt::to_string() const {
  if (digits->size() == 0) {
    return "0";
  }
  std::string str;
  if (negative) {
    str += "-";
  }
  str.reserve(digits->size() + (negative ? 1 : 0));
  std::reverse(digits->begin(), digits->end());
  for (auto d : *digits) {
    str.push_back(static_cast<char>(d + '0'));
  }
  std::reverse(digits->begin(), digits->end());
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
    uint8_t carry = 0;
    for (size_t index = 0; index < o.digits->size() || index < digits->size(); ++index) {
      auto left = index < digits->size() ? digits->at(index) : 0;
      auto right = index < o.digits->size() ? o.digits->at(index) : 0;
      auto num = left + right + carry;
      carry = num / 10;
      newDigits->emplace_back(num % 10);
    }
    if (carry) {
      newDigits->emplace_back();
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

    int16_t borrow = 0;
    bool abs_less = this->abs_less_than(o);
    const auto& bigger = abs_less ? o: *this;
    const auto& smaller = abs_less ? *this : o;
    for (size_t index = 0; index < bigger.digits->size() || index < smaller.digits->size(); ++index) {
      auto left = static_cast<int16_t>(index < bigger.digits->size() ? bigger.digits->at(index) : 0);
      auto right = static_cast<int16_t>(index < smaller.digits->size() ? smaller.digits->at(index) : 0);
      auto num = left - right + borrow;
      borrow = 0;
      while (num < 0) {
        borrow -= 1;
        num += 10;
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
