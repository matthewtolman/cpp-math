#pragma once

#include "big_int.h"
#include <memory>
#include <ostream>

namespace mtmath {
  /**
   * Base for rational numbers. Does not try to do structural sharing, works with full copies
   * @tparam T Underlying type for rational numbers
   */
  template<typename T>
  class RationalBase {
  public:
    RationalBase(T numerator, T denominator) noexcept : numerator(std::move(numerator)), denominator(std::move(denominator)) {
      simplify();
    }

    RationalBase()
        : numerator(0),
          denominator(1)
    {}

    RationalBase(RationalBase&& rb) noexcept
        : numerator(std::move(rb.numerator)),
          denominator(std::move(rb.denominator))
    {}

    RationalBase(const RationalBase& rb)
        : numerator(rb.numerator),
          denominator(rb.denominator)
    {}

    RationalBase& operator=(const RationalBase& rb) {
      numerator = rb.numerator;
      denominator = rb.denominator;
      return *this;
    }

    RationalBase& operator=(RationalBase&& rb) noexcept {
      std::swap(numerator, rb.numerator);
      std::swap(denominator, rb.denominator);
      return *this;
    }

    [[nodiscard]] bool is_nan() const noexcept { return denominator == T{0} && numerator == T{0}; }
    [[nodiscard]] bool is_infinite() const noexcept { return denominator == T{0} && numerator != T{0}; }
    [[nodiscard]] bool is_pos_infinity() const noexcept { return denominator == T{0} && numerator > T{0}; }
    [[nodiscard]] bool is_neg_infinity() const noexcept { return denominator == T{0} && numerator < T{0}; }
    [[nodiscard]] bool is_finite() const noexcept { return denominator != T{0}; }
    [[nodiscard]] std::strong_ordering operator<=>(const RationalBase& o) const noexcept {
      auto n1 = numerator;
      auto n2 = o.numerator;
      if (denominator != o.denominator) {
        n1 = numerator * o.denominator;
        n2 = o.numerator * denominator;
      }

      auto cmp = n1 <=> n2;

      if (cmp < 0) {
        return std::strong_ordering::less;
      }
      else if (cmp > 0) {
        return std::strong_ordering::greater;
      }
      else {
        return std::strong_ordering::equal;
      }
    }
    [[nodiscard]] bool operator==(const RationalBase& o) const noexcept {
      return ((*this) <=> o) == std::strong_ordering::equal;
    }

    RationalBase operator-() const noexcept {
      if constexpr (std::numeric_limits<T>::is_signed) {
        return mtmath::RationalBase<T>{-numerator, denominator};
      }
      return *this;
    }

    RationalBase& operator+=(const RationalBase& other) noexcept {
      if (is_finite() && other.is_finite()) {
        if (denominator == other.denominator) {
          numerator = numerator + other.numerator;
        }
        else {
          auto n1 = numerator * other.denominator;
          auto n2 = other.numerator * denominator;
          denominator = denominator * other.denominator;
          numerator = n1 + n2;
          simplify();
        }
      }
      else if ((is_pos_infinity() && other.is_pos_infinity()) || (is_neg_infinity() && other.is_neg_infinity())) {
        return *this;
      }
      else {
        numerator = 0;
        denominator = 0;
        return *this;
      }
      return *this;
    }

    RationalBase operator+(const RationalBase& other) const noexcept {
      auto copy = *this;
      copy += other;
      return copy;
    }

    RationalBase& operator-=(const RationalBase& other) noexcept {
      *this += -other;
      return *this;
    }

    RationalBase operator-(const RationalBase& other) const noexcept {
      auto copy = *this;
      copy -= other;
      return copy;
    }

    RationalBase& operator*=(const RationalBase& other) noexcept {
      numerator *= other.numerator;
      denominator *= other.denominator;
      simplify();
      return *this;
    }

    RationalBase operator*(const RationalBase& other) const noexcept {
      auto copy = *this;
      copy *= other;
      return copy;
    }

    RationalBase& operator/=(const RationalBase& other) noexcept {
      numerator *= other.denominator;
      denominator *= other.numerator;
      simplify();
      return *this;
    }

    RationalBase operator/(const RationalBase& other) const noexcept {
      auto copy = *this;
      copy /= other;
      return copy;
    }

    friend std::ostream& operator<<(std::ostream& o, const mtmath::RationalBase<T>& r)
    {
      o << r.numerator << "/" << r.denominator;
      return o;
    }

  private:
    T numerator;
    T denominator;

    T remainder(const T& n, const T&d) {
      if (n < 0) {
        return (-n) % d;
      }
      return n % d;
    }

    T gcd(const T& a, const T& b) {
      auto A = a;
      auto B = b;
      while (B != 0) {
        auto R = remainder(A, B);
        A = B;
        B = R;
      }
      if (A < 0) {
        return -A;
      }
      return A;
    }

    void simplify() {
      if (denominator < 0) {
        denominator = -denominator;
      }
      auto n = numerator;
      auto d = denominator;
      if (d == 1) {
        return;
      }
      else if (d != 0) {
        if (remainder(n, d) == 0) {
          numerator = n / d;
        }
        else {
          auto g = gcd(n, d);
          if (d > 0) {
            numerator = n / g;
            denominator = d / g;
          }
          else if (d < 0) {
            numerator = -n / g;
            denominator = -d / g;
          }
        }
      }
    }
  };

  using Rational = RationalBase<mtmath::BigInt>;

  namespace immut {
    class Rational {
    public:
      Rational(mtmath::immut::BigInt numerator, mtmath::immut::BigInt denominator) noexcept
          : numerator(std::move(numerator)), denominator(std::move(denominator))
      { simplify(); }

      Rational() : numerator(mtmath::immut::BigInt::zero()), denominator(mtmath::immut::BigInt::one()) {}

      Rational(Rational&& rb) noexcept : numerator(std::move(rb.numerator)), denominator(std::move(rb.denominator)) {}

      Rational(const Rational& rb) : numerator(rb.numerator), denominator(rb.denominator) {}

      Rational& operator=(const Rational& rb) {
        numerator = rb.numerator;
        denominator = rb.denominator;
        return *this;
      }

      Rational& operator=(Rational&& rb) noexcept {
        std::swap(numerator, rb.numerator);
        std::swap(denominator, rb.denominator);
        return *this;
      }

      [[nodiscard]] bool is_nan() const noexcept { return denominator.is_zero() && numerator.is_zero(); }
      [[nodiscard]] bool is_infinite() const noexcept { return denominator.is_zero() && !numerator.is_zero(); }
      [[nodiscard]] bool is_pos_infinity() const noexcept { return denominator.is_zero() && !numerator.is_zero() && !numerator.is_negative(); }
      [[nodiscard]] bool is_neg_infinity() const noexcept { return denominator.is_zero() && !numerator.is_zero() && numerator.is_negative(); }
      [[nodiscard]] bool is_finite() const noexcept { return !denominator.is_zero(); }
      [[nodiscard]] std::strong_ordering operator<=>(const Rational& o) const noexcept {
        auto n1 = numerator;
        auto n2 = o.numerator;
        if (denominator != o.denominator) {
          n1 = numerator * o.denominator;
          n2 = o.numerator * denominator;
        }

        return n1 <=> n2;
      }
      [[nodiscard]] bool operator==(const Rational& o) const noexcept {
        return ((*this) <=> o) == std::strong_ordering::equal;
      }

      Rational operator-() const noexcept {
        return Rational{-numerator, denominator};
      }

      Rational operator+(const Rational& other) const noexcept {
        if (is_finite() && other.is_finite()) {
          auto res = *this;
          if (denominator == other.denominator) {
            res.numerator = numerator + other.numerator;
          }
          else {
            auto n1 = numerator * other.denominator;
            auto n2 = other.numerator * denominator;
            res.denominator = denominator * other.denominator;
            res.numerator = n1 + n2;
            res.simplify();
          }
          return res;
        }
        else if ((is_pos_infinity() && other.is_pos_infinity()) || (is_neg_infinity() && other.is_neg_infinity())) {
          return *this;
        }
        else {
          return Rational{mtmath::immut::BigInt::zero(), mtmath::immut::BigInt::zero()};
        }
      }

      Rational operator-(const Rational& other) const noexcept {
        return *this + -other;
      }

      Rational operator*(const Rational& other) const noexcept {
        auto res = Rational{numerator * other.numerator, denominator * other.denominator};
        res.simplify();
        return res;
      }

      Rational operator/(const Rational& other) const noexcept {
        auto res = Rational{numerator * other.denominator, denominator * other.numerator};
        res.simplify();
        return res;
      }

      friend std::ostream& operator<<(std::ostream& o, const Rational& r)
      {
        o << r.numerator;
        o << "/";
        o << r.denominator;
        return o;
      }

    private:
      mtmath::immut::BigInt numerator;
      mtmath::immut::BigInt denominator;

      static mtmath::immut::BigInt remainder(const mtmath::immut::BigInt& n, const mtmath::immut::BigInt&d) {
        if (n < mtmath::immut::BigInt::zero()) {
          return (-n) % d;
        }
        return n % d;
      }

      mtmath::immut::BigInt gcd(const mtmath::immut::BigInt& a, const mtmath::immut::BigInt& b) {
        auto A = a;
        auto B = b;
        while (!B.is_zero()) {
          auto R = remainder(A, B);
          A = B;
          B = R;
        }
        if (A.is_negative()) {
          return -A;
        }
        return A;
      }

      void simplify() {
        denominator = denominator.abs();
        auto n = numerator;
        auto d = denominator;
        if (d == 1) {
          return;
        }
        else if (!d.is_zero()) {
          if (remainder(n, d) == 0) {
            numerator = n / d;
          }
          else {
            auto g = gcd(n, d);
            if (d > 0) {
              numerator = n / g;
              denominator = d / g;
            }
            else if (d < 0) {
              numerator = -n / g;
              denominator = -d / g;
            }
          }
        }
      }
    };
  }
}

template <typename RB>
class std::numeric_limits<mtmath::RationalBase<RB>> {
public:
  using Type = mtmath::RationalBase<RB>;
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = std::numeric_limits<RB>::is_signed;
  static constexpr bool is_integer = false;
  static constexpr bool is_exact = true;
  static constexpr bool has_infinity = true;
  static constexpr bool has_quiet_NaN = true;
  static constexpr bool has_signaling_NaN = false;
  static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
  static constexpr bool has_denorm_loss = false;
  static constexpr std::float_round_style round_style = std::numeric_limits<RB>::round_style;
  static constexpr bool is_iec559 = false;
  static constexpr bool is_bounded = std::numeric_limits<RB>::is_bounded;
  static constexpr bool is_modulo = std::numeric_limits<RB>::is_modulo;
  static constexpr int digits = std::numeric_limits<RB>::digits;
  static constexpr int digits10 = std::numeric_limits<RB>::digits10;
  static constexpr int max_digits10 = std::numeric_limits<RB>::max_digits10;
  static constexpr int radix = std::numeric_limits<RB>::radix;
  static constexpr int min_exponent = std::numeric_limits<RB>::min_exponent;
  static constexpr int min_exponent10 = std::numeric_limits<RB>::min_exponent10;
  static constexpr int max_exponent = std::numeric_limits<RB>::max_exponent;
  static constexpr int max_exponent10 = std::numeric_limits<RB>::max_exponent10;
  static constexpr bool traps = false;
  static constexpr bool tinyness_before = std::numeric_limits<RB>::tinyness_before;

  static constexpr Type min() noexcept {
    if constexpr (std::numeric_limits<RB>::is_bounded) {
      return Type{1, std::numeric_limits<RB>::max()};
    }
    else {
      return -infinity();
    }
  }

  static constexpr Type max() noexcept {
    if constexpr (std::numeric_limits<RB>::is_bounded) {
      return Type{std::numeric_limits<RB>::max(), RB{1}};
    }
    else {
      return infinity();
    }
  }

  static constexpr Type lowest() noexcept {
    if constexpr (std::numeric_limits<RB>::is_bounded && std::numeric_limits<RB>::min() != 0) {
      return Type{
        RB{1}, std::numeric_limits<RB>::max()
      };
    }
    else {
      return Type{ RB{0}, RB{1}};
    }
  }

  static constexpr Type epsilon() noexcept {
    if (std::numeric_limits<RB>::epsilon()) {
      return Type{std::numeric_limits<RB>::epsilon(), RB{1}};
    }
    else {
      return min();
    }
  }

  static constexpr Type round_error() noexcept {
    return Type { RB{1}, RB{2}};
  }

  static constexpr Type infinity() noexcept {
    return Type { RB{1}, RB{0}};
  }

  static constexpr Type quiet_NaN() noexcept {
    return Type { RB{0}, RB{0}};
  }

  static constexpr Type signaling_NaN() noexcept {
    return quiet_NaN();
  }

  static constexpr Type denorm_min() noexcept {
    return min();
  }
};

template<>
class std::numeric_limits<mtmath::RationalBase<mtmath::BigInt>> {
public:
  using Type = mtmath::immut::Rational;

#define MTMATH_RATIONAL_DEF_TRAIT(trait) static constexpr bool trait = std::numeric_limits<mtmath::BigInt>:: trait;

  static constexpr bool is_specialized = true;
  MTMATH_RATIONAL_DEF_TRAIT(is_signed)
  static constexpr bool is_integer = false;
  MTMATH_RATIONAL_DEF_TRAIT(is_exact)
  static constexpr bool has_infinity = true;
  static constexpr bool has_quiet_NaN = true;
  static constexpr bool has_signaling_NaN = false;
  static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
  static constexpr bool has_denorm_loss = false;
  static constexpr std::float_round_style round_style = std::numeric_limits<mtmath::BigInt>::round_style;
  MTMATH_RATIONAL_DEF_TRAIT(is_iec559)
  MTMATH_RATIONAL_DEF_TRAIT(is_bounded)
  MTMATH_RATIONAL_DEF_TRAIT(is_modulo)
  MTMATH_RATIONAL_DEF_TRAIT(digits)
  MTMATH_RATIONAL_DEF_TRAIT(digits10)
  MTMATH_RATIONAL_DEF_TRAIT(max_digits10)
  MTMATH_RATIONAL_DEF_TRAIT(radix)
  MTMATH_RATIONAL_DEF_TRAIT(min_exponent)
  MTMATH_RATIONAL_DEF_TRAIT(min_exponent10)
  MTMATH_RATIONAL_DEF_TRAIT(max_exponent)
  MTMATH_RATIONAL_DEF_TRAIT(max_exponent10)
  MTMATH_RATIONAL_DEF_TRAIT(traps)
  MTMATH_RATIONAL_DEF_TRAIT(tinyness_before)

#undef MTMATH_RATIONAL_DEF_TRAIT

  static Type min() noexcept {
    return -infinity();
  }

  static Type max() noexcept {
    return infinity();
  }

  static Type lowest() noexcept {
    return Type{mtmath::immut::BigInt::zero(), mtmath::immut::BigInt::one()};
  }

  static Type epsilon() noexcept {
    return Type{mtmath::immut::BigInt::zero(), mtmath::immut::BigInt::one()};
  }

  static Type round_error() noexcept {
    return Type{mtmath::immut::BigInt::one(), mtmath::immut::BigInt::two()};
  }

  static Type infinity() noexcept {
    return Type{mtmath::immut::BigInt::one(), mtmath::immut::BigInt::zero()};
  }

  static Type quiet_NaN() noexcept {
    return Type{mtmath::immut::BigInt::zero(), mtmath::immut::BigInt::zero()};
  }

  static Type signaling_NaN() noexcept {
    return quiet_NaN();
  }

  static Type denorm_min() noexcept {
    return min();
  }
};

template<>
class std::numeric_limits<mtmath::immut::Rational> {
public:
  using Type = mtmath::immut::Rational;

#define MTMATH_IMMUT_RATIONAL_DEF_TRAIT(trait) static constexpr bool trait = std::numeric_limits<mtmath::immut::BigInt>:: trait;

  static constexpr bool is_specialized = true;
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(is_signed)
  static constexpr bool is_integer = false;
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(is_exact)
  static constexpr bool has_infinity = true;
  static constexpr bool has_quiet_NaN = true;
  static constexpr bool has_signaling_NaN = false;
  static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
  static constexpr bool has_denorm_loss = false;
  static constexpr std::float_round_style round_style = std::numeric_limits<mtmath::immut::BigInt>::round_style;
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(is_iec559)
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(is_bounded)
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(is_modulo)
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(digits)
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(digits10)
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(max_digits10)
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(radix)
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(min_exponent)
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(min_exponent10)
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(max_exponent)
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(max_exponent10)
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(traps)
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(tinyness_before)

#undef MTMATH_IMMUT_RATIONAL_DEF_TRAIT

  static Type min() noexcept {
    return -infinity();
  }

  static Type max() noexcept {
    return infinity();
  }

  static Type lowest() noexcept {
    return Type{mtmath::immut::BigInt::zero(), mtmath::immut::BigInt::one()};
  }

  static Type epsilon() noexcept {
    return Type{mtmath::immut::BigInt::zero(), mtmath::immut::BigInt::one()};
  }

  static Type round_error() noexcept {
    return Type{mtmath::immut::BigInt::one(), mtmath::immut::BigInt::two()};
  }

  static Type infinity() noexcept {
    return Type{mtmath::immut::BigInt::one(), mtmath::immut::BigInt::zero()};
  }

  static Type quiet_NaN() noexcept {
    return Type{mtmath::immut::BigInt::zero(), mtmath::immut::BigInt::zero()};
  }

  static Type signaling_NaN() noexcept {
    return quiet_NaN();
  }

  static Type denorm_min() noexcept {
    return min();
  }
};
