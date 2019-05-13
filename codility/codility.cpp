#include <iostream>
#include <vector>
//#include <algorithm>
//#include <string>
//#include <numeric>
//#include <functional>
#include <stdio.h>

using namespace std;

//int codility01(vector<int> &A) {
//	vector<int> B;
//	int res = 1;
//	for (auto it = A.begin(); it != A.end(); it++) {
//		if (*it > 0) {
//			B.push_back(*it);
//		}
//	}
//	if (B.size() > 0) {
//		auto it = B.begin();
//		sort(it, B.end());
//		while (it != B.end()) {
//			int n = *it;
//			if (res < n) {
//				break;
//			}
//			it++;
//			res = n + 1;
//		}
//
//	}
//	return res;
//}
//
//void test01a() {
//	vector<int> A = { INT_MAX };
//	vector<int> B = { 1, 2, 3 };
//	vector<int> C = { -1, -3 };
//	vector<int> D = { 1, 3, 6, 4, 1, 2 };
//	cout << "01: " << codility01(A) << "\n";
//	cout << "01: " << codility01(B) << "\n";
//	cout << "01: " << codility01(C) << "\n";
//	cout << "01: " << codility01(D) << "\n";
//}
//
//int codility05a(vector<int> &A) {
//	long pairs = 0;
//	if (A.size() > 0) {
//		long carsMovingWest = 0;
//		auto it = A.end();
//		do {
//			it--;
//			if (*it == 1) carsMovingWest++;
//			else pairs += carsMovingWest;
//			if (pairs > 1000000000) {
//				pairs = -1;
//				break;
//			}
//		} while (it != A.begin());
//	}
//	return pairs;
//}
//
//vector<string> sixt02(vector<int> &A) {
//	vector<string> results;
//
//	// old-style for-loop
//	string str1;
//	for (std::vector<int>::const_iterator it = A.begin(); it != A.end(); ++it)
//	{
//		str1.append(to_string(*it));
//	}
//	results.push_back(str1);
//
//	// modern loop
//	string str2;
//	for (auto i : A)
//	{
//		str2.append(to_string(i));
//	}
//	results.push_back(str2);
//
//	// without loop
//	string str3;
//	results.push_back(accumulate(A.begin(), A.end(), string(), [](string& a, int b) -> string { return a.append(to_string(b)); }));
//
//	return results;
//}
//
//void test05a() {
//	vector<int> A = { 0, 1, 0, 1, 1, 0 };
//	vector<int> B = { 0, 1, 0, 1, 1 };
//	vector<int> C = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 };
//	vector<int> D;
//	for (int i = 0; i < 100000; i++) {
//		D.push_back(i % 2);
//	}
//	cout << "count: " << codility05a(A) << "\n";
//	cout << "count: " << codility05a(B) << "\n";
//	cout << "count: " << codility05a(C) << "\n";
//	cout << "count: " << codility05a(D) << "\n";
//}

int sixt03(vector<int> &A) {
	int result = INT_MIN;

	for (auto it1 = A.begin(); it1 < A.end(); it1++) {
		for (auto it2 = it1; it2 < A.end(); it2++) {
			int appeal = *it1 + *it2 + (int)(it2 - it1);
			if (appeal > result) {
				result = appeal;
			}
		}
	}

	return result;
}

void testSixt03() {
	vector<int> A = { 1, 3, -3 };
	vector<int> B = { -8, 4, 0, 5, -3, 6 };
	vector<int> C = { -1, -2, -3, -4, -1 };
	vector<int> D = { 999999999, 0, 0, 1, 0, 0, 0, 1000000000 };
	//vector<int> D;
	//for (int i = 0; i < 100000; i++) {
	//	D.push_back(i % 2);
	//}
	//cout << "count: " << sixt03(A) << "\n";
	//cout << "count: " << sixt03(B) << "\n";
	//cout << "count: " << sixt03(C) << "\n";
	cout << "count: " << sixt03(D) << "\n";
	//cout << "count: " << codility05a(D) << "\n";
}






//std::function<double(int)> mul(double factor) {
//	return [factor](int number) {
//		cout << factor << " * " << number << " = ";
//		return static_cast<double>(number) * factor;
//	};
//}

int main()
{
	// test01a();
	// test05a();
	//testSixt03();
	testMicroDoc();
}

