/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaaziz <ibaaziz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 04:50:15 by homohame          #+#    #+#             */
/*   Updated: 2026/02/24 16:45:38 by ibaaziz          ###   ########.fr       */
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

	memset(&addr, 0, sizeof(addr)); //A RECODER MEMSET PAS AUTORISE
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
	while (true)
	{
		if (poll(fds.data(), fds.size(), 0) < 0)
			break;
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
				handleMessage(i); //TODO deconnexion
		}
	}
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
		struct sockaddr_in adr;
		socklen_t adrlen = sizeof(adr);
		getpeername(new_socket, (struct sockaddr *)&adr, &adrlen);
		char ipstr[INET6_ADDRSTRLEN];
		std::cout << inet_ntop(adr.sin_family, (void *)&adr.sin_addr, ipstr, sizeof(ipstr)) << std::endl;
		std::cout << ipstr << "\n";
		std::cout << ntohl(adr.sin_port) << std::endl;
		struct pollfd pfd;
		pfd.fd = new_socket;
		pfd.events = POLLIN;
		pfd.revents = 0;
		fds.push_back(pfd);
		User user(new_socket);
		users.push_back(user); // FAIRE UN addUser pour verifier si le user existe deja
	} while (new_socket != -1);
}

std::vector<std::string> split(std::string buffer)
{
	std::vector<std::string> tokens;
	size_t pos= 0;
	std::string token;

	while ((pos = buffer.find("\r\n")) != std::string::npos)
	{
		token = buffer.substr(0, pos);
		if (!token.empty())
			tokens.push_back(token);
		buffer.erase(0, pos + 2);
	}
	if (!buffer.empty())
		tokens.push_back(buffer);
	return tokens;
}

void Server::handleMessage(int i)
{
	int rc, close_conn = 0;
	(void)close_conn;
	char buffer[512];
	//on recoit un msg du client
	rc = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
	if (rc < 0)
	{
		if (errno != EWOULDBLOCK)
		{
			std::cerr << " recv() faild" << std::endl;
			close_conn = 1;
		}
		return ;
	}
	if (rc == 0)
	{
		std::cout << "Connection closed" << std::endl;
		close_conn = 1;
		return ;
	}
	//ajout de \0 a la fin du buffer
	buffer[rc] = '\0';
	std::cout << buffer << std::endl;
	std::string c = "okay eh eh\n";
	send(fds[i].fd, c.c_str(), c.length(), 0);
	std::string received(buffer);
	std::vector<std::string> commands = split(received);
	//check si l utilisateur existe
	for (size_t j = 0; j < commands.size(); j++)
	{
		if (commands[j].find("PING") == 0)
		{
			std::string token = commands[j].substr(5); // On prend tout après "PING "
			std::string response = "PONG " + token + "\r\n";
			send(fds[i].fd, response.c_str(), response.length(), 0);
			std::cout << "Réponse au PING envoyée.\n" << std::endl;
		}
	}
}
