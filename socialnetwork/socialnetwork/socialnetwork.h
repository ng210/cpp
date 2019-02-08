#ifndef __SOCIALNETWORK_H
#define __SOCIALNETWORK_H

#include <map>

#include "user.h"

class SocialNetwork
{
	// running index of users
	int _currentUserId = 0;

	// for fast fetch users are stored in a map indexed by their id
	std::map<int, User> _users;

	// indices to support search
	//  - a list of user ids sorted by the indexed attribute
	//  - by sorting the list of users one index can be saved
	std::vector<int> _nameIndex;
	std::vector<int> _ageIndex;
	// hobbies are mapped onto set of users
	// - users are partitioned by hobbies
	std::map<std::string, std::vector<int>> _hobbyIndex;

public:
	SocialNetwork() {};
	virtual ~SocialNetwork() {};

	void addUser(User& user);
	void deleteUser(int id);

	User* getUserById(int id);

	size_t getUserCount();

	std::vector<int> searchUserByName(std::string name);
	std::vector<int> searchUserByAge(int age);
	std::vector<int> searchUserByHobbies(std::vector<std::string> hobbies);
	const std::vector<int> getFriendsOfUser(User& user);
};

#endif