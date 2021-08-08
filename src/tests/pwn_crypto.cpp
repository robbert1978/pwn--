#include <pwn.hpp>

#include "./catch.hpp"
#define NS "pwn::crypto"


TEST_CASE("crc", "[" NS "]")
{
    std::vector<BYTE> data {0x41, 0x42, 0x43, 0x44};
    REQUIRE(pwn::crypto::crc8(data) == 0xd7);
    REQUIRE(pwn::crypto::crc16(data) == 0xf9bd);
    REQUIRE(pwn::crypto::crc32(data) == 0x80e38938);
    REQUIRE(pwn::crypto::crc64(data) == 0xbda275f960e7cf9a);
}


#ifdef __PWNLIB_WINDOWS_BUILD__

TEST_CASE("md4/5", "[" NS "]")
{
    std::vector<BYTE> data {0x41, 0x42, 0x43, 0x44};
    std::array<BYTE, MD5LEN> expected1 {0xcb, 0x08, 0xca, 0x4a, 0x7b, 0xb5, 0xf9, 0x68, 0x3c, 0x19, 0x13, 0x3a, 0x84, 0x87, 0x2c, 0xa7};
    std::array<BYTE, MD5LEN> expected2 {0x71, 0x1e, 0xe0, 0x36, 0x48, 0xa6, 0x9e, 0x29, 0x6e, 0xcc, 0x7b, 0x77, 0x07, 0xed, 0x33, 0xeb};
    std::array<BYTE, MD5LEN> expected3 {0x3f, 0x2c, 0x0a, 0xa4, 0xc0, 0x76, 0x11, 0x84, 0x6e, 0xa3, 0x3f, 0x44, 0xa8, 0xcc, 0x9e, 0x32};
    REQUIRE(pwn::crypto::md5(data) == expected1);
    REQUIRE(pwn::crypto::md2(data) == expected2);
    REQUIRE(pwn::crypto::md4(data) == expected3);
}

TEST_CASE("sha1/256/512", "[" NS "]")
{
    std::vector<BYTE> data {0x41, 0x42, 0x43, 0x44};
    std::array<BYTE, SHA1LEN> expected1 {0xfb, 0x2f, 0x85, 0xc8, 0x85, 0x67, 0xf3, 0xc8, 0xce, 0x9b, 0x79, 0x9c, 0x7c, 0x54, 0x64, 0x2d, 0xc, 0x7b, 0x41, 0xf6};
    std::array<BYTE, SHA256LEN> expected2 {0xe1, 0x2e, 0x11, 0x5a, 0xcf, 0x45, 0x52, 0xb2, 0x56, 0x8b, 0x55, 0xe9, 0x3c, 0xbd, 0x39, 0x39, 0x4c, 0x4e, 0xf8, 0x1c, 0x82, 0x44, 0x7f, 0xaf, 0xc9, 0x97, 0x88, 0x2a, 0x2, 0xd2, 0x36, 0x77};
    std::array<BYTE, SHA512LEN> expected3 {0x49, 0xec, 0x55, 0xbd, 0x83, 0xfc, 0xd6, 0x78, 0x38, 0xe3, 0xd3, 0x85, 0xce, 0x83, 0x16, 0x69, 0xe3, 0xf8, 0x15, 0xa7, 0xf4, 0x4b, 0x7a, 0xa5, 0xf8, 0xd5, 0x2b, 0x5d, 0x42, 0x35, 0x4c, 0x46,
                                           0xd8, 0x9c, 0x8b, 0x9d, 0x6,  0xe4, 0x7a, 0x79, 0x7a, 0xe4, 0xfb, 0xd2, 0x22, 0x91, 0xbe, 0x15, 0xbc, 0xc3, 0x5b, 0x7,  0x73, 0x5c, 0x4a, 0x6f, 0x92, 0x35, 0x7f, 0x93, 0xd5, 0xa3, 0x3d, 0x9b};
    REQUIRE(pwn::crypto::sha1(data) == expected1);
    REQUIRE(pwn::crypto::sha256(data) == expected2);
    REQUIRE(pwn::crypto::sha512(data) == expected3);
}

#endif