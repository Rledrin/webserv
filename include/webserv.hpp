#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "server.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void parseConfigFile(std::vector<server> &serv, std::string path);
void printall(std::vector<server> &serv);

request parseRequest(std::string req);
std::string generateIndex(std::string path);


#endif
