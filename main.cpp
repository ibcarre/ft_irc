/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 05:43:32 by homohame          #+#    #+#             */
/*   Updated: 2026/02/25 13:00:13 by ibtihal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Wrong arguments\n";
		return 1;
	}
	int port = atoi(argv[1]); //A CHECK
	std::string password = argv[2];

	Server server(port, password);

	server.initServer();
	server.serverLooping();
}
