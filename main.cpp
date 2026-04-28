/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 05:43:32 by homohame          #+#    #+#             */
/*   Updated: 2026/04/28 18:23:12 by ibtihal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Wrong arguments\n" << "Correct format is ./ircserver <port> <password>\n";
		return 1;
	}
	if (std::string(argv[1]).size() > 4) {
		std::cerr << "Wrong first argument\n";
		return (-1);
	}
	for (int i = 0; argv[1][i]; i++) {
		if (!isdigit(argv[1][i])) {
			std::cerr << "Wrong first argument\n";
			return (-1);
		}
	}
	int port = atoi(argv[1]);
	std::string password = argv[2];
	if (!checkValid(password)) {
		std::cout << "Wrong password format" << std::endl;
		return (-1);
	}
	Server server(port, password);
	Server::_server = &server;
	signal(SIGINT, Server::closeServer);
	signal(SIGTSTP, Server::closeServer);
	server.initServer();
	server.serverLooping();
}
