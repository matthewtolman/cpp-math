#include "impl/big_int.h"
#include "doctest.h"

TEST_SUITE("BigInt") {
  TEST_CASE("To String") {
    SUBCASE("1")
    {
      auto bi = mtmath::BigInt(1485209);
      CHECK_EQ(bi.to_string(16), "0x16a999");
      CHECK_EQ(bi.to_string(10), "1485209");
    }

    SUBCASE("2")
    {
      auto bi = mtmath::BigInt("8524631");
      CHECK_EQ(bi.to_string(16), "0x821357");
      CHECK_EQ(bi.to_string(10), "8524631");
    }

    SUBCASE("3")
    {
      auto bi = mtmath::BigInt(std::string("532634"));
      CHECK_EQ(bi.to_string(16), "0x8209a");
      CHECK_EQ(bi.to_string(10), "532634");
    }

    SUBCASE("4")
    {
      auto bi = mtmath::BigInt(std::string_view("768462"));
      CHECK_EQ(bi.to_string(16), "0xbb9ce");
      CHECK_EQ(bi.to_string(10), "768462");
    }

    SUBCASE("5")
    {
      auto bi = mtmath::BigInt(std::string_view("-768462"), 10);
      CHECK_EQ(bi.to_string(16), "-0xbb9ce");
      CHECK_EQ(bi.to_string(10), "-768462");
    }

    SUBCASE("6")
    {
      auto bi = mtmath::BigInt("-768462");
      CHECK_EQ(bi.to_string(16), "-0xbb9ce");
      CHECK_EQ(bi.to_string(10), "-768462");
    }

    SUBCASE("7")
    {
      auto bi = mtmath::BigInt("+768462");
      CHECK_EQ(bi.to_string(16), "0xbb9ce");
      CHECK_EQ(bi.to_string(10), "768462");
    }

    SUBCASE("8")
    {
      auto bi = mtmath::BigInt("000023");
      CHECK_EQ(bi.to_string(16), "0x17");
      CHECK_EQ(bi.to_string(10), "23");
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

    SUBCASE("Specials") {
      CHECK_EQ(mtmath::BigInt::invalid().to_string(10), "NaN");
      CHECK_EQ(mtmath::BigInt::zero().to_string(10), "0");
      CHECK_EQ(mtmath::BigInt::zero().to_string(16), "0x0");
    }
  }

  TEST_CASE("Equality") {
    CHECK_EQ(mtmath::BigInt(1485209), mtmath::BigInt("1485209"));
    CHECK_NE(mtmath::BigInt(1485206), mtmath::BigInt("1485209"));
    CHECK_NE(mtmath::BigInt(-1485209), mtmath::BigInt("1485209"));
  }

  TEST_CASE("Negate") {
    CHECK_EQ((-mtmath::BigInt(1485209)), mtmath::BigInt("-1485209"));
    CHECK_EQ((-mtmath::BigInt(0)), mtmath::BigInt("0"));
    CHECK_EQ((-mtmath::BigInt::invalid()), mtmath::BigInt::invalid());
  }

  TEST_CASE("Add") {
    CHECK_EQ(mtmath::BigInt(1485209) += mtmath::BigInt("934889"), mtmath::BigInt("2420098"));
    CHECK_EQ(mtmath::BigInt(1485209) + mtmath::BigInt("-934889"), mtmath::BigInt("550320"));
    CHECK_EQ(mtmath::BigInt(-1485209) += mtmath::BigInt("934889"), mtmath::BigInt("-550320"));

    CHECK_EQ(mtmath::BigInt::invalid() + mtmath::BigInt("-2"), mtmath::BigInt::invalid());
    CHECK_EQ(mtmath::BigInt(-1485209) + mtmath::BigInt::invalid(), mtmath::BigInt::invalid());
  }

  TEST_CASE("Subtract") {
    std::string s = (mtmath::BigInt(1485209) - mtmath::BigInt("934889")).to_string(16);
    CHECK_EQ(s, "0x865b0");
    CHECK_EQ(mtmath::BigInt(1485209) -= mtmath::BigInt("934889"), mtmath::BigInt("550320"));
    CHECK_EQ(mtmath::BigInt(1485209) - mtmath::BigInt("-934889"), mtmath::BigInt("2420098"));
    CHECK_EQ(mtmath::BigInt(-1485209) -= mtmath::BigInt("934889"), mtmath::BigInt("-2420098"));
    CHECK_EQ(mtmath::BigInt::invalid() - mtmath::BigInt("-2"), mtmath::BigInt::invalid());
    CHECK_EQ(mtmath::BigInt(-1485209) - mtmath::BigInt::invalid(), mtmath::BigInt::invalid());
  }

  TEST_CASE("Bit Shift") {
    CHECK_EQ(mtmath::BigInt(1) << 2, mtmath::BigInt(4));
    CHECK_EQ(mtmath::BigInt(4) >> 2, mtmath::BigInt(1));
  }

  TEST_CASE("Compare") {
    CHECK_EQ(mtmath::BigInt(1485209) <=> mtmath::BigInt(1485209), std::strong_ordering::equal);
    CHECK_EQ(mtmath::BigInt(1485208) <=> mtmath::BigInt(1485209), std::strong_ordering::less);
    CHECK_EQ(mtmath::BigInt(1485209) <=> mtmath::BigInt(1485208), std::strong_ordering::greater);
    CHECK_EQ(mtmath::BigInt(1485209) <=> mtmath::BigInt(2485209), std::strong_ordering::less);
    CHECK_EQ(mtmath::BigInt(2485209) <=> mtmath::BigInt(1485209), std::strong_ordering::greater);
    CHECK_EQ(mtmath::BigInt(1485209) <=> mtmath::BigInt(948509), std::strong_ordering::greater);
    CHECK_EQ(mtmath::BigInt(948509) <=> mtmath::BigInt(1485209), std::strong_ordering::less);
    CHECK_EQ(mtmath::BigInt(1484209) <=> mtmath::BigInt(1485208), std::strong_ordering::less);
    CHECK_EQ(mtmath::BigInt(1485208) <=> mtmath::BigInt(1484209), std::strong_ordering::greater);

    CHECK_EQ(mtmath::BigInt::zero() <=> mtmath::BigInt::zero(), std::strong_ordering::equal);

    CHECK_EQ(mtmath::BigInt::invalid() <=> mtmath::BigInt(1484209), std::strong_ordering::less);
    CHECK_EQ(mtmath::BigInt(1484209) <=> mtmath::BigInt::invalid(), std::strong_ordering::greater);
    CHECK_EQ(mtmath::BigInt::invalid() <=> mtmath::BigInt::invalid(), std::strong_ordering::equal);

    CHECK_EQ(mtmath::BigInt(1484209) <=> mtmath::BigInt(-1485208), std::strong_ordering::greater);
    CHECK_EQ(mtmath::BigInt(-1485208) <=> mtmath::BigInt(1484209), std::strong_ordering::less);
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
    CHECK_EQ((mtmath::BigInt(1485209) *= mtmath::BigInt("2")).to_string(16), "0x2d5332");
    CHECK_EQ((mtmath::BigInt(1485209) * mtmath::BigInt("-2")).to_string(16), "-0x2d5332");
    CHECK_EQ((mtmath::BigInt(-1485209) *= mtmath::BigInt("2")).to_string(16), "-0x2d5332");
    CHECK_EQ((mtmath::BigInt(-1485209) * mtmath::BigInt("-2")).to_string(16), "0x2d5332");

    CHECK_EQ((mtmath::BigInt(250) * mtmath::BigInt("250")).to_string(10), "62500");

    CHECK_EQ(mtmath::BigInt::zero() * mtmath::BigInt("-2"), mtmath::BigInt::zero());
    CHECK_EQ(mtmath::BigInt(-1485209) * mtmath::BigInt::zero(), mtmath::BigInt::zero());

    CHECK_EQ(mtmath::BigInt::invalid() * mtmath::BigInt("-2"), mtmath::BigInt::invalid());
    CHECK_EQ(mtmath::BigInt(-1485209) * mtmath::BigInt::invalid(), mtmath::BigInt::invalid());
  }

  TEST_CASE("Divide") {
    std::string s = (mtmath::BigInt(1485209) / mtmath::BigInt("2")).to_string(16);
    CHECK_EQ(s, "0xb54cc");
    s = (mtmath::BigInt(1485209) / mtmath::BigInt("3")).to_string(16);
    CHECK_EQ(s, "0x78ddd");
    CHECK_EQ(mtmath::BigInt(1485209) /= mtmath::BigInt("3"), mtmath::BigInt("495069"));

    CHECK_FALSE((mtmath::BigInt(1485209) / mtmath::BigInt::zero()).is_valid());
    CHECK_FALSE((mtmath::BigInt(1485209) / mtmath::BigInt::invalid()).is_valid());
    CHECK_FALSE((mtmath::BigInt::invalid() / mtmath::BigInt(1485209)).is_valid());
  }

  TEST_CASE("Modulo") {
    std::string s = (mtmath::BigInt(1485209) % mtmath::BigInt("2")).to_string(16);
    CHECK_EQ(s, "0x1");
    CHECK_EQ(mtmath::BigInt(1485209) % mtmath::BigInt("3"), mtmath::BigInt("2"));
    CHECK_EQ(mtmath::BigInt(1485208) %= mtmath::BigInt("2"), mtmath::BigInt("0"));
    CHECK_EQ(mtmath::BigInt(1485210) % mtmath::BigInt("3"), mtmath::BigInt("0"));
    CHECK_EQ(mtmath::BigInt(1485211) %= mtmath::BigInt("3"), mtmath::BigInt("1"));
  }

  TEST_CASE("Numeric Properties") {
    using BI = mtmath::BigInt;
    using nl = std::numeric_limits<BI>;
    CHECK(nl::is_specialized);
    CHECK(nl::is_signed);
    CHECK(nl::is_integer);
    CHECK(nl::is_exact);
    CHECK_FALSE(nl::has_infinity);
    CHECK(nl::has_quiet_NaN);
    CHECK_FALSE(nl::has_signaling_NaN);
    CHECK_EQ(nl::has_denorm, std::denorm_absent);
    CHECK_FALSE(nl::has_denorm_loss);
    CHECK_EQ(nl::round_style, std::round_toward_zero);
    CHECK_FALSE(nl::is_iec559);
    CHECK_FALSE(nl::is_bounded);
    CHECK_FALSE(nl::is_modulo);
    CHECK_EQ(nl::digits, std::numeric_limits<int>::max());
    CHECK_EQ(nl::digits10, std::numeric_limits<int>::max());
    CHECK_EQ(nl::max_digits10, std::numeric_limits<int>::max());
    CHECK_EQ(nl::radix, 2);
    CHECK_EQ(nl::min_exponent, 0);
    CHECK_EQ(nl::min_exponent10, 0);
    CHECK_EQ(nl::max_exponent, 0);
    CHECK_EQ(nl::max_exponent10, 0);
    CHECK_FALSE(nl::traps);
    CHECK_FALSE(nl::tinyness_before);

    CHECK_EQ(nl::min(), BI::zero());
    CHECK_EQ(nl::lowest(), BI::zero());
    CHECK_EQ(nl::epsilon(), BI::one());
    CHECK_EQ(nl::round_error(), BI::zero());
    CHECK_EQ(nl::infinity(), BI::zero());
    CHECK_EQ(nl::quiet_NaN(), BI::invalid());
    CHECK_EQ(nl::signaling_NaN(), BI::invalid());
    CHECK_EQ(nl::denorm_min(), BI::zero());
  }

  TEST_CASE("Abs Values") {
    using BI = mtmath::BigInt;
    CHECK_EQ(BI{"1234"}.abs_val(), BI{"1234"});
    CHECK_EQ(BI{"-1234"}.abs_val(), BI{"1234"});
  }
}

TEST_SUITE("immut BigInt") {
  TEST_CASE("To String") {
    SUBCASE("1")
    {
      auto bi = mtmath::immut::BigInt(1485209);
      CHECK_EQ(bi.to_string(16), "0x16a999");
    }

    SUBCASE("2")
    {
      auto bi = mtmath::immut::BigInt("8524631");
      CHECK_EQ(bi.to_string(16), "0x821357");
    }

    SUBCASE("3")
    {
      auto bi = mtmath::immut::BigInt(std::string("532634"));
      CHECK_EQ(bi.to_string(16), "0x8209a");
    }

    SUBCASE("4")
    {
      auto bi = mtmath::immut::BigInt(std::string_view("768462"));
      CHECK_EQ(bi.to_string(16), "0xbb9ce");
    }

    SUBCASE("5")
    {
      auto bi = mtmath::immut::BigInt(std::string_view("-768462"), 10);
      CHECK_EQ(bi.to_string(16), "-0xbb9ce");
    }

    SUBCASE("6")
    {
      auto bi = mtmath::immut::BigInt("-768462");
      CHECK_EQ(bi.to_string(16), "-0xbb9ce");
    }

    SUBCASE("7")
    {
      auto bi = mtmath::immut::BigInt("+768462");
      CHECK_EQ(bi.to_string(16), "0xbb9ce");
    }

    SUBCASE("8")
    {
      auto bi = mtmath::immut::BigInt("000023");
      CHECK_EQ(bi.to_string(16), "0x17");
    }

    SUBCASE("from hex")
    {
      auto bi = mtmath::immut::BigInt("16a999", 16);
      CHECK_EQ(bi.to_string(16), "0x16a999");
    }

    SUBCASE("from octal")
    {
      auto bi = mtmath::immut::BigInt("5524631", 8);
      CHECK_EQ(bi.to_string(16), "0x16a999");
    }

    SUBCASE("from base 3")
    {
      auto bi = mtmath::immut::BigInt("2210110022202", 3);
      CHECK_EQ(bi.to_string(16), "0x16a999");
    }

    SUBCASE("from base 32")
    {
      auto bi = mtmath::immut::BigInt("1dacp", 32);
      CHECK_EQ(bi.to_string(16), "0x16a999");
    }

    SUBCASE("from base 36")
    {
      auto bi = mtmath::immut::BigInt("vtzt", 36);
      CHECK_EQ(bi.to_string(16), "0x16a999");
    }

    SUBCASE("from base 2")
    {
      auto bi = mtmath::immut::BigInt("101101010100110011001", 2);
      CHECK_EQ(bi.to_string(16), "0x16a999");
    }

    SUBCASE("Specials") {
      CHECK_EQ(mtmath::immut::BigInt::invalid().to_string(10), "NaN");
      CHECK_EQ(mtmath::immut::BigInt::zero().to_string(10), "0");
      CHECK_EQ(mtmath::immut::BigInt::zero().to_string(16), "0x0");
    }
  }

  TEST_CASE("Equality") {
    CHECK_EQ(mtmath::immut::BigInt(1485209), mtmath::immut::BigInt("1485209"));
    CHECK_NE(mtmath::immut::BigInt(1485206), mtmath::immut::BigInt("1485209"));
    CHECK_NE(mtmath::immut::BigInt(-1485209), mtmath::immut::BigInt("1485209"));
  }

  TEST_CASE("Negate") {
    CHECK_EQ((-mtmath::immut::BigInt(1485209)), mtmath::immut::BigInt("-1485209"));
    CHECK_EQ((-mtmath::immut::BigInt(0)), mtmath::immut::BigInt("0"));
    CHECK_EQ((-mtmath::immut::BigInt::invalid()), mtmath::immut::BigInt::invalid());
  }

  TEST_CASE("Add") {
    CHECK_EQ(mtmath::immut::BigInt(1485209) + mtmath::immut::BigInt("934889"), mtmath::immut::BigInt("2420098"));
    CHECK_EQ(mtmath::immut::BigInt(1485209) + mtmath::immut::BigInt("-934889"), mtmath::immut::BigInt("550320"));
    CHECK_EQ(mtmath::immut::BigInt(-1485209) + mtmath::immut::BigInt("934889"), mtmath::immut::BigInt("-550320"));
    CHECK_EQ(mtmath::immut::BigInt::invalid() + mtmath::immut::BigInt("-2"), mtmath::immut::BigInt::invalid());
    CHECK_EQ(mtmath::immut::BigInt(-1485209) + mtmath::immut::BigInt::invalid(), mtmath::immut::BigInt::invalid());
  }

  TEST_CASE("Subtract") {
    std::string s = (mtmath::immut::BigInt(1485209) - mtmath::immut::BigInt("934889")).to_string(16);
    CHECK_EQ(s, "0x865b0");
    CHECK_EQ(mtmath::immut::BigInt(1485209) - mtmath::immut::BigInt("934889"), mtmath::immut::BigInt("550320"));
    CHECK_EQ(mtmath::immut::BigInt(1485209) - mtmath::immut::BigInt("-934889"), mtmath::immut::BigInt("2420098"));
    CHECK_EQ(mtmath::immut::BigInt(-1485209) - mtmath::immut::BigInt("934889"), mtmath::immut::BigInt("-2420098"));
    CHECK_EQ(mtmath::immut::BigInt::invalid() - mtmath::immut::BigInt("-2"), mtmath::immut::BigInt::invalid());
    CHECK_EQ(mtmath::immut::BigInt(-1485209) - mtmath::immut::BigInt::invalid(), mtmath::immut::BigInt::invalid());
  }

  TEST_CASE("Bit Shift") {
    CHECK_EQ(mtmath::immut::BigInt(1) << 2, mtmath::immut::BigInt(4));
    CHECK_EQ(mtmath::immut::BigInt(4) >> 2, mtmath::immut::BigInt(1));
  }

  TEST_CASE("Compare") {
    CHECK_EQ(mtmath::immut::BigInt(1485209) <=> mtmath::immut::BigInt(1485209), std::strong_ordering::equal);
    CHECK_EQ(mtmath::immut::BigInt(1485208) <=> mtmath::immut::BigInt(1485209), std::strong_ordering::less);
    CHECK_EQ(mtmath::immut::BigInt(1485209) <=> mtmath::immut::BigInt(1485208), std::strong_ordering::greater);
    CHECK_EQ(mtmath::immut::BigInt(1485209) <=> mtmath::immut::BigInt(2485209), std::strong_ordering::less);
    CHECK_EQ(mtmath::immut::BigInt(2485209) <=> mtmath::immut::BigInt(1485209), std::strong_ordering::greater);
    CHECK_EQ(mtmath::immut::BigInt(1485209) <=> mtmath::immut::BigInt(948509), std::strong_ordering::greater);
    CHECK_EQ(mtmath::immut::BigInt(948509) <=> mtmath::immut::BigInt(1485209), std::strong_ordering::less);
    CHECK_EQ(mtmath::immut::BigInt(1484209) <=> mtmath::immut::BigInt(1485208), std::strong_ordering::less);
    CHECK_EQ(mtmath::immut::BigInt(1485208) <=> mtmath::immut::BigInt(1484209), std::strong_ordering::greater);

    CHECK_EQ(mtmath::immut::BigInt::zero() <=> mtmath::immut::BigInt::zero(), std::strong_ordering::equal);

    CHECK_EQ(mtmath::immut::BigInt::invalid() <=> mtmath::immut::BigInt(1484209), std::strong_ordering::less);
    CHECK_EQ(mtmath::immut::BigInt(1484209) <=> mtmath::immut::BigInt::invalid(), std::strong_ordering::greater);
    CHECK_EQ(mtmath::immut::BigInt::invalid() <=> mtmath::immut::BigInt::invalid(), std::strong_ordering::equal);

    CHECK_EQ(mtmath::immut::BigInt(1484209) <=> mtmath::immut::BigInt(-1485208), std::strong_ordering::greater);
    CHECK_EQ(mtmath::immut::BigInt(-1485208) <=> mtmath::immut::BigInt(1484209), std::strong_ordering::less);
  }

  TEST_CASE("Compare operators") {
    CHECK(mtmath::immut::BigInt(1485209) == mtmath::immut::BigInt(1485209));
    CHECK(mtmath::immut::BigInt(1485209) <= mtmath::immut::BigInt(1485209));
    CHECK(mtmath::immut::BigInt(1485209) >= mtmath::immut::BigInt(1485209));
    CHECK_FALSE(mtmath::immut::BigInt(1485209) == mtmath::immut::BigInt(1485309));
    CHECK_FALSE(mtmath::immut::BigInt(1485209) != mtmath::immut::BigInt(1485209));
    CHECK(mtmath::immut::BigInt(1485209) != mtmath::immut::BigInt(1485309));

    CHECK(mtmath::immut::BigInt(1485208) < mtmath::immut::BigInt(1485209));
    CHECK(mtmath::immut::BigInt(1485209) < mtmath::immut::BigInt(2485209));
    CHECK(mtmath::immut::BigInt(948509) < mtmath::immut::BigInt(1485209));
    CHECK(mtmath::immut::BigInt(1484209) < mtmath::immut::BigInt(1485208));
    CHECK(mtmath::immut::BigInt(1485208) <= mtmath::immut::BigInt(1485209));
    CHECK(mtmath::immut::BigInt(1485209) <= mtmath::immut::BigInt(2485209));
    CHECK(mtmath::immut::BigInt(948509) <= mtmath::immut::BigInt(1485209));
    CHECK(mtmath::immut::BigInt(1484209) <= mtmath::immut::BigInt(1485208));
    CHECK_FALSE(mtmath::immut::BigInt(1485208) > mtmath::immut::BigInt(1485209));
    CHECK_FALSE(mtmath::immut::BigInt(1485209) > mtmath::immut::BigInt(2485209));
    CHECK_FALSE(mtmath::immut::BigInt(948509) > mtmath::immut::BigInt(1485209));
    CHECK_FALSE(mtmath::immut::BigInt(1484209) > mtmath::immut::BigInt(1485208));
    CHECK_FALSE(mtmath::immut::BigInt(1485208) >= mtmath::immut::BigInt(1485209));
    CHECK_FALSE(mtmath::immut::BigInt(1485209) >= mtmath::immut::BigInt(2485209));
    CHECK_FALSE(mtmath::immut::BigInt(948509) >= mtmath::immut::BigInt(1485209));
    CHECK_FALSE(mtmath::immut::BigInt(1484209) >= mtmath::immut::BigInt(1485208));

    CHECK(mtmath::immut::BigInt(1485209) > mtmath::immut::BigInt(1485208));
    CHECK(mtmath::immut::BigInt(2485209) > mtmath::immut::BigInt(1485209));
    CHECK(mtmath::immut::BigInt(1485209) > mtmath::immut::BigInt(948509));
    CHECK(mtmath::immut::BigInt(1485208) > mtmath::immut::BigInt(1484209));
    CHECK(mtmath::immut::BigInt(1485209) >= mtmath::immut::BigInt(1485208));
    CHECK(mtmath::immut::BigInt(2485209) >= mtmath::immut::BigInt(1485209));
    CHECK(mtmath::immut::BigInt(1485209) >= mtmath::immut::BigInt(948509));
    CHECK(mtmath::immut::BigInt(1485208) >= mtmath::immut::BigInt(1484209));
    CHECK_FALSE(mtmath::immut::BigInt(1485209) < mtmath::immut::BigInt(1485208));
    CHECK_FALSE(mtmath::immut::BigInt(2485209) < mtmath::immut::BigInt(1485209));
    CHECK_FALSE(mtmath::immut::BigInt(1485209) < mtmath::immut::BigInt(948509));
    CHECK_FALSE(mtmath::immut::BigInt(1485208) < mtmath::immut::BigInt(1484209));
    CHECK_FALSE(mtmath::immut::BigInt(1485209) <= mtmath::immut::BigInt(1485208));
    CHECK_FALSE(mtmath::immut::BigInt(2485209) <= mtmath::immut::BigInt(1485209));
    CHECK_FALSE(mtmath::immut::BigInt(1485209) <= mtmath::immut::BigInt(948509));
    CHECK_FALSE(mtmath::immut::BigInt(1485208) <= mtmath::immut::BigInt(1484209));
  }

  TEST_CASE("Multiply") {
    CHECK_EQ((mtmath::immut::BigInt(1485209) * mtmath::immut::BigInt("2")).to_string(16), "0x2d5332");
    CHECK_EQ((mtmath::immut::BigInt(1485209) * mtmath::immut::BigInt("-2")).to_string(16), "-0x2d5332");
    CHECK_EQ((mtmath::immut::BigInt(-1485209) * mtmath::immut::BigInt("2")).to_string(16), "-0x2d5332");
    CHECK_EQ((mtmath::immut::BigInt(-1485209) * mtmath::immut::BigInt("-2")).to_string(16), "0x2d5332");

    CHECK_EQ((mtmath::immut::BigInt(250) * mtmath::immut::BigInt("250")).to_string(10), "62500");

    CHECK_EQ(mtmath::immut::BigInt::zero() * mtmath::immut::BigInt("-2"), mtmath::immut::BigInt::zero());
    CHECK_EQ(mtmath::immut::BigInt(-1485209) * mtmath::immut::BigInt::zero(), mtmath::immut::BigInt::zero());

    CHECK_EQ(mtmath::immut::BigInt::invalid() * mtmath::immut::BigInt("-2"), mtmath::immut::BigInt::invalid());
    CHECK_EQ(mtmath::immut::BigInt(-1485209) * mtmath::immut::BigInt::invalid(), mtmath::immut::BigInt::invalid());
  }

  TEST_CASE("Divide") {
    std::string s = (mtmath::immut::BigInt(1485209) / mtmath::immut::BigInt("2")).to_string(16);
    CHECK_EQ(s, "0xb54cc");
    s = (mtmath::immut::BigInt(1485209) / mtmath::immut::BigInt("3")).to_string(16);
    CHECK_EQ(s, "0x78ddd");
    CHECK_EQ(mtmath::immut::BigInt(1485209) / mtmath::immut::BigInt("3"), mtmath::immut::BigInt("495069"));
    CHECK_FALSE((mtmath::immut::BigInt(1485209) / mtmath::immut::BigInt::zero()).is_valid());
    CHECK_FALSE((mtmath::immut::BigInt(1485209) / mtmath::immut::BigInt::invalid()).is_valid());
    CHECK_FALSE((mtmath::immut::BigInt::invalid() / mtmath::immut::BigInt(1485209)).is_valid());
  }

  TEST_CASE("Modulo") {
    std::string s = (mtmath::immut::BigInt(1485209) % mtmath::immut::BigInt("2")).to_string(16);
    CHECK_EQ(s, "0x1");
    CHECK_EQ(mtmath::immut::BigInt(1485209) % mtmath::immut::BigInt("3"), mtmath::immut::BigInt("2"));
    CHECK_EQ(mtmath::immut::BigInt(1485208) % mtmath::immut::BigInt("2"), mtmath::immut::BigInt("0"));
    CHECK_EQ(mtmath::immut::BigInt(1485210) % mtmath::immut::BigInt("3"), mtmath::immut::BigInt("0"));
    CHECK_EQ(mtmath::immut::BigInt(1485211) % mtmath::immut::BigInt("3"), mtmath::immut::BigInt("1"));
  }

  TEST_CASE("Numeric Properties") {
    using BI = mtmath::immut::BigInt;
    using nl = std::numeric_limits<BI>;
    CHECK(nl::is_specialized);
    CHECK(nl::is_signed);
    CHECK(nl::is_integer);
    CHECK(nl::is_exact);
    CHECK_FALSE(nl::has_infinity);
    CHECK(nl::has_quiet_NaN);
    CHECK_FALSE(nl::has_signaling_NaN);
    CHECK_EQ(nl::has_denorm, std::denorm_absent);
    CHECK_FALSE(nl::has_denorm_loss);
    CHECK_EQ(nl::round_style, std::round_toward_zero);
    CHECK_FALSE(nl::is_iec559);
    CHECK_FALSE(nl::is_bounded);
    CHECK_FALSE(nl::is_modulo);
    CHECK_EQ(nl::digits, std::numeric_limits<int>::max());
    CHECK_EQ(nl::digits10, std::numeric_limits<int>::max());
    CHECK_EQ(nl::max_digits10, std::numeric_limits<int>::max());
    CHECK_EQ(nl::radix, 2);
    CHECK_EQ(nl::min_exponent, 0);
    CHECK_EQ(nl::min_exponent10, 0);
    CHECK_EQ(nl::max_exponent, 0);
    CHECK_EQ(nl::max_exponent10, 0);
    CHECK_FALSE(nl::traps);
    CHECK_FALSE(nl::tinyness_before);

    CHECK_EQ(nl::min(), BI::zero());
    CHECK_EQ(nl::lowest(), BI::zero());
    CHECK_EQ(nl::epsilon(), BI::one());
    CHECK_EQ(nl::round_error(), BI::zero());
    CHECK_EQ(nl::infinity(), BI::zero());
    CHECK_EQ(nl::quiet_NaN(), BI::invalid());
    CHECK_EQ(nl::signaling_NaN(), BI::invalid());
    CHECK_EQ(nl::denorm_min(), BI::zero());
  }

  TEST_CASE("Abs Values") {
    using BI = mtmath::immut::BigInt;
    CHECK_EQ(BI{"1234"}.abs_val(), BI{"1234"});
    CHECK_EQ(BI{"-1234"}.abs_val(), BI{"1234"});
  }
}
