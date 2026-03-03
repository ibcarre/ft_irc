/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 04:50:15 by homohame          #+#    #+#             */
/*   Updated: 2026/03/03 17:06:26 by ibtihal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//Server &Server::operator=(const Server &other)
//{
//	// TODO: insert return statement here
//}

Server::Server(int port, std::string &password)
{
	this->port = port;
	this->password = password;
	//INITIALISATION DE MAP POUR COMMANDES A FAIRE
	//TODO
}

Server::~Server()
{
}

void Server::initServer()
{
	int on = 1;

	serv_socket = socket(PF_INET, SOCK_STREAM, 0); //AF_INET : IPV4 SOCK_STREAM : TCP
	if (serv_socket < 0)
		exit (EXIT_FAILURE);

	if (setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) //set_SocketOPTION : set Socket (SOL_SOCKET), pour le permettre de reutiliser les memes adresses
	{
		std::cerr << "setsockopt() failed" << std::endl;
		close(serv_socket);
		exit(EXIT_FAILURE);
	}

	fcntl(serv_socket, F_SETFL, O_NONBLOCK);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET; //IPv4
	addr.sin_addr.s_addr = INADDR_ANY; //binds socket to all interfaces
	addr.sin_port = htons(port);

	if (bind(serv_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind failed");
		close(serv_socket);
		exit (EXIT_FAILURE);
	}

	if (listen(serv_socket, 32) < 0)
	{
		std::cerr << "Failed to listen on socket. errno: " << errno << std::endl;
		close(serv_socket);
		exit(EXIT_FAILURE);
	}
	struct pollfd pfd;
	pfd.fd = serv_socket;
	pfd.events = POLLIN;
	pfd.revents = 0;
	fds.push_back(pfd);
}

void Server::serverLooping()
{
	std::size_t i = 0;
	while (poll(fds.data(), fds.size(), -1) >= 0)
	{
		for (i = 0; i < fds.size(); i++)
		{
			if(fds[i].revents == 0)
				continue ;
			if (fds[i].revents != POLLIN) //POLLIN : there is data to read
			{
				handleDisconnection(i);
				--i;
				continue ;
			}
			if (fds[i].fd == serv_socket) // quand un client se connecte pour la premiere fois son fd est égal au socket du serveur
				handleConnection();
			else
			{
				char buffer[512];
				int n = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
				if (n < 0) {
					if (errno == EAGAIN || errno == EWOULDBLOCK) {
						std::cout << "QSDDQD\n";
						continue;
					}
					else {
						perror("Recv failed\n");
						closeServer();
						return ;
					}
				}
				else if (n == 0) { // n == 0 => fd closed (shutdown)
					handleDisconnection(i);
					i--;
				}
				else {
					buffer[n] = 0;
					buffers[i - 1].append(buffer);
					if (buffers[i - 1].find("\r\n"))
						handleMessage(i, buffers[i - 1]);
				}
			}
		}
	}
}

void Server::closeServer() {
	std::cout << "Server Closed\n";
}

void Server::handleDisconnection(int i)
{
	std::cout << "Déconnexion fd: " << fds[i].fd << std::endl;
	close(fds[i].fd);
	fds.erase(fds.begin() + i);
}

void Server::handleConnection()
{
	int	new_socket;
	do
	{
		new_socket = accept(serv_socket, NULL, NULL);
		if (new_socket < 0)
		{
			if (errno != EWOULDBLOCK)
			{
				std::cerr << "accept() failed " << std::endl;
				exit (EXIT_FAILURE);
			}
			break ;
		}
		fcntl(new_socket, F_SETFL, O_NONBLOCK);
		std::cout << "Nouvelle connexion fd: " << new_socket << std::endl;
		struct pollfd pfd;
		pfd.fd = new_socket;
		pfd.events = POLLIN;
		pfd.revents = 0;
		fds.push_back(pfd);
		buffers.push_back("");
		User user(new_socket);
		user.registered = false;
		users.push_back(user);
	} while (new_socket != -1);
}

void	Server::registerUser(User &u, struct pollfd fd) {
	char buffer[512];

	int r = recv(u.getFd(), buffer, sizeof(buffer) - 1, 0);
	buffer[r] = 0;
	(void) fd;
	std::string s(buffer);
	char x[] = "CAP * LS : \r\n";
	send(u.getFd(), &x, sizeof(x), 0);
	std::cout << s << std::endl;
}


std::string firstword(std::string &s, std::string &params) {
	std::string word;
	size_t idx = s.find(' ');
	if (idx == std::string::npos) {
		std::string news = s;
		s.erase();
		return (news);
	}
	word = s.substr(0,idx);
	size_t idx2 = s.find("\r\n");
	params = s.substr(idx + 1, idx2);
	s.erase(0, idx2 + 2); // 2 => "\r\n"
	return (word);
}

int Server::handleMessage(int i, std::string &msg)
{
	(void)i;
	std::string params("");
	std::string cmd = firstword(msg, params);
	bool isUser = true;
	User u = getUser(fds[i].fd, &isUser);
	if (!isUser) {
		std::cerr << "Error : failing to find User who used command " << cmd << "\n";
		return (0);
	}
	Command command(cmd, params, u, *this);
	command.processCmd();
	return (1);
}

User &Server::getUser(int fd, bool *isUser) {
	for (std::vector<User>::iterator i = users.begin(); i < users.end(); i++) {
		if ((*i).getFd() == fd)
			return (*i);
	}
	std::cout << "USER DOESNT EXIST ?\n";
	*isUser = 0;
	return (*users.begin());
}

std::string Server::getPassword() {
	return (password);
}
