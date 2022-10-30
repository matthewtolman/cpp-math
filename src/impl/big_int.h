#pragma once

#include <vector>
#include <string>
#include <tuple>
#include "byte_array.h"
#include <compare>

namespace mtmath {
  namespace immut {
    class BigInt;
  }

  class BigInt {
    enum FLAGS {
      NEGATIVE = 0x1,
      INVALID = 0x2
    };

    uint8_t flags = 0x0;
    ByteArray digits = {};
    BigInt(uint8_t flags, ByteArray digits) : flags(flags), digits(std::move(digits)) {}

  public:
    BigInt() = default;
    BigInt(const BigInt& other) = default;
    BigInt(BigInt&& other) noexcept : flags(other.flags), digits(std::move(other.digits)) {}
    BigInt& operator=(const BigInt& other) = default;
    BigInt& operator=(BigInt&& other) noexcept { flags = other.flags; std::swap(digits, other.digits); return *this; }

    [[nodiscard]] static BigInt zero() { return BigInt{}; }
    [[nodiscard]] static BigInt one() { return BigInt{1}; }
    [[nodiscard]] static BigInt invalid() { return BigInt{INVALID, ByteArray{}}; }

    [[nodiscard]] bool is_zero() const noexcept { return digits.empty(); }
    [[nodiscard]] bool is_valid() const noexcept { return !(flags & INVALID); }
    [[nodiscard]] bool is_negative() const noexcept { return flags & NEGATIVE; }

    BigInt& abs() noexcept { flags &= ~NEGATIVE; return *this; }
    [[nodiscard]] BigInt abs_val() const noexcept { auto copy = *this; return copy.abs(); }

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
              digits.emplace_back(ch - 'a' + 10);
            }
          }
        }
        else if (ch - '0' < base) {
          digits.emplace_back(ch - '0');
        }
        else {
          return false;
        }
        return true;
      };

      if constexpr (std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, std::string_view>) {
        digits.reserve(number.size());
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
        digits.emplace_back(n & std::numeric_limits<uint8_t>::max());
        n >>= std::numeric_limits<uint8_t>::digits;
      }
      simplify();
    }

    [[nodiscard]] std::string to_string(int base) const;

    [[nodiscard]] BigInt operator-() const;
    BigInt& operator+=(const BigInt& o) noexcept;
    BigInt& operator-=(const BigInt& o) noexcept;
    BigInt& operator/=(const BigInt& o) noexcept;
    BigInt& operator%=(const BigInt& o) noexcept;
    BigInt& operator*=(const BigInt& o) noexcept;
    [[nodiscard]] BigInt operator+(const BigInt& o) const { auto copy = *this; return copy += o; }
    [[nodiscard]] BigInt operator-(const BigInt& o) const { auto copy = *this; return copy -= o; }
    [[nodiscard]] BigInt operator/(const BigInt& o) const { auto copy = *this; return copy /= o; }
    [[nodiscard]] BigInt operator%(const BigInt& o) const { auto copy = *this; return copy %= o; }
    [[nodiscard]] BigInt operator*(const BigInt& o) const { auto copy = *this; return copy *= o; }
    [[nodiscard]] std::tuple<BigInt, BigInt> divide(const BigInt& denominator) const noexcept;

    [[nodiscard]] std::strong_ordering operator<=>(const BigInt& o) const noexcept;
    [[nodiscard]] bool operator==(const BigInt& o) const noexcept {
      return *this <=> o == std::strong_ordering::equal;
    }

    [[nodiscard]] BigInt operator>>(size_t i) const { auto copy = *this; copy >>= i; return copy; }
    [[nodiscard]] BigInt operator<<(size_t i) const { auto copy = *this; copy <<= i; return copy; }
    BigInt& operator<<=(size_t i);
    BigInt& operator>>=(size_t i);

    [[nodiscard]] immut::BigInt to_immut() const;

    friend ::mtmath::immut::BigInt;

  private:
    void simplify();
    void compress(int base);
    [[nodiscard]] int abs_compare(const BigInt& o) const noexcept;
  };

  namespace immut {
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
      [[nodiscard]] BigInt abs_val() const noexcept { return abs(); }

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

      [[nodiscard]] std::strong_ordering operator<=>(const BigInt& o) const noexcept;
      [[nodiscard]] bool operator==(const BigInt& o) const noexcept {
        return *this <=> o == std::strong_ordering::equal;
      }

      [[nodiscard]] BigInt operator<<(size_t i) const noexcept;
      [[nodiscard]] BigInt operator>>(size_t i) const noexcept;

      friend ::mtmath::BigInt;

      [[nodiscard]] ::mtmath::BigInt to_mut() const;

    private:
      void simplify();
      void compress(int base);
      [[nodiscard]] bool abs_less_than(const BigInt& o) const noexcept;
    };
  }
}

template<>
class std::numeric_limits<mtmath::BigInt> {
public:
  using Type = mtmath::BigInt;
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = true;
  static constexpr bool has_signaling_NaN = false;
  static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
  static constexpr bool has_denorm_loss = false;
  static constexpr std::float_round_style round_style = std::round_to_nearest;
  static constexpr bool is_iec559 = false;
  static constexpr bool is_bounded = false;
  static constexpr bool is_modulo = false;
  static constexpr int digits = std::numeric_limits<int>::max();
  static constexpr int digits10 = std::numeric_limits<int>::max();
  static constexpr int max_digits10 = std::numeric_limits<int>::max();
  static constexpr int radix = 2;
  static constexpr int min_exponent = 0;
  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent = 0;
  static constexpr int max_exponent10 = 0;
  static constexpr bool traps = false;
  static constexpr bool tinyness_before = false;

  static Type min() {
    return Type{ 0 };
  }

  static Type lowest() {
    return Type{ 0 };
  }

  static Type epsilon() {
    return Type {1};
  }

  static Type round_error() {
    return Type {0};
  }

  static Type infinity() {
    return Type { 0 };
  }

  static Type quiet_NaN() {
    return Type::invalid();
  }

  static Type signaling_NaN() {
    return quiet_NaN();
  }

  static Type denorm_min() {
    return min();
  }
};

template<>
class std::numeric_limits<mtmath::immut::BigInt> {
public:
  using Type = mtmath::immut::BigInt;
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = true;
  static constexpr bool has_signaling_NaN = false;
  static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
  static constexpr bool has_denorm_loss = false;
  static constexpr std::float_round_style round_style = std::round_to_nearest;
  static constexpr bool is_iec559 = false;
  static constexpr bool is_bounded = false;
  static constexpr bool is_modulo = false;
  static constexpr int digits = std::numeric_limits<int>::max();
  static constexpr int digits10 = std::numeric_limits<int>::max();
  static constexpr int max_digits10 = std::numeric_limits<int>::max();
  static constexpr int radix = 2;
  static constexpr int min_exponent = 0;
  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent = 0;
  static constexpr int max_exponent10 = 0;
  static constexpr bool traps = false;
  static constexpr bool tinyness_before = false;

  static Type min() {
    return Type::zero();
  }

  static Type lowest() {
    return Type::zero();
  }

  static Type epsilon() {
    return Type::one();
  }

  static Type round_error() {
    return Type::zero();
  }

  static Type infinity() {
    return Type::zero();
  }

  static Type quiet_NaN() {
    return Type::invalid();
  }

  static Type signaling_NaN() {
    return quiet_NaN();
  }

  static Type denorm_min() {
    return min();
  }
};

inline std::ostream& operator<< (std::ostream& o, const mtmath::BigInt& b)
{
  o << b.to_string(10);
  return o;
}

inline std::ostream& operator<< (std::ostream& o, const mtmath::immut::BigInt& b)
{
  o << b.to_string(10);
  return o;
}
