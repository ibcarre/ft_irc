/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaaziz <ibaaziz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 07:19:13 by homohame          #+#    #+#             */
/*   Updated: 2026/02/24 06:59:49 by ibaaziz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_HPP
# define COMMANDS_HPP

#include "Server.hpp"

#define RPL_WELCOME(client) ":localhost 001 " + std::string(client) + " :Bienvenue sur le serv'bg !\r\n"

class User;

class Commands
{
private:
	std::string					cmd;
	std::vector<std::string>	params;

	void	cmdPASS(User user);
	void	cmdNick(User user);
	void	cmdUser(User user);
	void	cmdPong(User user);
public:
	Commands(std::string command);
	~Commands();
};

#endif
