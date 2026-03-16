/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 07:57:19 by homohame          #+#    #+#             */
/*   Updated: 2026/03/16 14:53:12 by ibtihal          ###   ########.fr       */
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
		std::cout << "Wrong command\n"; //421 "<client> <command> : Unknown command
		return ;
	}
	_user->disconnect = false;
    (this->*(func[i]))();
	if (_user->disconnect)
		*fd = _user->getFd();
}

void Command::answer(std::string msg) {
	std::cout << "MSG SENT " << msg << "\n";
	std::cout << "NB SENT " << send(_user->getFd(), msg.c_str(), msg.length(), 0) << "\n";
}

void Command::cmdPass() {
	if (_user->registered) {
		std::cerr << "Wrong command\n"; // ERR_ALREADYREGISTERED (462)
		answer(ERR_ALREADYREGISTERED);
		return ;
	}
	_user->registered = true;
	_user->in_pwd = _params;
}

void Command::cmdNick() {
	
}

void Command::cmdUser() {
	
}

void Command::cmdPong() {
	
}

void Command::cmdJoin() {
	if (!_user->registered)
		answer(ERR_NOTREGISTERED);
}