/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 07:19:13 by homohame          #+#    #+#             */
/*   Updated: 2026/04/04 19:56:58 by ibtihal          ###   ########.fr       */
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
		void	oper();
		void	mode();
		void	restart();
		void	kill();
		void	quit();
		void	motd();
		void	answer(std::string);
		void	authenticate();
		std::vector<std::string> split();
	public:
		void processCmd(int *fd);
		Command(std::string &cmd, std::string &params, User &u, Server &s);
};

#endif