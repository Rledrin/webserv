#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <fstream>
#include <poll.h>
#include <fcntl.h>
#include <cstring>
#include <stdio.h>
#include <string>
#include <sstream>

std::string		parse_chunked_encoding(std::vector<std::string> encoded)
{
	std::string ret = "";
	
	if (!encoded.size())
		return (ret);
	int bytes = 0;
	int i = 0;
	int j = 0;
	std::string hexnum = "";
	std::stringstream ss;

	while (i < encoded.size())
	{
		j = 0;
		hexnum = "";
		ss.clear();
		while (encoded[i][j] != '\r')
		{
			hexnum.push_back(encoded[i][j]);
			j++;
		}
		ss << std::hex << hexnum;
		ss >> bytes;
		j += 2;
		while (bytes > 0)
		{
			ret.push_back(encoded[i][j]);
			j++;
			bytes--;
		}
		i++;
	}
	ret.push_back('\0');
	return (ret);
}

int main()
{
	std::string ret;
	std::vector<std::string> encoded = {"4\r\nWiki\r\n", "6\r\npedia \r\n", "E\r\nin \r\n\r\nchunks.\r\n", "0\r\n"};
	ret = parse_chunked_encoding(encoded);
	std::cout << "RETURN = " << ret << std::endl;

	return (0);
}