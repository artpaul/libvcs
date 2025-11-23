#pragma once

#include <cstdint>
#include <cstring>
#include <format>
#include <memory>
#include <optional>
#include <ostream>
#include <ranges>
#include <string>
#include <string_view>

#include "data.h"
#include "util/hex.h"

namespace vcs {

class HashId {
 public:
  /// Builds HashId in streamed fashion.
  class Builder {
    class Impl;

   public:
    Builder() noexcept;

    ~Builder() noexcept;

    Builder& Append(const DataHeader header) noexcept;
    Builder& Append(const void* data, const size_t len) noexcept;
    Builder& Append(const std::string_view data) noexcept;

    HashId Build() noexcept;

   private:
    Impl* Cast() noexcept {
      return reinterpret_cast<Impl*>(std::addressof(impl_));
    }

    /// Memory buffer for calculating the hash.
    alignas(uint64_t) std::byte impl_[112];
  };

 public:
  /// Copies hash data from the provided location
  static std::optional<HashId> FromBytes(const void* data, const size_t len) noexcept;

  /// Copies hash data from the provided location.
  static std::optional<HashId> FromBytes(const std::string_view data) noexcept;

  /// Copies hash data from the provided location.
  static std::optional<HashId> FromBytes(const unsigned char (&data)[32]) noexcept;

  /// Parse hex representation of the id.
  static std::optional<HashId> FromHex(const std::string_view hex) noexcept;

  /// Checks whether the string is a valid representation of an id in bytes format.
  static constexpr bool IsBytes(const std::string_view data) noexcept {
    return data.size() == sizeof(data_);
  }

  /// Checks whether the string is a valid representation of an id in hex format.
  static constexpr bool IsHex(const std::string_view hex) noexcept {
    if (hex.size() == 2 * sizeof(data_)) {
      return std::ranges::find_if_not(hex, util::IsHex) == hex.end();
    } else {
      return false;
    }
  }

  /// Makes canonical object hash.
  static HashId Make(const DataType type, const std::string_view content) noexcept {
    return Builder().Append(DataHeader::Make(type, content.size())).Append(content).Build();
  }

  /// Maximum value of HashId.
  static constexpr HashId Max() noexcept {
    HashId id;
    std::fill_n(id.data_, sizeof(id.data_), 255);
    return id;
  }

  /// Minimum value of HashId.
  static constexpr HashId Min() noexcept {
    return Zero();
  }

  /// Zero value.
  static constexpr HashId Zero() noexcept {
    return HashId();
  }

 public:
  constexpr auto Data() const noexcept -> const unsigned char (&)[32] {
    return data_;
  }

  /// Byte size of the data.
  constexpr size_t Size() const noexcept {
    return sizeof(data_);
  }

  /// Hex representation of the hash.
  constexpr std::string ToHex() const {
    return util::BytesToHex(std::span(data_));
  }

  /// Raw data of the hash.
  std::string ToBytes() const {
    return std::string(reinterpret_cast<const char*>(data_), sizeof(data_));
  }

 public:
  explicit constexpr operator bool() const noexcept {
    // Check for non null.
    if consteval {
      return std::ranges::find_if(data_, [](auto c) { return bool(c); }) != std::end(data_);
    } else {
      static constinit unsigned char zeroes[sizeof(data_)] = {};

      return std::memcmp(zeroes, data_, sizeof(data_)) != 0;
    }
  }

  bool operator<(const HashId& other) const noexcept {
    return std::memcmp(data_, other.data_, sizeof(data_)) < 0;
  }

  bool operator==(const HashId& other) const noexcept {
    if (this == &other) {
      return true;
    }
    return std::memcmp(data_, other.data_, sizeof(data_)) == 0;
  }

  friend std::ostream& operator<<(std::ostream& output, const HashId& id);

 private:
  alignas(alignof(uint64_t)) unsigned char data_[32];
};

/// Ensure the value of HashId is 32 bytes long.
static_assert(sizeof(HashId) == 32);

/// Ensure the value of HashId is memcpy copyable.
static_assert(std::is_trivially_copyable<HashId>::value);

/// Ensure HashId is 64-bit aligned.
static_assert(std::alignment_of<HashId>::value == std::alignment_of<uint64_t>::value);

/// Ensure HashId::Data() returns pointer to an array of fixed size.
static_assert(std::is_bounded_array_v<std::remove_reference_t<decltype(HashId().Data())>>);

} // namespace vcs

template <>
struct std::formatter<vcs::HashId> : std::formatter<std::string> {
  template <typename FormatContext>
  auto format(const vcs::HashId& id, FormatContext& ctx) const {
    return std::formatter<std::string>::format(id.ToHex(), ctx);
  }
};

template <>
class std::hash<vcs::HashId> {
 public:
  constexpr std::size_t operator()(const vcs::HashId& id) const noexcept {
    constexpr auto align_offset = (std::alignment_of_v<std::size_t> - std::alignment_of_v<vcs::HashId>);
    // Ensure align_offset is positive.
    static_assert(std::alignment_of_v<std::size_t> >= std::alignment_of_v<vcs::HashId>);
    // Ensure no buffer overrun.
    static_assert(sizeof(decltype(id.Data())) >= ((sizeof(std::size_t) + align_offset)));

    return *std::bit_cast<const std::size_t*>(
        std::bit_cast<const unsigned char*>(&id.Data()) + align_offset);
  }
};
