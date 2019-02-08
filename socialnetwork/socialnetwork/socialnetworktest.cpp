#include <iostream>
#include <string>
#include "socialnetwork.h"

#define ADD_USER(name, gender) {  }
#define TEST(label, left, right, op) std::cout << " - " << label << ((left op right) ? ": passed" : ": failed") << "\n";
#define USERCOUNT 1000
#define FRIENDCOUNT 4
#define HOBBYCOUNT 3

SocialNetwork* network;
User* testUsers[USERCOUNT];
int testCount = 1;

const char* hobbies[] = { "Travelling", "Sport", "Books", "Movies", "Coding", "Carpenting" };

User* addUser(int ui, const char* name, GENDER gender) {
	char buffer[64] = "";
	snprintf(buffer, 100, "%s%2d", name, ui);
	User* user = new User(buffer, 20 + (ui % 20), 170 + (ui % 25), gender);
	// add hobbies
	int dummy = sizeof(hobbies) / sizeof(hobbies[0]);
	for (int hi = 0; hi < HOBBYCOUNT; hi++) {
		user->addHobby(hobbies[(ui + hi) % (sizeof(hobbies)/sizeof(hobbies[0]))]);
	}
	network->addUser(*user);
	return user;
}

void testAddUser() {
	std::cout << "\n" << testCount << ". Test add user\n";

	network = new SocialNetwork();

	User user("Anton", 35, 180, GENDER_MALE);
	network->addUser(user);
	TEST("user count is 1", network->getUserCount(), 1, ==);
	User* hit = network->getUserById(0);
	TEST("name", hit->getName(), user.getName(), ==);
	TEST("age", hit->getAge(), user.getAge(), == );
	TEST("height", hit->getHeight(), user.getHeight(), == );

	delete network;
}

void testAddUsers() {
	std::cout << "\n" << testCount << ". Test add "<< USERCOUNT << " users\n";
	char buffer[64] = "";
	const char* names[] = { "Anton", "Marta", "Emil", "Steffi" };
	for (int i = 0; i < USERCOUNT; i++) {
		testUsers[i] = addUser(i, names[i % (sizeof(names)/sizeof(names[0]))], i % 2 ? GENDER_FEMALE : GENDER_MALE);
	}
	User* userA = NULL;
	int id = 0;
	size_t count = network->getUserCount();
	// iterate through all users and add 4 friends each
	while ((userA = network->getUserById(id++)) != NULL) {
		// add 4 friends
		User* userB = NULL;
		for (int i = 0; i < FRIENDCOUNT; i++) {
			userB = network->getUserById((id + i) % count);
			userA->addFriend(userB->getId());
		}
	}

	// checks
	TEST("user count is " << USERCOUNT, network->getUserCount(), USERCOUNT, == );
	int errors = 0;
	for (int ui = 0; ui < USERCOUNT; ui++) {
		userA = network->getUserById(ui);
		snprintf(buffer, 64, "%s%2d", names[ui % (sizeof(names) / sizeof(names[0]))], ui);
		User* user = testUsers[ui];
		if (user->getName().compare(userA->getName()) != 0 &&
			user->getAge() != userA->getAge() &&
			user->getHeight() != userA->getHeight()) {
			errors++;
		}
		// check friends
		for (int fi = 0; fi < FRIENDCOUNT; fi++) {
			int fid = userA->getFriends()[fi];
			int tid = (ui + 1 + fi) % USERCOUNT;
			if (fid != tid) {
				std::cout << userA->getName() << "." << fi << ":" << fid << "," << tid << "\n";
				errors++;
			}
		}

		// check hobbies
		for (int hi = 0; hi < HOBBYCOUNT; hi++) {
			auto hobby = hobbies[(ui + hi) % (sizeof(hobbies) / sizeof(hobbies[0]))]; 
			if (userA->getHobbies()[hi].compare(hobby) != 0) {
				std::cout << userA->getName() << "." << hobby << ":" << hobby << "\n";
				errors++;
			}
		}
	}
	TEST("add successful", errors, 0, == );
}

void testRemoveUsers() {
	std::cout << "\n" << testCount << ". Test remove 10 users\n";
	// delete every 10th user
	for (int ui = 0; ui < USERCOUNT; ui += 10) {
		network->deleteUser(ui);
	}

	// check results
	int errors = 0;
	if (network->getUserCount() != USERCOUNT - (USERCOUNT/10)) {
		errors++;
	}
	for (int uid = 0; uid < USERCOUNT; uid += 10) {
		User* user = network->getUserById(uid);
		if (user != NULL) {
			errors++;
		}
	}
	TEST("removed successful", errors, 0, == );
	
}

void testSearchByName() {
	std::cout << "\n" << testCount << ". Test search by name\n";
	auto uidList = network->searchUserByName("Anton12");
	bool found = false;
	if (uidList.size() > 0) {
		found = true;
		for (int uid : uidList) {
			User* user = network->getUserById(uid);
			if (user->getName().compare("Anton12") != 0) {
				found = false;
				break;
			}
		}
	}
	TEST("search for 'Anton12'", found, true, == );

	uidList = network->searchUserByName("Anton11");
	TEST("'Anton11' not found", uidList.size(), 0, ==);
}

void testSearchByAge() {
	std::cout << "\n" << testCount << ". Test search by age\n";
	auto uidList = network->searchUserByAge(25);
	bool found = false;
	if (uidList.size() > 0) {
		found = true;
		for (int uid : uidList) {
			User* user = network->getUserById(uid);
			if (user->getAge() != 25) {
				found = false;
				break;
			}
		}
	}
	TEST("search for age=25", found, true, == );


	std::cout << " - found " << uidList.size() << " user(s)\n";
	uidList = network->searchUserByName("120");
	TEST("Age '120' not found", uidList.size(), 0, == );
}

void testSearchByHobbies() {
	std::cout << "\n" << testCount << ". Test search by hobbies\n";
	int errors = 0;
	// search list
	std::vector<std::string> hobbies = { "Sport", "Books" };
	// retrieve list of user ids
	auto uidList = network->searchUserByHobbies(hobbies);
	// check every user in the list
	for (auto uid : uidList) {
		User* user = network->getUserById(uid);
		// compare lists
		// every member of hobbies has to be contained in the list of the user's hobbies
		for (auto hobbyA : hobbies) {
			bool found = false;
			for (auto hobbyB : user->getHobbies()) {
				if (hobbyA.compare(hobbyB)) {
					// hobby found, continue with the next searched hobby
					found = true;
					break;
				}
			}
			if (!found) {
				// we have an error
				errors++;
				// check next user
				break;
			}
		}
	}
	TEST("search by hobbies", errors, 0, == );

	// negative test
	uidList = network->searchUserByHobbies( { "Travelling", "Carpenting" } );
	TEST("hobbies not found", uidList.size(), 0, == );

}

void testGetFriendsOfUser() {
	std::cout << "\n" << testCount << ". Test getFriendsOfUser\n";
	// check friends
	int id = 12;
	User* user = network->getUserById(id);
	int errors = 0;
	if (user != NULL) {
		auto friends = network->getFriendsOfUser(*user);
		for (int i = 0; i < friends.size(); i++) {
			int tid = (id + 1 + i) % USERCOUNT;
			if (friends[i] != tid) {
				errors++;
			}
		}
	}
	TEST("check friends of user #12", errors, 0, == );
}

int main()
{
    std::cout << "Social network\nUnit tests\n";

	// #1. Add user
	testAddUser();
	testCount++;

	network = new SocialNetwork();

	// #2. Add users
	testAddUsers();
	testCount++;

	// #3. Remove users
	testRemoveUsers();
	testCount++;

	// #4. Search by name
	testSearchByName();
	testCount++;

	// #5. Search by age
	testSearchByAge();
	testCount++;

	// #6. Search by hobbies
	testSearchByHobbies();
	testCount++;

	// #7. Get friends of user
	testGetFriendsOfUser();
	testCount++;

	delete network;

	for (int i = 0; i < USERCOUNT; i++) {
		delete testUsers[i];
	}

	std::cout << "Done.\n\n";
}
