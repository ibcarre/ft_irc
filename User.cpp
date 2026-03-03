/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 05:35:35 by homohame          #+#    #+#             */
/*   Updated: 2026/03/03 16:03:44 by ibtihal          ###   ########.fr       */
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