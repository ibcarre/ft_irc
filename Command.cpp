/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 07:57:19 by homohame          #+#    #+#             */
/*   Updated: 2026/03/24 15:30:46 by ibtihal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

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
	std::cout << "COMMAND " << _cmd << "\n";
	size_t i;
	const int nb = 5;
	typedef void    (Command::*fptr) (void);
    fptr    func[] = {&Command::cmdPass, &Command::cmdNick, &Command::cmdUser, &Command::cmdPong, &Command::cmdJoin};
	std::string arr[nb] = {"PASS", "NICK", "USER", "PONG", "JOIN"};
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
	std::cout << "NICKNAME IS " << _params << "\n";
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
	for (std::vector<User>::iterator it = _server->getAllusers().begin(); it == _server->getAllusers().end() ; it++) {
		if (_params == it->getNick())
		{
			answer(ERR_NICKNAMEINUSE(_params));
			return ;
		}
	}
	_user->setNick(_params);
}

void Command::cmdUser() {
	
}

void Command::cmdPong() {
	
}

void Command::cmdJoin() {
	if (!_user->registered)
		answer(ERR_NOTREGISTERED);
}