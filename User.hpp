/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 05:09:19 by homohame          #+#    #+#             */
/*   Updated: 2026/04/21 16:04:44 by ibtihal          ###   ########.fr       */
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
		std::vector<Channel *> _channels;
		std::string realname;
	public:
		bool oper;
		User(int fd);
		~User();
		void sendAllChannels(std::string);
		void	setNick(std::string);
		std::string getNick() const;
		std::string getUsername() const;
		std::string getRealName() const;
		void setRealName(std::string);
		void	setUsername(std::string);
		bool	registered;
		int		getFd() const;
		std::string in_pwd;
		std::vector<Channel *> getChannels() const;
		void clearChannels();
		void addChannel(Channel &);
};

#endif