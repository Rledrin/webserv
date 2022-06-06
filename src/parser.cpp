#include <fstream>
#include <cstdio>
#include <cctype>
#include <string>

#include "server.hpp"

static bool checkBracket(std::string &stringFile)
{
	uint32_t size = stringFile.length();
	int brack = 0;

	for (uint32_t i = 0; i < size; i++)
	{
		if (stringFile[i] == '{')
			brack++;
		else if (stringFile[i] == '}')
			brack--;
	}
	if (brack == 0)
		return true;
	return false;
}

static uint32_t crossServer(std::string &stringFile, uint32_t j)
{
	uint32_t size = stringFile.length();
	int brack = 1;

	while (j < size && brack != 0)
	{
		if (stringFile[j] == '{')
			brack++;
		else if (stringFile[j] == '}')
			brack--;
		j++;
	}
	return j;
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

static std::string extractLocation(std::string &servConfig)
{
	uint32_t size = servConfig.length();
	size_t pos = 0;
	std::string ret;

	while ((pos = servConfig.find("location", pos)) != std::string::npos)
	{
		for (uint32_t i = pos; i < size; i++)
		{
			if (servConfig[i] == '}')
			{
				ret.append(servConfig.substr(pos, i - pos));
				servConfig.erase(pos, i - pos);
				size = servConfig.length();
				pos++;
				break;
			}
		}
	}
	return ret;
}

static void parseHost(const std::string &servConfig, server &tmp)
{
	size_t pos = 0;

	while ((pos = servConfig.find("host", pos)) != std::string::npos)
	{
		if (servConfig[pos - 1] != 'l')
			tmp._ip = parseString(&servConfig.c_str()[pos + 4]);
		pos++;
	}
}

static void parseListen(const std::string &servConfig, server &tmp)
{
	size_t pos = 0;
	// std::vector<listener> tmpListener;
	// tmpListener.resize(200);
	// int counter = 0;
	while ((pos = servConfig.find("listen", pos)) != std::string::npos)
	{
		// listener tmpListener(tmp._ip.c_str(), parseInt(&servConfig.c_str()[pos++]));
		// (tmp._ip.c_str(), parseInt(&servConfig.c_str()[pos++]));
		// tmpListener[counter] = listener(tmp._ip.c_str(), parseInt(&servConfig.c_str()[pos++]));
		// tmp._listener.push_back(tmpListener[counter++]);
		tmp._listener.push_back(new listener(tmp._ip.c_str(), parseInt(&servConfig.c_str()[pos++])));
	}
}

static void parseName(const std::string &servConfig, server &tmp)
{
	size_t pos = 0;

	while ((pos = servConfig.find("server_name", pos)) != std::string::npos)
	{
		tmp._name = parseString(&servConfig.c_str()[pos + 11]);
		pos++;
	}
}

static void parseRoot(const std::string &servConfig, server &tmp)
{
	size_t pos = 0;

	while ((pos = servConfig.find("root", pos)) != std::string::npos)
	{
		tmp._rootPath = parseString(&servConfig.c_str()[pos + 4]);
		pos++;
	}
}

static void parseBodySize(const std::string &servConfig, server &tmp)
{
	size_t pos = 0;

	while ((pos = servConfig.find("client_max_body_size", pos)) != std::string::npos)
	{
		tmp._maxBodySize = parseInt(&servConfig.c_str()[pos++]);
	}
}

static void parseError(const std::string &servConfig, server &tmp)
{
	size_t pos = 0;
	uint32_t i = 0;
	uint32_t pathPos = 0;
	uint32_t size = servConfig.length();

	while ((pos = servConfig.find("default_error", pos)) != std::string::npos)
	{
		i = pos + 12;
		while(++i < size)
		{
			while(isdigit(servConfig[++i]))
			{
				while(isspace(servConfig[++i]))
				{
					pathPos = i++;
					pos++;
					break;
				}
			}
		}
		tmp._error.insert(std::make_pair(parseInt(&servConfig.c_str()[pos]), parseString(&servConfig.c_str()[pathPos])));
	}
}

static void parseMethod(const std::string &servConfig, server &tmp)
{
	uint32_t size = servConfig.length();
	size_t pos = 0;
	std::string method;

	if ((pos = servConfig.find("method", pos)) != std::string::npos)
	{
		for (uint32_t i = pos; servConfig[i] != '\n' && i < size; i++)
			method.push_back(servConfig[i]);
		if (method.find("GET") != std::string::npos)
			tmp._get = true;
		if (method.find("POST") != std::string::npos)
			tmp._post = true;
		if (method.find("DELETE") != std::string::npos)
			tmp._delete = true;
	}
	if (tmp._get == false && tmp._post == false && tmp._delete == false)
	{
		tmp._get = true;
		tmp._post = true;
		tmp._delete = true;
	}
}

static void parseIndex(const std::string &servConfig, server &tmp)
{
	uint32_t size = servConfig.length();
	size_t pos = 0;
	std::string method;

	if ((pos = servConfig.find("autoindex", pos)) != std::string::npos)
	{
		for (uint32_t i = pos; servConfig[i] != '\n' && i < size; i++)
			method.push_back(servConfig[i]);
		if (method.find("on") != std::string::npos)
			tmp._autoIndex = true;
		else if (method.find("off") != std::string::npos)
			tmp._autoIndex = false;
		else
			std::cout << "autoindex incomplete" << std::endl;
	}
}

static void parseDefaultDir(const std::string &servConfig, server &tmp)
{
	size_t pos = 0;

	while ((pos = servConfig.find("default_dir", pos)) != std::string::npos)
	{
		tmp._defaultDir = parseString(&servConfig.c_str()[pos + 11]);
		pos++;
	}
}

static void parseCgiExt(const std::string &servConfig, server &tmp)
{
	size_t pos = 0;

	while ((pos = servConfig.find("cgi_extension", pos)) != std::string::npos)
	{
		tmp._cgiExtension = parseString(&servConfig.c_str()[pos + 13]);
		pos++;
		return;
	}
}

static void parseCgiPath(const std::string &servConfig, server &tmp)
{
	size_t pos = 0;

	while ((pos = servConfig.find("cgi_path", pos)) != std::string::npos)
	{
		tmp._cgiPath = parseString(&servConfig.c_str()[pos + 8]);
		pos++;
	}
}

static void parseUploadPath(const std::string &servConfig, server &tmp)
{
	size_t pos = 0;

	while ((pos = servConfig.find("upload_path", pos)) != std::string::npos)
	{
		tmp._uploadPath = parseString(&servConfig.c_str()[pos + 11]);
		pos++;
	}
}

static void parseRedir(const std::string &servConfig, server &tmp)
{
	size_t pos = 0;

	while ((pos = servConfig.find("return", pos)) != std::string::npos)
	{
		tmp._redir = parseString(&servConfig.c_str()[pos + 6]);
		break;
	}
}

static void parseLocation(std::string &locationString, server &tmp)
{
	std::string locName;
	size_t pos = 0;
	size_t lenLoc = 0;
	std::string tmpLocString;

	while ((pos = locationString.find("location", pos)) != std::string::npos)
	{
		server loc;
		if ((lenLoc = locationString.find("location", pos + 1)) == std::string::npos)
			lenLoc = locationString.length() - pos;
		tmpLocString = locationString.substr(pos, lenLoc - pos);

		locName = parseString(&locationString.c_str()[pos + 8]);
		parseMethod(tmpLocString, loc);
		parseIndex(tmpLocString, loc);
		parseDefaultDir(tmpLocString, loc);
		parseCgiExt(tmpLocString, loc);
		parseCgiPath(tmpLocString, loc);
		parseUploadPath(tmpLocString, loc);
		parseRedir(tmpLocString, loc);

		location tmpLoc(locName, loc._defaultDir, loc._rootPath, loc._cgiExtension, loc._cgiPath, loc._uploadPath, loc._redir, loc._get, loc._post, loc._delete, loc._autoIndex);
		tmp._loc.push_back(tmpLoc);
		pos++;
	}
}

static void parseServer(std::vector<server> &serv, std::string servConfig)
{
	server tmp;

	std::string location = extractLocation(servConfig);

	// std::cout << "apres extraction Serv: " << servConfig << std::endl;
	// std::cout << "apres extraction loc: " << location << std::endl;

	parseHost(servConfig, tmp);
	parseListen(servConfig, tmp);
	parseName(servConfig, tmp);
	parseRoot(servConfig, tmp);
	parseBodySize(servConfig, tmp);
	parseError(servConfig, tmp);
	parseMethod(servConfig, tmp);
	parseIndex(servConfig, tmp);
	parseDefaultDir(servConfig, tmp);
	parseCgiExt(servConfig, tmp);
	parseCgiPath(servConfig, tmp);
	parseUploadPath(servConfig, tmp);
	parseRedir(servConfig, tmp);

	parseLocation(location, tmp);

	serv.push_back(tmp);
}

void parseConfigFile(std::vector<server> &serv, std::string path)
{
	std::ifstream file(path.c_str());
	std::string line;
	std::string stringFile;
	if (!file.is_open())
	{
		std::cout << "Can't open config file" << std::endl;
		exit(1);
	}
	while(std::getline(file, line))
	{
		stringFile.append(line);
		stringFile.append("\n");
	}
	if (checkBracket(stringFile) == false)
	{
		std::cout << "Config file not valid (bracket)" << std::endl;
		exit(1);
	}
	uint32_t size = stringFile.length();
	size_t pos;
	uint32_t startServ;
	uint32_t endServ;
	for (uint32_t i = 0; i < size; i++)
	{
		if ((pos = stringFile.find("server", i)) == std::string::npos)
			break;
		for (uint32_t j = pos; j < size; j++)
		{
			while (!isspace(stringFile[j]) && j < size)
				j++;
			while (isspace(stringFile[j]) && j < size)
				j++;
			if (stringFile[j] == '{')
			{
				endServ = crossServer(stringFile, ++j);
				startServ = j;
				i = endServ;
				parseServer(serv, stringFile.substr(startServ, endServ - startServ));
				break;
			}
			else if (j == size || stringFile[j] != '{')
			{
				std::cout << "Config file not valid (lautre)" << std::endl;
				exit(1);
			}

		}
	}

}
