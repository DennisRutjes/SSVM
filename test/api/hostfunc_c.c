// SPDX-License-Identifier: Apache-2.0
//===-- ssvm/test/api/hostfunc_c.c - Spec test host functions for C API ---===//
//
// Part of the SSVM Project.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file parse and run tests of Wasm test suites extracted by wast2json.
/// Test Suits: https://github.com/WebAssembly/spec/tree/master/test/core
/// wast2json: https://webassembly.github.io/wabt/doc/wast2json.1.html
///
//===----------------------------------------------------------------------===//

#include "hostfunc_c.h"

void SpecTestPrint(SSVM_MemoryInstanceContext *Cxt, const SSVM_Value *In,
                   SSVM_Value *Out) {}

void SpecTestPrintI32(SSVM_MemoryInstanceContext *Cxt, const SSVM_Value *In,
                      SSVM_Value *Out) {}

void SpecTestPrintF32(SSVM_MemoryInstanceContext *Cxt, const SSVM_Value *In,
                      SSVM_Value *Out) {}

void SpecTestPrintF64(SSVM_MemoryInstanceContext *Cxt, const SSVM_Value *In,
                      SSVM_Value *Out) {}

void SpecTestPrintI32F32(SSVM_MemoryInstanceContext *Cxt, const SSVM_Value *In,
                         SSVM_Value *Out) {}

void SpecTestPrintF64F64(SSVM_MemoryInstanceContext *Cxt, const SSVM_Value *In,
                         SSVM_Value *Out) {}

SSVM_ImportObjectContext *createSpecTestModule() {
  SSVM_String HostName;
  SSVM_FunctionTypeContext *HostFType = NULL;
  SSVM_HostFunctionContext *HostFunc = NULL;
  SSVM_TableInstanceContext *HostTable = NULL;
  SSVM_MemoryInstanceContext *HostMemory = NULL;
  SSVM_GlobalInstanceContext *HostGlobal = NULL;
  enum SSVM_ValType Param[2];

  HostName = SSVM_StringCreateByCString("spectest");
  SSVM_ImportObjectContext *ImpObj = SSVM_ImportObjectCreate(HostName);
  SSVM_StringDelete(HostName);

  /// Add host function "print": {} -> {}
  HostFType = SSVM_FunctionTypeCreate(NULL, 0, NULL, 0);
  HostFunc = SSVM_HostFunctionCreate(HostFType, SpecTestPrint, 0);
  SSVM_FunctionTypeDelete(HostFType);
  HostName = SSVM_StringCreateByCString("print");
  SSVM_ImportObjectAddHostFunction(ImpObj, HostName, HostFunc);
  SSVM_StringDelete(HostName);

  /// Add host function "print_i32": {i32} -> {}
  Param[0] = SSVM_ValType_I32;
  HostFType = SSVM_FunctionTypeCreate(Param, 1, NULL, 0);
  HostFunc = SSVM_HostFunctionCreate(HostFType, SpecTestPrintI32, 0);
  SSVM_FunctionTypeDelete(HostFType);
  HostName = SSVM_StringCreateByCString("print_i32");
  SSVM_ImportObjectAddHostFunction(ImpObj, HostName, HostFunc);
  SSVM_StringDelete(HostName);

  /// Add host function "print_f32": {f32} -> {}
  Param[0] = SSVM_ValType_F32;
  HostFType = SSVM_FunctionTypeCreate(Param, 1, NULL, 0);
  HostFunc = SSVM_HostFunctionCreate(HostFType, SpecTestPrintF32, 0);
  SSVM_FunctionTypeDelete(HostFType);
  HostName = SSVM_StringCreateByCString("print_f32");
  SSVM_ImportObjectAddHostFunction(ImpObj, HostName, HostFunc);
  SSVM_StringDelete(HostName);

  /// Add host function "print_f64": {f64} -> {}
  Param[0] = SSVM_ValType_F64;
  HostFType = SSVM_FunctionTypeCreate(Param, 1, NULL, 0);
  HostFunc = SSVM_HostFunctionCreate(HostFType, SpecTestPrintF64, 0);
  SSVM_FunctionTypeDelete(HostFType);
  HostName = SSVM_StringCreateByCString("print_f64");
  SSVM_ImportObjectAddHostFunction(ImpObj, HostName, HostFunc);
  SSVM_StringDelete(HostName);

  /// Add host function "print_i32_f32": {i32, f32} -> {}
  Param[0] = SSVM_ValType_I32;
  Param[1] = SSVM_ValType_F32;
  HostFType = SSVM_FunctionTypeCreate(Param, 2, NULL, 0);
  HostFunc = SSVM_HostFunctionCreate(HostFType, SpecTestPrintI32F32, 0);
  SSVM_FunctionTypeDelete(HostFType);
  HostName = SSVM_StringCreateByCString("print_i32_f32");
  SSVM_ImportObjectAddHostFunction(ImpObj, HostName, HostFunc);
  SSVM_StringDelete(HostName);

  /// Add host function "print_f64_f64": {f64, f64} -> {}
  Param[0] = SSVM_ValType_F64;
  Param[1] = SSVM_ValType_F64;
  HostFType = SSVM_FunctionTypeCreate(Param, 2, NULL, 0);
  HostFunc = SSVM_HostFunctionCreate(HostFType, SpecTestPrintF64F64, 0);
  SSVM_FunctionTypeDelete(HostFType);
  HostName = SSVM_StringCreateByCString("print_f64_f64");
  SSVM_ImportObjectAddHostFunction(ImpObj, HostName, HostFunc);
  SSVM_StringDelete(HostName);

  /// Add host table "table"
  SSVM_Limit TabLimit = {.HasMax = true, .Min = 10, .Max = 20};
  HostTable = SSVM_TableInstanceCreate(SSVM_RefType_FuncRef, TabLimit);
  HostName = SSVM_StringCreateByCString("table");
  SSVM_ImportObjectAddTable(ImpObj, HostName, HostTable);
  SSVM_StringDelete(HostName);

  /// Add host memory "memory"
  SSVM_Limit MemLimit = {.HasMax = true, .Min = 1, .Max = 2};
  HostMemory = SSVM_MemoryInstanceCreate(MemLimit);
  HostName = SSVM_StringCreateByCString("memory");
  SSVM_ImportObjectAddMemory(ImpObj, HostName, HostMemory);
  SSVM_StringDelete(HostName);

  /// Add host global "global_i32": const 666
  HostGlobal = SSVM_GlobalInstanceCreate(
      SSVM_ValType_I32, SSVM_Mutability_Const, SSVM_ValueGenI32(666));
  HostName = SSVM_StringCreateByCString("global_i32");
  SSVM_ImportObjectAddGlobal(ImpObj, HostName, HostGlobal);
  SSVM_StringDelete(HostName);

  /// Add host global "global_i64": const 666
  HostGlobal = SSVM_GlobalInstanceCreate(
      SSVM_ValType_I64, SSVM_Mutability_Const, SSVM_ValueGenI64(666));
  HostName = SSVM_StringCreateByCString("global_i64");
  SSVM_ImportObjectAddGlobal(ImpObj, HostName, HostGlobal);
  SSVM_StringDelete(HostName);

  /// Add host global "global_f32": const 666.0
  HostGlobal = SSVM_GlobalInstanceCreate(
      SSVM_ValType_F32, SSVM_Mutability_Const, SSVM_ValueGenF32(666.0));
  HostName = SSVM_StringCreateByCString("global_f32");
  SSVM_ImportObjectAddGlobal(ImpObj, HostName, HostGlobal);
  SSVM_StringDelete(HostName);

  /// Add host global "global_f64": const 666.0
  HostGlobal = SSVM_GlobalInstanceCreate(
      SSVM_ValType_F64, SSVM_Mutability_Const, SSVM_ValueGenF64(666.0));
  HostName = SSVM_StringCreateByCString("global_f64");
  SSVM_ImportObjectAddGlobal(ImpObj, HostName, HostGlobal);
  SSVM_StringDelete(HostName);

  return ImpObj;
}