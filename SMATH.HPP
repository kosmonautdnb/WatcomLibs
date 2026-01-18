// ---
// This is not a serious class, it was just needed for a test.
// Actually it should do the most simple mathematics operations on strings instead of ints/doubles/floats.
// It's not memory friendly or serious in any way. 
// ---
#ifndef __SMATH_HPP__
#define __SMATH_HPP__

#include "STRING.HPP"

typedef String SNumber;

class SMath {
public:    
  static SNumber __ERROR__;
  static SNumber __EPSILON__;
  static SNumber __PI__;
  static SNumber __TINY_PI__;
  String toString(const SNumber& number);
  SNumber fromString(const String& string);
  SNumber fromInt(const int number);
  SNumber part(const SNumber& number, int index0, int index1);
  SNumber signPart(const SNumber& number);
  SNumber integerPart(const SNumber& number);
  SNumber fractionalPart(const SNumber& number);
  bool hasIntegerPart(const SNumber& number);
  bool hasFractionalPart(const SNumber& number);
  bool hasDot(const SNumber& number);
  int integerPartSize(const SNumber& number);
  int fractionalPartSize(const SNumber& number);
  int dotPos(const SNumber& number);
  SNumber removeDot(const SNumber& number);
  bool isNegative(const SNumber& number);
  SNumber cutZeros(const SNumber& number);
  SNumber clean(const SNumber& number);
  SNumber negate(const SNumber& number);
  void makeSameFraction(SNumber& number1, SNumber& number2);
  void padFrontWithZeros(SNumber& number1, SNumber& number2);
  SNumber tennerComplement(const SNumber& number);
  SNumber addUnsigned(const SNumber& number1, const SNumber& number2);
  SNumber subUnsigned(const SNumber& number1, const SNumber& number2);
  SNumber add(const SNumber& number1, const SNumber& number2);
  SNumber sub(const SNumber& number1, const SNumber& number2);
  SNumber mul10(const SNumber& number);
  SNumber div10(const SNumber& number);
  SNumber div2(const SNumber& number);
  SNumber mulIterative(const SNumber& number, const int count);
  SNumber mul0to9(const SNumber& number, const int count);
  SNumber mulUnsigned(const SNumber& number1, const SNumber& number2);
  SNumber mul(const SNumber& number1, const SNumber& number2);
  bool isEqual(const SNumber& number1, const SNumber& number2);
  bool isBigger(const SNumber& number1, const SNumber& number2);
  bool isBiggerEqual(const SNumber& number1, const SNumber& number2);
  bool isSmaller(const SNumber& number1, const SNumber& number2);
  bool isSmallerEqual(const SNumber& number1, const SNumber& number2);
  bool isZero(const SNumber& number);
  SNumber floor(const SNumber& number);
  SNumber ceil(const SNumber& number);
  SNumber trunc(const SNumber& number);
  SNumber round(const SNumber& number);
  SNumber abs(const SNumber& number);
  SNumber sign(const SNumber& number);
  SNumber divApprox(const SNumber& number1, const SNumber& number2, const SNumber& epsilon = __EPSILON__);
  SNumber divUnsigned(const SNumber& number1, const SNumber& number2, const int additionalFractions = 0);
  SNumber div(const SNumber& number1, const SNumber& number2, const int additionalFractions = 0);
  SNumber modApprox(const SNumber& number1, const SNumber& number2, const SNumber& epsilon = __EPSILON__);
  SNumber piNilakanthaApprox(const int iterations, const SNumber& epsilon = __EPSILON__);
  SNumber piLeibnitzApprox(const int iterations, const SNumber& epsilon = __EPSILON__);
public:
  bool identical(const SNumber& a, const SNumber& b);
  void unitTest();
  bool TEST_identical(const SNumber& a, const SNumber& b);
  bool TEST_unidentical(const SNumber& a, const SNumber& b);
  bool TEST_true(bool value);
  bool TEST_false(bool value);
};

#endif //__SMATH_HPP__
