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
	std::cout << "CHANNEL'S (" << name <<")PASSWORD IS " <<  key << std::endl;
	OperCanChangeTopic = false;
	invOnly = false;
}

Channel::~Channel() {}

int Channel::addUser(User&user, bool admin) {
	std::cout << nb << " users in chan" << std::endl;
	if (nb >= user_limit) {
		answerto(ERR_CHANNELISFULL(name), user.getFd());
		return (-1);
	}
	if (nb == 0) {
		std::cout << "EMPTY CHANNEL" << std::endl;
		admin = true;
		topic = "";
	}
	for (std::vector<s_user>::iterator it = users.begin() ; it != users.end(); it ++) {
		if (&user == (it->user)) {
			if (users.size() == 1) {
				it->isAdmin = true;
			}
			return (1);
		}
	}
	nb++;
	users.push_back(s_user(user, admin));
	sendtoUsers(MSG_JOINED(name, user.getNick(), user.getUsername()));
	std::string nameReply = ":" + SERVER + " 353 " + user.getNick() + " = #" + name + " :";
	for (size_t i = 0; i < users.size(); i++) {
		if (users[i].isAdmin)
			nameReply += "@";
		nameReply += users[i].user->getNick();
		if (i != users.size() - 1)
			nameReply += ' ';
	}
	nameReply += CRLF;
	answerto(nameReply, user.getFd());
	answerto(RPL_ENDOFNAMES(user.getNick(), name), user.getFd());
	if (!topic.empty()) {
		answerto(RPL_TOPIC(user.getNick(), name, topic), user.getFd());
	}
	else {
		answerto(RPL_NOTOPIC(user.getNick(), name), user.getFd());
	}
	return (1);
}

void Channel::removeUser(User& user) {
	bool oper = false;
	//for (std::vector<s_user>::iterator it = users.begin(); it != users.end(); it++) {
	std::vector<s_user>::iterator it = users.begin();
	std::cout << it->user->getFd() << std::endl;
	for (size_t i = 0; i < users.size(); i++) {
		if (users[i].user->getFd() == user.getFd()) { //it->user->getFd()
			std::cout << "I " <<  i <<  std::endl;
			bool admin = users[i].isAdmin;
			s_user u = *(users.begin() + i);
			std::cout << user.getNick() << " ASDASDASD" << std::endl;
			std::cout << u.user->getNick() << " USER NICK" << std::endl;
			users.erase(users.begin() + i);
			nb--;
			if (nb == 0) {
				invOnly = false;
				setPassword("");
				isPassProtected = false;
			}
			if (!admin || oper) 
				return ;
		}
		else if (users[i].isAdmin)
			oper = true;
	}
	if (!oper && nb > 0) {
		std::cout << users.begin()->user->getNick() << " Is now chan operator" << std::endl;
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
	key = password;
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

bool Channel::isUsAdmin(User &u) {
	for (size_t i = 0; i < users.size(); i++) {
		if (u.getNick() == users[i].user->getNick()) {
			return (users[i].isAdmin);
		}
	}
	return (false);
}

bool Channel::isUsinChannel(User &u) {
	for (size_t i = 0; i < users.size(); i++) {
		if (u.getNick() == users[i].user->getNick()) {
			return (true);
		}
	}
	return (false);
}


void Channel::modeCmd(std::vector<std::string> mode, User &u) {
	std::cout << mode[0] << std::endl;
	std::vector<char> minus;
	std::vector<char> plus;
	std::vector<std::string> args;
	for (size_t i = 0; i < mode[0].size(); i++) {
		if (mode[0][i] == '+')
		{
			i++;
			while(i < mode[0].size() && mode[0][i] != '-') {
				plus.push_back(mode[0][i]);
				if (mode[0][i] != 'l' && mode[0][i] != 'i' && mode[0][i] != 't' && mode[0][i] != 'k' && mode[0][i] != 'o') {
					answerto(ERR_UMODEUNKNOWNFLAG, u.getFd());
					return ;
				}
				i++;
			}
		}
		else if (mode[0][i] == '-')
		{
			i++;
			while((i < mode[0].size()) && mode[0][i] != '+') {
				minus.push_back(mode[0][i]);
				if (mode[0][i] != 'l' && mode[0][i] != 'i' && mode[0][i] != 't' && mode[0][i] != 'k' && mode[0][i] != 'o') {
					answerto(ERR_UMODEUNKNOWNFLAG, u.getFd());
					return ;
				}
				i++;
			}
		}
		else
			i++;
		i--;
	}
	if (minus.size() == 0 && plus.size() == 0) {
		if (mode[0].length() == 1 && mode[0][0] == 'b') {
			std::string msg = ":" + SERVER + " 368 " + u.getNick() + " #" + name + " :End of channel ban list" + CRLF;
			answerto(msg, u.getFd());
			return;
		}
		answerto(ERR_NEEDMOREPARAMS("MODE", u.getNick()),u.getFd());
		return ;
	}
	if (!u.oper) {
		for (size_t i = 0; i < users.size(); i++) {
			if (users[i].user->getFd() == u.getFd()) {
				if (!users[i].isAdmin) {
					answerto(ERR_CHANOPRIVSNEEDED(name, u.getNick()), u.getFd());
					return;
				}
			}
		}
	}
	mode.erase(mode.begin());
	int idx = 0;
	for (size_t i = 0; i < plus.size(); i++) {
		if (plus[i] == 'l')
			modeLimit('+', u, mode, idx++);
		else if (plus[i] == 'i')
			modeInv('+', u);
		else if (plus[i] == 't')
			modeTopic('+', u);
		else if (plus[i] == 'k')
			modeKey('+', u, mode, idx++);
		else if (plus[i] == 'o')
			modeOp('+', u, mode, idx++);
	}
	for (size_t i = 0; i < minus.size(); i++) {
		if (minus[i] == 'l')
			modeLimit('-', u, mode, idx);
		else if (minus[i] == 'i')
			modeInv('-', u);
		else if (minus[i] == 't')
			modeTopic('-', u);
		else if (minus[i] == 'k')
			modeKey('-', u, mode, idx);
		else if (minus[i] == 'o')
			modeOp('-', u, mode, idx++);
	}
}

void Channel::modeTopic(char c, User &u) {
	std::string msg = ":" + u.getNick() + "!" + u.getUsername() + "@" + HOST + " ";
	if (c == '+') {
		if (mode.find("t") == std::string::npos)
			mode += "t";
		OperCanChangeTopic = true;
		msg += "MODE #" + name + " +t           ; Only opers can now change the channel topic" + CRLF;
		answerto(msg, u.getFd());
	}
	else {
		size_t i = mode.find("t");
		if (i != std::string::npos) 
			mode.erase(i, 1);
		OperCanChangeTopic = false;
		msg += "MODE #" + name + " -t           ; Everyone can now change the channel topic" + CRLF;
		answerto(msg, u.getFd());
	}
}

void Channel::modeInv(char c, User &u) {
	std::string msg = ":" + u.getNick() + "!" + u.getUsername() + "@" + HOST + " ";
	if (c == '+') {
		invOnly = true;
		if (mode.find("i") == std::string::npos) {
			mode += "i";
		}
		msg += "MODE #" + name + " +i           ; This channel is now Inv only" + CRLF;
		answerto(msg, u.getFd());
	}
	else {
		invOnly = false;
		size_t i = mode.find("i");
		if (i != std::string::npos)
			mode.erase(i, 1);
		msg += "MODE #" + name + " -i           ; This channel is not Inv only (anymore)" + CRLF;
		answerto(msg, u.getFd());
	}
}
void Channel::modeLimit(char c, User &u, std::vector<std::string> &s, int i) {
	std::string msg = ":" + u.getNick() + "!" + u.getUsername() + "@" + HOST + " ";
	if (c == '+') {
		if (static_cast<size_t>(i) >= s.size()) {
			answerto(ERR_NEEDMOREPARAMS("+l", u.getNick()), u.getFd());
			return ;
		}
		if (s[i].size() > 3) {
			answerto(ERR_UNKNOWNERROR(u.getNick(), "Limit too big"), u.getFd());
			return ;
		}
		for (size_t j = 0; j < s[i].size() ; j++) {
			if (!isdigit(s[i][j])) {
				answerto(ERR_UNKNOWNERROR(u.getNick(), "Limit should be a number"), u.getFd());
				return ;
			}
		}
		if (mode.find("l") == std::string::npos)
			mode += "l";
		user_limit = atoi(s[i].c_str());
		msg += "MODE #" + name + " +l           ; This channel is now limited to " + s[i] + " users" + CRLF;
	}
	else {
		size_t j = mode.find("l");
		if (j != std::string::npos) {
			mode.erase(j, 1);
		}
		msg += "MODE #" + name + " -l           ; This channel is not limited" + CRLF;
		user_limit = INT_MAX;
	}
	answerto(msg, u.getFd());
}

void Channel::modeKey(char c, User &u, std::vector<std::string> &s, int i) {
	std::string msg = ":" + u.getNick() + "!" + u.getUsername() + "@" + HOST + " ";
	if (c == '+') {
		if (static_cast<size_t>(i) >= s.size()) {
			answerto(ERR_NEEDMOREPARAMS("+l", u.getNick()), u.getFd());
			return ;
		}
		std::string pass = s[i];
		if (!checkValid(pass)) {
			answerto(ERR_UNKNOWNERROR(u.getNick(), "Invalid Password"), u.getFd());
			return ;
		}
		if (mode.find("k") == std::string::npos)
			mode += "k";
		key = pass;
		isPassProtected = true;
		msg += "MODE #" + name + " +k           ; This channel is now password protected" + CRLF;
	}
	else {
		size_t j = mode.find("k");
		if (j != std::string::npos)
			mode.erase(j, 1);
		key = "";
		isPassProtected = false;
		msg += "MODE #" + name + " +k           ; This channel is not password protected anymore" + CRLF;
	}
	answerto(msg, u.getFd());
}
void Channel::modeOp(char c, User &u, std::vector<std::string> &s, int i) {
	std::string msg = ":" + u.getNick() + "!" + u.getUsername() + "@" + HOST + " ";
	if (static_cast<size_t>(i) >= s.size()) {
		answerto(ERR_NEEDMOREPARAMS("+o", u.getNick()), u.getFd());
		return ;
	}
	std::string user = s[i];
	size_t j;
	for (j = 0; j < users.size(); j++) {
		if (users[j].user->getNick() == user)
			break;
	}
	if (j == users.size()) {
		answerto(ERR_USERNOTINCHANNEL(name, u.getNick()), u.getFd());
		return ;
	}
	if (c == '+') {
		users[j].isAdmin = true;
		msg += "MODE #" + name + " +o           ; Added an operator" + CRLF;
		answerto(RPL_YOUREOPER(users[j].user->getNick()), users[j].user->getFd());
	}
	else {
		users[j].isAdmin = false;
		msg += "MODE #" + name + " -o           ; Removed an operator" + CRLF;
	}
	answerto(msg, u.getFd());
}

void Channel::setTopic(std::vector<std::string> s, User &u) {
	// check que s est pas vide et : si que 2 petits points alors clear
	if (s.empty()) {
		if (topic.empty()) {
			answerto(RPL_NOTOPIC(u.getNick(), name), u.getFd());
			return ;
		}
		else {
			answerto(RPL_TOPIC(u.getNick(), name, topic), u.getFd());
			return ;
		}
	}
	if (s.size() == 1 && s[0].size() == 1 && s[0][0] == ':') {
		topic = "";
		std::string msg = "TOPIC #" + name + " :            ; Channel " + name + " got its topic cleared" + CRLF;
		sendtoUsers(msg);
		return ;
	}
	if (s[0][0] == ':')
		s[0].erase(0,1);
	topic = "";
	for (size_t i = 0; i < s.size(); i++) {
		topic += s[i];
		topic += " ";
	}
	std::string msg = ":" + u.getNick() + "!" + u.getUsername() + "@" + HOST + " ";
	msg += "TOPIC #" + name + " :" + topic + "       ; Channel " + name + " got a new topic" + CRLF;
	sendtoUsers(msg);
}