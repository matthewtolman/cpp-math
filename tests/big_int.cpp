#include "impl/big_int.h"
#include "doctest.h"

TEST_SUITE("BigInt") {
  TEST_CASE("To String") {
    SUBCASE("1")
    {
      auto bi = mtmath::BigInt(1485209);
      CHECK_EQ(bi.to_string(16), "0x16a999");
    }

    SUBCASE("2")
    {
      auto bi = mtmath::BigInt("8524631");
      CHECK_EQ(bi.to_string(16), "0x821357");
    }

    SUBCASE("3")
    {
      auto bi = mtmath::BigInt(std::string("532634"));
      CHECK_EQ(bi.to_string(16), "0x8209a");
    }

    SUBCASE("4")
    {
      auto bi = mtmath::BigInt(std::string_view("768462"));
      CHECK_EQ(bi.to_string(16), "0xbb9ce");
    }

    SUBCASE("5")
    {
      auto bi = mtmath::BigInt(std::string_view("-768462"), 10);
      CHECK_EQ(bi.to_string(16), "-0xbb9ce");
    }

    SUBCASE("6")
    {
      auto bi = mtmath::BigInt("-768462");
      CHECK_EQ(bi.to_string(16), "-0xbb9ce");
    }

    SUBCASE("7")
    {
      auto bi = mtmath::BigInt("+768462");
      CHECK_EQ(bi.to_string(16), "0xbb9ce");
    }

    SUBCASE("8")
    {
      auto bi = mtmath::BigInt("000023");
      CHECK_EQ(bi.to_string(16), "0x17");
    }

    SUBCASE("from hex")
    {
      auto bi = mtmath::BigInt("16a999", 16);
      CHECK_EQ(bi.to_string(16), "0x16a999");
    }

    SUBCASE("from octal")
    {
      auto bi = mtmath::BigInt("5524631", 8);
      CHECK_EQ(bi.to_string(16), "0x16a999");
    }

    SUBCASE("from base 3")
    {
      auto bi = mtmath::BigInt("2210110022202", 3);
      CHECK_EQ(bi.to_string(16), "0x16a999");
    }

    SUBCASE("from base 32")
    {
      auto bi = mtmath::BigInt("1dacp", 32);
      CHECK_EQ(bi.to_string(16), "0x16a999");
    }

    SUBCASE("from base 36")
    {
      auto bi = mtmath::BigInt("vtzt", 36);
      CHECK_EQ(bi.to_string(16), "0x16a999");
    }

    SUBCASE("from base 2")
    {
      auto bi = mtmath::BigInt("101101010100110011001", 2);
      CHECK_EQ(bi.to_string(16), "0x16a999");
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
    std::string s = (mtmath::BigInt(1485209) - mtmath::BigInt("934889")).to_string(16);
    CHECK_EQ(s, "0x865b0");
    CHECK_EQ(mtmath::BigInt(1485209) - mtmath::BigInt("934889"), mtmath::BigInt("550320"));
    CHECK_EQ(mtmath::BigInt(1485209) - mtmath::BigInt("-934889"), mtmath::BigInt("2420098"));
    CHECK_EQ(mtmath::BigInt(-1485209) - mtmath::BigInt("934889"), mtmath::BigInt("-2420098"));
  }

  TEST_CASE("Compare") {
    CHECK_EQ(mtmath::BigInt(1485209).compare(mtmath::BigInt(1485209)), 0);
    CHECK_EQ(mtmath::BigInt(1485208).compare(mtmath::BigInt(1485209)), -1);
    CHECK_EQ(mtmath::BigInt(1485209).compare(mtmath::BigInt(1485208)), 1);
    CHECK_EQ(mtmath::BigInt(1485209).compare(mtmath::BigInt(2485209)), -1);
    CHECK_EQ(mtmath::BigInt(2485209).compare(mtmath::BigInt(1485209)), 1);
    CHECK_EQ(mtmath::BigInt(1485209).compare(mtmath::BigInt(948509)), 1);
    CHECK_EQ(mtmath::BigInt(948509).compare(mtmath::BigInt(1485209)), -1);
    CHECK_EQ(mtmath::BigInt(1484209).compare(mtmath::BigInt(1485208)), -1);
    CHECK_EQ(mtmath::BigInt(1485208).compare(mtmath::BigInt(1484209)), 1);
  }


  TEST_CASE("Compare operators") {
    CHECK(mtmath::BigInt(1485209) == mtmath::BigInt(1485209));
    CHECK(mtmath::BigInt(1485209) <= mtmath::BigInt(1485209));
    CHECK(mtmath::BigInt(1485209) >= mtmath::BigInt(1485209));
    CHECK_FALSE(mtmath::BigInt(1485209) == mtmath::BigInt(1485309));
    CHECK_FALSE(mtmath::BigInt(1485209) != mtmath::BigInt(1485209));
    CHECK(mtmath::BigInt(1485209) != mtmath::BigInt(1485309));

    CHECK(mtmath::BigInt(1485208) < mtmath::BigInt(1485209));
    CHECK(mtmath::BigInt(1485209) < mtmath::BigInt(2485209));
    CHECK(mtmath::BigInt(948509) < mtmath::BigInt(1485209));
    CHECK(mtmath::BigInt(1484209) < mtmath::BigInt(1485208));
    CHECK(mtmath::BigInt(1485208) <= mtmath::BigInt(1485209));
    CHECK(mtmath::BigInt(1485209) <= mtmath::BigInt(2485209));
    CHECK(mtmath::BigInt(948509) <= mtmath::BigInt(1485209));
    CHECK(mtmath::BigInt(1484209) <= mtmath::BigInt(1485208));
    CHECK_FALSE(mtmath::BigInt(1485208) > mtmath::BigInt(1485209));
    CHECK_FALSE(mtmath::BigInt(1485209) > mtmath::BigInt(2485209));
    CHECK_FALSE(mtmath::BigInt(948509) > mtmath::BigInt(1485209));
    CHECK_FALSE(mtmath::BigInt(1484209) > mtmath::BigInt(1485208));
    CHECK_FALSE(mtmath::BigInt(1485208) >= mtmath::BigInt(1485209));
    CHECK_FALSE(mtmath::BigInt(1485209) >= mtmath::BigInt(2485209));
    CHECK_FALSE(mtmath::BigInt(948509) >= mtmath::BigInt(1485209));
    CHECK_FALSE(mtmath::BigInt(1484209) >= mtmath::BigInt(1485208));

    CHECK(mtmath::BigInt(1485209) > mtmath::BigInt(1485208));
    CHECK(mtmath::BigInt(2485209) > mtmath::BigInt(1485209));
    CHECK(mtmath::BigInt(1485209) > mtmath::BigInt(948509));
    CHECK(mtmath::BigInt(1485208) > mtmath::BigInt(1484209));
    CHECK(mtmath::BigInt(1485209) >= mtmath::BigInt(1485208));
    CHECK(mtmath::BigInt(2485209) >= mtmath::BigInt(1485209));
    CHECK(mtmath::BigInt(1485209) >= mtmath::BigInt(948509));
    CHECK(mtmath::BigInt(1485208) >= mtmath::BigInt(1484209));
    CHECK_FALSE(mtmath::BigInt(1485209) < mtmath::BigInt(1485208));
    CHECK_FALSE(mtmath::BigInt(2485209) < mtmath::BigInt(1485209));
    CHECK_FALSE(mtmath::BigInt(1485209) < mtmath::BigInt(948509));
    CHECK_FALSE(mtmath::BigInt(1485208) < mtmath::BigInt(1484209));
    CHECK_FALSE(mtmath::BigInt(1485209) <= mtmath::BigInt(1485208));
    CHECK_FALSE(mtmath::BigInt(2485209) <= mtmath::BigInt(1485209));
    CHECK_FALSE(mtmath::BigInt(1485209) <= mtmath::BigInt(948509));
    CHECK_FALSE(mtmath::BigInt(1485208) <= mtmath::BigInt(1484209));
  }

  TEST_CASE("Multiply") {
    CHECK_EQ((mtmath::BigInt(1485209) * mtmath::BigInt("2")).to_string(16), "0x2d5332");
    CHECK_EQ((mtmath::BigInt(1485209) * mtmath::BigInt("-2")).to_string(16), "-0x2d5332");
    CHECK_EQ((mtmath::BigInt(-1485209) * mtmath::BigInt("2")).to_string(16), "-0x2d5332");
    CHECK_EQ((mtmath::BigInt(-1485209) * mtmath::BigInt("-2")).to_string(16), "0x2d5332");
  }

//  TEST_CASE("Divide") {
//    std::string s = (mtmath::BigInt(1485209) / mtmath::BigInt("2")).to_string(16);
//    CHECK_EQ(s, "0xb54cc");
//    CHECK_EQ(mtmath::BigInt(1485209) - mtmath::BigInt("934889"), mtmath::BigInt("550320"));
//    CHECK_EQ(mtmath::BigInt(1485209) - mtmath::BigInt("-934889"), mtmath::BigInt("2420098"));
//    CHECK_EQ(mtmath::BigInt(-1485209) - mtmath::BigInt("934889"), mtmath::BigInt("-2420098"));
//  }
}
