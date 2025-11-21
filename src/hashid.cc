#include "libvcs/object/hashid.h"

#include <sha/sha256.h>

#include <ranges>

namespace vcs {

class HashId::Builder::Impl {
 public:
  Impl() noexcept {
    ::platform_SHA256_Init(&ctx_);
  }

  void Append(const void* data, const size_t len) noexcept {
    ::platform_SHA256_Update(&ctx_, static_cast<const char*>(data), len);
  }

  HashId Build() noexcept {
    HashId id;
    ::platform_SHA256_Final(id.data_, &ctx_);
    return id;
  }

 private:
  ::platform_SHA256_CTX ctx_;
};

HashId::Builder::Builder() noexcept {
  static_assert(sizeof(Builder::impl_) >= sizeof(Impl));

  new (impl_) Impl();
}

HashId::Builder::~Builder() noexcept {
  Cast()->~Impl();
}

HashId::Builder& HashId::Builder::Append(const DataHeader header) noexcept {
  Cast()->Append(header.Data(), header.Bytes());
  return *this;
}

HashId::Builder& HashId::Builder::Append(const void* data, const size_t len) noexcept {
  Cast()->Append(data, len);
  return *this;
}

HashId::Builder& HashId::Builder::Append(const std::string_view data) noexcept {
  Cast()->Append(data.data(), data.size());
  return *this;
}

HashId HashId::Builder::Build() noexcept {
  return Cast()->Build();
}

std::optional<HashId> HashId::FromBytes(const void* data, const size_t len) noexcept {
  if (len != sizeof(data_)) {
    return {};
  }

  auto id = std::make_optional<HashId>();
  std::memcpy(id->data_, data, len);
  return id;
}

std::optional<HashId> HashId::FromBytes(const std::string_view data) noexcept {
  return FromBytes(data.data(), data.size());
}

std::optional<HashId> HashId::FromBytes(const unsigned char (&data)[32]) noexcept {
  static_assert(sizeof(data) == sizeof(data_));

  auto id = std::make_optional<HashId>();
  std::memcpy(id->data_, data, sizeof(data));
  return id;
}

std::optional<HashId> HashId::FromHex(const std::string_view hex) noexcept {
  if (hex.size() != 2 * sizeof(data_)) {
    return {};
  }

  auto id = std::make_optional<HashId>();
  for (size_t i = 0; i < sizeof(data_); ++i) {
    auto hi = util::HexToByte(hex[2 * i]);
    auto lo = util::HexToByte(hex[2 * i + 1]);

    if (hi == 0xff || lo == 0xff) {
      return {};
    } else {
      id->data_[i] = hi << 4 | lo;
    }
  }
  return id;
}

std::ostream& operator<<(std::ostream& output, const HashId& id) {
  char hex[2 * sizeof(id.data_)];
  util::BytesToHex(std::span(id.data_), hex);
  output.write(hex, sizeof(hex));
  return output;
}

} // namespace vcs
