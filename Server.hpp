/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaaziz <ibaaziz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 03:58:02 by homohame          #+#    #+#             */
/*   Updated: 2026/02/24 15:29:26 by ibaaziz          ###   ########.fr       */
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
# include <vector>
# include <map>
# include "User.hpp"
# include "Commands.hpp"

//typedef int (Server::*Method) ();

class Server
{
private:
	int								serv_socket;
	struct sockaddr_in				addr;
	std::vector<struct pollfd>		fds;
	std::vector<User>				users;
	int								port;
	std::string						password;
	//std::map<std::string, Method>	commands; //A FAIRE //TODO

	Server();
	Server(const Server& copy);
	Server &operator=(const Server& other);
public:
	Server(int port, std::string &password);
	~Server();

	void	initServer();
	void	serverLooping();
	void	handleDisconnection(int i);
	void	handleConnection();
	void	handleMessage(int i);

};

#endif
