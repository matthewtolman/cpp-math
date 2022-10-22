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
              digits->template emplace_back(ch - 'a' + 10);
            }
          }
        }
        else if (ch - '0' < base) {
          digits->template emplace_back(ch - '0');
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
            negative = true;
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
          negative = true;
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
        negative = true;
        n *= -1;
      }
      while (n > 0) {
        digits->template emplace_back(n & std::numeric_limits<uint8_t>::max());
        n >>= std::numeric_limits<uint8_t>::digits;
      }
      simplify();
    }

    [[nodiscard]] std::string to_string(int base) const;

    [[nodiscard]] BigInt operator-() const;
    [[nodiscard]] BigInt operator+(const BigInt& o) const noexcept;
    [[nodiscard]] BigInt operator-(const BigInt& o) const noexcept;
//    [[nodiscard]] BigInt operator/(const BigInt& o) const noexcept;
//    [[nodiscard]] BigInt operator*(const BigInt& o) const noexcept;

    [[nodiscard]] int compare(const BigInt& o) const noexcept;
    [[nodiscard]] bool operator==(const BigInt& o) const noexcept { return compare(o) == 0; }
    [[nodiscard]] bool operator!=(const BigInt& o) const noexcept { return compare(o) != 0; }
    [[nodiscard]] bool operator<(const BigInt& o) const noexcept { return compare(o) < 0; }
    [[nodiscard]] bool operator<=(const BigInt& o) const noexcept { return compare(o) <= 0; }
    [[nodiscard]] bool operator>(const BigInt& o) const noexcept { return compare(o) > 0; }
    [[nodiscard]] bool operator>=(const BigInt& o) const noexcept { return compare(o) >= 0; }

  private:
    void simplify();
    void compress(int base);
    bool abs_less_than(const BigInt& o) const noexcept;
  };
}
