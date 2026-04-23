/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 03:58:02 by homohame          #+#    #+#             */
/*   Updated: 2026/04/23 13:56:09 by ibtihal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/types.h>
# include <sys/socket.h>
#include <deque>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <errno.h>
# include <cstring>
# include <netdb.h>
# include <fcntl.h>
# include <sys/poll.h>
# include <iostream>
# include <string>
# include <stdlib.h>
#include <signal.h> 
# include <stdio.h>
#include <algorithm>
# include <vector>
# include <map>
# include "User.hpp"
# include "Command.hpp"
# include "Channel.hpp"
#include <ctime>
# define OPERNAME std::string("oper")
# define OPERPASS std::string("pwd")

//typedef int (Server::*Method) ();
class Command;
class Channel;

class Server
{
private:
	int								serv_socket;
	struct sockaddr_in				addr;
	std::vector<struct pollfd>		fds;
	std::vector<std::string>		buffers;
	std::deque<User>				users;
	int								port;
	std::string						password;
	time_t creation_time;
	std::deque<Channel> _channels;

	Server();
	Server(const Server& copy);
	Server &operator=(const Server& other);
public:
	static Server *_server;
	Server(int port, std::string &password);
	~Server();

	time_t getCreation_time();
	void	initServer();
	void	serverLooping();
	void	handleDisconnection(int i);
	void	handleDisconnectionFD(int);
	void	handleConnection();
	int		handleMessage(int i, std::string &msg);
	void	registerUser(User &u, struct pollfd fd);
	User	&getUser(int, bool*);
	User	&getUser(std::string, bool&);
	void	closeServer();
	static void closeServer(int sig);
	std::string getPassword();
	std::deque<Channel> getChannels() const;
	void addChannel(Channel &);
	Channel *findChannel(std::string);
	std::deque<User>				&getAllusers();
};

void	answerto(std::string, int);

#endif
