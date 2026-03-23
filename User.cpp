/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 05:35:35 by homohame          #+#    #+#             */
/*   Updated: 2026/03/17 14:55:29 by ibtihal          ###   ########.fr       */
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
	//tell everyone in channels that nick has changed
	this->nick = nick;
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