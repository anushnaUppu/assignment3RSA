#include <stdio.h>

#include <stdlib.h>

#include <math.h>

#include<time.h>

#define MAXDIGITS 10000

#define PLUS 1
#define MINUS - 1

typedef struct {
  char digits[MAXDIGITS];
  int signbit;
  int lastdigit;
}
bignum;

clock_t start, end;
double cpu_time_used;
void encrypt();
void decryptByNaiveApproach();
void decryptByCrt();
void subtractBignum();
void zeroJustify();
int compareBignum();
void addBignum();
void divideBignum();
void multiplyBignum();
void convertToBignum();
void power();
void modulo();
void printBignum();
bignum modInverse();
void configure();
void preCompute();

bignum p, q, n, d, m, c, e, output, d1, d2, one, zero, xp, xq;
int num;

int main() {
  printf("Enter the number:\n");
  scanf("%d", & num);
  configure();
  encrypt(e, m);
  start = clock();
  decryptByNaiveApproach();
  end = clock();
  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("Time taken by Naive method = %lf s", cpu_time_used);
  preCompute();
  start = clock();
  decryptByCrt(d1, d2, xp, xq);
  end = clock();
  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("Time taken by using CRT  = %lf s\n", cpu_time_used);
  return 0;
}

void configure() {
  convertToBignum(num, & m);
  convertToBignum(173, & p);
  convertToBignum(149, & q);
  convertToBignum(3, & e);
  convertToBignum(16971, & d);
  convertToBignum(1, & one);
  multiplyBignum( & p, & q, & n);
}

void preCompute() {
  bignum ptemp, qtemp;
  subtractBignum( & p, & one, & ptemp);
  subtractBignum( & q, & one, & qtemp);
  modulo( & d, & ptemp, & d1);
  modulo( & d, & qtemp, & d2);
  bignum modInverse1 = modInverse(q, p);
  bignum modInverse2 = modInverse(p, q);
  multiplyBignum( & q, & modInverse1, & xp);
  multiplyBignum( & p, & modInverse2, & xq);
}

void encrypt(bignum e, bignum m) {
  bignum cx;
  power( & m, & e, & cx);
  modulo( & cx, & n, & c);
  printf("THE ENCRYPTED NUMBER IS\n");
  printBignum( & c);
}

void decryptByNaiveApproach() {
  bignum ct, key, ctemp, ntemp;
  ct = one;
  key = d;
  convertToBignum(0, & zero);
  bignum keytemp = key;
  while (compareBignum( & key, & zero) != 0) {
    subtractBignum( & keytemp, & one, & key);
    multiplyBignum( & ct, & c, & ctemp);
    modulo( & ctemp, & n, & ct);
    keytemp = key;
  }
  printf("THE DECRYPTED NUMBER BY NAIVE METHOD IS\n");
  printBignum( & ct);
}
void decryptByCrt(bignum d1, bignum d2, bignum xp, bignum xq) {
  bignum c1, c2, keytemp, vp, vptemp, vq, vqtemp, op;
  vp = one;
  vq = one;
  modulo( & c, & p, & c1);
  modulo( & c, & q, & c2);
  convertToBignum(0, & zero);
  while (compareBignum( & d1, & zero) != 0) {
    keytemp = d1;
    subtractBignum( & keytemp, & one, & d1);
    multiplyBignum( & vp, & c1, & vptemp);
    modulo( & vptemp, & p, & vp);
  }
  while (compareBignum( & d2, & zero) != 0) {
    keytemp = d2;
    subtractBignum( & keytemp, & one, & d2);
    multiplyBignum( & vq, & c2, & vqtemp);
    modulo( & vqtemp, & q, & vq);
  }
  bignum result1, result2, result3;
  multiplyBignum( & xp, & vp, & result2);
  multiplyBignum( & xq, & vq, & result3);
  addBignum( & result2, & result3, & result1);
  modulo( & result1, & n, & op);
  printf("\nTHE DECRYPTED NUMBER BY USING CRT IS\n");
  printBignum( & op);
}

void gcdExtended(bignum a, bignum b, bignum * s, bignum * t) {
  convertToBignum(0, & zero);
  if (compareBignum( & zero, & a) == 0) {
    convertToBignum(1, & one);
    * s = zero;
    * t = one;
    return;
  }
  bignum x1, y1, c, var1, var2;
  modulo( & b, & a, & c);
  gcdExtended(c, a, & x1, & y1);
  divideBignum( & b, & a, & var2);
  multiplyBignum( & var2, & x1, & var1);
  subtractBignum( & y1, & var1, s);
  * t = x1;

}

bignum modInverse(bignum a, bignum m) {
  bignum s, t, res, temp1, temp2;
  gcdExtended(a, m, & s, & t);
  modulo( & s, & m, & temp2);
  addBignum( & temp2, & m, & temp1);
  modulo( & temp1, & m, & res);
  return res;
}

void printBignum(bignum * n) {
  int i;

  for (i = n -> lastdigit; i >= 0; i--)
    printf("%c", '0' + n -> digits[i]);

  printf("\n");
}

void convertToBignum(int s, bignum * n) {
  int i;
  int t;

  n -> signbit = PLUS;

  for (i = 0; i < MAXDIGITS; i++) n -> digits[i] = (char) 0;

  n -> lastdigit = -1;

  t = abs(s);

  while (t > 0) {
    n -> lastdigit++;
    n -> digits[n -> lastdigit] = (t % 10);
    t = t / 10;
  }

  if (s == 0) n -> lastdigit = 0;
}

void initializeBignum(bignum * n) {
  convertToBignum(0, n);
}

int max(int a, int b) {
  if (a > b) return (a);
  else return (b);
}

void addBignum(bignum * a, bignum * b, bignum * c) {
  int carry;
  int i;

  initializeBignum(c);

  if (a -> signbit == b -> signbit) c -> signbit = a -> signbit;
  else {
    if (a -> signbit == MINUS) {
      a -> signbit = PLUS;
      subtractBignum(b, a, c);
      a -> signbit = MINUS;
    } else {
      b -> signbit = PLUS;
      subtractBignum(a, b, c);
      b -> signbit = MINUS;
    }
    return;
  }

  c -> lastdigit = max(a -> lastdigit, b -> lastdigit) + 1;
  carry = 0;

  for (i = 0; i <= (c -> lastdigit); i++) {
    c -> digits[i] = (char)(carry + a -> digits[i] + b -> digits[i]) % 10;
    carry = (carry + a -> digits[i] + b -> digits[i]) / 10;
  }

  zeroJustify(c);
}

void subtractBignum(bignum * a, bignum * b, bignum * c) {
  int borrow;
  int v;
  int i;

  initializeBignum(c);

  if ((a -> signbit == MINUS) || (b -> signbit == MINUS)) {
    b -> signbit = -1 * b -> signbit;
    addBignum(a, b, c);
    b -> signbit = -1 * b -> signbit;
    return;
  }

  if (compareBignum(a, b) == PLUS) {
    subtractBignum(b, a, c);
    c -> signbit = MINUS;
    return;
  }

  c -> lastdigit = max(a -> lastdigit, b -> lastdigit);
  borrow = 0;

  for (i = 0; i <= (c -> lastdigit); i++) {
    v = (a -> digits[i] - borrow - b -> digits[i]);
    if (a -> digits[i] > 0)
      borrow = 0;
    if (v < 0) {
      v = v + 10;
      borrow = 1;
    }

    c -> digits[i] = (char) v % 10;
  }

  zeroJustify(c);
}

int compareBignum(bignum * a, bignum * b) {
  int i;

  if ((a -> signbit == MINUS) && (b -> signbit == PLUS)) return (PLUS);
  if ((a -> signbit == PLUS) && (b -> signbit == MINUS)) return (MINUS);

  if (b -> lastdigit > a -> lastdigit) return (PLUS * a -> signbit);
  if (a -> lastdigit > b -> lastdigit) return (MINUS * a -> signbit);

  for (i = a -> lastdigit; i >= 0; i--) {
    if (a -> digits[i] > b -> digits[i]) return (MINUS * a -> signbit);
    if (b -> digits[i] > a -> digits[i]) return (PLUS * a -> signbit);
  }

  return (0);
}

void zeroJustify(bignum * n) {
  while ((n -> lastdigit > 0) && (n -> digits[n -> lastdigit] == 0))
    n -> lastdigit--;

  if ((n -> lastdigit == 0) && (n -> digits[0] == 0))
    n -> signbit = PLUS;
}

void digit_shift(bignum * n, int d) {
  int i;

  if ((n -> lastdigit == 0) && (n -> digits[0] == 0)) return;

  for (i = n -> lastdigit; i >= 0; i--)
    n -> digits[i + d] = n -> digits[i];

  for (i = 0; i < d; i++) n -> digits[i] = 0;

  n -> lastdigit = n -> lastdigit + d;
}

void multiplyBignum(bignum * a, bignum * b, bignum * c) {
  bignum row;
  bignum tmp;
  int i, j;

  initializeBignum(c);

  row = * a;

  for (i = 0; i <= b -> lastdigit; i++) {
    for (j = 1; j <= b -> digits[i]; j++) {
      addBignum(c, & row, & tmp);
      * c = tmp;
    }
    digit_shift( & row, 1);
  }

  c -> signbit = a -> signbit * b -> signbit;

  zeroJustify(c);
}

void divideBignum(bignum * a, bignum * b, bignum * c) {
  bignum row;
  bignum tmp;
  int asign, bsign;
  int i, j;

  initializeBignum(c);

  c -> signbit = a -> signbit * b -> signbit;

  asign = a -> signbit;
  bsign = b -> signbit;

  a -> signbit = PLUS;
  b -> signbit = PLUS;

  initializeBignum( & row);
  initializeBignum( & tmp);

  c -> lastdigit = a -> lastdigit;

  for (i = a -> lastdigit; i >= 0; i--) {
    digit_shift( & row, 1);
    row.digits[0] = a -> digits[i];
    c -> digits[i] = 0;
    while (compareBignum( & row, b) != PLUS) {
      c -> digits[i]++;
      subtractBignum( & row, b, & tmp);
      row = tmp;
    }
  }

  zeroJustify(c);

  a -> signbit = asign;
  b -> signbit = bsign;
}
void modulo(bignum * num, bignum * divisor, bignum * remainder) {
  bignum nx1, nx2;
  initializeBignum( & nx1);
  initializeBignum( & nx2);
  divideBignum(num, divisor, & nx1);
  multiplyBignum(divisor, & nx1, & nx2);
  subtractBignum(num, & nx2, remainder);

}
void power(bignum * base, bignum * exponent, bignum * result) {
  bignum zero, one, nx1, nx2;
  convertToBignum(0, & zero);
  convertToBignum(1, & one);
  convertToBignum(1, & nx2);
  nx1 = * exponent;
  while (compareBignum( & zero, exponent) != 0) {
    multiplyBignum( & nx2, base, result);
    subtractBignum( & nx1, & one, exponent);
    nx1 = * exponent;
    nx2 = * result;
  }
}
