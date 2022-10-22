#include "impl/big_int.h"
#include "doctest.h"

TEST_SUITE("BigInt") {


  TEST_CASE("8524631")
  {
    auto bi = mtmath::BigInt("8524631");
    CHECK_EQ(bi.to_string(), "0x821357");
  }

  TEST_CASE("To String") {
    SUBCASE("1")
    {
      auto bi = mtmath::BigInt(1485209);
      CHECK_EQ(bi.to_string(), "0x16a999");
    }

    SUBCASE("2")
    {
      auto bi = mtmath::BigInt("8524631");
      CHECK_EQ(bi.to_string(), "0x821357");
    }

    SUBCASE("3")
    {
      auto bi = mtmath::BigInt(std::string("532634"));
      CHECK_EQ(bi.to_string(), "0x8209a");
    }

    SUBCASE("4")
    {
      auto bi = mtmath::BigInt(std::string_view("768462"));
      CHECK_EQ(bi.to_string(), "0xbb9ce");
    }

    SUBCASE("5")
    {
      auto bi = mtmath::BigInt(std::string_view("-768462"));
      CHECK_EQ(bi.to_string(), "-0xbb9ce");
    }

    SUBCASE("6")
    {
      auto bi = mtmath::BigInt("-768462");
      CHECK_EQ(bi.to_string(), "-0xbb9ce");
    }

    SUBCASE("7")
    {
      auto bi = mtmath::BigInt("+768462");
      CHECK_EQ(bi.to_string(), "0xbb9ce");
    }

    SUBCASE("8")
    {
      auto bi = mtmath::BigInt("000023");
      CHECK_EQ(bi.to_string(), "0x17");
    }
  }

  TEST_CASE("Equality") {
    CHECK_EQ(mtmath::BigInt(1485209), mtmath::BigInt("1485209"));
    CHECK_NE(mtmath::BigInt(1485206), mtmath::BigInt("1485209"));
    CHECK_NE(mtmath::BigInt(-1485209), mtmath::BigInt("1485209"));
  }

  TEST_CASE("Negate") {
    CHECK_EQ((-mtmath::BigInt(1485209)), mtmath::BigInt("-1485209"));
  }

  TEST_CASE("Add") {
    CHECK_EQ(mtmath::BigInt(1485209) + mtmath::BigInt("934889"), mtmath::BigInt("2420098"));
    CHECK_EQ(mtmath::BigInt(1485209) + mtmath::BigInt("-934889"), mtmath::BigInt("550320"));
    CHECK_EQ(mtmath::BigInt(-1485209) + mtmath::BigInt("934889"), mtmath::BigInt("-550320"));
  }

  TEST_CASE("Subtract") {
    std::string s = (mtmath::BigInt(1485209) - mtmath::BigInt("934889")).to_string();
    CHECK_EQ(s, "0x865b0");
    CHECK_EQ(mtmath::BigInt(1485209) - mtmath::BigInt("934889"), mtmath::BigInt("550320"));
    CHECK_EQ(mtmath::BigInt(1485209) - mtmath::BigInt("-934889"), mtmath::BigInt("2420098"));
    CHECK_EQ(mtmath::BigInt(-1485209) - mtmath::BigInt("934889"), mtmath::BigInt("-2420098"));
  }
}
