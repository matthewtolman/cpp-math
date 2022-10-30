#pragma once

#include <bit>
#include <vector>

namespace mtmath {
  class ByteArray {
    std::vector<uint8_t> bytes;

    void simplify();
  public:
    ByteArray() = default;
    explicit ByteArray(std::vector<uint8_t> bytes) : bytes(std::move(bytes)) {}

    [[nodiscard]] int compare(const std::vector<uint8_t>& b) const noexcept;
    [[nodiscard]] int compare(const ByteArray& o) const noexcept { return compare(o.bytes); }

    ByteArray operator&(const std::vector<uint8_t>& b) const noexcept;
    ByteArray operator|(const std::vector<uint8_t>& b) const noexcept;
    ByteArray operator^(const std::vector<uint8_t>& b) const noexcept;
    ByteArray& operator&=(const std::vector<uint8_t>& b) noexcept;
    ByteArray& operator|=(const std::vector<uint8_t>& b) noexcept;
    ByteArray& operator^=(const std::vector<uint8_t>& b) noexcept;

    ByteArray operator&(const ByteArray& b) const noexcept { return *this & b.bytes; }
    ByteArray& operator&=(const ByteArray& b) noexcept { return *this &= b.bytes; }
    ByteArray operator|(const ByteArray& b) const noexcept { return *this | b.bytes; }
    ByteArray& operator|=(const ByteArray& b) noexcept { return *this |= b.bytes; }
    ByteArray operator^(const ByteArray& b) const noexcept { return *this ^ b.bytes; }
    ByteArray& operator^=(const ByteArray& b) noexcept { return *this ^= b.bytes; }

    ByteArray operator<<(size_t amount) const;
    ByteArray& operator<<=(size_t amount);
    ByteArray operator>>(size_t amount) const;
    ByteArray& operator>>=(size_t amount);

    bool operator==(const std::vector<uint8_t>&b) const noexcept { return compare(b) == 0; }
    bool operator==(const ByteArray& o) const noexcept { return compare(o) == 0; }
    bool operator<=(const std::vector<uint8_t>&b) const noexcept { return compare(b) <= 0; }
    bool operator<=(const ByteArray& o) const noexcept { return compare(o) <= 0; }
    bool operator>=(const std::vector<uint8_t>&b) const noexcept { return compare(b) >= 0; }
    bool operator>=(const ByteArray& o) const noexcept { return compare(o) >= 0; }
    bool operator<(const std::vector<uint8_t>&b) const noexcept { return compare(b) < 0; }
    bool operator<(const ByteArray& o) const noexcept { return compare(o) < 0; }
    bool operator>(const std::vector<uint8_t>&b) const noexcept { return compare(b) > 0; }
    bool operator>(const ByteArray& o) const noexcept { return compare(o) > 0; }
    bool operator!=(const std::vector<uint8_t>&b) const noexcept { return compare(b) != 0; }
    bool operator!=(const ByteArray& o) const noexcept { return compare(o) != 0; }

    template <typename T>
    T as() const noexcept {
      T res{};
      if constexpr (std::endian::native == std::endian::little) {
        memcpy(&res, &bytes[0], std::min(sizeof(res), bytes.size()));
      }
      else {
        auto rcopy = bytes;
        std::reverse(rcopy.begin(), rcopy.end());
        memcpy(&res, &rcopy[0], std::min(sizeof(res), rcopy.size()));
      }
      return res;
    }

    template <typename T>
    static ByteArray from(T value) {
      ByteArray res;
      res.bytes.resize(sizeof(value));
      memcpy(&res.bytes[0], &value, std::min(sizeof(res), res.bytes.size()));
      if constexpr (std::endian::native != std::endian::little) {
        std::reverse(res.bytes.begin(), res.bytes.end());
      }
      res.simplify();
      return res;
    }
  };
}
