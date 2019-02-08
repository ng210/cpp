#ifndef __USER_H
#define __USER_H

#include <string>
#include <vector>

enum GENDER {
	GENDER_UNKNOWN,
	GENDER_MALE,
	GENDER_FEMALE
};

class User
{
	int _id;
	std::string _name;
	int _age;
	int _height;
	GENDER _gender;
	std::vector<int> _friends;
	std::vector<std::string> _hobbies;

	void initialize(int id = -1, std::string name = "", int age = -1, int height = -1, GENDER gender = GENDER_UNKNOWN);

public:
	User();
	User(std::string name, int age = -1, int height = -1, GENDER gender = GENDER_UNKNOWN);
	virtual ~User() {};

	void setId(int id) { this->_id = id;  }
	const int getId() { return this->_id; }
	const std::string getName() { return this->_name; }
	const int getAge() { return this->_age; }
	const int getHeight() { return this->_height; }
	const GENDER getGender() { return this->_gender; }
	const std::vector<int> getFriends() { return this->_friends; }
	const std::vector<std::string> getHobbies() { return this->_hobbies; }

	void addFriend(int id);
	void addHobby(std::string hobby);
};

#endif