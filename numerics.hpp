#pragma once

#define SERVER std::string("ircserver")

#define CRLF "\r\n"

#define RPL_WELCOME(client) std::string(":") + SERVER + std::string(client) + " :Bienvenue sur le serv'bg !" + CRLF

#define ERR_PASSWDMISMATCH std::string(":") + SERVER + " : 464 Wrong password" + CRLF

#define ERR_NOTREGISTERED std::string(":") + SERVER + " : 451 Not registered" + CRLF

#define ERR_ALREADYREGISTERED std::string(":") + SERVER + " : 462 Already Registered" + CRLF