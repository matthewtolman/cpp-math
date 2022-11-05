#ifndef MATH_MTMATH_C_H
#define MATH_MTMATH_C_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MtMath_ByteArray {
  unsigned long long len;
  unsigned char* bytes;
} MtMath_ByteArray;

typedef struct MtMath_BigInt {
  unsigned char flags;
  MtMath_ByteArray digits;
} MtMath_BigInt;

struct MtMath_Rational {
  MtMath_BigInt numerator;
  MtMath_BigInt denominator;
};

extern void init_byte_array(MtMath_ByteArray* ba);
extern void init_big_int(MtMath_BigInt* bi);
extern void init_rational(MtMath_Rational* ra);

extern void set_big_int_to_str_safe(const char* str, unsigned long long strlen, MtMath_BigInt* out);
extern void set_big_int_to_str(const char* str, MtMath_BigInt* out);
extern void set_big_int_to_int(int val, MtMath_BigInt* out);
extern void set_big_int_to_long(long val, MtMath_BigInt* out);
extern void set_big_int_to_long_long(long long val, MtMath_BigInt* out);
extern void set_big_int_to_uint(unsigned int val, MtMath_BigInt* out);
extern void set_big_int_to_ulong(unsigned long val, MtMath_BigInt* out);
extern void set_big_int_to_ulong_long(unsigned long long val, MtMath_BigInt* out);

extern void add_big_int(const MtMath_BigInt* left, const MtMath_BigInt* right, MtMath_BigInt* out);
extern void sub_big_int(const MtMath_BigInt* left, const MtMath_BigInt* right, MtMath_BigInt* out);
extern void mul_big_int(const MtMath_BigInt* left, const MtMath_BigInt* right, MtMath_BigInt* out);
extern void div_big_int(const MtMath_BigInt* left, const MtMath_BigInt* right, MtMath_BigInt* out);
extern void div_rem_big_int(const MtMath_BigInt* left, const MtMath_BigInt* right, MtMath_BigInt* quotient, MtMath_BigInt* remainder);
extern void rem_big_int(const MtMath_BigInt* left, const MtMath_BigInt* right, MtMath_BigInt* out);

extern long long big_int_ll(const MtMath_BigInt* val);
extern void big_int_str_alloc(const MtMath_BigInt* val, char** out);
extern void big_int_str(const MtMath_BigInt* val, char* buffer, unsigned long bufferSize);
extern bool big_int_str_alloc_rdx(const MtMath_BigInt* val, char** out, int radix);
extern bool big_int_str_rdx(const MtMath_BigInt* val, char* buffer, unsigned long bufferSize, int radix);

extern void set_rational(MtMath_Rational* ra, const MtMath_BigInt* numerator, const MtMath_BigInt* denominator);
extern void add_rational(const MtMath_Rational* left, const MtMath_Rational* right, MtMath_Rational* out);
extern void sub_rational(const MtMath_Rational* left, const MtMath_Rational* right, MtMath_Rational* out);
extern void mul_rational(const MtMath_Rational* left, const MtMath_Rational* right, MtMath_Rational* out);
extern void div_rational(const MtMath_Rational* left, const MtMath_Rational* right, MtMath_Rational* out);

extern int foo();

#ifdef __cplusplus
};
#endif

#endif