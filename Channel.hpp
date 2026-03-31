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
		int user_limit;
	public:
		Channel(std::string password, User &user);
		~Channel();
		int addUser(User&, bool);
		void removeUser(User&);
		int setLimit(int);
		void setPassword(std::string);
		void sendtoUsers(std::string, User&);
		User *getUser(int);
		s_user getListUser(int);
		int nb;
};