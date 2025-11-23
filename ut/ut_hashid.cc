#include <gtest/gtest.h>
#include <libvcs/hashid.h>

#include <sstream>

using namespace vcs;

static constexpr std::string_view STR_TEST = "test";
static constexpr std::string_view STR_HEX_ID =
    "9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08";

static HashId MakeHashId(const std::string_view data) {
  return HashId::Builder().Append(data).Build();
}

TEST(HashId, Builder) {
  EXPECT_EQ(MakeHashId("").ToHex(), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
  EXPECT_EQ(MakeHashId(STR_TEST).ToHex(), STR_HEX_ID);

  // Build from parts.
  EXPECT_EQ(HashId::Builder().Append("test").Build(), HashId::Builder().Append("te").Append("st").Build());
}

TEST(HashId, Compare) {
  EXPECT_LT(HashId::Min(), HashId::Max());
  EXPECT_EQ(HashId::Min(), HashId());
}

TEST(HashId, Empty) {
  static_assert(!bool(HashId()));
  static_assert(!bool(HashId::Zero()));

  EXPECT_EQ(HashId().ToHex(), "0000000000000000000000000000000000000000000000000000000000000000");
  EXPECT_FALSE(bool(HashId()));
}

TEST(HashId, FromBytes) {
  constexpr unsigned char data[32] = {
      1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255};
  constexpr std::string_view hex = "01000000000000000000000000000000000000000000000000000000000000ff";

  static_assert(HashId::IsBytes("00000000000000000000000000000000"));
  static_assert(HashId::Min().ToHex() ==
                std::string_view("0000000000000000000000000000000000000000000000000000000000000000"));

  // Range of raw bytes.
  EXPECT_EQ(HashId::FromBytes(data, sizeof(data))->ToHex(), hex);
  // String of raw bytes.
  EXPECT_EQ(
      HashId::FromBytes(std::string_view(reinterpret_cast<const char*>(data), sizeof(data)))->ToHex(), hex);
  // Fixed size array of raw bytes.
  EXPECT_EQ(HashId::FromBytes(data)->ToHex(), hex);
}

TEST(HashId, FromHex) {
  EXPECT_EQ(MakeHashId(STR_TEST), HashId::FromHex(STR_HEX_ID));
  EXPECT_EQ(HashId::FromHex(STR_HEX_ID)->ToHex(), STR_HEX_ID);
  EXPECT_NE(MakeHashId(STR_TEST),
      HashId::FromHex("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"));
}

TEST(HashId, IsHex) {
  EXPECT_TRUE(HashId::IsHex("9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08"));
  EXPECT_TRUE(HashId::IsHex("9f86d081884c7d659A2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08"));

  EXPECT_FALSE(HashId::IsHex("9f86d081884c7d659A2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a0z"));
  EXPECT_FALSE(HashId::IsHex("xf86d081884c7d659A2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08"));
  EXPECT_FALSE(HashId::IsHex("a94a8fe5ccb19ba61c"));
  EXPECT_FALSE(HashId::IsHex(""));
}

TEST(HashId, FormatOutput) {
  EXPECT_EQ(std::format("{}", *HashId::FromHex(STR_HEX_ID)), STR_HEX_ID);
}

TEST(HashId, StreamOutput) {
  std::stringstream ss;

  ss << *HashId::FromHex(STR_HEX_ID);

  EXPECT_EQ(ss.str(), STR_HEX_ID);
}
