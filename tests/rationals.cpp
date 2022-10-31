#include "impl/rational.h"
#include "doctest.h"
#include <sstream>

TEST_SUITE("Rational Base") {
  TEST_CASE("Default Constructor") {
    using Rational = mtmath::RationalBase<int64_t>;
    auto d = Rational{};
    CHECK_EQ(d, Rational{0, 1});
  }

  TEST_CASE("Assignment") {
    using Rational = mtmath::RationalBase<int64_t>;
    auto d = Rational{};
    auto a = Rational{1, 2};
    d = a;
    auto c = std::move(a);
    CHECK_EQ(d, c);
    a = std::move(c);
    CHECK_EQ(d, a);
  }

  TEST_CASE("is nan") {
    using Rational = mtmath::RationalBase<int64_t>;
    auto half = Rational{1, 2};
    CHECK_FALSE(half.is_nan());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK(nan.is_nan());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(inf.is_nan());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(neg_inf.is_nan());
  }

  TEST_CASE("is infinite") {
    using Rational = mtmath::RationalBase<int64_t>;
    auto half = Rational{1, 2};
    CHECK_FALSE(half.is_infinite());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK_FALSE(nan.is_infinite());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK(inf.is_infinite());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK(neg_inf.is_infinite());
  }

  TEST_CASE("is positive infinity") {
    using Rational = mtmath::RationalBase<int64_t>;
    auto half = Rational{1, 2};
    CHECK_FALSE(half.is_pos_infinity());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK_FALSE(nan.is_pos_infinity());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK(inf.is_pos_infinity());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(neg_inf.is_pos_infinity());
  }

  TEST_CASE("is negative infinity") {
    using Rational = mtmath::RationalBase<int64_t>;
    auto half = Rational{1, 2};
    CHECK_FALSE(half.is_neg_infinity());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK_FALSE(nan.is_neg_infinity());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(inf.is_neg_infinity());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK(neg_inf.is_neg_infinity());
  }

  TEST_CASE("is finite") {
    using Rational = mtmath::RationalBase<int64_t>;
    auto half = Rational{1, 2};
    CHECK(half.is_finite());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK_FALSE(nan.is_finite());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(inf.is_finite());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(neg_inf.is_finite());
  }

  TEST_CASE("can compare") {
    using Rational = mtmath::RationalBase<int64_t>;
    auto half = Rational{1, 2};
    auto half2 = Rational{2, 4};
    auto third = Rational{1, 3};
    auto twoThirds = Rational{2, 3};

    CHECK(half == half2);
    CHECK(half <= half2);
    CHECK(half >= half2);
    CHECK_FALSE(half < half2);
    CHECK_FALSE(half > half2);
    CHECK_FALSE(half != half2);

    CHECK(half < twoThirds);
    CHECK(half <= twoThirds);
    CHECK(half != twoThirds);
    CHECK_FALSE(half == twoThirds);
    CHECK_FALSE(half >= twoThirds);
    CHECK_FALSE(half > twoThirds);

    CHECK(half > third);
    CHECK(half >= third);
    CHECK(half != third);
    CHECK_FALSE(half == third);
    CHECK_FALSE(half <= third);
    CHECK_FALSE(half < third);
  }

  TEST_CASE("sum rationals") {
    using Rational = mtmath::RationalBase<int64_t>;
    CHECK_EQ(Rational{1, 3} + Rational{1, 3}, Rational{2, 3});
    CHECK_EQ(Rational{2, 3} + Rational{5, 6}, Rational{3, 2});
  }

  TEST_CASE("sub rationals") {
    using Rational = mtmath::RationalBase<int64_t>;
    CHECK_EQ(Rational{2, 3} - Rational{1, 3}, Rational{1, 3});
    CHECK_EQ(Rational{3, 2} - Rational{5, 6}, Rational{2, 3});
  }

  TEST_CASE("mult rationals") {
    using Rational = mtmath::RationalBase<int64_t>;
    CHECK_EQ(Rational{2, 7} * Rational{3, 5}, Rational{6, 35});
    CHECK_EQ(Rational{5, 7} * Rational{3, 5}, Rational{3, 7});
  }

  TEST_CASE("div rationals") {
    using Rational = mtmath::RationalBase<int64_t>;
    CHECK_EQ(Rational{2, 7} / Rational{5, 3}, Rational{6, 35});
    CHECK_EQ(Rational{5, 7} / Rational{5, 3}, Rational{3, 7});
  }

  TEST_CASE("Special Values") {
    using Rational = mtmath::RationalBase<int64_t>;
    auto inf = std::numeric_limits<Rational>::infinity();
    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    auto neg_inf = -inf;

    SUBCASE("Comparisons and identifiers") {
      CHECK(inf.is_infinite());
      CHECK(inf.is_pos_infinity());
      CHECK_FALSE(inf.is_neg_infinity());
      CHECK_FALSE(inf.is_nan());
      CHECK_FALSE(inf.is_finite());
      CHECK_EQ(inf, inf);
      CHECK_NE(inf, neg_inf);
      CHECK_NE(inf, nan);

      CHECK(neg_inf.is_infinite());
      CHECK_FALSE(neg_inf.is_pos_infinity());
      CHECK(neg_inf.is_neg_infinity());
      CHECK_FALSE(neg_inf.is_nan());
      CHECK_FALSE(neg_inf.is_finite());
      CHECK_EQ(neg_inf, neg_inf);
      CHECK_NE(neg_inf, inf);
      CHECK_NE(neg_inf, nan);

      CHECK_FALSE(nan.is_infinite());
      CHECK_FALSE(nan.is_pos_infinity());
      CHECK_FALSE(nan.is_neg_infinity());
      CHECK(nan.is_nan());
      CHECK_FALSE(nan.is_finite());
      CHECK_NE(nan, neg_inf);
      CHECK_NE(nan, inf);
      CHECK_EQ(nan, nan);
    }

    SUBCASE("Addition") {
      CHECK_EQ(inf + Rational{1, 2}, inf);
      CHECK_EQ(inf + Rational{-1, 2}, inf);
      CHECK_EQ(inf + inf, inf);
      CHECK_EQ(inf + neg_inf, nan);

      CHECK_EQ(neg_inf + Rational{1, 2}, neg_inf);
      CHECK_EQ(neg_inf + Rational{-1, 2}, neg_inf);
      CHECK_EQ(neg_inf + neg_inf, neg_inf);
      CHECK_EQ(neg_inf + inf, nan);

      CHECK_EQ(nan + Rational{1, 2}, nan);
      CHECK_EQ(nan + nan, nan);
      CHECK_EQ(nan + inf, nan);
      CHECK_EQ(nan + neg_inf, nan);
    }

    SUBCASE("Subtraction") {
      CHECK_EQ(inf - Rational{1, 2}, inf);
      CHECK_EQ(inf - Rational{-1, 2}, inf);
      CHECK_EQ(inf - neg_inf, inf);
      CHECK_EQ(inf - inf, nan);

      CHECK_EQ(neg_inf - Rational{1, 2}, neg_inf);
      CHECK_EQ(neg_inf - Rational{-1, 2}, neg_inf);
      CHECK_EQ(neg_inf - inf, neg_inf);
      CHECK_EQ(neg_inf - neg_inf, nan);

      CHECK_EQ(nan - Rational{1, 2}, nan);
      CHECK_EQ(nan - nan, nan);
      CHECK_EQ(nan - neg_inf, nan);
      CHECK_EQ(nan - inf, nan);
    }

    SUBCASE("Multiplication") {
      CHECK_EQ(inf * Rational{1, 2}, inf);
      CHECK_EQ(inf * Rational{-1, 2}, neg_inf);
      CHECK_EQ(inf * inf, inf);
      CHECK_EQ(inf * neg_inf, neg_inf);
      CHECK_EQ(inf * Rational{0}, nan);

      CHECK_EQ(neg_inf * Rational{1, 2}, neg_inf);
      CHECK_EQ(neg_inf * Rational{-1, 2}, inf);
      CHECK_EQ(neg_inf * neg_inf, inf);
      CHECK_EQ(neg_inf * inf, neg_inf);
      CHECK_EQ(neg_inf * Rational{0}, nan);

      CHECK_EQ(nan * Rational{1, 2}, nan);
      CHECK_EQ(nan * nan, nan);
      CHECK_EQ(nan * inf, nan);
      CHECK_EQ(nan * neg_inf, nan);
      CHECK_EQ(nan * Rational{0}, nan);
    }

    SUBCASE("Division") {
      CHECK_EQ(inf / Rational{1, 2}, inf);
      CHECK_EQ(inf / Rational{-1, 2}, neg_inf);
      CHECK_EQ(inf / inf, nan);
      CHECK_EQ(inf / neg_inf, nan);
      CHECK_EQ(inf / Rational{0}, inf);
      CHECK_EQ(Rational{1, 2} / inf, Rational{0});
      CHECK_EQ(Rational{-1, 2} / inf, Rational{0});
      CHECK_EQ(Rational{0} / inf, Rational{0});

      CHECK_EQ(neg_inf / Rational{1, 2}, neg_inf);
      CHECK_EQ(neg_inf / Rational{-1, 2}, inf);
      CHECK_EQ(neg_inf / neg_inf, nan);
      CHECK_EQ(neg_inf / inf, nan);
      CHECK_EQ(neg_inf / Rational{0}, neg_inf);
      CHECK_EQ(Rational{1, 2} / neg_inf, Rational{0});
      CHECK_EQ(Rational{-1, 2} / neg_inf, Rational{0});
      CHECK_EQ(Rational{0} / neg_inf, Rational{0});

      CHECK_EQ(nan / Rational{1, 2}, nan);
      CHECK_EQ(nan / nan, nan);
      CHECK_EQ(nan / inf, nan);
      CHECK_EQ(nan / neg_inf, nan);
      CHECK_EQ(nan / Rational{0}, nan);
      CHECK_EQ(Rational{0} / nan, nan);
      CHECK_EQ(Rational{1, 2} / nan, nan);
    }
  }

  TEST_CASE("Type Traits") {
    using Rational = mtmath::RationalBase<int64_t>;
    using nl = std::numeric_limits<Rational>;

    CHECK_EQ(nl::is_specialized, true);
    CHECK_EQ(nl::is_signed, true);
    CHECK_EQ(nl::is_integer, false);
    CHECK_EQ(nl::is_exact, false);
    CHECK_EQ(nl::has_infinity, true);
    CHECK_EQ(nl::has_quiet_NaN, true);
    CHECK_EQ(nl::has_signaling_NaN, false);
    CHECK_EQ(nl::has_denorm, std::denorm_absent);
    CHECK_EQ(nl::has_denorm_loss, false);
    CHECK_EQ(nl::round_style, std::round_toward_zero);
    CHECK_EQ(nl::is_iec559, false);
    CHECK_EQ(nl::is_bounded, true);
    CHECK_EQ(nl::is_modulo, std::numeric_limits<int64_t>::is_modulo);
    CHECK_EQ(nl::digits, std::numeric_limits<int64_t>::digits);
    CHECK_EQ(nl::digits10, std::numeric_limits<int64_t>::digits10);
    CHECK_EQ(nl::max_digits10, std::numeric_limits<int64_t>::max_digits10);
    CHECK_EQ(nl::radix, std::numeric_limits<int64_t>::radix);
    CHECK_EQ(nl::min_exponent, std::numeric_limits<int64_t>::min_exponent);
    CHECK_EQ(nl::min_exponent10, std::numeric_limits<int64_t>::min_exponent10);
    CHECK_EQ(nl::min_exponent10, std::numeric_limits<int64_t>::min_exponent10);
    CHECK_EQ(nl::max_exponent, std::numeric_limits<int64_t>::max_exponent);
    CHECK_EQ(nl::max_exponent10, std::numeric_limits<int64_t>::max_exponent10);
    CHECK_EQ(nl::traps, false);
    CHECK_EQ(nl::tinyness_before, std::numeric_limits<int64_t>::tinyness_before);

    CHECK_EQ(nl::min(), Rational{std::numeric_limits<int64_t>::min()});
    CHECK_EQ(nl::max(), Rational{std::numeric_limits<int64_t>::max()});
    CHECK_EQ(nl::lowest(), Rational{1, std::numeric_limits<int64_t>::max()});
    CHECK_EQ(nl::epsilon(), Rational{1, std::numeric_limits<int64_t>::max()});
    CHECK_EQ(nl::round_error(), Rational{1, 2});
    CHECK_EQ(nl::infinity(), Rational{1, 0});
    CHECK_EQ(nl::quiet_NaN(), Rational{0, 0});
    CHECK_EQ(nl::signaling_NaN(), Rational{0, 0});
    CHECK_EQ(nl::denorm_min(), Rational{std::numeric_limits<int64_t>::min()});
  }
}

TEST_SUITE("Rational") {
  TEST_CASE("Default Constructor") {
    using Rational = mtmath::Rational;
    auto d = Rational{};
    CHECK_EQ(d, Rational{0, 1});
  }

  TEST_CASE("Assignment") {
    using Rational = mtmath::Rational;
    auto d = Rational{};
    auto a = Rational{1, 2};
    d = a;
    auto c = std::move(a);
    CHECK_EQ(d, c);
    a = std::move(c);
    CHECK_EQ(d, a);
  }

  TEST_CASE("is nan") {
    using Rational = mtmath::Rational;
    auto half = Rational{1, 2};
    CHECK_FALSE(half.is_nan());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK(nan.is_nan());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(inf.is_nan());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(neg_inf.is_nan());
  }

  TEST_CASE("is infinite") {
    using Rational = mtmath::Rational;
    auto half = Rational{1, 2};
    CHECK_FALSE(half.is_infinite());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK_FALSE(nan.is_infinite());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK(inf.is_infinite());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK(neg_inf.is_infinite());
  }

  TEST_CASE("is positive infinity") {
    using Rational = mtmath::Rational;
    auto half = Rational{1, 2};
    CHECK_FALSE(half.is_pos_infinity());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK_FALSE(nan.is_pos_infinity());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK(inf.is_pos_infinity());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(neg_inf.is_pos_infinity());
  }

  TEST_CASE("is negative infinity") {
    using Rational = mtmath::Rational;
    auto half = Rational{1, 2};
    CHECK_FALSE(half.is_neg_infinity());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK_FALSE(nan.is_neg_infinity());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(inf.is_neg_infinity());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK(neg_inf.is_neg_infinity());
  }

  TEST_CASE("is finite") {
    using Rational = mtmath::Rational;
    auto half = Rational{1, 2};
    CHECK(half.is_finite());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK_FALSE(nan.is_finite());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(inf.is_finite());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(neg_inf.is_finite());
  }

  TEST_CASE("can compare") {
    using Rational = mtmath::Rational;
    auto half = Rational{1, 2};
    auto half2 = Rational{2, 4};
    auto third = Rational{1, 3};
    auto twoThirds = Rational{2, 3};
    CHECK_FALSE(half < half2);

    CHECK(half == half2);
    CHECK(half <= half2);
    CHECK(half >= half2);
    CHECK_FALSE(half > half2);
    CHECK_FALSE(half != half2);

    CHECK(half < twoThirds);
    CHECK(half <= twoThirds);
    CHECK(half != twoThirds);
    CHECK_FALSE(half == twoThirds);
    CHECK_FALSE(half >= twoThirds);
    CHECK_FALSE(half > twoThirds);

    CHECK(half > third);
    CHECK(half >= third);
    CHECK(half != third);
    CHECK_FALSE(half == third);
    CHECK_FALSE(half <= third);
    CHECK_FALSE(half < third);
  }

  TEST_CASE("ostream") {
    using Rational = mtmath::Rational;
    std::stringstream ss;
    ss << Rational{2, 6};
    CHECK_EQ(ss.str(), "1/3");
  }

  TEST_CASE("sum rationals") {
    using Rational = mtmath::Rational;
    CHECK_EQ(Rational{1, 3} + Rational{1, 3}, Rational{2, 3});
    CHECK_EQ(Rational{2, 3} + Rational{5, 6}, Rational{3, 2});
  }

  TEST_CASE("sub rationals") {
    using Rational = mtmath::Rational;
    CHECK_EQ(Rational{2, 3} - Rational{1, 3}, Rational{1, 3});
    CHECK_EQ(Rational{3, 2} - Rational{5, 6}, Rational{2, 3});
  }

  TEST_CASE("mult rationals") {
    using Rational = mtmath::Rational;
    CHECK_EQ(Rational{2, 7} * Rational{3, 5}, Rational{6, 35});
    CHECK_EQ(Rational{5, 7} * Rational{3, 5}, Rational{3, 7});
  }

  TEST_CASE("div rationals") {
    using Rational = mtmath::Rational;
    CHECK_EQ(Rational{2, 7} / Rational{5, 3}, Rational{6, 35});
    CHECK_EQ(Rational{5, 7} / Rational{5, 3}, Rational{3, 7});
  }

  TEST_CASE("Special Values") {
    using Rational = mtmath::Rational;
    auto inf = std::numeric_limits<Rational>::infinity();
    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    auto neg_inf = -inf;

    SUBCASE("Comparisons and identifiers") {
      CHECK(inf.is_infinite());
      CHECK(inf.is_pos_infinity());
      CHECK_FALSE(inf.is_neg_infinity());
      CHECK_FALSE(inf.is_nan());
      CHECK_FALSE(inf.is_finite());
      CHECK_EQ(inf, inf);
      CHECK_NE(inf, neg_inf);
      CHECK_NE(inf, nan);

      CHECK(neg_inf.is_infinite());
      CHECK_FALSE(neg_inf.is_pos_infinity());
      CHECK(neg_inf.is_neg_infinity());
      CHECK_FALSE(neg_inf.is_nan());
      CHECK_FALSE(neg_inf.is_finite());
      CHECK_EQ(neg_inf, neg_inf);
      CHECK_NE(neg_inf, inf);
      CHECK_NE(neg_inf, nan);

      CHECK_FALSE(nan.is_infinite());
      CHECK_FALSE(nan.is_pos_infinity());
      CHECK_FALSE(nan.is_neg_infinity());
      CHECK(nan.is_nan());
      CHECK_FALSE(nan.is_finite());
      CHECK_NE(nan, neg_inf);
      CHECK_NE(nan, inf);
      CHECK_EQ(nan, nan);
    }

    SUBCASE("Addition") {
      CHECK_EQ(inf + Rational{1, 2}, inf);
      CHECK_EQ(inf + Rational{-1, 2}, inf);
      CHECK_EQ(inf + inf, inf);
      CHECK_EQ(inf + neg_inf, nan);

      CHECK_EQ(neg_inf + Rational{1, 2}, neg_inf);
      CHECK_EQ(neg_inf + Rational{-1, 2}, neg_inf);
      CHECK_EQ(neg_inf + neg_inf, neg_inf);
      CHECK_EQ(neg_inf + inf, nan);

      CHECK_EQ(nan + Rational{1, 2}, nan);
      CHECK_EQ(nan + nan, nan);
      CHECK_EQ(nan + inf, nan);
      CHECK_EQ(nan + neg_inf, nan);
    }

    SUBCASE("Subtraction") {
      CHECK_EQ(inf - Rational{1, 2}, inf);
      CHECK_EQ(inf - Rational{-1, 2}, inf);
      CHECK_EQ(inf - neg_inf, inf);
      CHECK_EQ(inf - inf, nan);

      CHECK_EQ(neg_inf - Rational{1, 2}, neg_inf);
      CHECK_EQ(neg_inf - Rational{-1, 2}, neg_inf);
      CHECK_EQ(neg_inf - inf, neg_inf);
      CHECK_EQ(neg_inf - neg_inf, nan);

      CHECK_EQ(nan - Rational{1, 2}, nan);
      CHECK_EQ(nan - nan, nan);
      CHECK_EQ(nan - neg_inf, nan);
      CHECK_EQ(nan - inf, nan);
    }

    SUBCASE("Multiplication") {
      CHECK_EQ(inf * Rational{1, 2}, inf);
      CHECK_EQ(inf * Rational{-1, 2}, neg_inf);
      CHECK_EQ(inf * inf, inf);
      CHECK_EQ(inf * neg_inf, neg_inf);
      CHECK_EQ(inf * Rational{0}, nan);

      CHECK_EQ(neg_inf * Rational{1, 2}, neg_inf);
      CHECK_EQ(neg_inf * Rational{-1, 2}, inf);
      CHECK_EQ(neg_inf * neg_inf, inf);
      CHECK_EQ(neg_inf * inf, neg_inf);
      CHECK_EQ(neg_inf * Rational{0}, nan);

      CHECK_EQ(nan * Rational{1, 2}, nan);
      CHECK_EQ(nan * nan, nan);
      CHECK_EQ(nan * inf, nan);
      CHECK_EQ(nan * neg_inf, nan);
      CHECK_EQ(nan * Rational{0}, nan);
    }

    SUBCASE("Division") {
      CHECK_EQ(inf / Rational{1, 2}, inf);
      CHECK_EQ(inf / Rational{-1, 2}, neg_inf);
      CHECK_EQ(inf / inf, nan);
      CHECK_EQ(inf / neg_inf, nan);
      CHECK_EQ(inf / Rational{0}, inf);
      CHECK_EQ(Rational{1, 2} / inf, Rational{0});
      CHECK_EQ(Rational{-1, 2} / inf, Rational{0});
      CHECK_EQ(Rational{0} / inf, Rational{0});

      CHECK_EQ(neg_inf / Rational{1, 2}, neg_inf);
      CHECK_EQ(neg_inf / Rational{-1, 2}, inf);
      CHECK_EQ(neg_inf / neg_inf, nan);
      CHECK_EQ(neg_inf / inf, nan);
      CHECK_EQ(neg_inf / Rational{0}, neg_inf);
      CHECK_EQ(Rational{1, 2} / neg_inf, Rational{0});
      CHECK_EQ(Rational{-1, 2} / neg_inf, Rational{0});
      CHECK_EQ(Rational{0} / neg_inf, Rational{0});

      CHECK_EQ(nan / Rational{1, 2}, nan);
      CHECK_EQ(nan / nan, nan);
      CHECK_EQ(nan / inf, nan);
      CHECK_EQ(nan / neg_inf, nan);
      CHECK_EQ(nan / Rational{0}, nan);
      CHECK_EQ(Rational{0} / nan, nan);
      CHECK_EQ(Rational{1, 2} / nan, nan);
    }
  }

  TEST_CASE("Type Traits") {
    using Rational = mtmath::Rational;
    using nl = std::numeric_limits<Rational>;

    CHECK_EQ(nl::is_specialized, true);
    CHECK_EQ(nl::is_signed, true);
    CHECK_EQ(nl::is_integer, false);
    CHECK_EQ(nl::is_exact, true);
    CHECK_EQ(nl::has_infinity, true);
    CHECK_EQ(nl::has_quiet_NaN, true);
    CHECK_EQ(nl::has_signaling_NaN, false);
    CHECK_EQ(nl::has_denorm, std::denorm_absent);
    CHECK_EQ(nl::has_denorm_loss, false);
    CHECK_EQ(nl::round_style, std::round_toward_zero);
    CHECK_EQ(nl::is_iec559, false);
    CHECK_EQ(nl::is_bounded, false);
    CHECK_EQ(nl::is_modulo, std::numeric_limits<mtmath::BigInt>::is_modulo);
    CHECK_EQ(nl::digits, std::numeric_limits<mtmath::BigInt>::digits);
    CHECK_EQ(nl::digits10, std::numeric_limits<mtmath::BigInt>::digits10);
    CHECK_EQ(nl::max_digits10, std::numeric_limits<mtmath::BigInt>::max_digits10);
    CHECK_EQ(nl::radix, std::numeric_limits<mtmath::BigInt>::radix);
    CHECK_EQ(nl::min_exponent, std::numeric_limits<mtmath::BigInt>::min_exponent);
    CHECK_EQ(nl::min_exponent10, std::numeric_limits<mtmath::BigInt>::min_exponent10);
    CHECK_EQ(nl::min_exponent10, std::numeric_limits<mtmath::BigInt>::min_exponent10);
    CHECK_EQ(nl::max_exponent, std::numeric_limits<mtmath::BigInt>::max_exponent);
    CHECK_EQ(nl::max_exponent10, std::numeric_limits<mtmath::BigInt>::max_exponent10);
    CHECK_EQ(nl::traps, false);
    CHECK_EQ(nl::tinyness_before, std::numeric_limits<mtmath::BigInt>::tinyness_before);

    CHECK_EQ(nl::min(), -nl::infinity());
    CHECK_EQ(nl::max(), nl::infinity());
    CHECK_EQ(nl::lowest(), Rational{0});
    CHECK_EQ(nl::epsilon(), Rational{0});
    CHECK_EQ(nl::round_error(), Rational{1, 2});
    CHECK_EQ(nl::infinity(), Rational{1, 0});
    CHECK_EQ(nl::quiet_NaN(), Rational{0, 0});
    CHECK_EQ(nl::signaling_NaN(), Rational{0, 0});
    CHECK_EQ(nl::denorm_min(), -nl::infinity());
  }
}

TEST_SUITE("Immut Rational") {
  TEST_CASE("Default Constructor") {
    using Rational = mtmath::immut::Rational;
    auto d = Rational{};
    CHECK_EQ(d, Rational{0, 1});
  }

  TEST_CASE("Assignment") {
    using Rational = mtmath::immut::Rational;
    auto d = Rational{};
    auto a = Rational{1, 2};
    d = a;
    auto c = std::move(a);
    CHECK_EQ(d, c);
    a = std::move(c);
    CHECK_EQ(d, a);
  }

  TEST_CASE("is nan") {
    using Rational = mtmath::immut::Rational;
    auto half = Rational{1, 2};
    CHECK_FALSE(half.is_nan());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK(nan.is_nan());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(inf.is_nan());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(neg_inf.is_nan());
  }

  TEST_CASE("is infinite") {
    using Rational = mtmath::immut::Rational;
    auto half = Rational{1, 2};
    CHECK_FALSE(half.is_infinite());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK_FALSE(nan.is_infinite());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK(inf.is_infinite());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK(neg_inf.is_infinite());
  }

  TEST_CASE("is positive infinity") {
    using Rational = mtmath::immut::Rational;
    auto half = Rational{1, 2};
    CHECK_FALSE(half.is_pos_infinity());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK_FALSE(nan.is_pos_infinity());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK(inf.is_pos_infinity());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(neg_inf.is_pos_infinity());
  }

  TEST_CASE("is negative infinity") {
    using Rational = mtmath::immut::Rational;
    auto half = Rational{1, 2};
    CHECK_FALSE(half.is_neg_infinity());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK_FALSE(nan.is_neg_infinity());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(inf.is_neg_infinity());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK(neg_inf.is_neg_infinity());
  }

  TEST_CASE("is finite") {
    using Rational = mtmath::immut::Rational;
    auto half = Rational{1, 2};
    CHECK(half.is_finite());

    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    CHECK_FALSE(nan.is_finite());

    auto inf = std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(inf.is_finite());

    auto neg_inf = -std::numeric_limits<Rational>::infinity();
    CHECK_FALSE(neg_inf.is_finite());
  }

  TEST_CASE("can compare") {
    using Rational = mtmath::immut::Rational;
    auto half = Rational{1, 2};
    auto half2 = Rational{2, 4};
    auto third = Rational{1, 3};
    auto twoThirds = Rational{2, 3};
    CHECK_FALSE(half < half2);

    CHECK(half == half2);
    CHECK(half <= half2);
    CHECK(half >= half2);
    CHECK_FALSE(half > half2);
    CHECK_FALSE(half != half2);

    CHECK(half < twoThirds);
    CHECK(half <= twoThirds);
    CHECK(half != twoThirds);
    CHECK_FALSE(half == twoThirds);
    CHECK_FALSE(half >= twoThirds);
    CHECK_FALSE(half > twoThirds);

    CHECK(half > third);
    CHECK(half >= third);
    CHECK(half != third);
    CHECK_FALSE(half == third);
    CHECK_FALSE(half <= third);
    CHECK_FALSE(half < third);
  }

  TEST_CASE("ostream") {
    using Rational = mtmath::immut::Rational;
    std::stringstream ss;
    ss << Rational{2, 6};
    CHECK_EQ(ss.str(), "1/3");
  }

  TEST_CASE("sum rationals") {
    using Rational = mtmath::immut::Rational;
    CHECK_EQ(Rational{1, 3} + Rational{1, 3}, Rational{2, 3});
    CHECK_EQ(Rational{2, 3} + Rational{5, 6}, Rational{3, 2});
  }

  TEST_CASE("sub rationals") {
    using Rational = mtmath::immut::Rational;
    CHECK_EQ(Rational{2, 3} - Rational{1, 3}, Rational{1, 3});
    CHECK_EQ(Rational{3, 2} - Rational{5, 6}, Rational{2, 3});
  }

  TEST_CASE("mult rationals") {
    using Rational = mtmath::immut::Rational;
    CHECK_EQ(Rational{2, 7} * Rational{3, 5}, Rational{6, 35});
    CHECK_EQ(Rational{5, 7} * Rational{3, 5}, Rational{3, 7});
  }

  TEST_CASE("div rationals") {
    using Rational = mtmath::immut::Rational;
    CHECK_EQ(Rational{2, 7} / Rational{5, 3}, Rational{6, 35});
    CHECK_EQ(Rational{5, 7} / Rational{5, 3}, Rational{3, 7});
  }

  TEST_CASE("copy rationals") {
    using Rational = mtmath::immut::Rational;
    auto a = Rational{2, 3};
    Rational b{};
    b = a;
    CHECK_EQ(a, b);
    auto c = std::move(a);
    CHECK_EQ(b, c);
    a = std::move(c);
    CHECK_EQ(b, a);
  }

  TEST_CASE("Special Values") {
    using Rational = mtmath::immut::Rational;
    auto inf = std::numeric_limits<Rational>::infinity();
    auto nan = std::numeric_limits<Rational>::quiet_NaN();
    auto neg_inf = -inf;

    SUBCASE("Comparisons and identifiers") {
      CHECK(inf.is_infinite());
      CHECK(inf.is_pos_infinity());
      CHECK_FALSE(inf.is_neg_infinity());
      CHECK_FALSE(inf.is_nan());
      CHECK_FALSE(inf.is_finite());
      CHECK_EQ(inf, inf);
      CHECK_NE(inf, neg_inf);
      CHECK_NE(inf, nan);

      CHECK(neg_inf.is_infinite());
      CHECK_FALSE(neg_inf.is_pos_infinity());
      CHECK(neg_inf.is_neg_infinity());
      CHECK_FALSE(neg_inf.is_nan());
      CHECK_FALSE(neg_inf.is_finite());
      CHECK_EQ(neg_inf, neg_inf);
      CHECK_NE(neg_inf, inf);
      CHECK_NE(neg_inf, nan);

      CHECK_FALSE(nan.is_infinite());
      CHECK_FALSE(nan.is_pos_infinity());
      CHECK_FALSE(nan.is_neg_infinity());
      CHECK(nan.is_nan());
      CHECK_FALSE(nan.is_finite());
      CHECK_NE(nan, neg_inf);
      CHECK_NE(nan, inf);
      CHECK_EQ(nan, nan);
    }

    SUBCASE("Addition") {
      CHECK_EQ(inf + Rational{1, 2}, inf);
      CHECK_EQ(inf + Rational{-1, 2}, inf);
      CHECK_EQ(inf + inf, inf);
      CHECK_EQ(inf + neg_inf, nan);

      CHECK_EQ(neg_inf + Rational{1, 2}, neg_inf);
      CHECK_EQ(neg_inf + Rational{-1, 2}, neg_inf);
      CHECK_EQ(neg_inf + neg_inf, neg_inf);
      CHECK_EQ(neg_inf + inf, nan);

      CHECK_EQ(nan + Rational{1, 2}, nan);
      CHECK_EQ(nan + nan, nan);
      CHECK_EQ(nan + inf, nan);
      CHECK_EQ(nan + neg_inf, nan);
    }

    SUBCASE("Subtraction") {
      CHECK_EQ(inf - Rational{1, 2}, inf);
      CHECK_EQ(inf - Rational{-1, 2}, inf);
      CHECK_EQ(inf - neg_inf, inf);
      CHECK_EQ(inf - inf, nan);

      CHECK_EQ(neg_inf - Rational{1, 2}, neg_inf);
      CHECK_EQ(neg_inf - Rational{-1, 2}, neg_inf);
      CHECK_EQ(neg_inf - inf, neg_inf);
      CHECK_EQ(neg_inf - neg_inf, nan);

      CHECK_EQ(nan - Rational{1, 2}, nan);
      CHECK_EQ(nan - nan, nan);
      CHECK_EQ(nan - neg_inf, nan);
      CHECK_EQ(nan - inf, nan);
    }

    SUBCASE("Multiplication") {
      CHECK_EQ(inf * Rational{1, 2}, inf);
      CHECK_EQ(inf * Rational{-1, 2}, neg_inf);
      CHECK_EQ(inf * inf, inf);
      CHECK_EQ(inf * neg_inf, neg_inf);
      CHECK_EQ(inf * Rational{0}, nan);

      CHECK_EQ(neg_inf * Rational{1, 2}, neg_inf);
      CHECK_EQ(neg_inf * Rational{-1, 2}, inf);
      CHECK_EQ(neg_inf * neg_inf, inf);
      CHECK_EQ(neg_inf * inf, neg_inf);
      CHECK_EQ(neg_inf * Rational{0}, nan);

      CHECK_EQ(nan * Rational{1, 2}, nan);
      CHECK_EQ(nan * nan, nan);
      CHECK_EQ(nan * inf, nan);
      CHECK_EQ(nan * neg_inf, nan);
      CHECK_EQ(nan * Rational{0}, nan);
    }

    SUBCASE("Division") {
      CHECK_EQ(inf / Rational{1, 2}, inf);
      CHECK_EQ(inf / Rational{-1, 2}, neg_inf);
      CHECK_EQ(inf / inf, nan);
      CHECK_EQ(inf / neg_inf, nan);
      CHECK_EQ(inf / Rational{0}, inf);
      CHECK_EQ(Rational{1, 2} / inf, Rational{0});
      CHECK_EQ(Rational{-1, 2} / inf, Rational{0});
      CHECK_EQ(Rational{0} / inf, Rational{0});

      CHECK_EQ(neg_inf / Rational{1, 2}, neg_inf);
      CHECK_EQ(neg_inf / Rational{-1, 2}, inf);
      CHECK_EQ(neg_inf / neg_inf, nan);
      CHECK_EQ(neg_inf / inf, nan);
      CHECK_EQ(neg_inf / Rational{0}, neg_inf);
      CHECK_EQ(Rational{1, 2} / neg_inf, Rational{0});
      CHECK_EQ(Rational{-1, 2} / neg_inf, Rational{0});
      CHECK_EQ(Rational{0} / neg_inf, Rational{0});

      CHECK_EQ(nan / Rational{1, 2}, nan);
      CHECK_EQ(nan / nan, nan);
      CHECK_EQ(nan / inf, nan);
      CHECK_EQ(nan / neg_inf, nan);
      CHECK_EQ(nan / Rational{0}, nan);
      CHECK_EQ(Rational{0} / nan, nan);
      CHECK_EQ(Rational{1, 2} / nan, nan);
    }
  }

  TEST_CASE("Type Traits") {
    using Rational = mtmath::immut::Rational;
    using nl = std::numeric_limits<Rational>;

    CHECK_EQ(nl::is_specialized, true);
    CHECK_EQ(nl::is_signed, true);
    CHECK_EQ(nl::is_integer, false);
    CHECK_EQ(nl::is_exact, true);
    CHECK_EQ(nl::has_infinity, true);
    CHECK_EQ(nl::has_quiet_NaN, true);
    CHECK_EQ(nl::has_signaling_NaN, false);
    CHECK_EQ(nl::has_denorm, std::denorm_absent);
    CHECK_EQ(nl::has_denorm_loss, false);
    CHECK_EQ(nl::round_style, std::round_toward_zero);
    CHECK_EQ(nl::is_iec559, false);
    CHECK_EQ(nl::is_bounded, false);
    CHECK_EQ(nl::is_modulo, std::numeric_limits<mtmath::immut::BigInt>::is_modulo);
    CHECK_EQ(nl::digits, std::numeric_limits<mtmath::immut::BigInt>::digits);
    CHECK_EQ(nl::digits10, std::numeric_limits<mtmath::immut::BigInt>::digits10);
    CHECK_EQ(nl::max_digits10, std::numeric_limits<mtmath::immut::BigInt>::max_digits10);
    CHECK_EQ(nl::radix, std::numeric_limits<mtmath::immut::BigInt>::radix);
    CHECK_EQ(nl::min_exponent, std::numeric_limits<mtmath::immut::BigInt>::min_exponent);
    CHECK_EQ(nl::min_exponent10, std::numeric_limits<mtmath::immut::BigInt>::min_exponent10);
    CHECK_EQ(nl::min_exponent10, std::numeric_limits<mtmath::immut::BigInt>::min_exponent10);
    CHECK_EQ(nl::max_exponent, std::numeric_limits<mtmath::immut::BigInt>::max_exponent);
    CHECK_EQ(nl::max_exponent10, std::numeric_limits<mtmath::immut::BigInt>::max_exponent10);
    CHECK_EQ(nl::traps, false);
    CHECK_EQ(nl::tinyness_before, std::numeric_limits<mtmath::immut::BigInt>::tinyness_before);

    CHECK_EQ(nl::min(), -nl::infinity());
    CHECK_EQ(nl::max(), nl::infinity());
    CHECK_EQ(nl::lowest(), Rational{0});
    CHECK_EQ(nl::epsilon(), Rational{0});
    CHECK_EQ(nl::round_error(), Rational{1, 2});
    CHECK_EQ(nl::infinity(), Rational{1, 0});
    CHECK_EQ(nl::quiet_NaN(), Rational{0, 0});
    CHECK_EQ(nl::signaling_NaN(), Rational{0, 0});
    CHECK_EQ(nl::denorm_min(), -nl::infinity());
  }
}
