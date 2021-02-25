// SPDX-License-Identifier: Apache-2.0
//===-- ssvm/test/api/APIVMCoreTest.cpp - SSVM C API core tests -----------===//
//
// Part of the SSVM Project.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains tests of Wasm test suites extracted by wast2json.
/// Test Suits: https://github.com/WebAssembly/spec/tree/master/test/core
/// wast2json: https://webassembly.github.io/wabt/doc/wast2json.1.html
///
//===----------------------------------------------------------------------===//

#include "api/ssvm.h"

#include "../spec/spectest.h"
#include "hostfunc_c.h"

#include "gtest/gtest.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace {

using namespace std::literals;
using namespace SSVM;
static SpecTest T(std::filesystem::u8path("../spec/testSuites"sv));
static Proposal ProposalList[] = {Proposal::Annotations,
                                  Proposal::BulkMemoryOperations,
                                  Proposal::ExceptionHandling,
                                  Proposal::FunctionReferences,
                                  Proposal::Memory64,
                                  Proposal::ReferenceTypes,
                                  Proposal::SIMD,
                                  Proposal::TailCall,
                                  Proposal::Threads};

SSVM_ConfigureContext *createConf(const Configure &Conf) {
  auto *Cxt = SSVM_ConfigureCreate();
  for (auto &I : ProposalList) {
    if (Conf.hasProposal(I)) {
      SSVM_ConfigureAddProposal(Cxt, static_cast<SSVM_Proposal>(I));
    }
  }
  return Cxt;
}

ErrCode convResult(SSVM_Result Res) { return static_cast<ErrCode>(Res.Code); }

std::vector<ValVariant> convToValVec(const std::vector<SSVM_Value> &CVals) {
  std::vector<ValVariant> Vals(CVals.size());
  std::transform(CVals.cbegin(), CVals.cend(), Vals.begin(),
                 [](const SSVM_Value &Val) { return ValVariant(Val.Value); });
  return Vals;
}
std::vector<SSVM_Value> convFromValVec(const std::vector<ValVariant> &Vals) {
  std::vector<SSVM_Value> CVals(Vals.size());
  std::transform(
      Vals.cbegin(), Vals.cend(), CVals.begin(), [](const ValVariant &Val) {
        return SSVM_Value{.Value = retrieveValue<unsigned __int128>(Val)};
      });
  return CVals;
}

/// Parameterized testing class.
class CoreTest : public testing::TestWithParam<std::string> {};

TEST_P(CoreTest, TestSuites) {
  const auto [Proposal, Conf, UnitName] = T.resolve(GetParam());
  SSVM_ConfigureContext *ConfCxt = createConf(Conf);
  SSVM_VMContext *VM = SSVM_VMCreate(ConfCxt, nullptr);
  SSVM_ConfigureDelete(ConfCxt);
  SSVM_ImportObjectContext *TestModCxt = createSpecTestModule();
  SSVM_VMRegisterModuleFromImport(VM, TestModCxt);

  T.onModule = [&VM](const std::string &ModName,
                     const std::string &Filename) -> Expect<void> {
    SSVM_Result Res;
    if (!ModName.empty()) {
      SSVM_String ModStr =
          SSVM_StringCreateByBuffer(ModName.data(), ModName.length());
      Res = SSVM_VMRegisterModuleFromFile(VM, ModStr, Filename.c_str());
      SSVM_StringDelete(ModStr);
    } else {
      Res = SSVM_VMLoadWasmFromFile(VM, Filename.c_str());
      if (!SSVM_ResultOK(Res)) {
        return Unexpect(convResult(Res));
      }
      Res = SSVM_VMValidate(VM);
      if (!SSVM_ResultOK(Res)) {
        return Unexpect(convResult(Res));
      }
      Res = SSVM_VMInstantiate(VM);
    }
    if (!SSVM_ResultOK(Res)) {
      return Unexpect(convResult(Res));
    }
    return {};
  };
  T.onValidate = [&VM](const std::string &Filename) -> Expect<void> {
    SSVM_Result Res = SSVM_VMLoadWasmFromFile(VM, Filename.c_str());
    if (!SSVM_ResultOK(Res)) {
      return Unexpect(convResult(Res));
    }
    Res = SSVM_VMValidate(VM);
    if (!SSVM_ResultOK(Res)) {
      return Unexpect(convResult(Res));
    }
    return {};
  };
  T.onInstantiate = [&VM](const std::string &Filename) -> Expect<void> {
    SSVM_Result Res = SSVM_VMLoadWasmFromFile(VM, Filename.c_str());
    if (!SSVM_ResultOK(Res)) {
      return Unexpect(convResult(Res));
    }
    Res = SSVM_VMValidate(VM);
    if (!SSVM_ResultOK(Res)) {
      return Unexpect(convResult(Res));
    }
    Res = SSVM_VMInstantiate(VM);
    if (!SSVM_ResultOK(Res)) {
      return Unexpect(convResult(Res));
    }
    return {};
  };
  /// Helper function to call functions.
  T.onInvoke = [&VM](const std::string &ModName, const std::string &Field,
                     const std::vector<ValVariant> &Params)
      -> Expect<std::vector<ValVariant>> {
    SSVM_Result Res;
    std::vector<SSVM_Value> CParams = convFromValVec(Params);
    std::vector<SSVM_Value> CReturns;
    SSVM_String FieldStr =
        SSVM_StringCreateByBuffer(Field.data(), Field.length());
    if (!ModName.empty()) {
      /// Invoke function of named module. Named modules are registered in
      /// Store Manager.
      /// Get the function type to specify the return nums.
      SSVM_String ModStr =
          SSVM_StringCreateByBuffer(ModName.data(), ModName.length());
      SSVM_FunctionTypeContext *FuncType =
          SSVM_VMGetFunctionTypeRegistered(VM, ModStr, FieldStr);
      if (FuncType == nullptr) {
        SSVM_StringDelete(FieldStr);
        return Unexpect(ErrCode::FuncNotFound);
      }
      CReturns.resize(SSVM_FunctionTypeGetReturnsLength(FuncType));
      SSVM_FunctionTypeDelete(FuncType);
      /// Execute.
      Res = SSVM_VMExecuteRegistered(VM, ModStr, FieldStr, &CParams[0],
                                     CParams.size(), &CReturns[0],
                                     CReturns.size());
      SSVM_StringDelete(ModStr);
    } else {
      /// Invoke function of anonymous module. Anonymous modules are
      /// instantiated in VM.
      /// Get function type to specify the return nums.
      SSVM_FunctionTypeContext *FuncType = SSVM_VMGetFunctionType(VM, FieldStr);
      if (FuncType == nullptr) {
        SSVM_StringDelete(FieldStr);
        return Unexpect(ErrCode::FuncNotFound);
      }
      CReturns.resize(SSVM_FunctionTypeGetReturnsLength(FuncType));
      SSVM_FunctionTypeDelete(FuncType);
      /// Execute.
      Res = SSVM_VMExecute(VM, FieldStr, &CParams[0], CParams.size(),
                           &CReturns[0], CReturns.size());
    }
    SSVM_StringDelete(FieldStr);
    if (!SSVM_ResultOK(Res)) {
      return Unexpect(convResult(Res));
    }
    return convToValVec(CReturns);
  };
  /// Helper function to get values.
  T.onGet = [&VM](const std::string &ModName,
                  const std::string &Field) -> Expect<std::vector<ValVariant>> {
    /// Get module instance.
    SSVM_StoreContext *StoreCxt = SSVM_VMGetStoreContext(VM);
    SSVM_ModuleInstanceContext *ModCxt = nullptr;
    if (ModName.empty()) {
      ModCxt = SSVM_StoreGetActiveModule(StoreCxt);
    } else {
      SSVM_String ModStr =
          SSVM_StringCreateByBuffer(ModName.data(), ModName.length());
      ModCxt = SSVM_StoreFindModule(StoreCxt, ModStr);
      SSVM_StringDelete(ModStr);
    }
    if (ModCxt == nullptr) {
      return Unexpect(ErrCode::WrongInstanceAddress);
    }
    /// Get global instance.
    uint32_t ExportLength = SSVM_ModuleInstanceGetGlobalExportsLength(ModCxt);
    if (ExportLength == 0) {
      return Unexpect(ErrCode::WrongInstanceAddress);
    }
    uint32_t GlobAddrIdx = ExportLength;
    std::vector<SSVM_String> Names(ExportLength);
    std::vector<SSVM_InstanceAddress> Addrs(ExportLength);
    SSVM_ModuleInstanceGetGlobalExports(ModCxt, &Names[0], &Addrs[0],
                                        ExportLength);
    SSVM_String FieldStr =
        SSVM_StringCreateByBuffer(Field.data(), Field.length());
    for (uint32_t I = 0; I < ExportLength; I++) {
      if (SSVM_StringIsEqual(Names[I], FieldStr)) {
        GlobAddrIdx = I;
      }
      SSVM_StringDelete(Names[I]);
    }
    SSVM_StringDelete(FieldStr);
    if (GlobAddrIdx >= ExportLength) {
      return Unexpect(ErrCode::WrongInstanceAddress);
    }
    SSVM_GlobalInstanceContext *GlobCxt =
        SSVM_StoreGetGlobal(StoreCxt, Addrs[GlobAddrIdx]);
    return std::vector<ValVariant>{SSVM_GlobalInstanceGetValue(GlobCxt).Value};
  };

  T.run(Proposal, UnitName);

  SSVM_VMDelete(VM);
  SSVM_ImportObjectDelete(TestModCxt);
}

/// Initiate test suite.
INSTANTIATE_TEST_SUITE_P(TestUnit, CoreTest, testing::ValuesIn(T.enumerate()));
} // namespace

GTEST_API_ int main(int argc, char **argv) {
  SSVM_LogSetErrorLevel();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
