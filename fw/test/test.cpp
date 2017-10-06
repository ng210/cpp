#include <stdio.h>
//#include <stdlib.h>
#include "base/str.h"
#include "consoleapp.h"
#include "base/MemoryMgr.h"

NAMESPACE_FRMWRK_USE


void test() {
	String str1 = "AbCdEf";
	String str2 = "GhIjKl";
	String str3 = "MnOpQr";
	String str = str1 + "_" + str2 + " " + str3;
	printf("1. str3=str1+\"_\"+str2+\" \"='%s'\n", str.toCString());
	printf("2.1 str3.charAt(2)='%c'\n", str[2]);
	printf("2.2 str.charAt(-1)='%c'\n", str[-1]);
	printf("2.3 str.charAt(25)='%c'\n", str[25]);
	printf("3.1 str.indexOf(str1)=%lld\n", str.indexOf(str1));
	printf("3.2 str.indexOf(str2)=%lld\n", str.indexOf(str2));
	printf("3.3 str.indexOf(str3)=%lld\n", str.indexOf(str3));
	printf("4.1 str.lastIndexOf(str1)=%lld\n", str.lastIndexOf(str1));
	printf("4.2 str.lastIndexOf(str2)=%lld\n", str.lastIndexOf(str2));
	printf("4.3 str.lastIndexOf(str3)=%lld\n", str.lastIndexOf(str3));
	printf("4.4 str.lastIndexOf(\"Ef\")=%lld\n", str.lastIndexOf("Ef"));
	printf("5. str.startsWith('str1')='%s'\n", str.startsWith(str1) ? "true" : "false");
	printf("6. str.startsWith('str2')='%s'\n", str.startsWith(str2) ? "true" : "false");
	printf("7. str.endsWith('str2')='%s'\n", str.endsWith(str2) ? "true" : "false");
	printf("7. str.endsWith('str3')='%s'\n", str.endsWith(str3) ? "true" : "false");
	printf("8.1 str.substring(2,6)='%s'\n", str.substring(2,6).toCString());
	printf("8.2 str.substring(-2,6)='%s'\n", str.substring(-2, 6).toCString());
	printf("8.3 str.substring(-22,6)='%s'\n", str.substring(-22, 6).toCString());
	printf("8.4 str.substring(2,-6)='%s'\n", str.substring(2, -6).toCString());
	printf("8.5 str.substring(2)='%s'\n", str.substring(2).toCString());
	printf("9.1 str.substr(2,6)='%s'\n", str.substr(2, 6).toCString());
	printf("9.2 str.substr(-2,6)='%s'\n", str.substr(-2, 6).toCString());
	printf("9.3 str.substr(-22,6)='%s'\n", str.substr(-22, 6).toCString());
	printf("9.4 str.substr(2,-6)='%s'\n", str.substr(2, -6).toCString());
	printf("9.5 str.substr(2)='%s'\n", str.substr(2).toCString());
	String str4 = "\t\n";
	str = str4 + str + str4;
	printf("10. str.trim()='%s'\n", str.trim().toCString());
	printf("10. String(\"  a  \").trim()='%s'\n", String("  a  ").trim().toCString());
}

//class A;
//class A_ {
//	friend class A;
//	int id_;
//protected:
//	int refCount_;
//	char name_[16];
//
//	int addRef() {
//		return ++refCount_;
//	}
//
//	int delRef() {
//		return --refCount_;
//	}
//	A_() : id_(-1),refCount_(1) {
//		printf("A_(1)\n");
//		for (int i = 0; i < 16; i++) name_[0] = '\0';
//	}
//	A_(char *name, int id) : id_(id) {
//		printf("A_(2)\n");
//		strncpy(name_, 16, name);
//	}
//	virtual ~A_() {
//		printf("~A_\n");
//	}
//	virtual const char* getName() {
//		return strdup(name_);
//	}
//};
//
//class B;
//class B_ : public A_ {
//	friend class B;
//protected:
//	char prefix_[16];
//
//	B_() {
//		printf("B_(1)\n");
//		for (int i = 0; i < 16; i++) prefix_[0] = '\0';
//		for (int i = 0; i < 16; i++) name_[0] = '\0';
//	}
//	B_(const char *prefix, const char *name) {
//		printf("B_(2)\n");
//		strncpy(prefix_, 16, prefix);
//		strncpy(name_, 16, name);
//	}
//	~B_() {
//		printf("~B_\n");
//	}
//	const char* getName() {
//		char temp[128];
//		size_t len = strlen(prefix_);
//		strncpy(temp, 16, prefix_);
//		strncpy(&temp[len], 16, name_);
//		return strdup(temp);
//	}
//};
//
//class C;
//class C_ : public A_ {
//	friend class C;
//	char postfix_[16];
//protected:
//	C_() {
//		printf("C_(1)\n");
//		for (int i = 0; i < 16; i++) name_[0] = '\0';
//		for (int i = 0; i < 16; i++) postfix_[0] = '\0';
//	}
//	C_(const char *prefix, const char *name, const char *postfix) {
//		printf("B_(2)\n");
//		strncpy(name_, 16, name);
//		strncpy(postfix_, 16, postfix);
//	}
//	~C_() {
//		printf("~C_\n");
//	}
//	const char* getName() {
//		char temp[128];
//		size_t len = strlen(name_);
//		strncpy(temp, 16, name_);
//		strncpy(&temp[len], 16, postfix_);
//		return strdup(temp);
//	}
//};
//
//class N;
//class N_ : public A_ {
//	friend class N;
//	N_() {
//		printf("N_(1)\n");
//		strncpy(name_, 16, "null");
//	}
//protected:
//};
//
//
//class A {
//protected:
//	A_* ptr_;
//
//	A() {
//		printf("A()\n");
//	}
//	void addRef();
//	void delRef();
//	void null_();
//
//public:
//	A(char *name, int id);
//	virtual ~A();
//
//	A& operator=(A& ref) {
//		printf("operator=\n");
//		delRef();
//		ptr_ = ref.ptr_;
//		ptr_->addRef();
//		return *this;
//	}
//
//	virtual A& operator=(const N& ref);
//
//	int getRefcount() {
//		return ptr_->refCount_;
//	}
//
//
//	const char* getName();
//};
//
//class B : public A {
//public:
//	B() {
//		printf("B()\n");
//	}
//	B(const char *prefix, const char *name);
//	virtual ~B();
//	virtual B& operator=(const N& ref);
//
//	char* getPrefix();
//};
//
//class C : public A {
//public:
//	C() {
//		printf("C()\n");
//	}
//	C(const char *prefix, const char *name, const char *postfix);
//	virtual ~C();
//	char* getPostfix();
//};
//
//class N : public A {
//	friend class A;
//protected:
//
//	N() {
//		printf("N()\n");
//		NEW_(N_, ptr_);
//	}
//	virtual ~N() {
//		printf("~N\n");
//	}
//public:
//	static const N n_;
//};
//const N N::n_;
//#define null N::n_;
//
//
//A::A(char *name, int id) {
//	printf("alloc A(2)\n");
//	NEW_(A_, ptr_, name, id);
//}
//A::~A() {
//	printf("free Destroy A\n");
//	delRef();
//}
//void A::addRef() {
//	ptr_->addRef();
//}
//void A::delRef() {
//	if (ptr_->delRef() == 0) {
//		if (ptr_ != N::n_.ptr_) {
//			DEL_(ptr_);
//			ptr_ = N::n_.ptr_;
//		} else {
//			printf("Fatal: null cannot be deleted!");
//		}
//	}
//}
//A& A::operator=(const N& ref) {
//	printf("operator=\n");
//	null_();
//	return *this;
//}
//void A::null_() {
//	this->delRef();
//	this->ptr_ = N::n_.ptr_;
//	N::n_.ptr_->addRef();
//}
//
//const char* A::getName() {
//	return ptr_->getName();
//}
//
//#define PTR ((B_&)*ptr_)
//B::B(const char *prefix, const char *name) {
//	printf("alloc B(2)\n");
//	NEW_(B_, ptr_, prefix, name);
//}
//B::~B() {
//	printf("Destroy B\n");
//}
//B& B::operator=(const N& ref) {
//	printf("operator=\n");
//	null_();
//	return *this;
//}
//char* B::getPrefix() {
//	return PTR.prefix_;
//}
//
//#undef PTR
//#define PTR ((C_&)*ptr_)
//C::C(const char *prefix, const char *name, const char *postfix) {
//	printf("alloc C(2)\n");
//	NEW_(C_, ptr_, prefix, name, postfix);
//}
//C::~C() {
//	printf("Destroy C\n");
//}
//char* C::getPostfix() {
//	return PTR.postfix_;
//}



//void test() {
//	//printf(" * Create A\n");
//	//A a("name A", 12);
//	printf(" * Create B\n");
//	B b("Prefix ", "name B");
//	printf("%s\n", b.getName());
//	//printf(" * Create C\n");
//	//C c("Prefix ", "name ", "Postfix C");
//	//printf(" *** \n");
//	//printf("%s\n", a.getName());
//	//printf("%s\n", b.getName());
//	//printf("%s\n", c.getName());
//	printf(" * Create B2\n");
//	B b2("Prefix2 ", "name2 B");
//	b = b2;
//	printf("%s\n", b.getName());
//	printf("%s\n", b.getName());
//	b = N::n_;
//	printf("%s\n", b.getName());
//}

int main(int argc, char *argv[]) {
	int error = 0;

	//MemoryMgr::isDebugOn = true;

	printf("\n\n***\n");
	test();

	MemoryMgr::checkMemDbgInfo();
	printf("***\n\n\n");

	return error;
}