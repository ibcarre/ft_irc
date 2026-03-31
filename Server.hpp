/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 03:58:02 by homohame          #+#    #+#             */
/*   Updated: 2026/03/30 17:07:01 by ibtihal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <sys/types.h>
# include <sys/socket.h>
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
# include <stdio.h>
# include <vector>
# include <map>
# include "User.hpp"
# include "Command.hpp"
#include <ctime>
# define OPERNAME std::string("oper")
# define OPERPASS std::string("pwd")

//typedef int (Server::*Method) ();
class Command;

class Server
{
private:
	int								serv_socket;
	struct sockaddr_in				addr;
	std::vector<struct pollfd>		fds;
	std::vector<std::string>		buffers;
	std::vector<User>				users;
	int								port;
	std::string						password;
	time_t creation_time;
	//std::map<std::string, Method>	commands; //A FAIRE //TODO

	Server();
	Server(const Server& copy);
	Server &operator=(const Server& other);
public:
	Server(int port, std::string &password);
	~Server();

	time_t getCreation_time();
	void	initServer();
	void	serverLooping();
	void	handleDisconnection(int i);
	void	handleConnection();
	int		handleMessage(int i, std::string &msg);
	void	registerUser(User &u, struct pollfd fd);
	User	&getUser(int, bool*);
	void	closeServer();
	std::string getPassword();
	std::vector<User>				getAllusers();
};

void	answerto(std::string, int);

#endif
