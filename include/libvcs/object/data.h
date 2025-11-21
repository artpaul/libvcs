#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace vcs {

/**
 * Types of compression algorithms.
 */
enum class Compression : uint8_t {
  None = 1,
  Lz4 = 2,
};

class DataType {
 public:
  enum E : uint8_t {
    None = 0,
    /// Content object.
    Blob = 1,
    /// Tree object.
    Tree = 2,
    /// Commit object.
    Commit = 3,
    /// History adjustment object.
    Renames = 4,
    /// Tag object.
    Tag = 5,
  };

  constexpr DataType() noexcept = default;

  constexpr explicit DataType(const E e) noexcept : value_(e) {
  }

  constexpr explicit DataType(const uint8_t v) noexcept : value_(E(v)) {
  }

 public:
  constexpr bool operator==(const DataType other) const noexcept {
    return value_ == other.value_;
  }

  constexpr bool operator==(const DataType::E e) const noexcept {
    return value_ == e;
  }

  constexpr explicit operator uint8_t() const noexcept {
    return uint8_t(value_);
  }

  constexpr operator E() const noexcept {
    return value_;
  }

 private:
  E value_;
};

/// Ensure DataType is trivially constructible.
static_assert(std::is_trivially_default_constructible_v<DataType>);

/**
 * @note The data model supports objects up to 256 Terabytes in size.
 */
class DataHeader {
 public:
  /// Makes DataHeader instance.
  static constexpr DataHeader Make(const DataType type, const uint64_t size) {
    // Count number of bytes necessary to represent the size.
    const uint8_t bytes =
        size == 0 ? 0 : (std::numeric_limits<decltype(size)>::digits - std::countl_zero(size) + 7) / 8;
    DataHeader result{};
    // Type tag.
    result.tag_ = (bytes << 4) | uint8_t(type);
    // Pack size.
    switch (bytes) {
      case 8:
      case 7:
        throw std::invalid_argument("the value of the size exceeds 48 bit");
      case 6:
        result.size_[5] = (size >> 40) & 0xFF;
      case 5:
        result.size_[4] = (size >> 32) & 0xFF;
      case 4:
        result.size_[3] = (size >> 24) & 0xFF;
      case 3:
        result.size_[2] = (size >> 16) & 0xFF;
      case 2:
        result.size_[1] = (size >> 8) & 0xFF;
      case 1:
        result.size_[0] = (size & 0xFF);
    }
    return result;
  }

 public:
  /// Returns count of packed bytes.
  constexpr size_t Bytes() const noexcept {
    return 1 + ((tag_ >> 4) & 0x07);
  }

  constexpr auto Data() const noexcept -> const uint8_t (&)[8] {
    return data_;
  }

  /// Unpacks type of the object.
  constexpr DataType Type() const noexcept {
    return DataType(tag_ & 0x0F);
  }

  /// Unpacks size of the object.
  constexpr uint64_t Size() const noexcept {
    uint64_t result = 0;
    switch ((tag_ >> 4) & 0x07) {
      case 7:
        result |= uint64_t(size_[6]) << 48;
      case 6:
        result |= uint64_t(size_[5]) << 40;
      case 5:
        result |= uint64_t(size_[4]) << 32;
      case 4:
        result |= uint64_t(size_[3]) << 24;
      case 3:
        result |= uint64_t(size_[2]) << 16;
      case 2:
        result |= uint64_t(size_[1]) << 8;
      case 1:
        result |= uint64_t(size_[0]);
    }
    return result;
  }

  constexpr operator bool() const noexcept {
    return DataType(tag_ & 0x0F) != DataType::None;
  }

 private:
  // |-----------------------------------------------|
  // |       The layout of packed data header        |
  // |-----------------------------------------------|
  // |    1 bit |      3 bit | 4 bit | up to 6 bytes |
  // |----------|------------|-------|---------------|
  // | reserved | size bytes | type  |  packed size  |
  // |-----------------------------------------------|
  union {
    struct {
      uint8_t tag_;
      uint8_t size_[7];
    };

    uint8_t data_[8];
  };
};

/// Ensure the value of DataHeader is memcpy copyable.
static_assert(std::is_trivial<DataHeader>::value);

/// Ensure DataHeader::Data() returns pointer to an array of fixed size.
static_assert(std::is_bounded_array_v<std::remove_reference_t<decltype(DataHeader().Data())>>);

} // namespace vcs
