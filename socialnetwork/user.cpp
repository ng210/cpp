#include "user.h"

User::User() {
	this->initialize();
}

User::User(std::string name, int age, int height, GENDER gender) {
	initialize(-1, name, age, height, gender);
}

void User::initialize(int id, std::string name, int age, int height, GENDER gender) {
	this->_id = id;
	this->_name = name;
	this->_age = age;
	this->_height = height;
	this->_gender = gender;
}

void User::addFriend(int id) {
	this->_friends.push_back(id);
}
void User::addHobby(std::string hobby) {
	this->_hobbies.push_back(hobby);
}