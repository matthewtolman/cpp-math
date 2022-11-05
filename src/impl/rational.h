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
    static_assert(std::numeric_limits<T>::is_signed, "Rationals must use signed numbers");

    RationalBase(T numerator, T denominator = 1) noexcept : m_numerator(std::move(numerator)), m_denominator(std::move(denominator)) {
      simplify();
    }

    RationalBase()
        : m_numerator(0),
          m_denominator(1)
    {}

    RationalBase(RationalBase&& rb) noexcept
        : m_numerator(std::move(rb.m_numerator)),
          m_denominator(std::move(rb.m_denominator))
    {}

    RationalBase(const RationalBase& rb)
        : m_numerator(rb.m_numerator),
          m_denominator(rb.m_denominator)
    {}

    RationalBase& operator=(const RationalBase& rb) {
      m_numerator = rb.m_numerator;
      m_denominator = rb.m_denominator;
      return *this;
    }

    RationalBase& operator=(RationalBase&& rb) noexcept {
      std::swap(m_numerator, rb.m_numerator);
      std::swap(m_denominator, rb.m_denominator);
      return *this;
    }

    [[nodiscard]] bool is_nan() const noexcept { return m_denominator == T{0} && m_numerator == T{0}; }
    [[nodiscard]] bool is_infinite() const noexcept { return m_denominator == T{0} && m_numerator != T{0}; }
    [[nodiscard]] bool is_pos_infinity() const noexcept { return m_denominator == T{0} && m_numerator > T{0}; }
    [[nodiscard]] bool is_neg_infinity() const noexcept { return m_denominator == T{0} && m_numerator < T{0}; }
    [[nodiscard]] bool is_finite() const noexcept { return m_denominator != T{0}; }
    [[nodiscard]] std::strong_ordering operator<=>(const RationalBase& o) const noexcept {
      auto n1 = m_numerator;
      auto n2 = o.m_numerator;
      if (m_denominator != o.m_denominator) {
        n1 = m_numerator * o.m_denominator;
        n2 = o.m_numerator * m_denominator;
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
      return mtmath::RationalBase<T>{-m_numerator, m_denominator};
    }

    RationalBase& operator+=(const RationalBase& other) noexcept {
      if (is_finite() && other.is_finite()) {
        if (m_denominator == other.m_denominator) {
          m_numerator = m_numerator + other.m_numerator;
        }
        else {
          auto n1 = m_numerator * other.m_denominator;
          auto n2 = other.m_numerator * m_denominator;
          m_denominator = m_denominator * other.m_denominator;
          m_numerator = n1 + n2;
          simplify();
        }
        return *this;
      }
      else if (is_pos_infinity() || is_neg_infinity()) {
        if (other.is_finite() || *this == other) {
          return *this;
        }
        m_numerator = 0;
        m_denominator = 0;
        return *this;
      }
      else {
        m_numerator = 0;
        m_denominator = 0;
        return *this;
      }
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
      m_numerator *= other.m_numerator;
      m_denominator *= other.m_denominator;
      simplify();
      return *this;
    }

    RationalBase operator*(const RationalBase& other) const noexcept {
      auto copy = *this;
      copy *= other;
      return copy;
    }

    RationalBase& operator/=(const RationalBase& other) noexcept {
      if (!is_finite()) {
        if (other.is_finite()) {
          if (other.m_numerator < 0) {
            m_numerator *= -1;
          }
        }
        else {
          m_numerator = 0;
          m_denominator = 0;
        }
      }
      else if (!other.is_finite()) {
        if (other.is_infinite()) {
          m_numerator = 0;
          m_denominator = 1;
        }
        else {
          m_numerator = 0;
          m_denominator = 0;
        }
      }
      else {
        m_numerator *= other.m_denominator;
        m_denominator *= other.m_numerator;
      }
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
      o << r.m_numerator << "/" << r.m_denominator;
      return o;
    }

    const T& numerator() const noexcept {
        return m_numerator;
    }

    const T& denominator() const noexcept {
      return m_denominator;
    }

  private:
    T m_numerator;
    T m_denominator;

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
      if constexpr (std::numeric_limits<T>::has_quiet_NaN) {
        auto nan = std::numeric_limits<T>::quiet_NaN();
        if (m_numerator == nan || m_denominator == nan) {
          m_numerator = 0;
          m_denominator = 0;
          return;
        }
      }

      if constexpr (std::numeric_limits<T>::has_infinity) {
        auto pos_infinity = std::numeric_limits<T>::infinity();
        auto neg_infinity = -std::numeric_limits<T>::infinity();
        if (m_denominator == pos_infinity || m_denominator == neg_infinity) {
          m_numerator = 0;
          m_denominator = 0;
          return;
        }
        else if (m_numerator == pos_infinity) {
          m_numerator = 1;
          m_denominator = 0;
          return;
        }
        else if (m_numerator == neg_infinity) {
          m_numerator = -1;
          m_denominator = 0;
        }
      }

      if (m_denominator < 0) {
        m_denominator = -m_denominator;
      }
      auto n = m_numerator;
      auto d = m_denominator;
      if (d == 1) {
        return;
      }
      else if (d != 0) {
        if (remainder(n, d) == 0) {
          m_numerator = n / d;
        }
        else {
          auto g = gcd(n, d);
          if (d > 0) {
            m_numerator = n / g;
            m_denominator = d / g;
          }
          else if (d < 0) {
            m_numerator = -n / g;
            m_denominator = -d / g;
          }
        }
      }
    }
  };

  extern template class RationalBase<mtmath::BigInt>;
  using Rational = RationalBase<mtmath::BigInt>;

  namespace c {
    void into(const mtmath::Rational& bi, MtMath_Rational* out);
    void into(const MtMath_Rational& cbi, mtmath::Rational* out);
  }

  namespace immut {
    class Rational {
    public:
      Rational(mtmath::immut::BigInt numerator, mtmath::immut::BigInt denominator = 1) noexcept;

      Rational();

      Rational(Rational&& rb) noexcept;

      Rational(const Rational& rb) = default;

      Rational& operator=(const Rational& rb);

      Rational& operator=(Rational&& rb) noexcept;

      [[nodiscard]] bool is_nan() const noexcept;
      [[nodiscard]] bool is_infinite() const noexcept;
      [[nodiscard]] bool is_pos_infinity() const noexcept;
      [[nodiscard]] bool is_neg_infinity() const noexcept;
      [[nodiscard]] bool is_finite() const noexcept;
      [[nodiscard]] std::strong_ordering operator<=>(const Rational& o) const noexcept;
      [[nodiscard]] bool operator==(const Rational& o) const noexcept;

      Rational operator-() const noexcept;

      Rational operator+(const Rational& other) const noexcept;

      Rational operator-(const Rational& other) const noexcept;

      Rational operator*(const Rational& other) const noexcept;

      Rational operator/(const Rational& other) const noexcept;

      friend std::ostream& operator<<(std::ostream& o, const Rational& r)  {
        o << r.numerator;
        o << "/";
        o << r.denominator;
        return o;
      }

    private:
      mtmath::immut::BigInt numerator;
      mtmath::immut::BigInt denominator;

      static mtmath::immut::BigInt remainder(const mtmath::immut::BigInt& n, const mtmath::immut::BigInt&d);

      static mtmath::immut::BigInt gcd(const mtmath::immut::BigInt& a, const mtmath::immut::BigInt& b);

      void simplify();
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
  static constexpr bool is_exact = !std::numeric_limits<RB>::is_bounded;
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
      return Type{std::numeric_limits<RB>::min(), 1};
    }
    else {
      return -infinity();
    }
  }

  static constexpr Type max() noexcept {
    if constexpr (std::numeric_limits<RB>::is_bounded) {
      return Type{std::numeric_limits<RB>::max()};
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
    return lowest();
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
  using Type = mtmath::RationalBase<mtmath::BigInt>;

#define MTMATH_RATIONAL_DEF_TRAIT(trait) static constexpr decltype(std::numeric_limits<mtmath::BigInt>:: trait) trait = std::numeric_limits<mtmath::BigInt>:: trait;

  static constexpr bool is_specialized = true;
  MTMATH_RATIONAL_DEF_TRAIT(is_signed)
  static constexpr bool is_integer = false;
  MTMATH_RATIONAL_DEF_TRAIT(is_exact)
  static constexpr bool has_infinity = true;
  static constexpr bool has_quiet_NaN = true;
  static constexpr bool has_signaling_NaN = false;
  static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
  static constexpr bool has_denorm_loss = false;
  MTMATH_RATIONAL_DEF_TRAIT(round_style)
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
    return Type{mtmath::BigInt::zero(), mtmath::BigInt::one()};
  }

  static Type epsilon() noexcept {
    return Type{mtmath::BigInt::zero(), mtmath::BigInt::one()};
  }

  static Type round_error() noexcept {
    return Type{mtmath::BigInt::one(), mtmath::BigInt::two()};
  }

  static Type infinity() noexcept {
    return Type{mtmath::BigInt::one(), mtmath::BigInt::zero()};
  }

  static Type quiet_NaN() noexcept {
    return Type{mtmath::BigInt::zero(), mtmath::BigInt::zero()};
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

#define MTMATH_IMMUT_RATIONAL_DEF_TRAIT(trait) static constexpr decltype(std::numeric_limits<mtmath::immut::BigInt>:: trait) trait = std::numeric_limits<mtmath::immut::BigInt>:: trait;

  static constexpr bool is_specialized = true;
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(is_signed)
  static constexpr bool is_integer = false;
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(is_exact)
  static constexpr bool has_infinity = true;
  static constexpr bool has_quiet_NaN = true;
  static constexpr bool has_signaling_NaN = false;
  static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
  static constexpr bool has_denorm_loss = false;
  MTMATH_IMMUT_RATIONAL_DEF_TRAIT(round_style)
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
