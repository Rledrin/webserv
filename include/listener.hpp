#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <poll.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <dirent.h>


class listener
{
public:
	std::string _ip;
	int _port;
	int _socket; 

	listener(const char *ip, int port);
	listener(const listener & op);
	listener();
	~listener();
};

#endif
