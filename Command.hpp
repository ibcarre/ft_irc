/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 07:19:13 by homohame          #+#    #+#             */
/*   Updated: 2026/03/03 17:11:05 by ibtihal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
# define COMMAND_HPP

#include "Server.hpp"

#define RPL_WELCOME(client) ":localhost 001 " + std::string(client) + " :Bievenue sur le serv'bg !\r\n"

class User;
class Server;

class Command
{
	private:
		enum Authenticate {
			CAP,
			PASS,
			NICK
		};
		std::string	_cmd;
		std::string	_params;
		User *_user;
		Server *_server;

		void	cmdPass();
		void	cmdNick();
		void	cmdUser();
		void	cmdPong();
		void	cmdCap();
	public:
		void processCmd();
		Command(std::string &cmd, std::string &params, User &u, Server &s);
};

#endif