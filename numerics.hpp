#pragma once

#define SERVER std::string("ircserver")

#define HOST std::string("localhost")

#define CRLF "\r\n"

#define ERR_PASSWDMISMATCH std::string(":") + SERVER + " : 464 Wrong password" + CRLF

#define ERR_NOTREGISTERED std::string(":") + SERVER + " : 451 Not registered" + CRLF

#define ERR_ALREADYREGISTERED std::string(":") + SERVER + " : 462 Already Registered" + CRLF

#define ERR_NEEDMOREPARAMS(cmd) std::string(":") + SERVER + " : " + cmd + " 461 Need more params" + CRLF

#define ERR_NONICKNAMEGIVEN std::string(":") + SERVER + " : " + "431 No nickname" + CRLF

#define ERR_ERRONEUSNICKNAME std::string(":") + SERVER + " : " + "432 Invalid nickname" + CRLF

#define ERR_NICKNAMEINUSE(nick) std::string(":") + SERVER + " : " + "433 " + nick + " Nickname in use" + CRLF

#define RPL_WELCOME(nick, username) ":" + SERVER + " 001 " + nick + " :Bienvenue sur le serveur IRC42, " + nick + "!" + username + "@" + HOST + CRLF

#define RPL_YOURHOST(nick) ":" + SERVER + " 002 " + nick + " :Your host is " + SERVER + ", running version 1.0" + CRLF

#define RPL_CREATED(nick, time) ":" + SERVER + " 003 " + nick + " :This server was created " + time + CRLF

#define RPL_MOTDSTART(nick) ":" + SERVER + " 375 " + nick + " :- Message of the day !" + CRLF

#define RPL_MOTD(nick) ":" + SERVER + " 372 " + nick + " :-          *   +                          |                                      \n" + ":" + SERVER + " 372 " + nick + " :-            +         .                - o -             .  .               '   \n" + ":" + SERVER + " 372 " + nick + " :-               '                         |                     +    '           \n" + ":" + SERVER + " 372 " + nick + " :-      o                    '                         o                  *       \n" + ":" + SERVER + " 372 " + nick + " :-    .                            *                         +                 .:'\n" + ":" + SERVER + " 372 " + nick + " :- '                   '                    ' .        +         + *       _.::'  \n" + ":" + SERVER + " 372 " + nick + " :-        '                                        o                      (_.'    \n" + ":" + SERVER + " 372 " + nick + " :-   '         ' o            .-.          .    o         o              \\       +\n" + ":" + SERVER + " 372 " + nick + " :-       +   +               ( (    ' +                         o         \\       \n" + ":" + SERVER + " 372 " + nick + " :-                            `-'       .       +             '            *      \n" + ":" + SERVER + " 372 " + nick + " :-            + '     .                                |                     .    \n" + ":" + SERVER + " 372 " + nick + " :-                           * *                     - o - +        \\             \n" + ":" + SERVER + " 372 " + nick + " :-*   |               +                                |        .    \\      .     \n" + ":" + SERVER + " 372 " + nick + " :-   -o-           .                    . +  *   '                    *           \n" + ":" + SERVER + " 372 " + nick + " :-    |       *                    .                                              \n" + ":" + SERVER + " 372 " + nick + " :-                   .      .              .        '           *         +       \n" + ":" + SERVER + " 372 " + nick + " :-  +                            '                                                \n" + ":" + SERVER + " 372 " + nick + " :-                             .         +    o    *              .  *    o       \n" + ":" + SERVER + " 372 " + nick + " :-       .             * +                               +    '                   \n" + ":" + SERVER + " 372 " + nick + " :-                                                   *                            \n" + CRLF

#define RPL_MOTDEND(nick) ":" + SERVER + " 376 " + nick + " :End of MOTD" + CRLF

#define QUITMSG(nick, username, params) ":" + nick + "!" + username + "@" + HOST + " QUIT " + ":" + params + " " + nick + "\n" + nick + " is exiting the network with \"" + params + "\"" + CRLF

#define QUITMSGNOPARAM(nick, username) ":" + nick + "!" + username + "@" + HOST + " QUIT " + ":Quit: See you later " + nick + "\n" + nick + " is exiting the network" + CRLF

#define ERR_NOPRIVILEGES SERVER + " :Permission Denied- You're not an IRC operator" + CRLF

#define ERR_NOSUCHNICK(nick) SERVER + " " + nick + " :No such Nickname" + CRLF

#define ERR_BADCHANNELKEY(channel) SERVER + " " + channel + " :Wrong key. Can't Join Channel" + CRLF

#define MSG_JOINED(channel, nick, username) ":" + nick + "!" + username + "@" + HOST + " JOIN " + " #" + channel + "      ; " + nick + " has joined the channel " + channel +CRLF

#define RPL_ENDOFNAMES(nick, channel) ":" + HOST + " 366 " + nick + " #" + channel + " :End of Names LIST" + CRLF

#define ERR_INVITEONLYCHAN(channel) SERVER + channel + " :Channel is invite only" + CRLF

#define ERR_CHANNELISFULL(channel) SERVER + channel + " :Channel is full" + CRLF

#define MSG_PARTED(channel, nick, username) ":" + nick + "!" + username + "@" + HOST + " PART " + " #" + channel + "      ; " + nick + " has parted the channel " + channel +CRLF

#define ERR_NOTEXTTOSEND HOST + " :No text to send" + CRLF

#define ERR_NOSUCHCHANNEL(channel) SERVER + " " + channel + " :No such Channel" + CRLF

#define ERR_UMODEUNKNOWNFLAG SERVER + " :Unknown MODE flag" + CRLF