/*
 * CS:APP Data Lab
 *
 * Kshitij Khode
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code
  must conform to the following style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>

  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.


  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function.
     The max operator count is checked by dlc. Note that '=' is not
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 *
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce
 *      the correct answers.
 */


#endif
//1
/*
 * evenBits - return word with all even-numbered bits set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */

int evenBits(void) {
  return (0x55 << 24) | (0x55 << 16) | (0x55 << 8) | 0x55;
}
/*
 * bitNor - ~(x|y) using only ~ and &
 *   Example: bitNor(0x6, 0x5) = 0xFFFFFFF8
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */

/* Using property ~(x & y) = ~x | y and the other way around */
int bitNor(int x, int y) {
  return ~x & ~y;
}
/*
 * TMax - return maximum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */

/* Setting bit to 0 for positive and rest of bits to 1 for max value */
int tmax(void) {
  return ~(0x80 << 24);
}
//2
/*
 * implication - return x -> y in propositional logic - 0 for false, 1
 * for true
 *   Example: implication(1,1) = 1
 *            implication(1,0) = 0
 *   Legal ops: ! ~ ^ |
 *   Max ops: 5
 *   Rating: 2
 */

/* Return needs to mirror value of y, when x has any bit set. Hence when x has a bit set
 * y will be or'ed with bunch of 0s, thus returning y */
int implication(int x, int y) {
  return !x | y;
}

/*
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */

/* >> is arithmetic division by 2 and << is multiplication by 2. To deal with negatives
 * example x = -5, n = 1. Then simple x >> (0x01 << n), assuming 8 bit ints, will generate
 * 11111101b instead of 11111110. This is because in negetive we're rounding to infinity in
 * division. So by (0x01 << n) + ~0x00), we generate n trailing 1s, then add 1 to it to 
 * generate n+1 carry. As a result, it adds 2^(n+1) to original number to make it round to 0 */
int divpwr2(int x, int n) {
  return (x + ((x >> 31) & ((0x01 << n) + ~0x00))) >> n;
}
/*
 * isNegative - return 1 if x < 0, return 0 otherwise
 *   Example: isNegative(-1) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */

/* Move MSB signed bit to LSB and & it with 0x01 to return true if MSB signed bit was set or not. */
int isNegative(int x) {
  return (x >> 31) & 0x01;
}
//3
/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */

/* If x is true, then !x will make it 0x00000000, 2s compliment of which will also give 0. And'ing
 * that with z will mute its content, while !!x will generate 0xFFFFFFFF, &'ing that off with y
 * will generate y itself. Or'ing Zs muted content and Y, will generate y. */
int conditional(int x, int y, int z) {
  int not_x = !x;
  return (((~not_x)+1) & z) | (((~!not_x)+1) & y);
}
/*
 * rotateRight - Rotate x to the right by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateRight(0x87654321,4) = 0x18765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 25
 *   Rating: 3
 */

/* Generate trailing n 1s. Then ~'ing it will make them leading 1s. &'ing them with bits pushed out
 * will restore them. */
int rotateRight(int x, int n) {
  /* Can use 2^n - 1 for masking as well, but this manipulation seems more appropriate for a rotate fn*/
  int neg_n = ~n + 1;
  return ((x >> n) & ~(~0x00 << (32 + neg_n))) | (x << (32 + neg_n));
}
//4
/*
 * absVal - absolute value of x
 *   Example: absVal(-1) = 1.
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */

/* Using conditional structure to return x when MSB signed bit is set and it's 2s complement otherwise */
int absVal(int x) {
  int is_neg = (x >> 31);
  return (is_neg & (~x + 1)) | (~is_neg & x);
}
/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */

/* Only 0, will have itself and its 2s compliments MSB set to 0. */
int bang(int x) {
  return ((((x >> 31) & 0x01) | (((~x+1) >> 31) & 0x01)) ^ 0x01);
}
//float
/*
 * float_abs - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument..
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */

/* Remove signed bit if uf below min positive float value */
unsigned float_abs(unsigned uf) {
  unsigned abs_uf = uf & 0x7FFFFFFF;
  if (abs_uf >= 0x7F800001) return uf;
  return abs_uf;
}
/*
 * float_pwr2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */

/* If x is greater than the highest value of exp, then round to max floating value
 * If x allows normalized values then return normalized version
 * If x does not allow normalized value then set appropriate fraction part */
unsigned float_pwr2(int x) {
  if      (x > 127)                 return 0x7f800000;
  else if (x <= 127 && x > -127)    return (x + 0x7F) << 23;
  else if (x <= -127 && x >= -149)  return 1 << (149 + x);
  return 0;
}
/*
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */

/* If 0x00000000, then return 0 in floating point representation which is 0 itself
 * If -0 provided, then return as such
 * If -ve number provided then do 2s complement to prep for positive fraction rule
 * Then calculate the exp required, setting x to have a 1 near MSB (for the assumed 1
 * for fraction part)
 * Shift x accordingly to the 1st 23 bits as required for fraction part.
 * Finally round appropriately for 1s being thrown away. */
unsigned float_i2f(int x) {
  /* http://binary-system.base-conversion.ro used as reference for figuring corner cases */ 
  int adj_exp   = 158;
  int sign_mask = 0x80000000;
  int is_signed = sign_mask & x;
  int fraction  = 0; 
  if (!x)              return 0;
  if (x == sign_mask)  return 0xCF000000;

  if (is_signed) {
    x = ~x + 1;
  }
  while (!(sign_mask & x)) {
    adj_exp--;
    x = x << 1;
  }
  fraction = (x & ~sign_mask) >> 8;
  if ((x & 0x80) && ((x & 0x7F) > 0 || fraction & 0x01)) {
    fraction++;
  }
  return is_signed + (adj_exp << 23) + fraction;
}
