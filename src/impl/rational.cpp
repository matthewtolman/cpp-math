#include "rational.h"
#include "big_int.h"
#include <compare>

template class mtmath::RationalBase<mtmath::BigInt>;

mtmath::immut::Rational::Rational(mtmath::immut::BigInt numerator, mtmath::immut::BigInt denominator) noexcept
  : numerator(std::move(numerator)), denominator(std::move(denominator))
{ simplify(); }

mtmath::immut::Rational::Rational() : numerator(mtmath::immut::BigInt::zero()), denominator(mtmath::immut::BigInt::one()) {}

mtmath::immut::Rational::Rational(mtmath::immut::Rational &&rb) noexcept : numerator(std::move(rb.numerator)), denominator(std::move(rb.denominator)) {}

mtmath::immut::Rational &mtmath::immut::Rational::operator=(const mtmath::immut::Rational &rb) {
  numerator = rb.numerator;
  denominator = rb.denominator;
  return *this;
}

mtmath::immut::Rational &mtmath::immut::Rational::operator=(mtmath::immut::Rational &&rb) noexcept {
  std::swap(numerator, rb.numerator);
  std::swap(denominator, rb.denominator);
  return *this;
}

bool mtmath::immut::Rational::is_nan() const noexcept { return denominator.is_zero() && numerator.is_zero(); }

bool mtmath::immut::Rational::is_infinite() const noexcept { return denominator.is_zero() && !numerator.is_zero(); }

bool mtmath::immut::Rational::is_pos_infinity() const noexcept { return denominator.is_zero() && !numerator.is_zero() && !numerator.is_negative(); }

bool mtmath::immut::Rational::is_neg_infinity() const noexcept { return denominator.is_zero() && !numerator.is_zero() && numerator.is_negative(); }

bool mtmath::immut::Rational::is_finite() const noexcept { return !denominator.is_zero(); }

std::strong_ordering mtmath::immut::Rational::operator<=>(const mtmath::immut::Rational &o) const noexcept {
  auto n1 = numerator;
  auto n2 = o.numerator;
  if (denominator != o.denominator) {
    n1 = numerator * o.denominator;
    n2 = o.numerator * denominator;
  }

  return n1 <=> n2;
}

bool mtmath::immut::Rational::operator==(const mtmath::immut::Rational &o) const noexcept {
  return ((*this) <=> o) == std::strong_ordering::equal;
}

mtmath::immut::Rational mtmath::immut::Rational::operator-() const noexcept {
  if (numerator.is_zero()) {
    return *this;
  }
  return Rational{-numerator, denominator};
}

mtmath::immut::Rational mtmath::immut::Rational::operator+(const mtmath::immut::Rational &other) const noexcept {
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
  else if (is_pos_infinity() || is_neg_infinity()) {
    if (other.is_finite() || *this == other) {
      return *this;
    }
    return std::numeric_limits<Rational>::quiet_NaN();
  }
  else {
    return std::numeric_limits<Rational>::quiet_NaN();
  }
}

mtmath::immut::Rational mtmath::immut::Rational::operator-(const mtmath::immut::Rational &other) const noexcept {
  return *this + -other;
}

mtmath::immut::Rational mtmath::immut::Rational::operator*(const mtmath::immut::Rational &other) const noexcept {
  auto res = Rational{numerator * other.numerator, denominator * other.denominator};
  res.simplify();
  return res;
}

mtmath::immut::Rational mtmath::immut::Rational::operator/(const mtmath::immut::Rational &other) const noexcept {
  if (!is_finite()) {
    if (other.is_finite()) {
      if (other.numerator < 0) {
        return Rational{numerator * -1, denominator};
      }
      else {
        return *this;
      }
    }
    else {
      return Rational{0, 0};
    }
  }
  else if (!other.is_finite()) {
    if (other.is_infinite()) {
      return Rational{0};
    }
    else {
      return Rational{0, 0};
    }
  }
  else {
    auto res = Rational{numerator * other.denominator, denominator * other.numerator};
    res.simplify();
    return res;
  }
}

mtmath::immut::BigInt mtmath::immut::Rational::remainder(const mtmath::immut::BigInt &n, const mtmath::immut::BigInt &d) {
  if (n < mtmath::immut::BigInt::zero()) {
    return (-n) % d;
  }
  return n % d;
}

mtmath::immut::BigInt mtmath::immut::Rational::gcd(const mtmath::immut::BigInt &a, const mtmath::immut::BigInt &b) {
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

void mtmath::immut::Rational::simplify() {
  if constexpr (std::numeric_limits<mtmath::immut::BigInt>::has_quiet_NaN) {
    auto nan = std::numeric_limits<mtmath::immut::BigInt>::quiet_NaN();
    if (numerator == nan || denominator == nan) {
      numerator = 0;
      denominator = 0;
      return;
    }
  }

  if constexpr (std::numeric_limits<mtmath::immut::BigInt>::has_infinity) {
    auto pos_infinity = std::numeric_limits<mtmath::immut::BigInt>::infinity();
    auto neg_infinity = -std::numeric_limits<mtmath::immut::BigInt>::infinity();
    if (denominator == pos_infinity || denominator == neg_infinity) {
      numerator = 0;
      denominator = 0;
      return;
    }
    else if (numerator == pos_infinity) {
      numerator = 1;
      denominator = 0;
      return;
    }
    else if (numerator == neg_infinity) {
      numerator = -1;
      denominator = 0;
    }
  }

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

void mtmath::c::into(const mtmath::Rational &bi, MtMath_Rational *out) {
  auto num = bi.numerator();
  auto den = bi.denominator();
  into(num, &out->numerator);
  into(den, &out->denominator);
}

void mtmath::c::into(const MtMath_Rational &cbi, mtmath::Rational *out) {
  mtmath::BigInt num;
  mtmath::BigInt den;
  into(cbi.numerator, &num);
  into(cbi.denominator, &den);
  *out = Rational(num, den);
}
