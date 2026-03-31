/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 05:35:35 by homohame          #+#    #+#             */
/*   Updated: 2026/03/31 14:03:47 by ibtihal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User(int fd)
{
	oper = false;
	this->fd = fd;
	this->registered = false;
}

User::~User()
{
}

void User::setNick(std::string nick)
{
	std::string msg = "-!@" + HOST + " NICK " + nick + CRLF;
	if (registered) {
		msg = this->nick + "!" + username + "@" + HOST + " NICK " + nick + CRLF;
	}
	answerto(msg, fd);
	this->nick = nick;
}

void User::setUsername(std::string username)
{
	this->username = username;
}

int	User::getFd() const {
	return (fd);
}

std::string User::getNick() const {
	return (nick);
}

void User::sendAllChannels(std::string s) {
	for (std::vector<Channel>::iterator it = _channels.begin() ; it != _channels.end() ; it++) {
		it->sendtoUsers(s, *this);
	}
}

std::string User::getUsername() const {
	return (username);
}

std::string User::getRealName() const {
	return (realname);
}

void User::setRealName(std::string s) {
	realname = s;
}

std::vector<Channel> User::getChannels() const {
	return (_channels);
}
