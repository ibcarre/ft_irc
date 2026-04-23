//map User list et si ils sont admins

#pragma once

#include "User.hpp"
#include "Server.hpp"
#include "limits.h"

class User;

class Channel {
	public:
	struct s_user{
		User *user;
		bool isAdmin;
		s_user(User& u, bool a) : user(&u), isAdmin(a) {};
	};
	private:
		bool isPassProtected;
		std::string name;
		std::vector<s_user> users; //if the users are operators or not
		std::string key;
		std::string topic;
		int user_limit;
		void modeTopic(char, User &);
		void modeInv(char, User &);
		void modeLimit(char, User &);
		void modeKey(char, User &);
		void modeOp(char, std::vector<std::string>, User &);
	public:
		bool OperCanChangeTopic;
		bool invOnly;
		Channel(std::string& name, std::string& password, User &user);
		~Channel();
		int addUser(User&, bool);
		void removeUser(User&);
		int setLimit(int);
		void setPassword(std::string);
		bool checkPassword(std::string &);
		void sendtoUsers(std::string);
		User *getUser(int);
		s_user getListUser(int);
		std::vector<s_user> getUsers() const;
		int nb;
		std::string setTopic();
		std::string getName() const;
		void sendtoUsersnoOP(std::string, User &);
		void modeCmd(std::vector<std::string>, User &);
};

// MODE - Change the channel’s mode:
// · i: Set/remove Invite-only channel
// · t: Set/remove the restrictions of the TOPIC command to channel operators
// · k: Set/remove the channel key (password)
// · o: Give/take channel operator privilege
// · l: Set/remove the user limit to channel