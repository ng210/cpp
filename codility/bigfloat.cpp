#include "bigfloat.h"

BigFloat BigFloat::Zero(0);
BigFloat BigFloat::One(1);

char* _itoa(int n, char * str) {
	char* dest = str;
	if (n < 0) {
		*dest++ = '-';
		n = -n;
	}
	char d = n % 10;
	n /= 10;
	if (n != 0) {
		dest = _itoa(n, dest);
	}
	*dest++ = d + '0';
	*dest = '\0';
	return dest;
}

BigFloat::BigFloat() {
	init();
}
BigFloat::BigFloat(int n) {
	init();
	char digits[DIGIT_COUNT + 1];
	_itoa(n, digits);
	parse((const char*)digits);
}
BigFloat::BigFloat(const char* str) {
	init();
	parse(str);
}

void BigFloat::init() {
	sign_ = 0;
	length_ = 0;
	exponent_ = 0;
	for (int i = 0; i < DIGIT_COUNT; i++) {
		digits_[i] = 0;
	}
}

BigFloat& BigFloat::parse(const char* str) {
	const char* ptr = str;
	// check sign
	if (*ptr == '-') {
		sign_ = 1;
		ptr++;
	}

	char c;
	int state = 0;
	length_ = 0;
	while (state != 5 && (c = *ptr++) != '\0') {
		switch (state) {
		case 0:
			if (c == '0') state = 1;
			else if (c == '.') state = 2;
			else if (c >= '1' && c <= '9') {
				digits_[length_++] = c - '0';
				state = 4;
			}
			else state = 5;
			break;
		case 1:
			if (c == '.') {
				exponent_--;
				state = 2;
			} else state = 5;
			break;
		case 2:
			if (c == '0') {
				exponent_--;
				break;
			}
			state = 3;
		case 3:
			if (c >= '0' && c <= '9') {
				digits_[length_++] = c - '0';
			}
			else state = 5;
			break;
		case 4:
			if (c >= '0' && c <= '9') {
				digits_[length_++] = c - '0';
				exponent_++;
			} else if (c == '.') state = 3;
			else state = 5;
		}
	}
	if (state == 0 || state == 5) {
		// error
		;
	}
	return *this;
}


void BigFloat::shiftDigits(int n) {
	if (n > 0) {
		for (int i = DIGIT_COUNT - 1; i >= n; i--) {
			digits_[i] = digits_[i - n];
		}
		for (int i = 0; i < n; i++) {
			digits_[i] = 0;
		}
	} else {
		for (int i = 0; i < DIGIT_COUNT + n; i++) {
			digits_[i] = digits_[i - n];
		}
		for (int i = DIGIT_COUNT + n; i < DIGIT_COUNT; i++) {
			digits_[i] = 0;
		}
	}
}

void BigFloat::adjust() {
	int len = 0;
	while (digits_[len] == 0) len++;
	shiftDigits(-len);
	exponent_ -= len;
	for (int i = 0; i < DIGIT_COUNT;) {
		if (digits_[i++] != 0) length_ = i;
	}
}

BigFloat& BigFloat::unsignedAdd(const BigFloat& o) {
	// shift o/n into range
	// m = 1500 => 1.50000E+3
	// n = 0.15 => 0.00015E+0
	// shift(n.digits, m.exp) = shift('15000000', 3) => n.digits = '00015000'
	BigFloat n = o;
	int finalShift = 0;
	if (exponent_ != n.exponent_) {
		if (exponent_ < n.exponent_) {
			finalShift = n.exponent_ - exponent_;
			shiftDigits(finalShift);
		} else {
			n.shiftDigits(exponent_ - n.exponent_);
		}
	}

	int carry = 0;
	int i = DIGIT_COUNT;
	while (--i >= 0) {
		char d = digits_[i] + n.digits_[i] + carry;
		if (d >= 10) {
			d -= 10;
			carry = 1;
		} else {
			carry = 0;
		}
		digits_[i] = d;
	}

	if (carry) {
		shiftDigits(1);
		digits_[0] = 1;
		exponent_++;
		length_++;
	}
	adjust();
	exponent_ += finalShift;
	//shiftDigits(-);

	return *this;
}
BigFloat& BigFloat::unsignedSub(const BigFloat& o) {
	BigFloat n = o;
	n.shiftDigits(exponent_ - n.exponent_);

	int borrow = 0;
	int i = DIGIT_COUNT;
	while (--i >= 0) {
		char d = digits_[i] - n.digits_[i] - borrow;
		if (d < 0) {
			d += 10;
			borrow = 1;
		} else {
			borrow = 0;
		}
		digits_[i] = d;
	}

	if (borrow) {
		shiftDigits(1);
		digits_[0] = 1;
		exponent_++;
		length_++;
	}
	adjust();

	return *this;
}
int BigFloat::unsignedCompare(const BigFloat& n) const {
	int res = exponent_ - n.exponent_;
	if (res == 0) {
		int len = length_ > n.length_ ? length_ : n.length_;
		for (int i = 0; i < len; i++) {
			if ((res = digits_[i] - n.digits_[i]) != 0) break;
		}
	}
	return res;
}


BigFloat& operator+(BigFloat n, const BigFloat& m) {
	return n += m;
}
BigFloat& operator-(BigFloat n, const BigFloat& m) {
	return n -= m;
}
BigFloat& BigFloat::operator+=(const BigFloat& n) {
	if (sign_ == n.sign_) {
		unsignedAdd(n);
	} else {
		int cmp = unsignedCompare(n);
		if (cmp > 0) {
			// 120 += (-112) => 120 - 112
			unsignedSub(n);
		} else if (cmp < 0) {
			// 112 += (-120) => -(120 - 112)
			BigFloat tmp = n;
			*this = tmp.unsignedSub(*this);
			this->sign_ = tmp.sign_;
		} else {
			*this = BigFloat::Zero;
		}
	}
	return *this;
}
BigFloat& BigFloat::operator-=(const BigFloat& n) {
	BigFloat tmp = n;
	tmp.sign_ = !tmp.sign_;
	return *this += tmp;
}
BigFloat& BigFloat::operator++() {
	return *this += BigFloat::One;
}
BigFloat BigFloat::operator++(int) {
	BigFloat tmp(*this);
	operator++();
	return tmp;
}
BigFloat& BigFloat::operator--() {
	return *this -= BigFloat::One;
}
BigFloat BigFloat::operator--(int) {
	BigFloat tmp(*this);
	operator--();
	return tmp;
}

BigFloat& operator*(BigFloat m, const BigFloat& n) {
	return m *= n;
}
BigFloat& operator/(BigFloat m, const BigFloat& n) {
	return m /= n;
}
BigFloat& BigFloat::operator*=(const BigFloat& n) {
	BigFloat acc;
	BigFloat tmp = *this;
	tmp.exponent_ += n.exponent_;
	for (int i = 0; i < n.length_; i++) {
		for (int j = 0; j < n.digits_[i]; j++) {
			acc.unsignedAdd(tmp);
		}
		tmp.exponent_--;
	}
	acc.exponent_ = this->exponent_ + n.exponent_;
	acc.sign_ = this->sign_ == n.sign_ ? 0 : 1;
	return *this = acc;
}
BigFloat& BigFloat::operator/=(const BigFloat& n) {
	BigFloat dividend = *this;
	BigFloat divisor = n;
	int i = 0;
	divisor.exponent_ = dividend.exponent_;
	while (i < DIGIT_COUNT && dividend != 0) {
		char d = 0;
		while (dividend.unsignedCompare(divisor) >= 0) {
			dividend.unsignedSub(divisor);
			d++;
		}
		dividend.exponent_++;
		this->digits_[i] = d;
		i++;
	}
	adjust();
	//this->length_ = i;

	this->exponent_ -= n.exponent_;
	this->sign_ = this->sign_ == n.sign_ ? 0 : 1;
	return *this;
}


bool operator<(const BigFloat& m, const BigFloat& n) {
	if (!n.sign_ && m.sign_) {
		return false;
	}
	if (n.sign_ && !m.sign_) {
		return true;
	}
	return n.unsignedCompare(m) < 0;

}
bool operator==(const BigFloat& m, const BigFloat& n) {
	return m.sign_ && n.sign_ && m.unsignedCompare(n) == 0;
}

char* BigFloat::toString(int digits) {
	// format: -x.xxxxxxE-xxxx
	if (digits == 0) digits = length_;
	char* str = new char[digits + 10 + 8];
	char* ptr = str;
	if (sign_) *ptr++ = '-';
	int i = 0;
	*ptr++ = digits_[i++] + '0';
	*ptr++ = '.';
	for (; i < digits; i++) {
		*ptr++ = digits_[i] + '0';
	}
	*ptr++ = 'e';
	_itoa(exponent_, ptr);
	return str;
}
