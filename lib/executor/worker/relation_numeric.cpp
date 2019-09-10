#include "executor/common.h"
#include "executor/entry/value.h"
#include "executor/worker.h"
#include "executor/worker/util.h"
#include "support/casting.h"
#include <cmath>

namespace SSVM {
namespace Executor {

template <typename T> ErrCode Worker::runEqzOp(const ValueEntry *Val) {
  /// Return 1 if i is zero, 0 otherwise.
  return StackMgr.pushValue((retrieveValue<T>(*Val) == 0) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runIEqOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T I1 = retrieveValue<T>(*Val1), I2 = retrieveValue<T>(*Val2);
  /// Return 1 if i1 == i2, 0 otherwise.
  return StackMgr.pushValue((I1 == I2) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runFEqOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T Z1 = retrieveValue<T>(*Val1), Z2 = retrieveValue<T>(*Val2);
  /// If either z1 or z2 is a NaN, then return 0.
  if (std::isnan(Z1) || std::isnan(Z2)) {
    return StackMgr.pushValue(0U);
  }
  /// If both z1 and z2 are zeroes, then return 1.
  if ((Z1 == +0.0f || Z1 == -0.0f) && (Z2 == +0.0f || Z2 == -0.0f)) {
    return StackMgr.pushValue(1U);
  }
  /// If both z1 and z2 are the same value, then return 1. Else return 0.
  return StackMgr.pushValue((Z1 == Z2) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runINeOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T I1 = retrieveValue<T>(*Val1), I2 = retrieveValue<T>(*Val2);
  /// Return 1 if i1 != i2, 0 otherwise.
  return StackMgr.pushValue((I1 != I2) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runFNeOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T Z1 = retrieveValue<T>(*Val1), Z2 = retrieveValue<T>(*Val2);
  /// If either z1 or z2 is a NaN, then return 1.
  if (std::isnan(Z1) || std::isnan(Z2)) {
    return StackMgr.pushValue(1U);
  }
  /// If both z1 and z2 are zeroes, then return 0.
  if ((Z1 == +0.0f || Z1 == -0.0f) && (Z2 == +0.0f || Z2 == -0.0f)) {
    return StackMgr.pushValue(0U);
  }
  /// If both z1 and z2 are the same value, then return 0. Else return 1.
  return StackMgr.pushValue((Z1 == Z2) ? 0U : 1U);
}

template <typename T>
ErrCode Worker::runILtUOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T I1 = retrieveValue<T>(*Val1), I2 = retrieveValue<T>(*Val2);
  /// Return 1 if i1 < i2, 0 otherwise.
  return StackMgr.pushValue((I1 < I2) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runILtSOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T I1 = retrieveValue<T>(*Val1), I2 = retrieveValue<T>(*Val2);
  /// Return 1 if signed(i1) < signed(i2), 0 otherwise.
  return StackMgr.pushValue((toSigned(I1) < toSigned(I2)) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runFLtOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T Z1 = retrieveValue<T>(*Val1), Z2 = retrieveValue<T>(*Val2);
  /// If either z1 or z2 is a NaN, then return 0.
  if (std::isnan(Z1) || std::isnan(Z2)) {
    return StackMgr.pushValue(0U);
  }
  /// If z1 and z2 are the same value, then return 0.
  if (Z1 == Z2) {
    return StackMgr.pushValue(0U);
  }
  /// If z1 is positive infinity, then return 0.
  if (std::isinf(Z1) && Z1 > 0) {
    return StackMgr.pushValue(0U);
  }
  /// If z1 is negative infinity, then return 1.
  if (std::isinf(Z1) && Z1 < 0) {
    return StackMgr.pushValue(1U);
  }
  /// If z2 is positive infinity, then return 1.
  if (std::isinf(Z2) && Z2 > 0) {
    return StackMgr.pushValue(1U);
  }
  /// If z2 is negative infinity, then return 0.
  if (std::isinf(Z2) && Z2 < 0) {
    return StackMgr.pushValue(0U);
  }
  /// If both z1 and z2 are zeroes, then return 0.
  if ((Z1 == +0.0f || Z1 == -0.0f) && (Z2 == +0.0f || Z2 == -0.0f)) {
    return StackMgr.pushValue(0U);
  }
  /// If z1 < z2, then return 1. Else return 0.
  return StackMgr.pushValue((Z1 < Z2) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runIGtUOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T I1 = retrieveValue<T>(*Val1), I2 = retrieveValue<T>(*Val2);
  /// Return 1 if i1 > i2, 0 otherwise.
  return StackMgr.pushValue((I1 > I2) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runIGtSOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T I1 = retrieveValue<T>(*Val1), I2 = retrieveValue<T>(*Val2);
  /// Return 1 if signed(i1) > signed(i2), 0 otherwise.
  return StackMgr.pushValue((toSigned(I1) > toSigned(I2)) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runFGtOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T Z1 = retrieveValue<T>(*Val1), Z2 = retrieveValue<T>(*Val2);
  /// If either z1 or z2 is a NaN, then return 0.
  if (std::isnan(Z1) || std::isnan(Z2)) {
    return StackMgr.pushValue(0U);
  }
  /// If z1 and z2 are the same value, then return 0.
  if (Z1 == Z2) {
    return StackMgr.pushValue(0U);
  }
  /// If z1 is positive infinity, then return 1.
  if (std::isinf(Z1) && Z1 > 0) {
    return StackMgr.pushValue(1U);
  }
  /// If z1 is negative infinity, then return 0.
  if (std::isinf(Z1) && Z1 < 0) {
    return StackMgr.pushValue(0U);
  }
  /// If z2 is positive infinity, then return 0.
  if (std::isinf(Z2) && Z2 > 0) {
    return StackMgr.pushValue(0U);
  }
  /// If z2 is negative infinity, then return 1.
  if (std::isinf(Z2) && Z2 < 0) {
    return StackMgr.pushValue(1U);
  }
  /// If both z1 and z2 are zeroes, then return 0.
  if ((Z1 == +0.0f || Z1 == -0.0f) && (Z2 == +0.0f || Z2 == -0.0f)) {
    return StackMgr.pushValue(0U);
  }
  /// If z1 > z2, then return 1. Else return 0.
  return StackMgr.pushValue((Z1 > Z2) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runILeUOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T I1 = retrieveValue<T>(*Val1), I2 = retrieveValue<T>(*Val2);
  /// Return 1 if i1 <= i2, 0 otherwise.
  return StackMgr.pushValue((I1 <= I2) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runILeSOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T I1 = retrieveValue<T>(*Val1), I2 = retrieveValue<T>(*Val2);
  /// Return 1 if signed(i1) <= signed(i2), 0 otherwise.
  return StackMgr.pushValue((toSigned(I1) <= toSigned(I2)) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runFLeOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T Z1 = retrieveValue<T>(*Val1), Z2 = retrieveValue<T>(*Val2);
  /// If either z1 or z2 is a NaN, then return 0.
  if (std::isnan(Z1) || std::isnan(Z2)) {
    return StackMgr.pushValue(0U);
  }
  /// If z1 and z2 are the same value, then return 1.
  if (Z1 == Z2) {
    return StackMgr.pushValue(1U);
  }
  /// If z1 is positive infinity, then return 0.
  if (std::isinf(Z1) && Z1 > 0) {
    return StackMgr.pushValue(0U);
  }
  /// If z1 is negative infinity, then return 1.
  if (std::isinf(Z1) && Z1 < 0) {
    return StackMgr.pushValue(1U);
  }
  /// If z2 is positive infinity, then return 1.
  if (std::isinf(Z2) && Z2 > 0) {
    return StackMgr.pushValue(1U);
  }
  /// If z2 is negative infinity, then return 0.
  if (std::isinf(Z2) && Z2 < 0) {
    return StackMgr.pushValue(0U);
  }
  /// If both z1 and z2 are zeroes, then return 1.
  if ((Z1 == +0.0f || Z1 == -0.0f) && (Z2 == +0.0f || Z2 == -0.0f)) {
    return StackMgr.pushValue(1U);
  }
  /// If z1 <= z2, then return 1. Else return 0.
  return StackMgr.pushValue((Z1 <= Z2) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runIGeUOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T I1 = retrieveValue<T>(*Val1), I2 = retrieveValue<T>(*Val2);
  /// Return 1 if i1 >= i2, 0 otherwise.
  return StackMgr.pushValue((I1 >= I2) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runIGeSOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T I1 = retrieveValue<T>(*Val1), I2 = retrieveValue<T>(*Val2);
  /// Return 1 if signed(i1) >= signed(i2), 0 otherwise.
  return StackMgr.pushValue((toSigned(I1) >= toSigned(I2)) ? 1U : 0U);
}

template <typename T>
ErrCode Worker::runFGeOp(const ValueEntry *Val1, const ValueEntry *Val2) {
  T Z1 = retrieveValue<T>(*Val1), Z2 = retrieveValue<T>(*Val2);
  /// If either z1 or z2 is a NaN, then return 0.
  if (std::isnan(Z1) || std::isnan(Z2)) {
    return StackMgr.pushValue(0U);
  }
  /// If z1 and z2 are the same value, then return 1.
  if (Z1 == Z2) {
    return StackMgr.pushValue(1U);
  }
  /// If z1 is positive infinity, then return 1.
  if (std::isinf(Z1) && Z1 > 0) {
    return StackMgr.pushValue(1U);
  }
  /// If z1 is negative infinity, then return 0.
  if (std::isinf(Z1) && Z1 < 0) {
    return StackMgr.pushValue(0U);
  }
  /// If z2 is positive infinity, then return 0.
  if (std::isinf(Z2) && Z2 > 0) {
    return StackMgr.pushValue(0U);
  }
  /// If z2 is negative infinity, then return 1.
  if (std::isinf(Z2) && Z2 < 0) {
    return StackMgr.pushValue(1U);
  }
  /// If both z1 and z2 are zeroes, then return 1.
  if ((Z1 == +0.0f || Z1 == -0.0f) && (Z2 == +0.0f || Z2 == -0.0f)) {
    return StackMgr.pushValue(1U);
  }
  /// If z1 >= z2, then return 1. Else return 0.
  return StackMgr.pushValue((Z1 >= Z2) ? 1U : 0U);
}

} // namespace Executor
} // namespace SSVM