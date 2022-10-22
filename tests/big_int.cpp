#include "impl/big_int.h"
#include "doctest.h"

TEST_SUITE("BigInt") {
  TEST_CASE("To String") {
    SUBCASE("1")
    {
      auto bi = mtmath::BigInt(1485209);
      CHECK_EQ(bi.to_string(), "1485209");
    }

    SUBCASE("2")
    {
      auto bi = mtmath::BigInt("8524631");
      CHECK_EQ(bi.to_string(), "8524631");
    }

    SUBCASE("3")
    {
      auto bi = mtmath::BigInt(std::string("532634"));
      CHECK_EQ(bi.to_string(), "532634");
    }

    SUBCASE("4")
    {
      auto bi = mtmath::BigInt(std::string_view("768462"));
      CHECK_EQ(bi.to_string(), "768462");
    }

    SUBCASE("5")
    {
      auto bi = mtmath::BigInt(std::string_view("-768462"));
      CHECK_EQ(bi.to_string(), "-768462");
    }

    SUBCASE("6")
    {
      auto bi = mtmath::BigInt("-768462");
      CHECK_EQ(bi.to_string(), "-768462");
    }

    SUBCASE("7")
    {
      auto bi = mtmath::BigInt("+768462");
      CHECK_EQ(bi.to_string(), "768462");
    }

    SUBCASE("8")
    {
      auto bi = mtmath::BigInt("000023");
      CHECK_EQ(bi.to_string(), "23");
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
    CHECK_EQ(s, "550320");
    CHECK_EQ(mtmath::BigInt(1485209) - mtmath::BigInt("934889"), mtmath::BigInt("550320"));
    CHECK_EQ(mtmath::BigInt(1485209) - mtmath::BigInt("-934889"), mtmath::BigInt("2420098"));
    CHECK_EQ(mtmath::BigInt(-1485209) - mtmath::BigInt("934889"), mtmath::BigInt("-2420098"));
  }
}
