/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: homohame <homohame@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 05:09:19 by homohame          #+#    #+#             */
/*   Updated: 2026/02/06 07:47:44 by homohame         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include "Server.hpp"
# include "Commands.hpp"
# include <string>

class User
{
	private:
		int			fd;
		std::string	nick;
		std::string	username;
		bool		registered;
	public:
		User(int fd);
		~User();
		void	setNick(std::string nick);
		void	setUsername(std::string username);
		void	welcome();
};

#endif