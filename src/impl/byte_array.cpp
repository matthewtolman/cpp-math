#include "byte_array.h"

int mtmath::ByteArray::compare(const std::vector<uint8_t> &b) const noexcept {
  if (bytes.size() != b.size()) {
    return bytes.size() < b.size() ? -1 : 1;
  }
  for (size_t i = 0; i < b.size(); ++i) {
    if (bytes.at(i) != b.at(i)) {
      return bytes.at(i) < b.at(i) ? -1 : 1;
    }
  }
  return 0;
}

mtmath::ByteArray mtmath::ByteArray::operator&(const std::vector<uint8_t> &b) const noexcept {
  auto copy = *this;
  copy &= b;
  return copy;
}

mtmath::ByteArray mtmath::ByteArray::operator|(const std::vector<uint8_t> &b) const noexcept {
  auto copy = *this;
  copy |= b;
  return copy;
}

mtmath::ByteArray mtmath::ByteArray::operator^(const std::vector<uint8_t> &b) const noexcept {
  auto copy = *this;
  copy ^= b;
  return copy;
}

mtmath::ByteArray& mtmath::ByteArray::operator&=(const std::vector<uint8_t> &b) noexcept {
  for (size_t i = 0; i < b.size() && i < bytes.size(); ++i) {
    bytes[i] &= b[i];
  }
  if (bytes.size() > b.size()) {
    bytes.resize(b.size());
  }
  return *this;
}

mtmath::ByteArray& mtmath::ByteArray::operator|=(const std::vector<uint8_t> &b) noexcept {
  if (bytes.size() < b.size()) {
    bytes.resize(b.size());
  }
  for (size_t i = 0; i < b.size() && i < bytes.size(); ++i) {
    bytes[i] |= b[i];
  }
  simplify();
  return *this;
}

mtmath::ByteArray& mtmath::ByteArray::operator^=(const std::vector<uint8_t> &b) noexcept {
  if (bytes.size() < b.size()) {
    bytes.resize(b.size());
  }
  for (size_t i = 0; i < b.size() && i < bytes.size(); ++i) {
    bytes[i] ^= b[i];
  }
  simplify();
  return *this;
}

void mtmath::ByteArray::simplify() {
  for (size_t i = bytes.size(); i > 0; --i) {
    if (bytes.at(i - 1) != 0) {
      if (i != bytes.size()) {
        bytes.erase(bytes.begin() + static_cast<std::vector<uint8_t>::difference_type>(i), bytes.end());
        bytes.shrink_to_fit();
      }
      return;
    }
  }
}
mtmath::ByteArray mtmath::ByteArray::operator<<(size_t amount) const {
  auto copy = *this;
  copy <<= amount;
  return copy;
}
mtmath::ByteArray mtmath::ByteArray::operator>>(size_t amount) const {
  auto copy = *this;
  copy >>= amount;
  return copy;
}

mtmath::ByteArray& mtmath::ByteArray::operator<<=(size_t amount) {
  // Doing right shifts since we store bits backwards
  // (makes mem copying on little endian systems more efficient)
  size_t numBytesToShift = amount / 8;
  size_t numInnerShifts = amount % 8;

  if (numBytesToShift) {
    bytes.insert(bytes.begin(), numBytesToShift, 0x00U);
  }

  if (numInnerShifts) {
    uint8_t carry = 0;
    uint8_t carryShift = 8 - numInnerShifts;
    uint8_t carryMask = (0xffU << carryShift);// rotate off the bits we aren't carrying over
    for (auto &byte: bytes) {
      auto newCarry = (byte & carryMask) >> carryShift;
      byte <<= numInnerShifts;
      byte |= carry;
      carry = newCarry;
    }
    if (carry) {
      bytes.emplace_back(carry);
    }
  }
  return *this;
}
mtmath::ByteArray& mtmath::ByteArray::operator>>=(size_t amount) {
  // Doing right shifts since we store bits backwards
  // (makes mem copying on little endian systems more efficient)
  size_t numBytesToShift = amount / 8;
  size_t numInnerShifts = amount % 8;

  if (numInnerShifts) {
    uint8_t carry = 0;
    uint8_t carryShift = 8 - numInnerShifts;
    uint8_t carryMask = ~(0xffU << numInnerShifts);// rotate off the bits we aren't carrying over
    for (auto iter = bytes.rbegin(); iter != bytes.rend(); ++iter) {
      auto newCarry = ((*iter) & carryMask) << carryShift;
      (*iter) >>= numInnerShifts;
      (*iter) |= carry;
      carry = newCarry;
    }
  }

  if (numBytesToShift) {
    if (numBytesToShift >= bytes.size()) {
      bytes.clear();
    }
    else {
      bytes.end() = bytes.erase(bytes.begin(), bytes.begin() + numBytesToShift);
    }
  }
  return *this;
}
