/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 07:57:19 by homohame          #+#    #+#             */
/*   Updated: 2026/04/04 20:30:50 by ibtihal          ###   ########.fr       */
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

void Command::processCmd(int *fd) {
	std::transform(_cmd.begin(), _cmd.end(), _cmd.begin(), ::toupper);
	std::cout << "COMMAND " << _cmd << "\n";
	std::cout << "PARAMS " << _params << "\n";
	split();
	size_t i;
	const int nb = 11;
	typedef void    (Command::*fptr) (void);
    fptr    func[nb] = {&Command::cmdPass, &Command::cmdNick, &Command::cmdUser, &Command::cmdPing, &Command::cmdJoin, &Command::motd, &Command::oper, &Command::quit, &Command::kill, &Command::restart};
	std::string arr[nb] = {"PASS", "NICK", "USER", "PING", "JOIN", "MOTD", "OPER", "QUIT", "KILL", "RESTART"};
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
	_user->disconnect = false;
    (this->*(func[i]))();
	if (_user->disconnect)
		*fd = _user->getFd();
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
	for (std::vector<User>::iterator it = _server->getAllusers().begin(); it != _server->getAllusers().end() ; it++) {
		if (_params == it->getNick())
		{
			answer(ERR_NICKNAMEINUSE(_params));
			return ;
		}
	}
	_user->setNick(_params);
	if (!_user->getUsername().empty() && !_user->registered) {
		authenticate();
	}
}

void Command::cmdUser() {
	//faire commande
	size_t first = _params.find(' ');
	if (first == std::string::npos){
		_user->setUsername(_params.substr(0, _params.length() - 2));
		_user->setRealName(_params.substr(0, _params.length() - 2));
	}
	else {
		//si : alors tout apres est realname ou username et realname
		_user->setUsername(_params.substr(0, first));
		size_t last = _params.find_last_of(' ');
		if (_params[last + 1] == ':') {
			std::string name = _params.substr(last + 2);
			_user->setRealName(name.substr(0, name.size() - 2));
		}
		else {
			std::string name = _params.substr(last + 1);
			_user->setRealName(name.substr(0, name.size() - 2));
		}
	}
	std::cout << _user->getNick() << std::endl;
	if (_user->getNick().empty())
		return ;
	authenticate();
}

void Command::cmdPing() {
	if (_params.empty()) {
		answer(ERR_NEEDMOREPARAMS(_cmd));
		return ;
	}
	answer(std::string("PONG :" + _params + CRLF));
}

void Command::cmdJoin() {
	if (!_user->registered)
		answer(ERR_NOTREGISTERED);
}

void Command::authenticate() {
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
	std::string nick = _user->getNick();
	int fd = _user->getFd();
	answerto(RPL_MOTDSTART(nick), fd);
	answerto(RPL_MOTD(nick), fd);
	answerto(RPL_MOTDEND(nick), fd);
}
//peut pas finir par PASS, peut finir par NICK OU USER et commencer par n'importe lequel des TROIS
//si NICK et USER alors connection se lance et teste PASS

void	Command::oper() {
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
			//A TESTER UNE FOIS QUE JAI FAIT LES CHANS
		}
	}
	_server->handleDisconnectionFD(_user->getFd());
}

void	Command::kill() {
	if (!_user->oper) {
		answer(ERR_NOPRIVILEGES);
		return ;
	}
	std::vector<std::string> params = split();
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
		reason += params[i];
	}
	std::string msg = "YOU GOT KILLED : " + reason + CRLF;
	answerto(msg, u.getFd());
	_server->handleDisconnectionFD(u.getFd());
}

// JOIN PRIVMSG OPER QUIT ERROR
// chan operator MODE +b +l +i +v +m TOPIC KICK INVITE
// serveur operator OPER KILL ET RESTART

std::vector<std::string>	Command::split() {
	size_t i = _params.find(" ");
	size_t i2 = 0;
	std::vector<std::string> list;
	while (i != std::string::npos) {
		list.push_back(_params.substr(i2, i - i2));
		i2 = i+1;
		i = _params.find(' ' ,i+1);
	}
	list.push_back(_params.substr(i2, i-i2));
	return (list);
}

void	Command::restart() {
	if (!_user->oper) {
		answer(ERR_NOPRIVILEGES);
		return ;
	}
	_server->closeServer();
	_server->initServer();
	std::cout << "SERVER RESTARTED" << std::endl;
}