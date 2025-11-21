#include <gtest/gtest.h>
#include <libvcs/util/hex.h>

#include <sstream>

TEST(Util, IsHex) {
  EXPECT_TRUE(util::IsHex('A'));
  EXPECT_TRUE(util::IsHex('a'));

  EXPECT_FALSE(util::IsHex('Z'));

  static_assert(util::IsHex('5') == true);
  static_assert(util::IsHex('z') == false);
}

TEST(Util, HexToByte) {
  EXPECT_EQ(util::HexToByte('A'), 10u);
  EXPECT_EQ(util::HexToByte('Z'), 255u);

  static_assert(util::HexToByte('5') == 5u);
  static_assert(util::HexToByte('z') == 255u);
}

TEST(Util, BytesToHex) {
  EXPECT_EQ(util::BytesToHex(std::span("\x34\xff", 2)), std::string_view("34ff"));

  static_assert(util::BytesToHex(std::span("\x34\xff", 2)) == std::string_view("34ff"));
}
