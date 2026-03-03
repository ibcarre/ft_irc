/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 07:57:19 by homohame          #+#    #+#             */
/*   Updated: 2026/03/03 17:12:51 by ibtihal          ###   ########.fr       */
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

void Command::processCmd() {
	std::cout << "COMMAND " << _cmd << "\n";
	int i;
	typedef void    (Command::*fptr) (void);
    fptr    func[] = {&Command::cmdCap, &Command::cmdPass, &Command::cmdNick, &Command::cmdUser, &Command::cmdPong};
	std::string arr[5] = {"CAP", "PASS", "NICK", "USER", "PONG"};
	for (i = 0; i < 5; i++) {
		if (_cmd == arr[i])
			break;
	}
	if (i == 5) {
		std::cout << "Wrong command\n"; //421 "<client> <command> : Unknown command
		return ;
	}
    (this->*(func[i]))();
}

void Command::cmdCap() {
	if (_user->registered)
		std::cout << "Wrong command\n";
	else
		send(_user->getFd(), "CAP * LS\r\n", sizeof("CAP * LS\r\n"), 0);
}

void Command::cmdPass() {
	if (_user->registered)
		std::cerr << "Wrong command\n"; // ERR_ALREADYREGISTERED (462) 
	if (_params != _server->getPassword()) {
		std::cerr << "Wrong Password\n"; //  ERR_PASSWDMISMATCH (464) 
		send(_user->getFd(), "464 * :Password incorrect", sizeof("464 * :Password incorrect"), 0);
	}
	else {
		_user->registered = true;
	}
}

void Command::cmdNick() {
	
}

void Command::cmdUser() {
	
}

void Command::cmdPong() {
	
}