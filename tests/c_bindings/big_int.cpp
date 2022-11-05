#include "../doctest.h"

#include "mtmath_c.h"

TEST_SUITE("C Bindings - Big Int") {
  TEST_CASE("Can initialize") {
    MtMath_BigInt bi;
    init_big_int(&bi);
    REQUIRE_EQ(bi.flags, 0);
    REQUIRE_EQ(bi.digits.len, 0);
    REQUIRE_EQ(bi.digits.bytes, nullptr);
  }

  TEST_CASE("Can Add") {
    MtMath_BigInt left;
    init_big_int(&left);

    MtMath_BigInt right;
    init_big_int(&right);

    set_big_int_to_int(25, &left);
    set_big_int_to_int(32, &right);

    MtMath_BigInt out;
    init_big_int(&out);

    add_big_int(&left, &right, &out);

    CHECK_EQ(big_int_ll(&left), 25);
    CHECK_EQ(big_int_ll(&right), 32);
    CHECK_EQ(big_int_ll(&out), 57);
  }

  TEST_CASE("Can Subtract") {
    MtMath_BigInt left;
    init_big_int(&left);

    MtMath_BigInt right;
    init_big_int(&right);

    set_big_int_to_int(25, &left);
    set_big_int_to_int(32, &right);

    char buffer[128];

    MtMath_BigInt out;
    init_big_int(&out);

    sub_big_int(&left, &right, &out);

    big_int_str(&out, buffer, 128);
    CHECK_EQ(strcmp(buffer, "-7"), 0);

    big_int_str(&left, buffer, 128);
    CHECK_EQ(strcmp(buffer, "25"), 0);

    big_int_str(&right, buffer, 128);
    CHECK_EQ(strcmp(buffer, "32"), 0);
  }

  TEST_CASE("Can Multiply") {
    MtMath_BigInt left;
    init_big_int(&left);

    MtMath_BigInt right;
    init_big_int(&right);

    set_big_int_to_int(25, &left);
    set_big_int_to_int(32, &right);

    char* outBuff;

    MtMath_BigInt out;
    init_big_int(&out);

    mul_big_int(&left, &right, &out);

    big_int_str_alloc(&out, &outBuff);
    CHECK_EQ(strcmp(outBuff, "800"), 0);
    free(outBuff);

    big_int_str_alloc_rdx(&left, &outBuff, 10);
    CHECK_EQ(strcmp(outBuff, "25"), 0);
    free(outBuff);

    big_int_str_alloc(&right, &outBuff);
    CHECK_EQ(strcmp(outBuff, "32"), 0);
    free(outBuff);
  }

  TEST_CASE("Can Divide") {
    MtMath_BigInt left;
    init_big_int(&left);

    MtMath_BigInt right;
    init_big_int(&right);

    set_big_int_to_int(802, &left);
    set_big_int_to_int(32, &right);

    char buffer[128];

    MtMath_BigInt quotient;
    init_big_int(&quotient);

    MtMath_BigInt remainder;
    init_big_int(&remainder);

    div_rem_big_int(&left, &right, &quotient, &remainder);

    big_int_str_rdx(&quotient, buffer, 128, 10);
    CHECK_EQ(strcmp(buffer, "25"), 0);
    CHECK_EQ(big_int_ll(&remainder), 2);
    CHECK_EQ(big_int_ll(&left), 802);
    CHECK_EQ(big_int_ll(&right), 32);
  }

  TEST_CASE("From String Safe") {
    auto str = "12345";

    MtMath_BigInt bi;
    init_big_int(&bi);

    set_big_int_to_str_safe(str, 5, &bi);
    REQUIRE_EQ(big_int_ll(&bi), 12345);

    set_big_int_to_str_safe(str, 3, &bi);
    REQUIRE_EQ(big_int_ll(&bi), 123);
  }

  TEST_CASE("From String") {
    auto str = "123456";

    MtMath_BigInt bi;
    init_big_int(&bi);

    set_big_int_to_str(str, &bi);
    REQUIRE_EQ(big_int_ll(&bi), 123456);
  }

  TEST_CASE("From Long") {
    MtMath_BigInt bi;
    init_big_int(&bi);

    set_big_int_to_long(43, &bi);
    REQUIRE_EQ(big_int_ll(&bi), 43);
  }

  TEST_CASE("From int") {
    MtMath_BigInt bi;
    init_big_int(&bi);

    set_big_int_to_int(43, &bi);
    REQUIRE_EQ(big_int_ll(&bi), 43);
  }

  TEST_CASE("From Long Long") {
    MtMath_BigInt bi;
    init_big_int(&bi);

    set_big_int_to_long_long(43, &bi);
    REQUIRE_EQ(big_int_ll(&bi), 43);
  }

  TEST_CASE("From Unsigned Int") {
    MtMath_BigInt bi;
    init_big_int(&bi);

    set_big_int_to_uint(43, &bi);
    REQUIRE_EQ(big_int_ll(&bi), 43);
  }

  TEST_CASE("From Unsigned Long") {
    MtMath_BigInt bi;
    init_big_int(&bi);

    set_big_int_to_ulong(43, &bi);
    REQUIRE_EQ(big_int_ll(&bi), 43);
  }

  TEST_CASE("From Unsigned Long Long") {
    MtMath_BigInt bi;
    init_big_int(&bi);

    set_big_int_to_ulong_long(43, &bi);
    REQUIRE_EQ(big_int_ll(&bi), 43);
  }
}
