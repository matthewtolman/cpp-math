#pragma once

#include <bit>
#include <vector>
#include <compare>

namespace mtmath {
  class ByteArray {
    std::vector<uint8_t> bytes;
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

    [[nodiscard]] std::strong_ordering operator<=>(const ByteArray& o) const noexcept;
    bool operator==(const ByteArray& o) const noexcept { return *this <=> o == std::strong_ordering::equal; }

    uint8_t& operator[](size_t i) { return at(i); }
    const uint8_t& operator[](size_t i) const noexcept { return at(i); }
    uint8_t& at(size_t i) { return bytes.at(i); }
    const uint8_t& at(size_t i) const { return bytes.at(i); }

    uint8_t get(size_t i) const { return i < bytes.size() ? bytes.at(i) : 0U; }

    ByteArray& emplace_back(uint8_t byte) { bytes.emplace_back(byte); return *this; }
    ByteArray& reserve(size_t size) { bytes.reserve(size); return *this; }
    ByteArray& resize(size_t size) { bytes.resize(size); return *this; }
    size_t size() const noexcept { return bytes.size(); }
    decltype(auto) begin() const { return bytes.begin(); }
    decltype(auto) begin() { return bytes.begin(); }
    decltype(auto) end() const { return bytes.end(); }
    decltype(auto) end() { return bytes.end(); }

    void erase(decltype(bytes)::iterator begin, decltype(bytes)::iterator end) {
      std::fill(begin, end, 0);
      simplify();
    }
    void simplify();

    void clear() {
      bytes.clear();
    }

    [[nodiscard]] bool empty() const { return bytes.empty(); }

    template <typename T>
    T as() const noexcept {
      T res{};
      if constexpr (std::endian::native == std::endian::little) {
        memcpy(static_cast<void*>(&res), static_cast<const void*>(bytes.data()), std::min(sizeof(res), bytes.size()));
      }
      else {
        auto rcopy = bytes;
        std::reverse(rcopy.begin(), rcopy.end());
        memcpy(static_cast<void*>(&res), static_cast<const void*>(rcopy.data()), std::min(sizeof(res), rcopy.size()));
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
