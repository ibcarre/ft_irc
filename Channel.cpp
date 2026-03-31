#include "Channel.hpp"

Channel::Channel(std::string password, User &user) {
	if (!password.empty())
		setPassword(password);
	else
		setPassword("");
	addUser(user, true);
}

Channel::~Channel() {}

int Channel::addUser(User&user, bool admin) {
	if (nb == user_limit)
		return (-1);
	nb++;
	users.push_back(s_user(user, admin));
	return (1);
}

void Channel::removeUser(User& user) {
	for (std::vector<s_user>::iterator it = users.begin(); it != users.end(); it++) {
		if (it->user->getFd() == user.getFd()) {
			users.erase(it);
			nb--;
			return ;
		}
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

void Channel::sendtoUsers(std::string s, User &u) {
	bool i = true;
	for (std::vector<s_user>::iterator it = users.begin(); it != users.end(); it++) {
		if (it->user->getNick() == u.getNick()) {
			if (i) {
				answerto(s, it->user->getFd());
				i = false;
			}
		}
		else {
			answerto(s, it->user->getFd());
		}
	}
}
