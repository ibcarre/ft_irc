/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 05:35:35 by homohame          #+#    #+#             */
/*   Updated: 2026/03/24 15:46:51 by ibtihal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User(int fd)
{
	this->fd = fd;
	this->nick = "nick";
	this->username = "username";
	this->registered = false;
}

User::~User()
{
}

void User::setNick(std::string nick)
{
	this->nick = nick;
	std::string msg = "-!@" + SERVER + "NICK " + nick;
	if (!registered)
		answerto(msg, fd);
}

void User::setUsername(std::string username)
{
	this->username = username;
}

void User::welcome()
{
	std::string msg = RPL_WELCOME(nick);
	send(fd, msg.c_str(), msg.length(), 0);
	this->registered = true;
}

int	User::getFd() const {
	return (fd);
}

std::string User::getNick() const {
	return (nick);
}

void User::sendAllChannels(std::string s) {
	for (std::vector<Channel>::iterator it = _channels.begin() ; it != _channels.end() ; it++) {
		it->sendtoUsers(s);
	}
}