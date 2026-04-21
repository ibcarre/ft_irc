#include "Channel.hpp"

Channel::Channel(std::string &aname, std::string &password, User &user) {
	if (!password.empty())
		setPassword(password);
	else
		isPassProtected = false;
	Channel::name = aname;
	nb = 0;
	user_limit = INT_MAX;
	addUser(user, true);
	sendtoUsers(MSG_JOINED(aname, user.getNick(), user.getUsername()));
	std::cout << "CHANNEL'S PASSWORD IS " <<  key << std::endl;
	ECanChangeTopic = false;
}

Channel::~Channel() {}

int Channel::addUser(User&user, bool admin) {
	if (nb == user_limit)
		return (-1);
	nb++;
	for (std::vector<s_user>::iterator it = users.begin() ; it != users.end(); it ++) {
		if (&user == (it->user))
			return (1);
	}
	sendtoUsers(MSG_JOINED(name, user.getNick(), user.getUsername()));
	if (!topic.empty()) {
		std::string msg = HOST + " " + name + " :" + topic;
		answerto(msg, user.getFd());
	}
	users.push_back(s_user(user, admin));
	std::string nameReply = ":" + HOST + " = " + "#" + name + " :";
	for (size_t i = 0; i < users.size(); i++) {
		if (users[i].isAdmin)
			nameReply += "@";
		nameReply += users[i].user->getNick() + " ";
	}
	nameReply += "albert";
	nameReply += CRLF;
	answerto(nameReply, user.getFd());
	answerto(RPL_ENDOFNAMES(user.getNick(), name), user.getFd());
	return (1);
}

void Channel::removeUser(User& user) {
	bool oper = false;
	for (std::vector<s_user>::iterator it = users.begin(); it != users.end(); it++) {
		if (it->user->getFd() == user.getFd()) {
			users.erase(it);
			nb--;
			if (!it->isAdmin)
				return ;
			if (oper)
				break;
		}
		else if (it->isAdmin)
			oper = true;
	}
	if (!oper) {
		users.begin()->isAdmin = true;
	}
}

int Channel::setLimit(int limit) {
	if (limit < nb)
		return (-1);
	user_limit = limit;
	return (1);
}

void Channel::setPassword(std::string password) {
	password = password;
}

User *Channel::getUser(int n) {
	if (static_cast<size_t>(n) == users.size()) // 0 -> size -1
		return (0);
	return (users[n].user);
}

Channel::s_user Channel::getListUser(int n) {
	if (static_cast<size_t>(n) == users.size()) // 0 -> size -1
		return (*(users.begin()));
	int i = 0;
	std::vector<s_user>::iterator it;
	for (it = users.begin(); it != users.end(); it++) {
		if (i == n)
			break;
		i++;
	}
	return (*it);
}

void Channel::sendtoUsers(std::string s) {
	//bool i = true;
	for (std::vector<s_user>::iterator it = users.begin(); it != users.end(); it++) {
		// if (it->user->getNick() == u.getNick()) {
		// 	if (i) {
		// 		answerto(s, it->user->getFd());
		// 		i = false;
		// 	}
		// }
		// else {
		answerto(s, it->user->getFd());
		std::cout << "MSG sent to " << it->user->getNick() << std::endl;
		// }
	}
}

bool Channel::checkPassword(std::string &inPass) {
	if (inPass.empty()) {
		if (isPassProtected)
			return false;
		return true;
	}
	if (inPass == key)
		return (true);
	return (false);
}

std::string Channel::getName() const {
	return (name);
}

std::vector<Channel::s_user> Channel::getUsers() const {
	return (users);
}
