/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibtihal <ibtihal@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 05:09:19 by homohame          #+#    #+#             */
/*   Updated: 2026/03/03 16:48:46 by ibtihal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include "Server.hpp"
# include "Command.hpp"
# include <string>

class User
{
	private:
		int			fd;
		std::string	nick;
		std::string	username;
	public:
		User(int fd);
		~User();
		void	setNick(std::string nick);
		void	setUsername(std::string username);
		void	welcome();
		bool	registered;
		int		getFd() const;
};

#endif