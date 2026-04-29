/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 05:35:35 by homohame          #+#    #+#             */
/*   Updated: 2026/04/29 16:33:03 by ibtihal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User(int fd)
{
	oper = false;
	this->fd = fd;
	this->registered = false;
	nick = "";
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
	this->username = '~';
	this->username += username;
}

int	User::getFd() const {
	return (fd);
}

std::string User::getNick() const {
	return (nick);
}

void User::sendAllChannels(std::string s) {
	for (std::vector<Channel *>::iterator it = _channels.begin() ; it != _channels.end() ; it++) {
		(*it)->sendtoUsers(s);
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

std::vector<Channel *> User::getChannels() const {
	return (_channels);
}

void User::addChannel(Channel &chan) {
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end() ; it++) {
		if (&chan == (*it))
			return ;
	}
	_channels.push_back(&chan);
}

void User::removeChannel(Channel &chan) {
	std::cout << nick << " got " << _channels.size() << " channels" << std::endl;
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end() ; it++) {
		if (&chan == (*it)) {
			_channels.erase(it);
			std::cout << nick << " NOW got " << _channels.size() << " channels" << std::endl;
			return ;
		}
	}
}

void User::clearChannels() {
	_channels.clear();
}