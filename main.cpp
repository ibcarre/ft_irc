/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaaziz <ibaaziz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 05:43:32 by homohame          #+#    #+#             */
/*   Updated: 2026/02/24 07:31:13 by ibaaziz          ###   ########.fr       */
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
	int port = std::atoi(argv[1]); //A CHECK
	std::string password = argv[2]; // A CHECK

	Server server(port, password);

	server.initServer();
	server.serverLooping();
}
