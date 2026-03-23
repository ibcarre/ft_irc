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
	users.insert({user, admin});
	nb++;
	return (1);
}

void Channel::removeUser(User& user) {
	std::map<User, bool>::iterator it = users.find(user);
	if (it == users.end())
		return ;
	nb --;
	users.erase(it);
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