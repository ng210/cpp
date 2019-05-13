/******************************************************************************
Class of the big float type with predefined precision.
This implementation uses the following data structure:
 - sign, 1 means negative number
 - digits (mantissa) is an array of characters, length is predefined by a constant,
 - exponent is an integer (32 bit)
 A number's value is calculated by multiplying its digits by the power of 10 specified by
 the exponent.
 The toString method gives a scientific representation in the form D.DDDDDDDDeEE.

 Supported operations
  - addition: operator+, operator++ (pre and post), operator+=
  - subtraction: operator-, operator-- (pre and post), operator-=
  - multiplication: operator*, operator*=
  - division: operator/, operator/=
  - all comparing operators

The addition and subtraction are implemented by 2 private methods ignoring the sign.
These 2 methods are used by the public operators.
The first operation, the addition calls the unsinged addition or the unsigned subtraction
depending on the signs of the operands. Matching signs leads to an addition, unmatching
signs to subtraction. To add 2 numbers in this representation the digits of the smaller
number are shifted to align the other number decimally.
For example let a = 12.35 and b = 0.025
	a: 12350 => 12350
	b: 25000 => 00250
This can lead to a loss of precision as digits might get shifted out on the right.

Subtraction is derived from addition completely.

Multiplication uses the paper&pencil method learned in elementary school.
Take the first digit d of the multiplier, sum up the number d times. Divide the number
by 10 and repeat this with the next digit.
Shift -that is multiplying by power of 10- is not necessary here, this is achieved by
summing the exponents.

Division is implemented using subtraction. The digits of the result is calculated by
subtracting the divisor from the dividend while the dividend is larger than the divisor.
Here too shifting is not needed, the right magnituted is calculated as the difference
of the exponents.

The class also exposes 2 static instances: Zero and One
******************************************************************************/

#ifndef __FLOAT_H
#define __FLOAT_H

#define DIGIT_COUNT 40

class BigFloat {
	int sign_;
	int length_;
	char digits_[DIGIT_COUNT];
	int exponent_;

	void init();
	BigFloat& unsignedAdd(const BigFloat&);
	BigFloat& unsignedSub(const BigFloat&);
	int unsignedCompare(const BigFloat&) const;
	void shiftDigits(int n);
	void adjust();
public:
	BigFloat();
	BigFloat(int);
	BigFloat(const char*);

	int length() { return length_; }
	int exponent() { return exponent_; }
	const char* digits() { return digits_;  }

	BigFloat& parse(const char*);

	// arithmetic operators
	friend BigFloat& operator+(BigFloat, const BigFloat&);
	friend BigFloat& operator-(BigFloat, const BigFloat&);
	friend BigFloat& operator*(BigFloat, const BigFloat&);
	friend BigFloat& operator/(BigFloat, const BigFloat&);

	BigFloat& operator+=(const BigFloat&);
	BigFloat& operator-=(const BigFloat&);
	BigFloat& operator++();
	BigFloat operator++(int);
	BigFloat& operator--();
	BigFloat operator--(int);

	BigFloat& operator*=(const BigFloat&);
	BigFloat& operator/=(const BigFloat&);

	// logic operators
	friend bool operator<(const BigFloat&, const BigFloat&);
	friend bool operator==(const BigFloat&, const BigFloat&);
	friend inline bool operator>(const BigFloat& n, const BigFloat& m) { return m < n; }
	friend inline bool operator<=(const BigFloat& n, const BigFloat& m) { return !(n > m); }
	friend inline bool operator>=(const BigFloat& n, const BigFloat& m) { return !(n < m); }
	friend inline bool operator!=(const BigFloat& n, const BigFloat& m) { return !(n == m); }

	char* toString(int digits = 0);

	static BigFloat Zero;
	static BigFloat One;
};



#endif