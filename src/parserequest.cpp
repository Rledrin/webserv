#include "webserv.hpp"

static std::string parsePath(const char *src)
{
	std::string ret;
	bool started = false;

	for (uint32_t i = 0; src[i] != '\n' && src[i] != '?' && src[i] != '\0'; i++)
	{
		if (!isspace(src[i]))
		{
			ret.append(&src[i], 1);
			started = true;
		}
		else if (started)
			return ret;
	}
	return ret;
}

static std::string parseString(const char *src)
{
	std::string ret;
	bool started = false;

	for (uint32_t i = 0; src[i] != '\n' && src[i] != '\0'; i++)
	{
		if (!isspace(src[i]))
		{
			ret.append(&src[i], 1);
			started = true;
		}
		else if (started)
			return ret;
	}
	return ret;
}

static uint32_t parseInt(const char *src)
{
	char number[16];
	uint32_t counter = 0;

	memset(number, 0, 16);

	for (uint32_t i = 0; src[i] != '\n' && src[i] != '\0'; i++)
	{
		if (isdigit(src[i]))
		{
			number[counter++] = src[i];
		}
		else if (counter != 0 && !isdigit(src[i]))
		{	
			return atoi(number);
		}
	}
	return atoi(number);
}

static bool checkHttp(std::string http)
{
	if (http.find("HTTP/1.1", 0) != std::string::npos)
		return true;
	return false;
}

static std::string parseContentType(const std::string &req)
{
	size_t pos = 0;
	std::string ret;

	while ((pos = req.find("Content-Type:", pos)) != std::string::npos)
	{
		ret = parseString(&req.c_str()[pos + 13]);
		ret.append(" ");
		ret.append(parseString(&req.c_str()[pos + 13 + ret.length()]));
		break;
	}
	return ret;
}

static std::string parseHost(const std::string &req)
{
	size_t pos = 0;
	std::string ret;

	while ((pos = req.find("Host:", pos)) != std::string::npos)
	{
		ret = parseString(&req.c_str()[pos + 5]);
		break;
	}
	return ret;
}

// static void	printParseRequest(int method, int contentLength, std::string host, std::string path, std::string body, std::string contentType)
// {
// 	std::cout << "METHOD:" << method << std::endl;
// 	std::cout << "contentLEngth:" << contentLength << std::endl;
// 	std::cout << "HOST:" << host << std::endl;
// 	std::cout << "PATH:" << path << std::endl;
// 	std::cout <<"BODY:" << body  << std::endl;
// 	std::cout << "CONTENTTYPE:" << contentType << std::endl;
// }

request parseRequest(std::string req)
{
	int method = 0;
	size_t indexInRequest = 0;
	int contentLength = 0;
	std::string body;
	std::string path;
	std::string contentType;
	std::string host;
	std::string queryString;
	bool err400 = false;

	if (req.find("GET", 0) != std::string::npos)
		method = 3;
	else if (req.find("POST", 0) != std::string::npos)
		method = 4;
	else if (req.find("DELETE", 0) != std::string::npos)
		method = 6;

	path = parsePath(&req.c_str()[method + 1]);

	if (method == 0)
		err400 = true;
	if ((indexInRequest = req.find("Content-Length:", 0)) != std::string::npos)
		contentLength = parseInt(&req.c_str()[indexInRequest + 15]);
	contentType = parseContentType(req);
	host = parseHost(req);
	if ((indexInRequest = req.find("?", 0)) != std::string::npos)
	{
		while (!isspace(req.c_str()[++indexInRequest]))
			queryString.push_back(req.c_str()[indexInRequest]);
	}
	if (!checkHttp(req))
		err400 = true;
	if ((indexInRequest = req.find("\r\n\r\n", 0)) != std::string::npos)
	{
		body = req.substr(indexInRequest + 4, (req.length() - indexInRequest));
	}
	
	// printParseRequest(method, contentLength, host, path, body, contentType);
	return (request(method, contentLength, body, path, contentType, host, queryString, err400));
}
