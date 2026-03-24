/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 05:09:19 by homohame          #+#    #+#             */
/*   Updated: 2026/03/24 15:30:37 by ibtihal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include "Server.hpp"
# include "Command.hpp"
# include <string>
# include "Channel.hpp"

class Channel;

class User
{
	private:
		int			fd;
		std::string	nick;
		std::string	username;
		std::vector<Channel> _channels;
	public:
		User(int fd);
		~User();
		void sendAllChannels(std::string);
		void	setNick(std::string);
		std::string getNick() const;
		void	setUsername(std::string);
		void	welcome();
		bool	registered;
		int		getFd() const;
		std::string in_pwd;
		bool disconnect;
};

#endif