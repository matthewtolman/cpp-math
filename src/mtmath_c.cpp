#include "mtmath_c.h"
#include "impl/big_int.h"
#include <string>

int foo() {
  return 43;
}

void set_big_int_to_str_safe(const char *str, unsigned long long strlen, MtMath_BigInt *out) {
  mtmath::c::into(mtmath::BigInt(std::string{str, static_cast<size_t>(strlen)}), out);
}

void set_big_int_to_str(const char *str, MtMath_BigInt *out) {
  mtmath::c::into(mtmath::BigInt(std::string{str}), out);
}

void set_big_int_to_int(int val, MtMath_BigInt *out) {
  mtmath::c::into(mtmath::BigInt(val), out);
}

void set_big_int_to_long(long val, MtMath_BigInt* out) {
  mtmath::c::into(mtmath::BigInt(val), out);
}

void set_big_int_to_long_long(long long val, MtMath_BigInt* out) {
  mtmath::c::into(mtmath::BigInt(val), out);
}

void set_big_int_to_uint(unsigned int val, MtMath_BigInt* out) {
  mtmath::c::into(mtmath::BigInt(val), out);
}

void set_big_int_to_ulong(unsigned long val, MtMath_BigInt* out) {
  if (!out) {
    return;
  }

  mtmath::c::into(mtmath::BigInt(val), out);
}

void set_big_int_to_ulong_long(unsigned long long val, MtMath_BigInt* out) {
  if (!out) {
    return;
  }

  mtmath::c::into(mtmath::BigInt(val), out);
}

void init_byte_array(MtMath_ByteArray *ba) {
  ba->len = 0;
  ba->bytes = nullptr;
}

void init_big_int(MtMath_BigInt* bi) {
  init_byte_array(&bi->digits);
  bi->flags = 0;
}

void init_rational(MtMath_Rational *ra) {
  init_big_int(&ra->numerator);
  init_big_int(&ra->denominator);
}

void add_big_int(const MtMath_BigInt *left, const MtMath_BigInt *right, MtMath_BigInt *out) {
  if (!out) {
    return;
  }

  mtmath::BigInt tmpLeft;
  mtmath::BigInt tmpRight;
  mtmath::c::into(*left, &tmpLeft);
  mtmath::c::into(*right, &tmpRight);
  tmpLeft += tmpRight;
  mtmath::c::into(tmpLeft, out);
}

void sub_big_int(const MtMath_BigInt *left, const MtMath_BigInt *right, MtMath_BigInt *out) {
  if (!out) {
    return;
  }

  mtmath::BigInt tmpLeft;
  mtmath::BigInt tmpRight;
  mtmath::c::into(*left, &tmpLeft);
  mtmath::c::into(*right, &tmpRight);
  tmpLeft -= tmpRight;
  mtmath::c::into(tmpLeft, out);
}

void mul_big_int(const MtMath_BigInt *left, const MtMath_BigInt *right, MtMath_BigInt *out) {
  if (!out) {
    return;
  }

  mtmath::BigInt tmpLeft;
  mtmath::BigInt tmpRight;
  mtmath::c::into(*left, &tmpLeft);
  mtmath::c::into(*right, &tmpRight);
  tmpLeft *= tmpRight;
  mtmath::c::into(tmpLeft, out);
}

void div_big_int(const MtMath_BigInt *left, const MtMath_BigInt *right, MtMath_BigInt *out) {
  div_rem_big_int(left, right, out, nullptr);
}

void div_rem_big_int(const MtMath_BigInt *left, const MtMath_BigInt *right, MtMath_BigInt *quotient, MtMath_BigInt *remainder) {
  mtmath::BigInt tmpLeft;
  mtmath::BigInt tmpRight;
  mtmath::c::into(*left, &tmpLeft);
  mtmath::c::into(*right, &tmpRight);
  auto [r, q] = tmpLeft.divide(tmpRight);
  if (quotient) {
    mtmath::c::into(q, quotient);
  }
  if (remainder) {
    mtmath::c::into(r, remainder);
  }
}

void rem_big_int(const MtMath_BigInt *left, const MtMath_BigInt *right, MtMath_BigInt *out) {
  div_rem_big_int(left, right, nullptr, out);
}


#include "impl/rational.h"

void set_rational(MtMath_Rational* ra, const MtMath_BigInt* numerator, const MtMath_BigInt* denominator) {
  mtmath::BigInt biNum{};
  mtmath::BigInt biDenom{};
  mtmath::c::into(*numerator, &biNum);
  mtmath::c::into(*denominator, &biDenom);
  mtmath::c::into(mtmath::Rational{biNum, biDenom}, ra);
}

void add_rational(const MtMath_Rational *left, const MtMath_Rational *right, MtMath_Rational *out) {
  mtmath::Rational rLeft;
  mtmath::Rational rRight;
  mtmath::c::into(*left, &rLeft);
  mtmath::c::into(*right, &rRight);
  rLeft += rRight;
  mtmath::c::into(rLeft, out);
}

void sub_rational(const MtMath_Rational *left, const MtMath_Rational *right, MtMath_Rational *out) {
  mtmath::Rational rLeft;
  mtmath::Rational rRight;
  mtmath::c::into(*left, &rLeft);
  mtmath::c::into(*right, &rRight);
  rLeft -= rRight;
  mtmath::c::into(rLeft, out);
}

void mul_rational(const MtMath_Rational *left, const MtMath_Rational *right, MtMath_Rational *out) {
  mtmath::Rational rLeft;
  mtmath::Rational rRight;
  mtmath::c::into(*left, &rLeft);
  mtmath::c::into(*right, &rRight);
  rLeft *= rRight;
  mtmath::c::into(rLeft, out);
}

void div_rational(const MtMath_Rational *left, const MtMath_Rational *right, MtMath_Rational *out) {
  mtmath::Rational rLeft;
  mtmath::Rational rRight;
  mtmath::c::into(*left, &rLeft);
  mtmath::c::into(*right, &rRight);
  rLeft /= rRight;
  mtmath::c::into(rLeft, out);
}

long long big_int_ll(const MtMath_BigInt *val) {
  mtmath::BigInt bi;
  mtmath::c::into(*val, &bi);
  return bi.as_i64();
}

void big_int_str_alloc(const MtMath_BigInt *val, char **out) {
  mtmath::BigInt bi;
  mtmath::c::into(*val, &bi);
  auto str = *bi.to_string(10);
  *out = static_cast<char *>(malloc(sizeof(char) * (str.size() + 1)));
  memcpy(*out, str.c_str(), str.size() + 1);
}

void big_int_str(const MtMath_BigInt *val, char *buffer, unsigned long long bufferSize) {
  if (!buffer || !bufferSize) {
    return;
  }
  mtmath::BigInt bi;
  mtmath::c::into(*val, &bi);
  auto str = *bi.to_string(10);
  auto resSize = std::min(static_cast<unsigned long long>(str.size() + 1), bufferSize);
  memcpy(buffer, str.c_str(), resSize);
  buffer[resSize - 1] = '\0';
}

bool big_int_str_alloc_rdx(const MtMath_BigInt *val, char **out, int base) {
  mtmath::BigInt bi;
  mtmath::c::into(*val, &bi);
  auto strOpt = bi.to_string(base);
  if (!strOpt.has_value()) {
    return false;
  }
  auto str = *strOpt;
  *out = static_cast<char *>(malloc(sizeof(char) * (str.size() + 1)));
  memcpy(*out, str.c_str(), str.size() + 1);
  return true;
}

bool big_int_str_rdx(const MtMath_BigInt *val, char *buffer, unsigned long long bufferSize, int radix) {
  if (!buffer || !bufferSize) {
    return false;
  }
  mtmath::BigInt bi;
  mtmath::c::into(*val, &bi);
  auto strOpt = bi.to_string(radix);
  if (!strOpt.has_value()) {
    return false;
  }

  auto str = *strOpt;
  auto resSize = std::min(static_cast<unsigned long long>(str.size() + 1), bufferSize);
  memcpy(buffer, str.c_str(), resSize);
  buffer[resSize - 1] = '\0';
  return true;
}
