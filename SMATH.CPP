#include "SMATH.HPP"

#define __smath_log printf
//#define SMathWITHUNITTEST

SNumber SMath::__ERROR__ = "<error>";
SNumber SMath::__EPSILON__ = "0.0000000000000000000000000000000000000000000000000000000000001";
SNumber SMath::__PI__ = "3.14159265358979323846264338327950288419716939937510582097494459230781640"
  "6286208998628034825342117067982148086513282306647093844609550582231725359"
  "4081284811174502841027019385211055596446229489549303819644288109756659334"
  "4612847564823378678316527120190914564856692346034861045432664821339360726"
  "0249141273724587006606315588174881520920962829254091715364367892590360011"
  "3305305488204665213841469519415116094330572703657595919530921861173819326"
  "1179310511854807446237996274956735188575272489122793818301194912983367336"
  "2440656643086021394946395224737190702179860943702770539217176293176752384"
  "6748184676694051320005681271452635608277857713427577896091736371787214684"
  "4090122495343014654958537105079227968925892354201995611212902196086403441"
  "815981362977477130996051870721134999999837297804995105973173281609631";
SNumber SMath::__TINY_PI__ = "3.1415926535897932384626433832795";

String SMath::toString(const SNumber& number) {
  return number;
}

SNumber SMath::fromString(const String& string) {
  return string;
}

SNumber SMath::fromInt(const int number) {
  return String::fromInt(number);
}

SNumber SMath::part(const SNumber& number, int index0, int index1) {
  return number.substr(index0, index1 - index0);
}

SNumber SMath::signPart(const SNumber& number) {
  if (isNegative(number)) return "-";
  return "";
}

SNumber SMath::integerPart(const SNumber& number) {
  for (int i = 0; i < number.size(); i++) {
    if (number[i] == '.') {
      return part(number, 0, i);
    }
  }
  return number;
}

SNumber SMath::fractionalPart(const SNumber& number) {
  for (int i = 0; i < number.size(); i++) {
    if (number[i] == '.') {
      return part(number, i, number.size());
    }
  }
  return SNumber(".");
}

bool SMath::hasIntegerPart(const SNumber& number) {
  bool integerPart = false;
  for (int i = 0; i < number.size(); i++) {
    if (number[i] >= '0' && number[i] <= '9') integerPart = true;
    if (number[i] == '.') {
      return integerPart;
    }
  }
  return integerPart;
}

bool SMath::hasFractionalPart(const SNumber& number) {
  bool integerPart = false;
  for (int i = 0; i < number.size(); i++) {
    if (number[i] == '.') {
      return number.size() > (i + 1);
    }
  }
  return false;
}

bool SMath::hasDot(const SNumber& number) {
  for (int i = 0; i < number.size(); i++) {
    if (number[i] == '.') {
      return true;
    }
  }
  return false;
}

int SMath::integerPartSize(const SNumber& number) {
  int integerPart = 0;
  for (int i = 0; i < number.size(); i++) {
    if (number[i] >= '0' && number[i] <= '9') integerPart++;
    if (number[i] == '.') {
      return integerPart;
    }
  }
  return integerPart;
}

int SMath::fractionalPartSize(const SNumber& number) {
  bool integerPart = false;
  for (int i = 0; i < number.size(); i++) {
    if (number[i] == '.') {
      return number.size() - (i + 1);
    }
  }
  return false;
}

int SMath::dotPos(const SNumber& number) {
  for (int i = 0; i < number.size(); i++) {
    if (number[i] == '.')
      return i;
  }
  return -1;
}

SNumber SMath::removeDot(const SNumber& number) {
  SNumber ret;
  for (int i = 0; i < number.size(); i++) {
    if (number[i] != '.')
      ret += String::fromChar(number[i]);
  }
  return ret;
}

bool SMath::isNegative(const SNumber& number) {
  if (number.empty()) return false;
  return number[0] == '-';
}

SNumber SMath::cutZeros(const SNumber& number) {
  SNumber ret;
  bool was0 = true;
  for (int i = 0; i < number.size(); i++) {
    if (number[i] == '0' && was0) continue;
    if (number[i] >= '1' && number[i] <= '9') was0 = false;
    ret += String::fromChar(number[i]);
    if (number[i] == '.') {
      was0 = true;
      String ret2;
      for (int j = number.size() - 1; j > i; j--) {
        if (number[j] == '0' && was0) continue;
        if (number[j] >= '1' && number[j] <= '9') was0 = false;
        ret2 = String::fromChar(number[j]) + ret2;
      }
      return ret + ret2;
    }
  }
  return ret;
}

SNumber SMath::clean(const SNumber& number) {
  SNumber ret = cutZeros(number);
  if (ret.empty()) return "0";
  if (ret.size() == 1 && ret[0] == '-') return "0";
  if (ret.size() == 1 && ret[0] == '.') return "0";
  if (ret.size() == 2 && ret[0] == '-' && ret[1] == '.') return "0";
  if (ret.size() > 1 && ret[0] == '.') return "0" + ret;
  if (ret.size() > 2 && ret[0] == '-' && ret[1] == '.') return "-0" + part(ret,1,ret.size());
  if (ret[ret.size() - 1] == '.') return part(ret, 0, ret.size() - 1);
  return ret;
}
  
SNumber SMath::negate(const SNumber& number) {
  if (isNegative(number))
    return part(number, 1, number.size());
  return "-" + number;
}

void SMath::makeSameFraction(SNumber& number1, SNumber& number2) {
  if (!hasDot(number1) && !hasDot(number2)) return;
  if (!hasDot(number1)) number1 += ".";
  if (!hasDot(number2)) number2 += ".";
  int f1 = fractionalPartSize(number1);
  int f2 = fractionalPartSize(number2);
  for (int i = f1; i < f2; ++i) number1 += "0";
  {for (int i = f2; i < f1; ++i) number2 += "0";}
}

void SMath::padFrontWithZeros(SNumber& number1, SNumber& number2) {
  SNumber f1 = number1;
  SNumber f2 = number2;
  if (isNegative(number1)) f1 = negate(f1);
  if (isNegative(number2)) f2 = negate(f2);
  int l1 = integerPartSize(f1);
  int l2 = integerPartSize(f2);
  for (int i = l1; i < l2; ++i) f1 = "0" + f1;
  {for (int i = l2; i < l1; ++i) f2 = "0" + f2;}
  if (isNegative(number1)) f1 = "-" + f1;
  if (isNegative(number2)) f2 = "-" + f2;
  number1 = f1;
  number2 = f2;
}

SNumber SMath::tennerComplement(const SNumber &number) {
  SNumber ret = number;
  for (int i = 0; i < ret.size(); ++i) {
    if (ret[i] >= '0' && ret[i] <= '9')
      ret[i] = (9 - (ret[i] - '0')) + '0';
  }
  int carry = 1;
  {for (int i = ret.size() - 1; i >= 0; i--) {
    char c = ret[i];
    if (c >= '0' && c <= '9') {
      c -= '0';
      c += carry;
      carry = 0;
      if (c >= 10) {
        carry = 1; c -= 10;
      }
      ret[i] = c + '0';
    }
  }}
  if (carry == 1)
    ret = signPart(ret) + "1" + integerPart(ret) + fractionalPart(ret);
  return clean(ret);
}

SNumber SMath::addUnsigned(const SNumber& number1, const SNumber& number2) {
  SNumber ret;
  SNumber r1 = clean(number1);
  SNumber r2 = clean(number2);
  makeSameFraction(r1, r2);
  int p0 = r1.size();
  int p1 = r2.size();
  int mx = p0 > p1 ? p0 : p1;
  int carry = 0;
  for (int i = 0; i < mx || carry != 0; i++) {
    p0--;
    p1--;
    int a = carry;
    if (p0 >= 0) {
      char c1 = r1[p0];
      if (c1 == '.') {ret = "." + ret; continue;}
      if (c1 >= '0' && c1 <= '9')
        a += (c1 - '0');
    }
    if (p1 >= 0) {
      char c2 = r2[p1];
      if (c2 == '.') { ret = "." + ret; continue; }
      if (c2 >= '0' && c2 <= '9')
        a += (c2 - '0');
    }
    carry = 0;
    if (a >= 10) {carry = 1; a -= 10;}
    char cipher = a + '0';
    ret = String::fromChar(cipher) + ret;
    if (i == mx)
      break;
  }
  ret = clean(ret);
  return ret;
}

SNumber SMath::subUnsigned(const SNumber& number1, const SNumber& number2) {
  SNumber ret;
  SNumber r1 = clean(number1);
  SNumber r2 = clean(number2);
  makeSameFraction(r1, r2);
  int p0 = r1.size();
  int p1 = r2.size();
  int mx = p0 > p1 ? p0 : p1;
  int carry = 0;
  for (int i = 0; i < mx || carry != 0; i++) {
    p0--;
    p1--;
    int a = carry;
    if (p0 >= 0) {
      char c1 = r1[p0];
      if (c1 == '.') { ret = "." + ret; continue; }
      if (c1 >= '0' && c1 <= '9')
        a += (c1 - '0');
    }
    if (p1 >= 0) {
      char c2 = r2[p1];
      if (c2 == '.') { ret = "." + ret; continue; }
      if (c2 >= '0' && c2 <= '9')
        a -= (c2 - '0');
    }
    carry = 0;
    if (a < 0) { carry = -1; a += 10; }
    char cipher = a + '0';
    ret = String::fromChar(cipher) + ret;
    if (i == mx) {
      if (carry == -1) {
        ret = "-"+tennerComplement(clean(ret));
      }
      break;
    }
  }
  ret = clean(ret);
  return ret;
}

SNumber SMath::add(const SNumber& number1, const SNumber& number2) {
  const bool n1 = isNegative(number1);
  const bool n2 = isNegative(number2);
  if ((!n1) && (!n2)) {
    return addUnsigned(number1, number2);
  }
  if ((n1) && (n2)) {
    return negate(addUnsigned(negate(number1), negate(number2)));
  }
  if ((!n1) && (n2)) {
    return subUnsigned(number1, negate(number2));
  }
  return subUnsigned(number2, negate(number1));
}

SNumber SMath::sub(const SNumber& number1, const SNumber& number2) {
  return add(number1, negate(number2));
}

SNumber SMath::mul10(const SNumber& number) {
  if (isZero(number)) return "0";
  if (hasDot(number)) {
    int k = dotPos(number);
    if (k == number.size() - 1) return part(number, 0, number.size() - 1) + "0";
    SNumber ret = number;
    ret[k] = ret[k + 1];
    ret[k + 1] = '.';
    return clean(ret);
  }
  return number + "0";
}

SNumber SMath::div10(const SNumber& number) {
  SNumber ret = clean(number);
  if (hasDot(ret)) {
    if (isNegative(number))
      ret = part(ret, 1, ret.size());
    int k = dotPos(ret);
    if (k > 0) {
      ret = part(ret, 0, k - 1) + "." + String::fromChar(ret[k - 1]) + part(ret, k + 1, ret.size());
    }
    else ret = ".0"+part(ret, 1, ret.size());
    if (isNegative(number)) ret = "-" + ret;
    return clean(ret);
  }
  return clean(part(ret,0,ret.size()-1) + "." + part(ret, ret.size() - 1, ret.size()));
}

SNumber SMath::div2(const SNumber& number) {
  SNumber ret = clean(number);
  SNumber ueberTrag = ret;
  for (int i = 0; i < ret.size(); i++) {
    char c = ret[i];
    if (c >= '0' && c <= '9') {
      c -= '0';
      ret[i] = (c / 2) + '0';
      ueberTrag[i] = c & 1 ? '5' : '0';
    }
  }
  ueberTrag = div10(ueberTrag);
  ret = add(ret, ueberTrag);
  return  ret;
}

SNumber SMath::mulIterative(const SNumber& number, const int count) {
  SNumber ret = "0";
  for (int i = 0; i < count; i++)
    ret = add(ret, number);
  return ret;
}

SNumber SMath::mul0to9(const SNumber& number, const int count) {
  switch (count) {
  case 0: return "0";
  case 1: return number;
  case 2: return add(number,number);
  case 3: return add(add(number, number),number);
  case 4: return mulIterative(add(number, number),2);
  case 5: return div2(mul10(number));
  case 6: return mulIterative(add(add(number, number), number),2);
  case 7: return add(number,mulIterative(add(add(number, number), number), 2));
  case 8: return mulIterative(add(number, number), 4);
  case 9: return add(number,mulIterative(add(number, number), 4));
  }
  return __ERROR__;
}

SNumber SMath::mulUnsigned(const SNumber& number1, const SNumber &number2) {
  SNumber ret = "0";
  SNumber r1 = clean(number1);
  SNumber r2 = clean(number2);
  makeSameFraction(r1, r2);
  int fractionals = 0;
  if (hasDot(r1)) {
    fractionals = fractionalPartSize(r1);
  }
  for (int i = r1.size()-1; i >= 0; i--) {
    int k = r1[i];
    if (k >= '0' && k <= '9') {
      ret = add(ret, mul0to9(r2,k - '0'));
      r2 = mul10(r2);
    }
  }
  {for (int i = 0; i < fractionals; i++) {
    ret = div10(ret);
  }}
  return clean(ret);
}

SNumber SMath::mul(const SNumber& number1, const SNumber& number2) {
  const bool n1 = isNegative(number1);
  const bool n2 = isNegative(number2);
  if ((!n1) && (!n2)) {
    return mulUnsigned(number1, number2);
  }
  if ((n1) && (n2)) {
    return mulUnsigned(negate(number1), negate(number2));
  }
  if ((!n1) && (n2)) {
    return negate(mulUnsigned(number1, negate(number2)));
  }
  return negate(mulUnsigned(negate(number1), number2));
}

bool SMath::isEqual(const SNumber& number1, const SNumber& number2) {
  return identical(sub(number1,number2),"0");
}

bool SMath::isBigger(const SNumber& number1, const SNumber& number2) {
  return isNegative(sub(number2,number1));
}

bool SMath::isBiggerEqual(const SNumber& number1, const SNumber& number2) {
  return !isNegative(sub(number1, number2));
}

bool SMath::isSmaller(const SNumber& number1, const SNumber& number2) {
  return isNegative(sub(number1, number2));
}

bool SMath::isSmallerEqual(const SNumber& number1, const SNumber& number2) {
  return !isNegative(sub(number2, number1));
}
bool SMath::isZero(const SNumber& number) {
  return clean(number)=="0";
}

SNumber SMath::floor(const SNumber& number) {
  if (isNegative(number) && !isEqual(number,integerPart(number))) return add("-1", integerPart(number));
  return clean(integerPart(number));
}

SNumber SMath::ceil(const SNumber& number) {
  if (!isNegative(number) && !isEqual(number, integerPart(number))) return add("1", integerPart(number));
  return clean(integerPart(number));
}

SNumber SMath::trunc(const SNumber& number) {
  return clean(integerPart(number));
}

SNumber SMath::round(const SNumber& number) {
  SNumber k = fractionalPart(number);
  if (!isNegative(number)) {
    return isBiggerEqual(k, "0.5") ? ceil(number) : floor(number);
  }
  return isSmaller(k, "0.5") ? ceil(number) : floor(number); /// actually -0.5..0.4...
}

SNumber SMath::abs(const SNumber& number) {
  if (isNegative(number)) return clean(negate(number));
  return clean(number);
}

SNumber SMath::sign(const SNumber& number) {
  if (isNegative(clean(number))) return "-1";
  return "1";
}

SNumber SMath::divApprox(const SNumber& number1, const SNumber& number2, const SNumber& epsilon) { // todo: needs propper implementation
  SNumber n1 = abs(clean(number1));
  SNumber n2 = abs(clean(number2));
  if (isZero(n1))  return "0";
  if (isZero(n2))  return __ERROR__;
  SNumber endMul = "1";
  SNumber ret = "0";
  while (isBigger(n2, n1)) {
    n2 = div2(n2);
    endMul = div2(endMul);
  }
  while (isBigger(n1, epsilon)) {
    if (isEqual(n1, n2)) {
      ret = add(ret, endMul);
      break;
    }
    if (isBigger(n1, n2)) {
      n2 = mul0to9(n2, 2);
      endMul = mul0to9(endMul, 2);
    }
    else {
      n2 = div2(n2);
      endMul = div2(endMul);
      n1 = sub(n1, n2);
      ret = add(ret, endMul);
      while (isBigger(n2, n1)) {
        n2 = div2(n2);
        endMul = div2(endMul);
      }
    }
  }
  if (signPart(number1) != signPart(number2)) ret = "-" + ret;
  return ret;
}

SNumber SMath::divUnsigned(const SNumber& number1, const SNumber& number2, const int additionalFractions) {
  SNumber n1 = abs(clean(number1));
  SNumber n2 = abs(clean(number2));
  SNumber result = "0";
  makeSameFraction(n1, n2);
  padFrontWithZeros(n1, n2);
  n1 = removeDot(n1);
  n2 = removeDot(n2);
  int l = n1.size();
  l += additionalFractions;
  SNumber uebertrag = "0";
  for (int i = 0; i < l; i++) {
    uebertrag = mul10(uebertrag);
    uebertrag = add(uebertrag, fromInt(i < n1.size() ? n1[i] - '0' : 0));
    SNumber subbed = sub(uebertrag, n2);
    int toAdd = 0;
    while (isBiggerEqual(subbed, "0")) {
      uebertrag = subbed;
      subbed = sub(uebertrag, n2);
      toAdd++;
    }
    result = mul10(result);
    result = add(result,fromInt(toAdd));
  }
  {for (int i = 0; i < additionalFractions; i++) result = div10(result);}
  return clean(result);
}

SNumber SMath::div(const SNumber& number1, const SNumber& number2, const int additionalFractions) {
  SNumber n1 = abs(clean(number1));
  SNumber n2 = abs(clean(number2));
  SNumber ret = divUnsigned(n1, n2, additionalFractions);
  if (isNegative(number1) != isNegative(number2)) ret = negate(ret);
  return ret;
}

SNumber SMath::modApprox(const SNumber& number1, const SNumber& number2, const SNumber &epsilon) {
  SNumber f1 = divApprox(number1, number2, epsilon);
  f1 = sub(f1,floor(f1));
  f1 = mul(f1, number2);
  return f1;
}

SNumber SMath::piNilakanthaApprox(const int iterations, const SNumber &epsilon) {
  SNumber ret = "3";
  for (int i = 1; i < iterations; ++i) {
    SNumber n1 = fromInt(i * 2 + 0);
    SNumber n2 = fromInt(i * 2 + 1);
    SNumber n3 = fromInt(i * 2 + 2);
    ret = add(ret, divApprox(i & 1 ? "4" : "-4", mul(n1, mul(n2, n3)), epsilon));
  }
  return clean(ret);
}

SNumber SMath::piLeibnitzApprox(const int iterations, const SNumber& epsilon) {
  SNumber ret = "1";
  for (int i = 1; i < iterations; ++i) {
    SNumber n = fromInt(i * 2 + 1);
    ret = add(ret, divApprox(i & 1 ? "-1" : "1", n, epsilon));
  }
  return clean(mul(ret,"4"));
}

bool SMath::identical(const SNumber& a, const SNumber& b) {
  return a == b;
}

#ifdef SMathWITHUNITTEST
#define TEST2_unidentical(__v1__,__v2__) {bool v = TEST_unidentical(__v1__,__v2__); if (!v) printf("unidentical:"###__v1__##","###__v2__##" failed!\n");}
#define TEST2_identical(__v1__,__v2__) {bool v = TEST_identical(__v1__,__v2__); if (!v) printf("identical:"###__v1__##","###__v2__##" failed!\n");}
#define TEST2_true(__v1__) {bool v = TEST_true(__v1__); if (!v) printf("true:"###__v1__##" failed!\n");}
#define TEST2_false(__v1__) {bool v = TEST_false(__v1__); if (!v) printf("false:"###__v1__##" failed!\n");}
void SMath::unitTest() {
  __smath_log("SMath::UnitTest start!\n");
  TEST2_unidentical("-10", negate("-10"));
  TEST2_identical("-10", negate("10"));
  TEST2_identical("10", negate("-10"));
  {
    TEST2_identical("-10", integerPart("-10"));
    TEST2_identical("-1000", integerPart("-1000"));
    TEST2_identical("10", integerPart("10"));
    TEST2_identical("1000", integerPart("1000"));
    TEST2_identical("-10", integerPart("-10.1212"));
    TEST2_identical("-1000", integerPart("-1000.1212"));
    TEST2_identical("10", integerPart("10.1212"));
    TEST2_identical("1000", integerPart("1000.1212"));
    TEST2_identical("-10", integerPart("-10."));
    TEST2_identical("-1000", integerPart("-1000."));
    TEST2_identical("10", integerPart("10."));
    TEST2_identical("1000", integerPart("1000."));
    TEST2_identical("-99", integerPart("-99.9"));
    TEST2_identical("-99999", integerPart("-99999.9"));
    TEST2_identical("99", integerPart("99.9"));
    TEST2_identical("99999", integerPart("99999.9"));
  }
  {
    TEST2_identical(".", fractionalPart("-10"));
    TEST2_identical(".", fractionalPart("-1000"));
    TEST2_identical(".", fractionalPart("10"));
    TEST2_identical(".", fractionalPart("1000"));
    TEST2_identical(".123", fractionalPart("-10.123"));
    TEST2_identical(".123", fractionalPart("-1000.123"));
    TEST2_identical(".123", fractionalPart("10.123"));
    TEST2_identical(".123", fractionalPart("1000.123"));
    TEST2_identical(".123", fractionalPart(".123"));
    TEST2_identical(".123", fractionalPart(".123"));
    TEST2_identical(".9123", fractionalPart("-10.9123"));
    TEST2_identical(".9123", fractionalPart("-1000.9123"));
    TEST2_identical(".9123", fractionalPart("10.9123"));
    TEST2_identical(".9123", fractionalPart("1000.9123"));
    TEST2_identical(".9123", fractionalPart(".9123"));
    TEST2_identical(".9123", fractionalPart(".9123"));
    TEST2_identical(".123", fractionalPart("-0.123"));
    TEST2_identical(".123", fractionalPart("-000.123"));
    TEST2_identical(".123", fractionalPart("0.123"));
    TEST2_identical(".123", fractionalPart("000.123"));
  }
  {
    TEST2_true(hasIntegerPart("0"));
    TEST2_true(hasIntegerPart("-0"));
    TEST2_true(hasIntegerPart("0.123"));
    TEST2_true(hasIntegerPart("1"));
    TEST2_true(hasIntegerPart("-2"));
    TEST2_true(hasIntegerPart("44.123"));
    TEST2_false(hasIntegerPart(".1213"));
    TEST2_false(hasIntegerPart("-.1213"));
    TEST2_false(hasIntegerPart("."));
    TEST2_false(hasIntegerPart("-."));
    TEST2_false(hasIntegerPart(".0123"));
    TEST2_false(hasIntegerPart("-.0123"));
  }
  {
    TEST2_false(hasFractionalPart("0"));
    TEST2_false(hasFractionalPart("-0"));
    TEST2_true(hasFractionalPart("0.123"));
    TEST2_false(hasFractionalPart("1"));
    TEST2_false(hasFractionalPart("-2"));
    TEST2_true(hasFractionalPart("44.123"));
    TEST2_true(hasFractionalPart(".1213"));
    TEST2_true(hasFractionalPart("-.1213"));
    TEST2_false(hasFractionalPart("."));
    TEST2_false(hasFractionalPart("-."));
    TEST2_true(hasFractionalPart(".0123"));
    TEST2_true(hasFractionalPart("-.0123"));
  }
  {
    TEST2_false(hasDot("0"));
    TEST2_false(hasDot("-0"));
    TEST2_true( hasDot("0.123"));
    TEST2_false(hasDot("1"));
    TEST2_false(hasDot("-2"));
    TEST2_true(hasDot("44.123"));
    TEST2_true(hasDot(".1213"));
    TEST2_true(hasDot("-.1213"));
    TEST2_true(hasDot("."));
    TEST2_true(hasDot("-."));
    TEST2_true(hasDot(".0123"));
    TEST2_true(hasDot("-.0123"));
  }
  {
    TEST2_true(integerPartSize("0.") == 1);
    TEST2_true(integerPartSize("00.") == 2);
    TEST2_true(integerPartSize("000.") == 3);
    TEST2_true(integerPartSize("0")==1);
    TEST2_true(integerPartSize("-0")==1);
    TEST2_true(integerPartSize("0.123")==1);
    TEST2_true(integerPartSize("1")==1);
    TEST2_true(integerPartSize("-2")==1);
    TEST2_true(integerPartSize("44.123")==2);
    TEST2_true(integerPartSize("444.123")==3);
    TEST2_true(integerPartSize("-444.123")==3);
    TEST2_true(integerPartSize("-444.") == 3);
    TEST2_true(integerPartSize("-444") == 3);
    TEST2_true(integerPartSize("-44") == 2);
    TEST2_true(integerPartSize("-") == 0);
    TEST2_true(integerPartSize(".1213")==0);
    TEST2_true(integerPartSize("-.1213")==0);
    TEST2_true(integerPartSize(".")==0);
    TEST2_true(integerPartSize("-.")==0);
    TEST2_true(integerPartSize(".0123")==0);
    TEST2_true(integerPartSize("-.0123")==0);
  }
  {
    TEST2_true(fractionalPartSize(".0") == 1);
    TEST2_true(fractionalPartSize(".00") == 2);
    TEST2_true(fractionalPartSize(".000") == 3);
    TEST2_true(fractionalPartSize("0") == 0);
    TEST2_true(fractionalPartSize("-0") == 0);
    TEST2_true(fractionalPartSize("0.123") == 3);
    TEST2_true(fractionalPartSize("1") == 0);
    TEST2_true(fractionalPartSize("-2") == 0);
    TEST2_true(fractionalPartSize("44.123") == 3);
    TEST2_true(fractionalPartSize("444.123") == 3);
    TEST2_true(fractionalPartSize("-444.123") == 3);
    TEST2_true(fractionalPartSize("-444.") == 0);
    TEST2_true(fractionalPartSize("-444") == 0);
    TEST2_true(fractionalPartSize("-44") == 0);
    TEST2_true(fractionalPartSize("-") == 0);
    TEST2_true(fractionalPartSize(".1213") == 4);
    TEST2_true(fractionalPartSize("-.1213") == 4);
    TEST2_true(fractionalPartSize(".") == 0);
    TEST2_true(fractionalPartSize("-.") == 0);
    TEST2_true(fractionalPartSize(".0123") == 4);
    TEST2_true(fractionalPartSize("-.0123") == 4);
  }
  {
    TEST2_true(isNegative("-."));
    TEST2_true(isNegative("-0."));
    TEST2_true(isNegative("-.0"));
    TEST2_true(isNegative("-0.0"));
    TEST2_true(isNegative("-00.00"));
    TEST2_true(isNegative("-11.11"));
    TEST2_true(isNegative("-.11"));
    TEST2_true(isNegative("-1"));
    TEST2_true(isNegative("-11"));
    TEST2_false(isNegative("."));
    TEST2_false(isNegative("0."));
    TEST2_false(isNegative(".0"));
    TEST2_false(isNegative("0.0"));
    TEST2_false(isNegative("00.00"));
    TEST2_false(isNegative("11.11"));
    TEST2_false(isNegative(".11"));
    TEST2_false(isNegative("1"));
    TEST2_false(isNegative("11"));
  }
  {
    TEST2_identical(".", cutZeros("0.0"));
    TEST2_identical(".", cutZeros("00.00"));
    TEST2_identical(".", cutZeros(".00"));
    TEST2_identical(".", cutZeros("00."));
    TEST2_identical(".", cutZeros("."));
    TEST2_identical("10.", cutZeros("10.0"));
    TEST2_identical(".1", cutZeros("00.10"));
    TEST2_identical("1234.", cutZeros("001234.00"));
    TEST2_identical(".001234", cutZeros("00.0012340"));
    TEST2_identical("1010.9", cutZeros("001010.9000"));
    TEST2_identical("", cutZeros("0"));
    TEST2_identical("-", cutZeros("-0"));
    TEST2_identical("-.", cutZeros("-0.0"));
    TEST2_identical("-.", cutZeros("-00.00"));
    TEST2_identical("-.", cutZeros("-.00"));
    TEST2_identical("-.", cutZeros("-00."));
    TEST2_identical("-.", cutZeros("-."));
    TEST2_identical("-10.", cutZeros("-10.0"));
    TEST2_identical("-.1", cutZeros("-00.10"));
    TEST2_identical("-1234.", cutZeros("-001234.00"));
    TEST2_identical("-.001234", cutZeros("-00.0012340"));
    TEST2_identical("-1010.9", cutZeros("-001010.9000"));
  }
  {
    TEST2_identical("0", clean("0.0"));
    TEST2_identical("0", clean("00.00"));
    TEST2_identical("0", clean(".00"));
    TEST2_identical("0", clean("00."));
    TEST2_identical("0", clean("."));
    TEST2_identical("10", clean("10.0"));
    TEST2_identical("0.1", clean("00.10"));
    TEST2_identical("1234", clean("001234.00"));
    TEST2_identical("0.001234", clean("00.0012340"));
    TEST2_identical("1010.9", clean("001010.9000"));
    TEST2_identical("0", clean("-0.0"));
    TEST2_identical("0", clean("-00.00"));
    TEST2_identical("0", clean("-.00"));
    TEST2_identical("0", clean("-00."));
    TEST2_identical("0", clean("-."));
    TEST2_identical("-10", clean("-10.0"));
    TEST2_identical("-0.1", clean("-00.10"));
    TEST2_identical("-1234", clean("-001234.00"));
    TEST2_identical("-0.001234", clean("-00.0012340"));
    TEST2_identical("-1010.9", clean("-001010.9000"));
  }
  {
    SNumber m1, m2;
    m1 = "1.91023"; m2 = "100.3232312312321"; makeSameFraction(m1, m2); TEST2_identical(m1,"1.9102300000000"); TEST2_identical(m2, "100.3232312312321");
    m1 = "1"; m2 = "10"; makeSameFraction(m1, m2); TEST2_identical(m1, "1"); TEST2_identical(m2, "10");
    m2 = "1.91023"; m1 = "100.3232312312321"; makeSameFraction(m1, m2); TEST2_identical(m2, "1.9102300000000"); TEST2_identical(m1, "100.3232312312321");
    m2 = "1"; m1 = "100.3232312312321"; makeSameFraction(m1, m2); TEST2_identical(m2, "1.0000000000000"); TEST2_identical(m1, "100.3232312312321");
    m2 = "-1"; m1 = "100.3232312312321"; makeSameFraction(m1, m2); TEST2_identical(m2, "-1.0000000000000"); TEST2_identical(m1, "100.3232312312321");
    m2 = "-1"; m1 = "-100.3232312312321"; makeSameFraction(m1, m2); TEST2_identical(m2, "-1.0000000000000"); TEST2_identical(m1, "-100.3232312312321");
  }
  {
    SNumber m1, m2;
    m1 = "1"; m2 = "100"; padFrontWithZeros(m1, m2); TEST2_identical(m1, "001"); TEST2_identical(m2, "100");
    m1 = "1.1"; m2 = "100.1"; padFrontWithZeros(m1, m2); TEST2_identical(m1, "001.1"); TEST2_identical(m2, "100.1");
    m1 = "-1"; m2 = "-100"; padFrontWithZeros(m1, m2); TEST2_identical(m1, "-001"); TEST2_identical(m2, "-100");
    m1 = "-1.1"; m2 = "-100.1"; padFrontWithZeros(m1, m2); TEST2_identical(m1, "-001.1"); TEST2_identical(m2, "-100.1");
    m1 = "100"; m2 = "1"; padFrontWithZeros(m1, m2); TEST2_identical(m1, "100"); TEST2_identical(m2, "001");
  }
  {
    TEST2_identical("30", addUnsigned("10", "20"));
    TEST2_identical("70", addUnsigned("30", "40"));
    TEST2_identical("170", addUnsigned("130", "40"));
    TEST2_identical("170.1234", addUnsigned("130.1234", "40"));
    TEST2_identical("170.1234", addUnsigned("130", "40.1234"));
    TEST2_identical("170.6234", addUnsigned("130.1234", "40.5"));
    TEST2_identical("170.6234", addUnsigned("130.5", "40.1234"));
  }
  {
    TEST2_identical("-10", subUnsigned("10", "20"));
    TEST2_identical("-20", subUnsigned("10", "30"));
    TEST2_identical("-2", subUnsigned("199", "201"));
    TEST2_identical("130", subUnsigned("130", "0"));
    TEST2_identical("120", subUnsigned("130", "10"));
    TEST2_identical("119.5", subUnsigned("130", "10.5"));
    TEST2_identical("119.75", subUnsigned("130.25", "10.5"));
    TEST2_identical("-1", subUnsigned("1", "2"));
    TEST2_identical("-1", subUnsigned("0", "1"));
    TEST2_identical("-2", subUnsigned("0", "2"));
    TEST2_identical("-2", subUnsigned("1", "3"));
    TEST2_identical("-9", subUnsigned("10", "-19"));
  }
  {
    TEST2_identical("100", tennerComplement("900"));
    TEST2_identical("900", tennerComplement("100"));
    TEST2_identical("200", tennerComplement("800"));
    TEST2_identical("800", tennerComplement("200"));
    TEST2_identical("300", tennerComplement("700"));
    TEST2_identical("700", tennerComplement("300"));
    TEST2_identical("400", tennerComplement("600"));
    TEST2_identical("600", tennerComplement("400"));
    TEST2_identical("500", tennerComplement("500"));
    TEST2_identical("500", tennerComplement("500"));
    TEST2_identical("10", tennerComplement("0"));
    TEST2_identical("100", tennerComplement("00"));
    TEST2_identical("1000", tennerComplement("000.0"));
    TEST2_identical("1", tennerComplement("999"));
    TEST2_identical("1", tennerComplement("9999"));
  }
  {
    TEST2_identical("-20",    add("10", "-30"));
    TEST2_identical("-30",    add("10", "-40"));
    TEST2_identical("-29.5",    add("10.5", "-40"));
    TEST2_identical("-30",    add("10.55", "-40.55"));
    TEST2_identical("-50",    add("-20", "-30"));
    TEST2_identical("-170",    add("-80", "-90"));
    TEST2_identical("-170.55",  add("-80.55", "-90"));
  }
  {
    TEST2_identical("-20", sub("10", "30"));
    TEST2_identical("0.55", sub("10.55", "10"));
    TEST2_identical("0.77", sub("10.99", "10.22"));
  }
  {
    TEST2_identical("100", mul10("10"));
    TEST2_identical("1000", mul10("100"));
    TEST2_identical("3330", mul10("333"));
    TEST2_identical("3330", mul10("333.0"));
    TEST2_identical("333", mul10("33.3"));
    TEST2_identical("332.5", mul10("33.25"));
    TEST2_identical("330", mul10("33."));
    TEST2_identical("0", mul10("."));
    TEST2_identical("-100", mul10("-10"));
    TEST2_identical("-1000", mul10("-100"));
    TEST2_identical("-3330", mul10("-333"));
    TEST2_identical("-3330", mul10("-333.0"));
    TEST2_identical("-333", mul10("-33.3"));
    TEST2_identical("-332.5", mul10("-33.25"));
    TEST2_identical("-330", mul10("-33."));
    TEST2_identical("0", mul10("-."));
  }
  {
    TEST2_identical("1", div10("10"));
    TEST2_identical("33.3", div10("333"));
    TEST2_identical("-1", div10("-10"));
    TEST2_identical("-33.3", div10("-333"));
    TEST2_identical("0", div10("."));
    TEST2_identical("0.1", div10("1.0"));
    TEST2_identical("1.1", div10("11.0"));
    TEST2_identical("11.11", div10("111.1"));
    TEST2_identical("0", div10("-."));
    TEST2_identical("-0.1", div10("-1.0"));
    TEST2_identical("-1.1", div10("-11.0"));
    TEST2_identical("-11.11", div10("-111.1"));
  }
  {
    TEST2_identical("90", mulIterative("10",9));
    TEST2_identical("-90", mulIterative("-10", 9));
    TEST2_identical("27", mulIterative("3", 9));
    TEST2_identical("-27", mulIterative("-3", 9));
    TEST2_identical("15", mulIterative("3", 5));
    TEST2_identical("-15", mulIterative("-3", 5));
    TEST2_identical("11", mulIterative("5.5", 2));
    TEST2_identical("-11", mulIterative("-5.5", 2));
    TEST2_identical("16.5", mulIterative("5.5", 3));
    TEST2_identical("-16.5", mulIterative("-5.5", 3));
  }
  {
    TEST2_identical("0", mul0to9("10", 0));
    TEST2_identical("10", mul0to9("10", 1));
    TEST2_identical("20", mul0to9("10", 2));
    TEST2_identical("30", mul0to9("10", 3));
    TEST2_identical("40", mul0to9("10", 4));
    TEST2_identical("50", mul0to9("10", 5));
    TEST2_identical("60", mul0to9("10", 6));
    TEST2_identical("70", mul0to9("10", 7));
    TEST2_identical("80", mul0to9("10", 8));
    TEST2_identical("90", mul0to9("10", 9));
    TEST2_identical("-90", mul0to9("-10", 9));
    TEST2_identical("27", mul0to9("3", 9));
    TEST2_identical("-27", mul0to9("-3", 9));
    TEST2_identical("15", mul0to9("3", 5));
    TEST2_identical("-15", mul0to9("-3", 5));
    TEST2_identical("11", mul0to9("5.5", 2));
    TEST2_identical("-11", mul0to9("-5.5", 2));
    TEST2_identical("16.5", mul0to9("5.5", 3));
    TEST2_identical("-16.5", mul0to9("-5.5", 3));
  }
  {
    TEST2_identical("600", mulUnsigned("30", "20"));
    TEST2_identical("30", mulUnsigned("10", "3"));
    TEST2_identical("99", mulUnsigned("33", "3"));
    TEST2_identical("0.25", mulUnsigned("0.5", "0.5"));
    TEST2_identical("21", mulUnsigned("10.5", "2"));
    TEST2_identical("31.5", mulUnsigned("10.5", "3"));
    TEST2_identical("3", mulUnsigned("0.5", "6.0"));
  }
  {
    TEST2_identical("9", mul("3", "3"));
    TEST2_identical("-9", mul("3", "-3"));
    TEST2_identical("-9", mul("-3", "3"));
    TEST2_identical("9", mul("-3", "-3"));
    TEST2_identical("0.125", mul("0.25", "0.5"));
    TEST2_identical("0.125", mul("0.5", "0.25"));
    TEST2_identical("-0.125", mul("-0.25", "0.5"));
    TEST2_identical("-0.125", mul("-0.5", "0.25"));
    TEST2_identical("-0.125", mul("0.25", "-0.5"));
    TEST2_identical("-0.125", mul("0.5", "-0.25"));
  }
  {
    TEST2_identical("0.5", div2("1"));
    TEST2_identical("5", div2("10"));
    TEST2_identical("10", div2("20"));
    TEST2_identical("15", div2("30"));
    TEST2_identical("20", div2("40"));
    TEST2_identical("25", div2("50"));
    TEST2_identical("25.5", div2("51"));
    TEST2_identical("26", div2("52"));
    TEST2_identical("26.5", div2("53"));
    TEST2_identical("27", div2("54"));
    TEST2_identical("27.5", div2("55"));
    TEST2_identical("28", div2("56"));
    TEST2_identical("28.5", div2("57"));
    TEST2_identical("29", div2("58"));
    TEST2_identical("29.5", div2("59"));
    TEST2_identical("30", div2("60"));
    TEST2_identical("30.3", div2("60.6"));
    TEST2_identical("1.2", div2("2.4"));
    TEST2_identical("-30.3", div2("-60.6"));
    TEST2_identical("-1.2", div2("-2.4"));
  }
  {
    TEST2_true(isEqual("10.0", "10"));
    TEST2_true(isEqual("-10.0", "-10"));
    TEST2_true(isEqual("-9.0000", "-9.00"));
    TEST2_true(isEqual("10.1", "10.1"));
    TEST2_true(isEqual("-10.1", "-10.1"));
    TEST2_true(isEqual("-9.1000", "-9.10"));
    TEST2_false(isEqual("10.1", "10"));
    TEST2_false(isEqual("-10.1", "-10"));
    TEST2_false(isEqual("-9.0001", "-9.00"));
  }
  {
    TEST2_true(isBigger("10.1", "10"));
    TEST2_true(isBigger("10.01", "10"));
    TEST2_true(isBigger("10.1", "10.01"));
    TEST2_false(isBigger("10.01", "10.01"));
    TEST2_true(isBigger("0", "-0.01"));
    TEST2_true(isBigger("0", "-.0001"));
    TEST2_true(isBigger("-2", "-2.3"));
    TEST2_false(isBigger("-2.4", "-2.3"));
    TEST2_false(isBigger("-2.4", "-2.4"));
    TEST2_false(isBigger("2.4", "2.4"));
  }
  {
    TEST2_true(isBiggerEqual("10.1", "10"));
    TEST2_true(isBiggerEqual("10.01", "10"));
    TEST2_true(isBiggerEqual("10.1", "10.01"));
    TEST2_true(isBiggerEqual("10.01", "10.01"));
    TEST2_true(isBiggerEqual("0", "-0.01"));
    TEST2_true(isBiggerEqual("0", "-.0001"));
    TEST2_true(isBiggerEqual("-2", "-2.3"));
    TEST2_false(isBiggerEqual("-2.4", "-2.3"));
    TEST2_true(isBiggerEqual("-2.4", "-2.4"));
    TEST2_true(isBiggerEqual("2.4", "2.4"));
  }
  {
    TEST2_false(isSmaller("10.1", "10"));
    TEST2_false(isSmaller("10.01", "10"));
    TEST2_false(isSmaller("10.1", "10.01"));
    TEST2_false(isSmaller("10.01", "10.01"));
    TEST2_false(isSmaller("0", "-0.01"));
    TEST2_false(isSmaller("0", "-.0001"));
    TEST2_false(isSmaller("-2", "-2.3"));
    TEST2_true(isSmaller("-2.4", "-2.3"));
    TEST2_false(isSmaller("-2.4", "-2.4"));
    TEST2_false(isSmaller("2.4", "2.4"));
  }
  {
    TEST2_false(isSmallerEqual("10.1", "10"));
    TEST2_false(isSmallerEqual("10.01", "10"));
    TEST2_false(isSmallerEqual("10.1", "10.01"));
    TEST2_true(isSmallerEqual("10.01", "10.01"));
    TEST2_false(isSmallerEqual("0", "-0.01"));
    TEST2_false(isSmallerEqual("0", "-.0001"));
    TEST2_false(isSmallerEqual("-2", "-2.3"));
    TEST2_true(isSmallerEqual("-2.4", "-2.3"));
    TEST2_true(isSmallerEqual("-2.4", "-2.4"));
    TEST2_true(isSmallerEqual("2.4", "2.4"));
  }
  {
    TEST2_false(isZero("10.1"));
    TEST2_false(isZero("10.01"));
    TEST2_false(isZero("-0.1"));
    TEST2_false(isZero("-.01"));
    TEST2_false(isZero("-.1"));
    TEST2_true(isZero("-0"));
    TEST2_true(isZero("-.0"));
    TEST2_true(isZero("-0.0"));
    TEST2_true(isZero("-."));
    TEST2_true(isZero("0"));
    TEST2_true(isZero(".0"));
    TEST2_true(isZero("0.0"));
    TEST2_true(isZero("."));
  }
  {
    TEST2_identical("0", floor("0"));
    TEST2_identical("1", floor("1"));
    TEST2_identical("-1", floor("-1"));
    TEST2_identical("0", floor("0.5"));
    TEST2_identical("1", floor("1.5"));
    TEST2_identical("2", floor("2.75"));
    TEST2_identical("3", floor("3.25"));
    TEST2_identical("-1", floor("-0.5"));
    TEST2_identical("-2", floor("-1.5"));
    TEST2_identical("-3", floor("-2.75"));
    TEST2_identical("-4", floor("-3.25"));
  }
  {
    TEST2_identical("0",  ceil("0"));
    TEST2_identical("1",  ceil("1"));
    TEST2_identical("-1", ceil("-1"));
    TEST2_identical("1",  ceil("0.5"));
    TEST2_identical("2",  ceil("1.5"));
    TEST2_identical("3",  ceil("2.75"));
    TEST2_identical("4",  ceil("3.25"));
    TEST2_identical("0", ceil("-0.5"));
    TEST2_identical("-1", ceil("-1.5"));
    TEST2_identical("-2", ceil("-2.75"));
    TEST2_identical("-3", ceil("-3.25"));
  }
  {
    TEST2_identical("0", round("0.4"));
    TEST2_identical("0", round("-0.4"));
    TEST2_identical("1", round("0.5"));
    TEST2_identical("-1", round("-0.5"));
    TEST2_identical("1", round("0.6"));
    TEST2_identical("-1", round("-0.6"));
    TEST2_identical("10", round("10.4"));
    TEST2_identical("-10", round("-10.4"));
    TEST2_identical("11", round("10.5"));
    TEST2_identical("-11", round("-10.5"));
    TEST2_identical("11", round("10.6"));
    TEST2_identical("-11", round("-10.6"));
  }
  {
    TEST2_identical("0", abs("0"));
    TEST2_identical("0", abs("-0"));
    TEST2_identical("0.24", abs("0.24"));
    TEST2_identical("0.24", abs("-0.24"));
    TEST2_identical("1", abs("1"));
    TEST2_identical("1", abs("-1"));
    TEST2_identical("1.24", abs("1.24"));
    TEST2_identical("1.24", abs("-1.24"));
  }
  {
    TEST2_identical("1", sign("0"));
    TEST2_identical("1", sign("-0"));
    TEST2_identical("1", sign("0.24"));
    TEST2_identical("-1", sign("-0.24"));
    TEST2_identical("1", sign("1"));
    TEST2_identical("-1", sign("-1"));
    TEST2_identical("1", sign("1.24"));
    TEST2_identical("-1", sign("-1.24"));
  }
  {
    TEST2_identical("0.5", divApprox("10", "20"));
    TEST2_identical("10", divApprox("10", "1"));
    TEST2_identical("5", divApprox("10", "2"));
    TEST2_identical("8", divApprox("16", "2"));
    TEST2_identical("0.75", divApprox("1.5", "2"));
    TEST2_identical("5.25", divApprox("10.5", "2"));
    TEST2_identical("9", divApprox("27", "3"));
    TEST2_identical("90", divApprox("270", "3"));
    TEST2_identical("9", divApprox("270", "30"));
  }
  {
    TEST2_identical("5", modApprox("5", "10"));
    TEST2_identical("7.5", modApprox("7.5", "10"));
    TEST2_identical("7.5", modApprox("17.5", "10"));
    TEST2_identical("7.5", modApprox("27.5", "10"));
    TEST2_identical("5", modApprox("25", "10"));
    TEST2_identical("15", modApprox("35", "20"));
  }
  {
    TEST2_identical("5", divUnsigned("10", "2"));
    TEST2_identical("20", divUnsigned("40", "2"));
    TEST2_identical("40", divUnsigned("400", "10"));
    TEST2_identical("1", divUnsigned("400", "400"));
    TEST2_identical("0.5", divUnsigned("400", "800",1));
    TEST2_identical("8.8888", divUnsigned("10", "1.125",4));
    TEST2_identical("10.1", divUnsigned("20.20", "2", 4));
    TEST2_identical("2", divUnsigned("1000", "500"));
    TEST2_identical("0.5", divUnsigned("500", "1000",1));
    TEST2_identical("1000", divUnsigned("500", "0.5", 0));
  }
  {
    TEST2_identical("-5", div("-10", "2"));
    TEST2_identical("-5", div("10", "-2"));
    TEST2_identical("5", div("10", "2"));
  } 
}

bool SMath::TEST_identical(const SNumber& a, const SNumber& b) {
  return identical(a, b);
}

bool SMath::TEST_unidentical(const SNumber& a, const SNumber& b) {
  return !identical(a, b);
}

bool SMath::TEST_true(bool value) {
  return value;
}

bool SMath::TEST_false(bool value) {
  return !value;
}
#else // SMathWITHUNITTEST
void SMath::unitTest() {
  __smath_log("SMath::UnitTest is disabled!\n");
}
#endif // SMathWITHUNITTEST
