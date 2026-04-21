/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 07:19:13 by homohame          #+#    #+#             */
/*   Updated: 2026/04/21 17:27:04 by ibtihal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
# define COMMAND_HPP

#include "Server.hpp"
#include "numerics.hpp"

class User;
class Server;

class Command
{
	private:
		std::string	_cmd;
		std::string	_params;
		User *_user;
		Server *_server;

		void	cmdPass();
		void	cmdNick();
		void	cmdUser();
		void	cmdPing();
		void	cmdJoin();
		void	cmdPrivMsg();
		void	oper();
		void	mode();
		void	restart();
		void	kill();
		void	quit();
		void	motd();
		void	answer(std::string);
		void	authenticate();
		std::vector<std::string> splitParams(char c);
	public:
		void processCmd();
		Command(std::string &cmd, std::string &params, User &u, Server &s);
};

#endif