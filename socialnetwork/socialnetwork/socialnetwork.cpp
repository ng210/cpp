#include <algorithm>
#include "socialnetwork.h"

// Class to support search by name
// implements comparator for std::find_if
class NameComparatorEQ {
	const std::string* _name;
	SocialNetwork& _network;
public:
	NameComparatorEQ(SocialNetwork& network, const std::string* name) : _network(network), _name(name) { }
	int operator()(const int id) {
		User *user = this->_network.getUserById(id);
		return _name->compare(user->getName()) == 0;
	}
};

// Class to support insert into index
class NameComparatorGT {
	const std::string* _name;
	SocialNetwork& _network;
public:
	NameComparatorGT(SocialNetwork& network, const std::string* name) : _network(network), _name(name) { }
	int operator()(const int id) {
		User *user = this->_network.getUserById(id);
		return _name->compare(user->getName()) < 0;
	}
};

// Class to support search by age
class AgeComparatorEQ {
	int _age;
	SocialNetwork& _network;
public:
	AgeComparatorEQ(SocialNetwork& network, int age) : _network(network), _age(age) { }
	int operator()(const int id) {
		User *user = this->_network.getUserById(id);
		return user != NULL ? user->getAge() == _age : false;
	}
};
// Class to support insert into index
class AgeComparatorGT {
	int _age;
	SocialNetwork& _network;
public:
	AgeComparatorGT(SocialNetwork& network, int age) : _network(network), _age(age) { }
	int operator()(const int id) {
		User *user = this->_network.getUserById(id);
		return user->getAge() > _age;
	}
};

// add user to the network
void SocialNetwork::addUser(User& user) {
	// set user id
	int id = this->_currentUserId++;
	user.setId(id);
	// store user
	this->_users[id] = user;
	// update indices
	//  - find the insertion point: right in front of the first element with attribute greater than the new one
	auto it = std::find_if(this->_nameIndex.begin(), this->_nameIndex.end(), NameComparatorGT{ *this, &user.getName() });
	this->_nameIndex.insert(it, id);
	it = std::find_if(this->_ageIndex.begin(), this->_ageIndex.end(), AgeComparatorGT{ *this, user.getAge() });
	this->_ageIndex.insert(it, id);
}

// delete user from network
void SocialNetwork::deleteUser(int id) {
	auto it = this->_users.find(id);
	if (it != _users.end()) {
		// update indices
		for (auto iit = this->_nameIndex.begin(); iit < this->_nameIndex.end(); ++iit) {
			// remove entry for the user id from the index
			if (*iit == id) {
				this->_nameIndex.erase(iit);
				break;
			}
		}
		for (auto iit = this->_ageIndex.begin(); iit < this->_ageIndex.end(); ++iit) {
			if (*iit == id) {
				this->_ageIndex.erase(iit);
				break;
			}
		}
		// remove user
		this->_users.erase(it);
	}
}

// get user by id
// returns poiner to user or NULL
User* SocialNetwork::getUserById(int id) {
	User* user = NULL;
	auto it = this->_users.find(id);
	if (it != this->_users.end()) {
		user = &it->second;
	}
	return user;
}

// returns count of users in the network
size_t SocialNetwork::getUserCount() {
	return this->_users.size();
}

// returns a list of users with the given name or empty list
std::vector<int> SocialNetwork::searchUserByName(std::string name) {
	std::vector<int> hits;
	// search the first matching user in the index
	auto first = std::find_if(this->_nameIndex.begin(), this->_nameIndex.end(), NameComparatorEQ{ *this, &name });
	// add every matching users to the list
	while (first != this->_nameIndex.end()) {
		hits.push_back(*first);
		first++;
		User& user = this->_users[*first];
		if (user.getName().compare(name) != 0) {
			break;
		}
	}
	return hits;
}

// returns a list of users with the given age or empty list
std::vector<int> SocialNetwork::searchUserByAge(int age) {
	std::vector<int> hits;
	auto first = std::find_if(this->_ageIndex.begin(), this->_ageIndex.end(), AgeComparatorEQ{ *this, age });
	// add every matching users to the list
	while (first != this->_ageIndex.end()) {
		hits.push_back(*first);
		first++;
		User& user = this->_users[*first];
		if (user.getAge() != age) {
			break;
		}
	}
	return hits;

}

// returns a list of users having the given hobbies or empty list
std::vector<int> SocialNetwork::searchUserByHobbies(std::vector<std::string> hobbies) {
	// flag is true if all hobbies were found
	bool allFound = true;
	// cache for lists of users with hobbies
	std::vector<std::vector<int>*> cache;

	// first step, fetch the lists of users of the selected hobbies
	for (auto hobby : hobbies) {
		// get hobby from index
		auto it = this->_hobbyIndex.find(hobby);
		// if hobby was stored and it has at least 1 user
		if (it != this->_hobbyIndex.end() && !it->second.empty()) {
			// put it into the cache
			cache.push_back(&it->second);
		}
		else {
			// hobby was not found or had no users
			allFound = false;
			break;
		}
	}

	// second step, search for users common in every list
	std::vector<int> hits;
	if (allFound) {
		// candidates are fetched from the very first list
		for (auto uidA : *cache[0]) {
			bool found = true;
			// look for the user id in the other lists (start with list #1)
			for (int j = 1; j < cache.size(); j++) {
				// iterate through the elements
				for (auto idB : *cache[j]) {
					if (uidA != idB) {
						// user id not found, this candidate failed
						found = false;
						break;
					}
				}
				if (!found) {
					// continue with the next candidate
					break;
				}
			}
			if (found) {
				// candidate passed, add it to the result list
				hits.push_back(uidA);
			}
		}
	}
	return hits;
}

// returns ids of the users who are friends of user or empty list
const std::vector<int> SocialNetwork::getFriendsOfUser(User& user) {
	return user.getFriends();
}