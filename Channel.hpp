//map User list et si ils sont admins

#pragma once

#include "User.hpp"
#include "Server.hpp"

class User;

class Channel {
	private:
		std::string name;
		std::map<User, bool> users; //if the users are operators or not
		std::string key;
		int user_limit;
	public:
		Channel(std::string password, User &user);
		~Channel();
		int addUser(User&, bool);
		void removeUser(User&);
		int setLimit(int);
		void setPassword(std::string);
		void sendtoUsers(std::string);
		User getUser(int);
		std::pair<User, bool> getMapUser(int);
		int nb;
};