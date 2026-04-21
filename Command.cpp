/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 07:57:19 by homohame          #+#    #+#             */
/*   Updated: 2026/04/21 18:58:33 by ibtihal          ###   ########.fr       */
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
	const int nb = 12;
	typedef void    (Command::*fptr) (void);
    fptr    func[nb] = {&Command::cmdPass, &Command::cmdNick, &Command::cmdUser, &Command::cmdPing, &Command::cmdJoin, &Command::motd, &Command::oper, &Command::quit, &Command::kill, &Command::restart, &Command::cmdPrivMsg};
	std::string arr[nb] = {"PASS", "NICK", "USER", "PING", "JOIN", "MOTD", "OPER", "QUIT", "KILL", "RESTART", "PRIVMSG"};
	if (_cmd == "CAP") {
		std::cout << send(_user->getFd(), ":ircserv CAP * LS\r\n", 19, MSG_DONTWAIT | MSG_NOSIGNAL) << "\n";
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
		answer(ERR_NEEDMOREPARAMS(_cmd));
	}
	_user->in_pwd = _params;
}

void Command::cmdNick() {
	if (_params.empty()) {
		answer(ERR_NONICKNAMEGIVEN);
		return ;
	}
	if (*(_params.begin()) == '#' || *(_params.begin()) == ':')
	{
		answer(ERR_ERRONEUSNICKNAME);
		return ;
	}
	for (std::string::iterator it = _params.begin(); it != _params.end(); it++) {
		if (!isprint(*it) || *it == ' ') {
			answer(ERR_ERRONEUSNICKNAME);
			return ;
		}
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
	}	size_t first = _params.find(' ');
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
		answer(ERR_NEEDMOREPARAMS(_cmd));
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
	std::vector<std::string> params = splitParams(' ');
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
	 	answer(ERR_NEEDMOREPARAMS(_cmd));
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
			Channel newchan(chans[i], key, *_user);
			_server->addChannel(newchan);
			Channel *c = _server->findChannel(chans[i]);
			_user->addChannel(*c);
		}
		else if (c) {
			if (!c->checkPassword(key))
			{
				answer(ERR_BADCHANNELKEY(chans[i]));
			}
			if (c->invOnly) {
				answer(ERR_INVITEONLYCHAN(c->getName()));
			}
			else {
				int i = c->addUser(*_user, false);
				if (i == -1)
					answer(ERR_CHANNELISFULL(c->getName()));
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
		answer(ERR_NEEDMOREPARAMS("OPER"));
		return ;
	}
	std::string name = _params.substr(0, first);
	std::string password = _params.substr(first + 1);
	password.substr(0, password.size() - 2);
	if (name == OPERNAME && password == OPERPASS) {
		answer(SERVER + " :You are now an IRC operator" + CRLF);
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
			//A TESTER UNE FOIS QUE JAI FAIT LES CHANS
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
	std::vector<std::string> params = splitParams(' ');
	if (params.size() < 2) {
		answer(ERR_NEEDMOREPARAMS(_cmd));
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

std::vector<std::string>	Command::splitParams(char c) {
	size_t i = _params.find(c);
	size_t i2 = 0;
	std::vector<std::string> list;
	while (i != std::string::npos) {
		list.push_back(_params.substr(i2, i - i2));
		i2 = i+1;
		i = _params.find(c ,i2);
	}
	list.push_back(_params.substr(i2, i-i2));
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

void Command::cmdPrivMsg() {
	std::vector<std::string> par = splitParams(' ');
	if (par.empty()) {
		answer(ERR_NEEDMOREPARAMS("PRIVMSG"));
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
		if (par.empty()) {
			answer(ERR_NOTEXTTOSEND);
			return;
		}
		if (par[0][0] == ':')
			par[0].erase(0, 1);
		if (par.empty()) {
			answer(ERR_NOTEXTTOSEND);
			return;
		}
		std::string msg;
		for (size_t i = 0; i < par.size() ; i++) {
			msg += par[i];
			msg += ' ';
		}
		answerto(msg, u.getFd());
		return ;
	}
	answer(ERR_NOSUCHNICK(target));
}
