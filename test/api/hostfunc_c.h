// SPDX-License-Identifier: Apache-2.0
//===-- ssvm/test/api/hostfunc_c.h - Spec test host functions for C API ---===//
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

#ifndef __HOSTFUNC_C_H__
#define __HOSTFUNC_C_H__

#include "api/ssvm.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Function type: {} -> {}
void SpecTestPrint(SSVM_MemoryInstanceContext *Cxt, const SSVM_Value *In,
                   SSVM_Value *Out);

/// Function type: {i32} -> {}
void SpecTestPrintI32(SSVM_MemoryInstanceContext *Cxt, const SSVM_Value *In,
                      SSVM_Value *Out);

/// Function type: {f32} -> {}
void SpecTestPrintF32(SSVM_MemoryInstanceContext *Cxt, const SSVM_Value *In,
                      SSVM_Value *Out);

/// Function type: {f64} -> {}
void SpecTestPrintF64(SSVM_MemoryInstanceContext *Cxt, const SSVM_Value *In,
                      SSVM_Value *Out);

/// Function type: {i32, f32} -> {}
void SpecTestPrintI32F32(SSVM_MemoryInstanceContext *Cxt, const SSVM_Value *In,
                         SSVM_Value *Out);

/// Function type: {f64, f64} -> {}
void SpecTestPrintF64F64(SSVM_MemoryInstanceContext *Cxt, const SSVM_Value *In,
                         SSVM_Value *Out);

SSVM_ImportObjectContext *createSpecTestModule();

#ifdef __cplusplus
} /// extern "C"
#endif

#endif /// __HOSTFUNC_C_H__
