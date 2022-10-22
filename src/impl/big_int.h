#pragma once

#include <vector>
#include <string>

namespace mtmath {
  class BigInt {
    bool negative = false;
    std::shared_ptr<std::vector<uint8_t>> digits = std::make_shared<std::vector<uint8_t>>();
    BigInt(bool negative, std::shared_ptr<std::vector<uint8_t>> digits);
  public:
    BigInt();
    BigInt(const BigInt& other);
    BigInt(BigInt&& other) noexcept;
    BigInt& operator=(const BigInt& other);
    BigInt& operator=(BigInt&& other) noexcept;

    template<typename T>
    BigInt(const T& number) {
      if constexpr (std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, std::string_view>) {
        digits->reserve(number.size());
        if (!number.empty()) {
          if (number[0] == '-') {
            negative = true;
          }
        }
        for(size_t i = (number[0] == '-' || number[0] == '+'); i < number.size(); ++i) {
          auto ch = number[i];
          if (!std::isdigit(ch)) {
            break;
          }
          digits->template emplace_back(ch - '0');
        }
        compress();
      }
      else if constexpr (std::is_same_v<std::decay_t<T>, char*>|| std::is_same_v<std::decay_t<T>, const char*>) {
        if (number[0] == '-') {
          negative = true;
        }
        for(size_t i = (number[0] == '-' || number[0] == '+'); ; ++i) {
          auto ch = number[i];
          if (!std::isdigit(ch)) {
            break;
          }
          digits->template emplace_back(ch - '0');
        }
        compress();
      }
      else if constexpr (std::numeric_limits<T>::is_integer) {
        auto n = number;
        if (n < 0) {
          negative = true;
          n *= -1;
        }
        while (n > 0) {
          digits->template emplace_back(n & std::numeric_limits<uint8_t>::max());
          n >>= std::numeric_limits<uint8_t>::digits;
        }
      }
      simplify();
    }

    [[nodiscard]] std::string to_string() const;

    [[nodiscard]] BigInt operator-() const;
    [[nodiscard]] BigInt operator+(const BigInt& o) const noexcept;
    [[nodiscard]] BigInt operator-(const BigInt& o) const noexcept;
    [[nodiscard]] BigInt operator/(const BigInt& o) const noexcept;
    [[nodiscard]] BigInt operator*(const BigInt& o) const noexcept;

    [[nodiscard]] bool operator==(const BigInt& o) const noexcept;
    [[nodiscard]] bool operator!=(const BigInt& o) const noexcept;

  private:
    void simplify();
    void compress();
    bool abs_less_than(const BigInt& o) const noexcept;
  };
}
