#pragma once

#include <vector>
#include <string>
#include <tuple>
#include "byte_array.h"

namespace mtmath {
  class BigInt {
    enum FLAGS {
      NEGATIVE = 0x1,
      INVALID = 0x2
    };

    uint8_t flags = 0x0;
    std::shared_ptr<ByteArray> digits = std::make_shared<ByteArray>();
    BigInt(uint8_t flags, std::shared_ptr<ByteArray> digits);

    static BigInt zeroConst;
    static BigInt oneConst;
    static BigInt invalidConst;

    static BigInt fresh();

  public:
    BigInt();
    BigInt(const BigInt& other);
    BigInt(BigInt&& other) noexcept;
    BigInt& operator=(const BigInt& other);
    BigInt& operator=(BigInt&& other) noexcept;

    [[nodiscard]] static BigInt zero();
    [[nodiscard]] static BigInt one();
    [[nodiscard]] static BigInt invalid();

    [[nodiscard]] bool is_zero() const noexcept;
    [[nodiscard]] bool is_valid() const noexcept;
    [[nodiscard]] bool is_negative() const noexcept;

    [[nodiscard]] BigInt abs() const noexcept;

    template<typename T>
    BigInt(const T& number, int base) {
      static_assert(std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, std::string_view>
        || std::is_same_v<std::decay_t<T>, char*>|| std::is_same_v<std::decay_t<T>, const char*>,
          "Can only do base conversion with string types!");

      if (base < 2 || base > 36) {
        throw std::runtime_error("BigInt must come from strings in a base between 2 and 36.");
      }

      auto process_char = [&](auto ch) {
        if (!std::isdigit(ch)) {
          if (base <= 10 || !std::isalpha(ch)) {
            return false;
          }
          else {
            // to lowercase
            ch |= 1 << 5;
            if (ch - 'a' <= base - 10) {
              digits->emplace_back(ch - 'a' + 10);
            }
          }
        }
        else if (ch - '0' < base) {
          digits->emplace_back(ch - '0');
        }
        else {
          return false;
        }
        return true;
      };

      if constexpr (std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, std::string_view>) {
        digits->reserve(number.size());
        if (!number.empty()) {
          if (number[0] == '-') {
            flags |= NEGATIVE;
          }
        }
        for(size_t i = (number[0] == '-' || number[0] == '+'); i < number.size(); ++i) {
          if (!process_char(number[i])) {
            break;
          }
        }
        compress(base);
      }
      else if constexpr (std::is_same_v<std::decay_t<T>, char*>|| std::is_same_v<std::decay_t<T>, const char*>) {
        if (number[0] == '-') {
          flags |= NEGATIVE;
        }
        for(size_t i = (number[0] == '-' || number[0] == '+'); ; ++i) {
          if (!process_char(number[i])) {
            break;
          }
        }
        compress(base);
      }
      simplify();
    }

    explicit BigInt(const std::string& s) : BigInt(s, 10) {}
    explicit BigInt(const std::string_view& s) : BigInt(s, 10) {}
    explicit BigInt(const char* c) : BigInt(c, 10) {}
    explicit BigInt(char* c) : BigInt(c, 10) {}

    template<typename T>
    BigInt(const T& number) {
      static_assert(std::numeric_limits<T>::is_integer, "Can only initialize from strings and integers");
      auto n = number;
      if (n < 0) {
        flags |= NEGATIVE;
        n *= -1;
      }
      while (n > 0) {
        digits->emplace_back(n & std::numeric_limits<uint8_t>::max());
        n >>= std::numeric_limits<uint8_t>::digits;
      }
      simplify();
    }

    [[nodiscard]] std::string to_string(int base) const;

    [[nodiscard]] BigInt operator-() const;
    [[nodiscard]] BigInt operator+(const BigInt& o) const noexcept;
    [[nodiscard]] BigInt operator-(const BigInt& o) const noexcept;
    [[nodiscard]] BigInt operator/(const BigInt& o) const noexcept;
    [[nodiscard]] BigInt operator%(const BigInt& o) const noexcept;
    [[nodiscard]] BigInt operator*(const BigInt& o) const noexcept;
    [[nodiscard]] std::tuple<BigInt, BigInt> divide(const BigInt& denominator) const noexcept;

    [[nodiscard]] int compare(const BigInt& o) const noexcept;
    [[nodiscard]] bool operator==(const BigInt& o) const noexcept { return compare(o) == 0; }
    [[nodiscard]] bool operator!=(const BigInt& o) const noexcept { return compare(o) != 0; }
    [[nodiscard]] bool operator<(const BigInt& o) const noexcept { return compare(o) < 0; }
    [[nodiscard]] bool operator<=(const BigInt& o) const noexcept { return compare(o) <= 0; }
    [[nodiscard]] bool operator>(const BigInt& o) const noexcept { return compare(o) > 0; }
    [[nodiscard]] bool operator>=(const BigInt& o) const noexcept { return compare(o) >= 0; }

    [[nodiscard]] BigInt operator<<(size_t i) const noexcept;
    [[nodiscard]] BigInt operator>>(size_t i) const noexcept;

  private:
    void simplify();
    void compress(int base);
    bool abs_less_than(const BigInt& o) const noexcept;
  };
}
