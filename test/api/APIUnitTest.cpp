// SPDX-License-Identifier: Apache-2.0
#include "api/ssvm.h"

#include "gtest/gtest.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

TEST(APICoreTest, Version) {
  EXPECT_EQ(std::string(SSVM_VERSION), std::string(SSVM_VersionGet()));
  EXPECT_EQ(SSVM_VERSION_MAJOR, SSVM_VersionGetMajor());
  EXPECT_EQ(SSVM_VERSION_MINOR, SSVM_VersionGetMinor());
  EXPECT_EQ(SSVM_VERSION_PATCH, SSVM_VersionGetPatch());
}

TEST(APICoreTest, Log) {
  SSVM_LogSetDebugLevel();
  EXPECT_TRUE(true);
  SSVM_LogSetErrorLevel();
  EXPECT_TRUE(true);
}

TEST(APICoreTest, Value) {
  std::vector<uint32_t> Vec = {1U, 2U, 3U};
  SSVM_Value Val = SSVM_ValueGenI32(INT32_MAX);
  EXPECT_EQ(SSVM_ValueGetI32(Val), INT32_MAX);
  Val = SSVM_ValueGenI64(INT64_MAX);
  EXPECT_EQ(SSVM_ValueGetI64(Val), INT64_MAX);
  Val = SSVM_ValueGenF32(1 / 0.0f);
  EXPECT_EQ(SSVM_ValueGetF32(Val), 1 / 0.0f);
  Val = SSVM_ValueGenF64(-1 / 0.0f);
  EXPECT_EQ(SSVM_ValueGetF64(Val), -1 / 0.0f);
  Val = SSVM_ValueGenV128(static_cast<__int128>(INT64_MAX) * 2 + 1);
  EXPECT_EQ(SSVM_ValueGetV128(Val), static_cast<__int128>(INT64_MAX) * 2 + 1);
  Val = SSVM_ValueGenNullRef(SSVM_RefType_FuncRef);
  EXPECT_TRUE(SSVM_ValueIsNullRef(Val));
  Val = SSVM_ValueGenFuncRef(123U);
  EXPECT_EQ(SSVM_ValueGetFuncIdx(Val), 123U);
  Val = SSVM_ValueGenExternRef(&Vec);
  EXPECT_EQ(static_cast<std::vector<uint32_t> *>(SSVM_ValueGetExternRef(Val))
                ->data()[1],
            2U);
}

TEST(APICoreTest, String) {
  /// Test to delete nullptr.
  SSVM_String Str = {.Length = 0, .Buf = nullptr};
  SSVM_StringDelete(Str);
  EXPECT_TRUE(true);
  /// Test strings.
  SSVM_String Str1 = SSVM_StringCreateByCString("test_string");
  SSVM_String Str2 = SSVM_StringCreateByCString("test_string");
  EXPECT_TRUE(SSVM_StringIsEqual(Str1, Str2));
  const char CStr[] = "test_string_.....";
  SSVM_String Str3 = SSVM_StringCreateByBuffer(CStr, 11);
  EXPECT_TRUE(SSVM_StringIsEqual(Str1, Str3));
  SSVM_String Str4 = SSVM_StringWrap(CStr, 11);
  EXPECT_TRUE(SSVM_StringIsEqual(Str3, Str4));
  SSVM_StringDelete(Str1);
  SSVM_StringDelete(Str2);
  SSVM_StringDelete(Str3);
}

TEST(APICoreTest, Result) {
  SSVM_Result Res1 = {.Code = 0x00}; /// Success
  SSVM_Result Res2 = {.Code = 0x01}; /// Terminated -> Success
  SSVM_Result Res3 = {.Code = 0x02}; /// Failed
  EXPECT_TRUE(SSVM_ResultOK(Res1));
  EXPECT_TRUE(SSVM_ResultOK(Res2));
  EXPECT_FALSE(SSVM_ResultOK(Res3));
  EXPECT_EQ(SSVM_ResultGetCode(Res1), 0x00U);
  EXPECT_EQ(SSVM_ResultGetCode(Res2), 0x01U);
  EXPECT_EQ(SSVM_ResultGetCode(Res3), 0x02U);
  EXPECT_NE(SSVM_ResultGetMessage(Res1), nullptr);
  EXPECT_NE(SSVM_ResultGetMessage(Res2), nullptr);
  EXPECT_NE(SSVM_ResultGetMessage(Res3), nullptr);
}

TEST(APICoreTest, Configure) {
  SSVM_ConfigureContext *ConfNull = nullptr;
  SSVM_ConfigureContext *Conf = SSVM_ConfigureCreate();
  /// Tests for proposals.
  SSVM_ConfigureAddProposal(ConfNull, SSVM_Proposal_SIMD);
  SSVM_ConfigureAddProposal(Conf, SSVM_Proposal_SIMD);
  SSVM_ConfigureAddProposal(ConfNull, SSVM_Proposal_Memory64);
  SSVM_ConfigureAddProposal(Conf, SSVM_Proposal_Memory64);
  EXPECT_FALSE(SSVM_ConfigureHasProposal(ConfNull, SSVM_Proposal_SIMD));
  EXPECT_TRUE(SSVM_ConfigureHasProposal(Conf, SSVM_Proposal_SIMD));
  EXPECT_FALSE(SSVM_ConfigureHasProposal(ConfNull, SSVM_Proposal_Memory64));
  EXPECT_TRUE(SSVM_ConfigureHasProposal(Conf, SSVM_Proposal_Memory64));
  SSVM_ConfigureRemoveProposal(Conf, SSVM_Proposal_SIMD);
  SSVM_ConfigureRemoveProposal(ConfNull, SSVM_Proposal_SIMD);
  EXPECT_FALSE(SSVM_ConfigureHasProposal(ConfNull, SSVM_Proposal_SIMD));
  EXPECT_FALSE(SSVM_ConfigureHasProposal(Conf, SSVM_Proposal_SIMD));
  EXPECT_FALSE(SSVM_ConfigureHasProposal(ConfNull, SSVM_Proposal_Memory64));
  EXPECT_TRUE(SSVM_ConfigureHasProposal(Conf, SSVM_Proposal_Memory64));
  /// Tests for host registrations.
  SSVM_ConfigureAddHostRegistration(ConfNull, SSVM_HostRegistration_Wasi);
  SSVM_ConfigureAddHostRegistration(Conf, SSVM_HostRegistration_Wasi);
  EXPECT_FALSE(
      SSVM_ConfigureHasHostRegistration(ConfNull, SSVM_HostRegistration_Wasi));
  EXPECT_TRUE(
      SSVM_ConfigureHasHostRegistration(Conf, SSVM_HostRegistration_Wasi));
  SSVM_ConfigureRemoveHostRegistration(ConfNull, SSVM_HostRegistration_Wasi);
  SSVM_ConfigureRemoveHostRegistration(Conf, SSVM_HostRegistration_Wasi);
  EXPECT_FALSE(
      SSVM_ConfigureHasHostRegistration(ConfNull, SSVM_HostRegistration_Wasi));
  EXPECT_FALSE(
      SSVM_ConfigureHasHostRegistration(Conf, SSVM_HostRegistration_Wasi));
  /// Tests for memory limits.
  SSVM_ConfigureSetMaxMemoryPage(ConfNull, 1234U);
  SSVM_ConfigureSetMaxMemoryPage(Conf, 1234U);
  EXPECT_NE(SSVM_ConfigureGetMaxMemoryPage(ConfNull), 1234U);
  EXPECT_EQ(SSVM_ConfigureGetMaxMemoryPage(Conf), 1234U);
  /// Test to delete nullptr.
  SSVM_ConfigureDelete(ConfNull);
  EXPECT_TRUE(true);
  SSVM_ConfigureDelete(Conf);
  EXPECT_TRUE(true);
}

} // namespace

GTEST_API_ int main(int argc, char **argv) {
  SSVM_LogSetErrorLevel();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
