#include "Channel.hpp"

Channel::Channel(std::string &aname, std::string &password, User &user) {
	if (!password.empty())
		setPassword(password);
	else
		isPassProtected = false;
	Channel::name = aname;
	nb = 1;
	user_limit = INT_MAX;
	addUser(user, true);
	sendtoUsers(MSG_JOINED(aname, user.getNick(), user.getUsername()));
	std::cout << "CHANNEL'S PASSWORD IS " <<  key << std::endl;
	OperCanChangeTopic = false;
}

Channel::~Channel() {}

int Channel::addUser(User&user, bool admin) {
	if (nb >= user_limit) {
		answerto(ERR_CHANNELISFULL(name), user.getFd());
		return (-1);
	}
	if (nb == 0) {
		admin = true;
		topic = "";
	}
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
	nameReply += CRLF;
	answerto(nameReply, user.getFd());
	answerto(RPL_ENDOFNAMES(user.getNick(), name), user.getFd());
	return (1);
}

void Channel::removeUser(User& user) {
	bool oper = false;
	//for (std::vector<s_user>::iterator it = users.begin(); it != users.end(); it++) {
	std::vector<s_user>::iterator it = users.begin();
	std::cout << it->user->getFd() << std::endl;
	for (size_t i = 0; i < users.size(); i++) {
		if (users[i].user->getFd() == user.getFd()) { //it->user->getFd()
			users.erase(users.begin() + i);
			nb--;
			if (!users[i].isAdmin)
				return ;
			if (oper)
				break;
		}
		else if (users[i].isAdmin)
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

void Channel::sendtoUsersnoOP(std::string s, User &u) {
	for (std::vector<s_user>::iterator it = users.begin(); it != users.end(); it++) {
		if (it->user->getNick() != u.getNick()) {
			answerto(s, it->user->getFd());
			std::cout << "MSG sent to " << it->user->getNick() << std::endl;
		}
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

// -bl+i

void Channel::modeCmd(std::vector<std::string> mode, User &u) {
	std::cout << mode[0] << std::endl;
	std::vector<char> minus;
	std::vector<char> plus;
	std::vector<std::string> args;
	if (mode.size() > 2) {
		answerto(ERR_UMODEUNKNOWNFLAG, u.getFd());
		return ;
	}
	if (mode.size() == 2 && mode[0].find("o") == std::string::npos) {
		answerto(ERR_UMODEUNKNOWNFLAG, u.getFd());
		return ;
	}
	for (size_t i = 0; i < mode[0].size(); i++) {
		if (mode[0][i] == '+')
		{
			i++;
			while(mode[0][i] != '-' && i < mode[0].size()) {
				plus.push_back(mode[0][i]);
				i++;
				if (mode[0][i] != 'l' && mode[0][i] != 'i' && mode[0][i] != 't' && mode[0][i] != 'k' && mode[0][i] != 'o') {
					answerto(ERR_UMODEUNKNOWNFLAG, u.getFd());
					return ;
				}
			}
		}
		if (mode[0][i] == '-')
		{
			i++;
			while(mode[0][i] != '+' && i < mode[0].size()) {
				minus.push_back(mode[0][i]);
				i++;
				if (mode[0][i] != 'l' && mode[0][i] != 'i' && mode[0][i] != 't' && mode[0][i] != 'k' && mode[0][i] != 'o') {
					answerto(ERR_UMODEUNKNOWNFLAG, u.getFd());
					return ;
				}
			}
		}
		i--;
	}
	if (minus.size() == 0 && plus.size() == 0) {
		answerto(ERR_NEEDMOREPARAMS("MODE"),u.getFd());
		return ;
	}
}
