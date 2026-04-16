//map User list et si ils sont admins

#pragma once

#include "User.hpp"
#include "Server.hpp"

class User;

class Channel {
	struct s_user{
		User *user;
		bool isAdmin;
		s_user(User& u, bool a) : user(&u), isAdmin(a) {};
	};
	private:
		std::string name;
		std::vector<s_user> users; //if the users are operators or not
		std::string key;
		std::string topic;
		int user_limit;
		bool invOnly;
		bool ECanChangeTopic;
	public:
		Channel(std::string name, std::string password, User &user);
		~Channel();
		int addUser(User&, bool);
		void removeUser(User&);
		int setLimit(int);
		void setPassword(std::string);
		bool checkPassword(std::string);
		void sendtoUsers(std::string, User&);
		User *getUser(int);
		s_user getListUser(int);
		int nb;
		std::string setTopic();
		std::string getName() const;
};

// MODE - Change the channel’s mode:
// · i: Set/remove Invite-only channel
// · t: Set/remove the restrictions of the TOPIC command to channel operators
// · k: Set/remove the channel key (password)
// · o: Give/take channel operator privilege
// · l: Set/remove the user limit to channel