#include "impl/byte_array.h"
#include "doctest.h"

TEST_SUITE("Byte Array") {
  TEST_CASE("Initialize from vector") {
    auto ba = mtmath::ByteArray{{0x1f, 0x2f, 0x3f, 0x4f}};
    auto num = ba.as<uint64_t>();
    CHECK_EQ(num, 0x4f3f2f1f);
  }

  TEST_CASE("Initialize from long") {
    auto ba = mtmath::ByteArray::from<uint64_t>(0x4f3f2f1f);
    auto num = ba.as<uint64_t>();
    CHECK_EQ(num, 0x4f3f2f1f);
  }

  TEST_CASE("& operator") {
    auto ba = mtmath::ByteArray::from<uint64_t>(0xffffff0f);
    ba = ba & mtmath::ByteArray::from<uint64_t>(0x1f003f);
    auto num = ba.as<uint64_t>();
    CHECK_EQ(num, 0x001f000f);
  }

  TEST_CASE("| operator") {
    auto ba = mtmath::ByteArray::from<uint64_t>(0x30);
    ba = ba | mtmath::ByteArray::from<uint64_t>(0x001f003f);
    auto num = ba.as<uint64_t>();
    CHECK_EQ(num, 0x001f003f);
  }

  TEST_CASE("^ operator") {
    auto ba = mtmath::ByteArray::from<uint64_t>(0x30);
    ba = ba ^ mtmath::ByteArray::from<uint64_t>(0x001f003f);
    auto num = ba.as<uint64_t>();
    CHECK_EQ(num, 0x001f000f);
  }

  TEST_CASE("&= operator") {
    auto ba = mtmath::ByteArray::from<uint64_t>(0xffffff0f);
    ba &= mtmath::ByteArray::from<uint64_t>(0x1f003f);
    auto num = ba.as<uint64_t>();
    CHECK_EQ(num, 0x001f000f);
  }

  TEST_CASE("|= operator") {
    auto ba = mtmath::ByteArray::from<uint64_t>(0x30);
    ba |= mtmath::ByteArray::from<uint64_t>(0x001f003f);
    auto num = ba.as<uint64_t>();
    CHECK_EQ(num, 0x001f003f);
  }

  TEST_CASE("^= operator") {
    auto ba = mtmath::ByteArray::from<uint64_t>(0x30);
    ba ^= mtmath::ByteArray::from<uint64_t>(0x001f003f);
    auto num = ba.as<uint64_t>();
    CHECK_EQ(num, 0x001f000f);
  }

  TEST_CASE("[] operator") {
    auto ba = mtmath::ByteArray::from<uint64_t>(0x1030);
    CHECK_EQ(ba[0], 0x30);
    CHECK_EQ(ba[1], 0x10);
    const auto& ba2 = ba;
    CHECK_EQ(ba2[0], 0x30);
    CHECK_EQ(ba2[1], 0x10);
  }

  TEST_CASE("get") {
    auto ba = mtmath::ByteArray::from<uint64_t>(0x1030);
    CHECK_EQ(ba.get(0), 0x30);
    CHECK_EQ(ba.get(1), 0x10);
    CHECK_EQ(ba.get(1000), 0x0);
  }

  TEST_CASE("const range") {
    const auto ba = mtmath::ByteArray::from<uint64_t>(0x1030);
    auto it = 0;
    for (const auto& b : ba) {
      ++it;
    }
    CHECK_EQ(it, 2);
  }

  TEST_CASE("Erase") {
    auto ba = mtmath::ByteArray::from<uint64_t>(0x123456789);
    ba.erase(ba.begin() + 1, ba.begin() + 2);
    auto num = ba.as<uint64_t>();
    CHECK_EQ(num, 0x123450089);
  }

  TEST_CASE("<< operator") {
    SUBCASE("Shift Left 16") {
      auto ba = mtmath::ByteArray::from<uint64_t>(0x01020304);
      ba <<= 16;
      auto num = ba.as<uint64_t>();
      CHECK_EQ(num, 0x010203040000);
    }

    SUBCASE("Shift Left 4") {
      auto ba = mtmath::ByteArray::from<uint64_t>(0x01020304);
      ba <<= 4;
      auto num = ba.as<uint64_t>();
      CHECK_EQ(num, 0x10203040);
    }

    SUBCASE("Shift Left 15 Simple") {
      uint64_t orig = 0x00ffff;
      auto ba = mtmath::ByteArray::from<uint64_t>(orig);
      ba <<= 15;
      auto num = ba.as<uint64_t>();
      uint64_t expected = orig << 15;
      CHECK_EQ(num, expected);
    }

    SUBCASE("Shift Left 15 Complex") {
      auto ba = mtmath::ByteArray::from<uint64_t>(0xae8472818U);
      ba <<= 15;
      auto num = ba.as<uint64_t>();
      CHECK_EQ(num, 0xae8472818U << 15);
    }

    SUBCASE("Shift Left 4 Across Boundaries") {
      auto ba = mtmath::ByteArray::from<uint64_t>(0x10203040);
      ba <<= 4;
      auto num = ba.as<uint64_t>();
      CHECK_EQ(num, 0x102030400);
    }

    SUBCASE("Shift Left 3") {
      auto ba = mtmath::ByteArray::from<uint64_t>(0xae8472818U);
      ba <<= 3;
      auto num = ba.as<uint64_t>();
      CHECK_EQ(num, 0xae8472818U << 3);
    }
  }

  TEST_CASE(">> operator") {
    SUBCASE("Shift Right 16") {
      auto ba = mtmath::ByteArray::from<uint64_t>(0x01020304);
      ba >>= 16;
      auto num = ba.as<uint64_t>();
      CHECK_EQ(num, 0x0102);
    }

    SUBCASE("Shift Right 4 Across Boundaries") {
      auto ba = mtmath::ByteArray::from<uint64_t>(0x01020304);
      ba >>= 4;
      auto num = ba.as<uint64_t>();
      CHECK_EQ(num, 0x102030);
    }

    SUBCASE("Shift Right 15 Simple") {
      uint64_t orig = 0x00ffff;
      auto ba = mtmath::ByteArray::from<uint64_t>(orig);
      ba >>= 15;
      auto num = ba.as<uint64_t>();
      uint64_t expected = orig >> 15;
      CHECK_EQ(num, expected);
    }

    SUBCASE("Shift Right 15 Complex") {
      auto ba = mtmath::ByteArray::from<uint64_t>(0xae8472818U);
      ba >>= 15;
      auto num = ba.as<uint64_t>();
      CHECK_EQ(num, 0xae8472818U >> 15);
    }

    SUBCASE("Shift Right 4") {
      auto ba = mtmath::ByteArray::from<uint64_t>(0x10203040);
      ba >>= 4;
      auto num = ba.as<uint64_t>();
      CHECK_EQ(num, 0x1020304);
    }

    SUBCASE("Shift Right 3") {
      auto ba = mtmath::ByteArray::from<uint64_t>(0xae8472818U);
      ba >>= 3;
      auto num = ba.as<uint64_t>();
      CHECK_EQ(num, 0xae8472818U >> 3);
    }

    SUBCASE("Shift Right 3000") {
      auto ba = mtmath::ByteArray::from<uint64_t>(0xae8472818U);
      ba >>= 3000;
      auto num = ba.as<uint64_t>();
      CHECK_EQ(num, 0);
    }
  }

  TEST_CASE("Comparison") {
    CHECK_EQ(mtmath::ByteArray::from<uint64_t>(0x30), mtmath::ByteArray::from<uint64_t>(0x30));
    CHECK_NE(mtmath::ByteArray::from<uint64_t>(0x30), mtmath::ByteArray::from<uint64_t>(0x1030));
    CHECK_EQ(mtmath::ByteArray::from<uint64_t>(0x30) <=> mtmath::ByteArray::from<uint64_t>(0x30), std::strong_ordering::equal);
    CHECK_EQ(mtmath::ByteArray::from<uint64_t>(0x30) <=> mtmath::ByteArray::from<uint64_t>(0x1030), std::strong_ordering::less);
    CHECK_EQ(mtmath::ByteArray::from<uint64_t>(0x1030) <=> mtmath::ByteArray::from<uint64_t>(0x30), std::strong_ordering::greater);
    CHECK_EQ(mtmath::ByteArray::from<uint64_t>(0x30) <=> mtmath::ByteArray::from<uint64_t>(0x31), std::strong_ordering::less);
    CHECK_EQ(mtmath::ByteArray::from<uint64_t>(0x31) <=> mtmath::ByteArray::from<uint64_t>(0x30), std::strong_ordering::greater);
  }
}
