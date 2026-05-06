/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaaziz <ibaaziz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 07:57:19 by homohame          #+#    #+#             */
/*   Updated: 2026/05/06 15:28:44 by ibaaziz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include <algorithm>

std::string clean_string(std::string s) {
	std::string chars = "\t\n\v\f\r ";
	s.erase(s.find_last_not_of(chars) + 1);
	return s;
}

Command::Command(std::string &cmd, std::string &params, User& user, Server &s)
{
	_cmd = cmd;
	_params = params;
	_user = &user;
	_server = &s;
}

void Command::processCmd() {
	std::transform(_cmd.begin(), _cmd.end(), _cmd.begin(), ::toupper);
	std::cout << "COMMAND " << _cmd << "\n";
	std::cout << "PARAMS " << _params << "\n";
	size_t i;
	const int nb = 16;
	typedef void    (Command::*fptr) (void);
    fptr    func[nb] = {&Command::cmdPass, &Command::cmdNick, &Command::cmdUser, &Command::cmdPing, &Command::cmdJoin, &Command::motd, &Command::oper, &Command::quit, &Command::kill, &Command::restart, 
		&Command::cmdPrivMsg, &Command::mode, &Command::topic, &Command::WHO, &Command::kick, &Command::invite};
	std::string arr[nb] = {"PASS", "NICK", "USER", "PING", "JOIN", "MOTD", "OPER", "QUIT", "KILL", "RESTART", "PRIVMSG", "MODE", "TOPIC", "WHO", "KICK", "INVITE"};
	if (_cmd == "CAP") {
		std::string msg = ":ircserver CAP * LS" + CRLF;
		answer(msg);
		return ;
	}
	for (i = 0; i < nb; i++) {
		if (_cmd == arr[i])
			break;
	}
	if (i == nb) {
		std::cout << "Unknown command\n"; //421 "<client> <command> : Unknown command
		return ;
	}
	(this->*(func[i]))();
}

void Command::answer(std::string msg) {
	std::cout << "MSG SENT " << msg;
	std::cout << "NB SENT " << send(_user->getFd(), msg.c_str(), msg.length(), 0) << "\n";
}

void Command::cmdPass() {
	if (_user->registered) {
		std::cerr << "Already registered\n"; // ERR_ALREADYREGISTERED (462)
		answer(ERR_ALREADYREGISTERED);
		return ;
	}
	if (_params.empty()) {
		std::cerr << "PASS empty parameters\n";
		answer(ERR_NEEDMOREPARAMS(_cmd, "*"));
	}
	_user->in_pwd = _params;
}

void Command::cmdNick() {
	if (_params.empty()) {
		answer(ERR_NONICKNAMEGIVEN);
		return ;
	}
	if (!checkValid(_params)) {
		answer(ERR_ERRONEUSNICKNAME);
		return ;
	}
	for (std::deque<User>::iterator it = _server->getAllusers().begin(); it != _server->getAllusers().end() ; it++) {
		if (_params == it->getNick())
		{
			answer(ERR_NICKNAMEINUSE(_params));
			return ;
		}
	}
	_user->setNick(_params);
	if (!(_user->getUsername()).empty() && !_user->registered) {
		authenticate();
	}
}

void Command::cmdUser() {
	if (_user->registered) {
		answer(ERR_ALREADYREGISTERED);
		return ;
	}	
	size_t first = _params.find(' ');
	if (first == std::string::npos){
		_user->setUsername(_params.substr(0, _params.length()));
		_user->setRealName(_params.substr(0, _params.length()));
	}
	else {
		_user->setUsername(_params.substr(0, first));
		size_t last = _params.find_last_of(' ');
		if (_params[last + 1] == ':') {
			std::string name = _params.substr(last + 2);
			_user->setRealName(name);
		}
		else {
			std::string name = _params.substr(last + 1);
			_user->setRealName(name);
		}
	}
	if (_user->getNick().empty())
		return ;
	authenticate();
}

void Command::cmdPing() {
	if (!_user->registered) {
		answer(ERR_NOTREGISTERED);
		return ;
	}
	if (_params.empty()) {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return ;
	}
	answer(std::string("PONG :" + _params + CRLF));
}

void Command::cmdJoin() {
	if (!_user->registered) {
		answer(ERR_NOTREGISTERED);
		return ;
	}
	if (_params == "0") {
		std::vector<Channel *> chans = _user->getChannels();
		for (size_t i = 0; i < chans.size() ; i++) {
			chans[i]->removeUser(*_user);
			chans[i]->sendtoUsers(MSG_PARTED(chans[i]->getName(), _user->getNick(), _user->getUsername()));
		}
		_user->clearChannels();
		return ;
	}
	std::vector<std::string> params = splitParams(',', _params);
	params = splitParams(' ', _params);
	std::vector<std::string> chans;
	std::vector<std::string> pwd;
	for (std::vector<std::string>::iterator it = params.begin(); it != params.end() ; it++) {
		if ((*it)[0] == '#') {
			it->erase(0, 1);
			chans.push_back(*it);
		}
		else {
			pwd.push_back(*it);
		}
	}
	if (chans.size() == 0 || pwd.size() > chans.size()) {
	 	answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
	 	return ;
	}
	for (size_t i = 0 ; i < chans.size(); i++) {
		Channel *c = _server->findChannel(chans[i]);
		std::string key;
		if (pwd.size() == 0 || pwd.size()-1 < i) {
			key = "";
		}
		else {
			key = pwd[i];
		}
		if (!c) {
			if (!checkValid(chans[i]) || chans[i].length() > 50)
				answer(ERR_BADCHANMASK(chans[i], _user->getNick()));
			else {
				Channel newchan(chans[i], key, *_user);
				_server->addChannel(newchan);
				Channel *c = _server->findChannel(chans[i]);
				_user->addChannel(*c);
			}
		}
		else if (c) {
			if (c->invOnly) {
				if (c->isUsinChannel(*_user))
					return ;
				answer(ERR_INVITEONLYCHAN(c->getName()));
				return ;
			}
			if (!c->checkPassword(key))
			{
				answer(ERR_BADCHANNELKEY(chans[i]));
				return ;
			}
			else {
				if (c->addUser(*_user, false) == -1) {
					return ;
				}
				else
					_user->addChannel(*c);
			}
			std::vector<Channel::s_user> c_users = c->getUsers();
			std::cout << "CHANNEL " << c->getName() << std::endl;
			for (std::vector<Channel::s_user>::iterator it = c_users.begin() ; it != c_users.end() ; it++) {
				std::cout << "USER " << it->user->getNick() << "  ADMIN ? " << it->isAdmin << std::endl;
			}
		}
	}
}

// #foobar : channel foobar
// &foobar : local channel => non supporté

void Command::authenticate() {
	std::cout << "NICKNAME IS " << _user->getNick() << std::endl;
	std::cout << "USERNAME IS " << _user->getUsername() << std::endl;
	if (_user->in_pwd != _server->getPassword()) {
		answer(ERR_PASSWDMISMATCH);
		_server->handleDisconnectionFD(_user->getFd());
		return ;
	}
	std::string nick = _user->getNick();
	int fd = _user->getFd();
	_user->registered = true;
	answerto(RPL_WELCOME(nick, _user->getUsername()),fd);
	answerto(RPL_YOURHOST(nick) ,fd);
	time_t c = _server->getCreation_time();
	answerto(RPL_CREATED(nick, ctime(&c)),fd);
	motd();
}

void Command::motd() {
	if (!_user->registered) {
		answer(ERR_NOTREGISTERED);
		return ;
	}
	std::string nick = _user->getNick();
	int fd = _user->getFd();
	answerto(RPL_MOTDSTART(nick), fd);
	answerto(RPL_MOTD(nick), fd);
	answerto(RPL_MOTDEND(nick), fd);
}
//peut pas finir par PASS, peut finir par NICK OU USER et commencer par n'importe lequel des TROIS
//si NICK et USER alors connection se lance et teste PASS

void	Command::oper() {
	if (!_user->registered) {
		answer(ERR_NOTREGISTERED);
		return ;
	}
	size_t first = _params.find(' ');
	if (first == std::string::npos) {
		answer(ERR_NEEDMOREPARAMS("OPER", _user->getNick()));
		return ;
	}
	std::string name = _params.substr(0, first);
	std::string password = _params.substr(first + 1);
	password.substr(0, password.size() - 2);
	if (name == OPERNAME && password == OPERPASS) {
		answer(RPL_YOUREOPER(_user->getNick()));
		_user->oper = true;
	}
	else {
		answer(ERR_PASSWDMISMATCH);
	}
}

void	Command::quit() {
	if (_params.empty()) {
		if (_user->getChannels().empty())
			answer(QUITMSGNOPARAM(_user->getNick(), _user->getUsername()));
		else {
			_user->sendAllChannels(QUITMSGNOPARAM(_user->getNick(), _user->getUsername()));
		}
	}
	else {
		size_t i = _params.find(':');
		if (i != std::string::npos) {
			_params.erase(i, 1);
		}
		if (_user->getChannels().empty())
			answer(QUITMSG(_user->getNick(), _user->getUsername(), _params));
		else {
			_user->sendAllChannels(QUITMSGNOPARAM(_user->getNick(), _user->getUsername()));
		}
	}
	_server->handleDisconnectionFD(_user->getFd());
}

void	Command::kill() {
	if (!_user->registered) {
		answer(ERR_NOTREGISTERED);
		return ;
	}
	if (!_user->oper) {
		answer(ERR_NOPRIVILEGES);
		return ;
	}
	std::vector<std::string> params = splitParams(' ', _params);
	if (params.size() < 2) {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return ;
	}
	//ERR_NOSUCHNICK
	bool isUser = true;
	User &u = _server->getUser(params[0], isUser);
	if (!isUser) {
		answer(ERR_NOSUCHNICK(params[0]));
		return ;
	}
	std::string reason;
	for (size_t i = 1; i < params.size(); i++) {
		if (params[i][0] == ':')
			params[i].erase(0, 1);
		reason += params[i];
		reason += ' ';
	}
	std::string msg = "YOU GOT KILLED : " + reason + CRLF;
	answerto(msg, u.getFd());
	_server->handleDisconnectionFD(u.getFd());
}

// JOIN PRIVMSG OPER QUIT ERROR
// chan operator MODE +b +l +i +v +m TOPIC KICK INVITE
// serveur operator OPER KILL ET RESTART

std::vector<std::string>	Command::splitParams(char c, std::string &s) {
	size_t i = s.find(c);
	size_t i2 = 0;
	std::vector<std::string> list;
	while (i != std::string::npos) {
		s[i] = ' ';
		list.push_back(s.substr(i2, i - i2));
		i2 = i+1;
		i = s.find(c ,i2);
	}
	list.push_back(s.substr(i2, i-i2));
	return (list);
}

void	Command::restart() {
	if (!_user->registered) {
		answer(ERR_NOTREGISTERED);
		return ;
	}
	if (!_user->oper) {
		answer(ERR_NOPRIVILEGES);
		return ;
	}
	_server->closeServer();
	_server->initServer();
	std::cout << "SERVER RESTARTED" << std::endl;
}

std::string Command::handleMsg(std::vector<std::string> par, std::string target) {
	if (par.empty()) {
		answer(ERR_NOTEXTTOSEND);
		return "";
	}
	if (par[0][0] == ':')
		par[0].erase(0, 1);
	if (par.empty()) {
		answer(ERR_NOTEXTTOSEND);
		return "";
	}
	std::string msg;
	msg += ":" + _user->getNick() + " PRIVMSG " + target + " :";
	for (size_t i = 0; i < par.size() ; i++) {
		msg += par[i];
		msg += ' ';
	}
	msg += CRLF;
	return msg;
}

void Command::cmdPrivMsg() {
	if (!_user->registered) {
		answer(ERR_NOTREGISTERED);
		return ;
	}
	std::vector<std::string> par = splitParams(' ', _params);
	if (par.empty()) {
		answer(ERR_NEEDMOREPARAMS("PRIVMSG", _user->getNick()));
		return ;
	}
	std::string target = par[0];
	size_t i;
	par.erase(par.begin());
	std::deque<User> &users = _server->getAllusers();
	for (i = 0; i < users.size(); i++) {
		if (users[i].getNick() == target)
			break;
	}
	if (i != users.size()) {
		User &u = users[i];
		std::string msg = handleMsg(par, target);
		if (msg.empty())
			return ;
		answerto(msg, u.getFd());
		return ;
	}
	const std::vector<Channel *> &channels = _user->getChannels();
	if (target[0] == '#')
		target.erase(0,1);
	else {
		answer(ERR_NOSUCHNICK(target));
		return ;
	}
	if (target.empty()) {
		answer(ERR_NEEDMOREPARAMS("PRIVMSG", _user->getNick()));
		return ;
	}
	for (i = 0 ; i < channels.size(); i++) {
		if (target == channels[i]->getName())
			break;
	}
	if (i == channels.size()) {
		answer(ERR_NOSUCHCHANNEL(target, _user->getNick()));
		return ;
	}
	Channel *c = channels[i];
	target = "#" + target;
	std::string msg = handleMsg(par, target);
	if (msg.empty())
		return ;
	c->sendtoUsersnoOP(msg, *_user);
}

//MODE +l +i +t +k +o                           +t
//l : client limit 
//i : invonly 
//t : protected topic only oper can change topic
//k : set key mode (needs a param)
//o : MODE #channel +o ib => ib is now a channel operator

void	Command::mode() {
// SANS PARAM => liste modes de CHANNELS
	if (!_user->registered) {
		answer(ERR_NOTREGISTERED);
		return ;
	}
	if (_params.empty()) {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return ;
	}
	if (_params[0] != '#') {
		return ;
	}
	_params.erase(0, 1);
	if (_params.empty()) {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return ;
	}
	const std::vector<Channel *> &channels = _user->getChannels();
	std::vector<std::string> par = splitParams(' ', _params);
	std::string target = par[0];
	Channel *c = NULL;
	for (size_t i = 0; i < channels.size(); i++) {
		if (channels[i]->getName() == target) {
			c = channels[i];
			break;
		}
	}
	if (!c) {
		answer(ERR_NOSUCHCHANNEL(target, _user->getNick()));
		return ;
	}
	par.erase(par.begin());
	if (par.empty()) {
		std::string msg = ":" + SERVER + " 324 " + _user->getNick() + " #" + c->getName() + " +" + c->mode + CRLF;
		answer(msg);
		return ;
	}
	c->modeCmd(par, *_user);
}

void	Command::topic() {
	if (!_user->registered) {
		answer(ERR_NOTREGISTERED);
		return ;
	}
	std::vector<std::string> par = splitParams(' ', _params);
	if (par.empty()) {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return ;
	}
	if (par[0][0] != '#') {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return ;
	}
	std::string target = par[0];
	target.erase(0, 1);
	if (target.empty()) {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return ;
	}
	std::vector<Channel *> chans = _user->getChannels();
	Channel *c = NULL;
	for (size_t i = 0; i < chans.size(); i++) {
		if (chans[i]->getName() == target) {
			c = chans[i];
			break;
		}
	}
	if (!c) {
		answer(ERR_NOSUCHCHANNEL(target, _user->getNick()));
		return ;
	}
	par.erase(par.begin());
	if (c->OperCanChangeTopic) {
		if (_user->oper || c->isUsAdmin(*_user))
		{
			c->setTopic(par, *_user);
			return ;
		}
		else {
			answer(ERR_CHANOPRIVSNEEDED(target, _user->getNick()));
			return ;
		}
	}
	else {
		c->setTopic(par, *_user);
	}
}

void	Command::WHO() {
	if (!_user->registered) {
		answer(ERR_NOTREGISTERED);
		return ;
	}
	if (_params.empty()) {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return ;
	}
	if (_params[0] != '#') {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return ;
	}
	if (_params.find(' ') != std::string::npos) {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return ;
	}
	_params.erase(0,1);
	if (_params.empty()) {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return ;
	}
	const std::deque<Channel> &channels = _server->getChannels();
	size_t i;
	for (i = 0; i < channels.size() ; i++) {
		if (_params == channels[i].getName()) {
			break;
		}
	}
	if (i == channels.size()) {
		answer(ERR_NOSUCHCHANNEL(_params, _user->getNick()));
		return ;
	}
	const Channel &c = channels[i];
	const std::vector<Channel::s_user> &u = channels[i].getUsers();
	if (u.size() > 1) {
		for (i = 0; i < u.size(); i++) {
			answer(RPL_WHOREPLY(_user->getNick(), c.getName(), u[i].user->getUsername(),u[i].user->getNick(), u[i].user->getRealName()));
		}
	}
	answer(RPL_ENDOFWHO(c.getName()));
}

void	Command::kick() {
	if (!_user->registered) {
		answer(ERR_NOTREGISTERED);
		return ;
	}
	if (_params.empty()) {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return ;
	}
	if (_params[0] != '#') {
		answer(ERR_BADCHANMASK("", _user->getNick()));
		return ;
	}
	_params.erase(0,1);
	std::vector<std::string> split = splitParams(',', _params);
	split = splitParams(' ', _params);
	if (split.size() < 2) {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return ;
	}
	bool s_oper = _user->oper;
	for (size_t i = 0; i < split.size(); i++) {
		if (split[i][0] == '#') {
			answer(ERR_UNKNOWNERROR(_user->getNick(), "Can only kick from one channel at once"));
			return ;
		}
	}
	Channel *c = _server->findChannel(split[0]);
	if (!c) {
		answer(ERR_NOSUCHCHANNEL(split[0], _user->getNick()));
		return;
	}
	bool isAdmin = c->isUsAdmin(*_user);
	if (!isAdmin && s_oper) {
		answer(ERR_NOPRIVILEGES);
		return ;
	}
	std::string msg;
	if (_params.find(":") != std::string::npos) {
		msg = _params.substr(_params.find(":") + 1);
	}
	std::string m = ":" + _user->getNick() + "!" + _user->getUsername() + "@" + SERVER + " KICK #" + split[0] + " ";
	for (size_t i = 1; i < split.size(); i++) {
		if (split[i][0] == ':')
			break;
		bool isUser;
		User u = _server->getUser(split[i], isUser);
		if (!isUser) {
			answer(ERR_NOSUCHNICK(split[i]));
			continue;
		}
		if (!c->isUsinChannel(u)) {
			answer(ERR_USERNOTINCHANNEL(_user->getNick(), split[0]));
			continue ;
		}
		c->removeUser(u);
		u.removeChannel(*c);
		if (msg.empty()) {
			m += split[i] + "                ; KICK message from " + _user->getNick() + " to remove " + split[i] + " from " + split[0] + CRLF;
		}
		else {
			m += split[i] + "                ; KICK message from " + _user->getNick() + " to remove " + split[i] + " from " + split[0] + " the reason : "+ msg  + CRLF;
		}
		answer(m);
		answerto(m, u.getFd());
	}
}

void	Command::invite() {
	if (_params.empty()) {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return;
	}
	std::vector<std::string> par = splitParams(' ', _params);
	if (par.size() != 2) {
		answer(ERR_NEEDMOREPARAMS(_cmd, _user->getNick()));
		return ;
	}
	if (par[1][0] != '#') {
		answer(ERR_UNKNOWNERROR(_user->getNick(), "Need a #channel"));
		return ;
	}
	std::string user = par[0];
	std::string channel = par[1];
	channel.erase(0,1);
	Channel *c = _server->findChannel(channel);
	bool isUser;
	User &u = _server->getUser(user, isUser);
	if (!isUser) {
		answer(ERR_NOSUCHNICK(user));
		return ;
	}
	if (!c) {
		answer(ERR_NOSUCHCHANNEL(channel, _user->getNick()));
		return ;
	}
	if (!c->isUsinChannel(*_user)) {
		answer(ERR_NOTONCHANNEL(_user->getNick(), channel));
		return ;
	}
	if (c->invOnly && !c->isUsAdmin(*_user)) {
		answer(ERR_CHANOPRIVSNEEDED(channel, _user->getNick()));
		return ;
	}
	if (c->isUsinChannel(u)) {
		answer(ERR_USERONCHANNEL(_user->getNick(), channel));
		return;
	}
	answer(RPL_INVITING(_user->getNick(), channel));
	std::string msg = ":" + _user->getNick() + "!" + _user->getUsername() + "@" + HOST + " INVITE " + user + " #" + channel + "     ; " + _user->getNick() + " invited " + user + " to #" + channel + CRLF;
	answerto(msg, u.getFd());
	u.addChannel(*c);
	c->addUser(u, false);
}