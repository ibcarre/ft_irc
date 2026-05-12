*This project has been created as part of the 42 curriculum by ibaaziz, eduzun, homohame.*

# ft_irc — Internet Relay Chat Server

## Description

**ft_irc** is a fully functional IRC server implemented in C++98. The goal of this project is to understand and implement the IRC protocol by building a server that real IRC clients can connect to and interact with. The server supports multiple simultaneous clients, authentication, channels, private messaging, and a set of operator commands — all handled through non-blocking I/O.

## Instructions

### Requirements

- A C++ compiler supporting the **C++98** standard (`g++` or `clang++`)
- A Unix-based system (Linux or macOS)

### Compilation

```bash
make
```

This will produce the `ircserv` binary. Available Makefile targets: `all`, `clean`, `fclean`, `re`.

### Running the server

```bash
./ircserv <port> <password>
```

- `<port>`: The port number the server will listen on (e.g. `6667`)
- `<password>`: The password clients must provide to connect

**Example:**

```bash
./ircserv 6667 mysecretpassword
```

### Connecting a client

Use irssi to connect:

```
/connect 127.0.0.1 6667 mysecretpassword
```

### Testing with nc

To test partial data handling:

```bash
nc -C 127.0.0.1 6667
```

Then type `com`, press `Ctrl+D`, type `man`, press `Ctrl+D`, type `d`, press `Enter`.

## Features

- Non-blocking I/O with a single `poll()` loop — no forking
- TCP/IP (v4 and v6) communication
- Authentication with connection password
- Nickname and username registration
- Channel creation, joining, and messaging
- Private messaging between users
- **Operator commands:** `KICK`, `INVITE`, `TOPIC`, `MODE`
- Channel modes: `i` (invite-only), `t` (topic restriction), `k` (key/password), `o` (operator privilege), `l` (user limit)

## Resources

### IRC Protocol References

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — IRC: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/)
- [IRC Numerics Reference](https://www.alien.net.au/irc/irc2numerics.html)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [Poll](https://www.ibm.com/docs/fr/i/7.5.0?topic=designs-using-poll-instead-select)

### AI Usage

- **Documentation**: Drafting and structuring this README.

