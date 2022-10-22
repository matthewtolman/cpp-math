#include "impl/rational.h"
#include "doctest.h"

TEST_SUITE("Rationals") {
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
}
