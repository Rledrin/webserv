#include "webserv.hpp"

int				check_path(request &req, server &serv)
{
	int size = serv._loc.size();

	for (int i = 0; i < size; i++)
	{
		if (req.path.find(serv._loc[i]._name) != std::string::npos && (req.path.c_str()[serv._loc[i]._name.length()] == '/' || req.path.c_str()[serv._loc[i]._name.length()] == '\0' ))
			return i;
	}
	return (-1);
}

static char *newStr(std::string source)
{
	char *res;

	res = new char[source.size() + 1];
	for (size_t i = 0; i < source.size(); ++i)
		res[i] = source[i];
	res[source.size()] = 0;
	return (res);
}

static char **mapToChar(std::map<std::string, std::string> &env)
{
	char **result;
	std::map<std::string, std::string>::iterator it;
	size_t i;

	result = new char*[env.size() + 1];
	it = env.begin();
	i = 0;
	while (it != env.end())
	{
		result[i++] = newStr(it->first + "=" + it->second);
		++it;
	}
	result[env.size()] = 0;
	return (result);
}
static void deleteStr(char **arr)
{
	int i = 0;
	while(arr[i])
		delete[] arr[i++];
	delete[] arr;
}

std::string execCgi(request &req, server &serv)
{
	pid_t pid = 0;
	int pipeFd[2];
	std::string ret;
	char buf[2];
	std::string meth = req.method == 3 ? "GET" : "POST";
	std::map<std::string, std::string> _env;
	_env["SERVER_SOFTWARE"] = "webserv";
	_env["SERVER_NAME"] = serv._name;
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["REQUEST_METHOD"] = meth;
	_env["PATH_INFO"] = req.path;
	_env["QUERY_STRING"] = req.queryString;
	_env["CONTENT_TYPE"] = "Content-Type: text/html";
	char **env = mapToChar(_env);
	memset(buf, 0, 2);
	if (pipe(pipeFd) < 0)
		return NULL;
	if ((pid = fork()) == 0)
	{
		close(pipeFd[0]);
		dup2(pipeFd[1], 1);
		close(pipeFd[1]);
		char **arg = {NULL};
		if (execve(req.path.c_str(), arg, env) < 0)
			exit(1);
	}
	close(pipeFd[1]);
	waitpid(0, NULL, 0);
	
	std::string tmpRead;
	while (read(pipeFd[0], &buf[0], 1) == 1)
	{
		tmpRead.push_back(buf[0]);
		buf[0] = 0;
	}
	deleteStr(env);

	if (tmpRead.empty())
	{
		std::map<int, std::string>::iterator it;
		std::ifstream file;
		std::string fileString;
		std::string tmpPath;
		tmpPath = serv._rootPath;
		ret.append("HTTP/1.1 404 KO\n\n");
		if ((it = serv._error.find(404)) != serv._error.end())
		{
			tmpPath.append(it->second.c_str());
			file.open(tmpPath.c_str());
		}
		else
			file.open("www/html/Error404.html");
		std::string tmp;
		while (std::getline(file, tmp))
		{
			fileString.append(tmp);
			fileString.append("\n");
		}
		ret.append(fileString);
	}
	else
	{
		ret.append("HTTP/1.1 200 OK\n");
		ret.append(tmpRead);
	}
	return ret;
}

std::string		res_get(request &req, server &serv)
{
	std::string ret;
	std::ifstream file;
	std::string fileString;
	std::map<int, std::string>::iterator it;
	int locIndx = 0;
	std::string rootPath;

	ret.append("HTTP/1.1 ");
	if (req.err400 == true)
	{
		ret.append("400 KO\n\n");
		if ((it = serv._error.find(400)) != serv._error.end())
			file.open(it->second.c_str());
		else
			file.open("www/html/Error400.html");
		if (file.is_open() == false)
			return (ret);
		std::string tmp;
		while (std::getline(file, tmp))
		{
			fileString.append(tmp);
			fileString.append("\n");
		}
		ret.append(fileString);
		return ret;
	}
	locIndx = check_path(req, serv);
	if (locIndx == -1)
	{
		if (!serv._get)
		{
			if ((it = serv._error.find(405)) != serv._error.end())
				file.open(it->second.c_str());
			else
				file.open("www/html/Error405.html");
			ret.append("405 KO\n\n");
			std::string tmp;
			while (std::getline(file, tmp))
			{
				fileString.append(tmp);
				fileString.append("\n");
			}
			ret.append(fileString);
			return ret;
		}
		rootPath = serv._rootPath;
		rootPath.append(req.path);
		struct stat path_stat;
		stat(rootPath.c_str(), &path_stat);
		if (S_ISDIR(path_stat.st_mode))
		{
			if (serv._autoIndex)
				return generateIndex(rootPath);
			rootPath = serv._rootPath;
			rootPath.append(serv._defaultDir);
			if (serv._redir.empty() == false)
			{
				ret.append("301 OK\nLocation: ");
				ret.append(serv._redir);
				ret.append("\n\n");
				return ret;
			}

		}
		req.path = rootPath;
		if (serv._cgiPath.empty() == false && serv._cgiExtension.empty() == false 
			&& req.path.find(serv._cgiPath, 0) != std::string::npos && req.path.find(serv._cgiExtension, 0) != std::string::npos)
			return execCgi(req, serv);
	}
	else
	{
		if (!serv._loc[locIndx]._get)
		{
			if ((it = serv._error.find(405)) != serv._error.end())
				file.open(it->second.c_str());
			else
				file.open("www/html/Error405.html");
			ret.append("405 KO\n\n");
			std::string tmp;
			while (std::getline(file, tmp))
			{
				fileString.append(tmp);
				fileString.append("\n");
			}
			ret.append(fileString);
			return ret;
		}
		if (serv._loc[locIndx]._rootPath.length() != 0)
			req.path.replace(req.path.find(serv._loc[locIndx]._name), serv._loc[locIndx]._name.length(), serv._loc[locIndx]._rootPath);
		else
			req.path.replace(req.path.find(serv._loc[locIndx]._name), serv._loc[locIndx]._name.length(), serv._rootPath);
		struct stat path_stat;
		stat(req.path.c_str(), &path_stat);
		if (S_ISDIR(path_stat.st_mode))
		{
			rootPath = req.path;
			if (serv._loc[locIndx]._defaultDir.length() != 0)
				rootPath.append(serv._loc[locIndx]._defaultDir);
			else
				rootPath.append(serv._defaultDir);
			if (serv._loc[locIndx]._redir.length() != 0)
			{
				ret.append("301 OK\nLocation: ");
				ret.append(serv._loc[locIndx]._redir);
				ret.append("\n\n");
				return ret;
			}
			if (req.path.find(serv._loc[locIndx]._cgiPath, 0) != std::string::npos && req.path.find(serv._loc[locIndx]._cgiExtension, 0) != std::string::npos)
			{
				return execCgi(req, serv);
			}
		}
		else
			rootPath = req.path;
	}

	file.open(rootPath.c_str());
	if (file.is_open() == false)
	{
		ret.append("404 KO\n\n");
		std::string tmpPath;
		tmpPath = serv._rootPath;
		if ((it = serv._error.find(404)) != serv._error.end())
		{
			tmpPath.append(it->second.c_str());
			file.open(tmpPath.c_str());
		}
		else
			file.open("www/html/Error404.html");
	}
	else
		ret.append("200 OK\n\n");
	std::string tmp;
	while (std::getline(file, tmp))
	{
		fileString.append(tmp);
		fileString.append("\n");
	}
	ret.append(fileString);
	return (ret);
}

std::string		res_post(request &req, server &serv)
{
	std::string ret;
	std::ofstream file;
	std::string fileString;
	std::map<int, std::string>::iterator it;
	int locIndx = 0;
	std::string rootPath;

	ret.append("HTTP/1.1 ");
	if (req.err400 == true)
	{
		ret.append("400 KO\n\n");
		return ret;
	}
	locIndx = check_path(req, serv);
	if (locIndx == -1)
	{
		if (!serv._post)
		{
			ret.append("405 KO\n\n");
			return ret;
		}
		else if(req.contentLength > serv._maxBodySize)
		{
			ret.append("413 KO\n\n");
			return ret;
		}
		rootPath = serv._rootPath;
		rootPath.append("/");
		rootPath.append(serv._uploadPath);
		rootPath.append(req.path);
		struct stat path_stat;
		stat(rootPath.c_str(), &path_stat);
		if (S_ISDIR(path_stat.st_mode))
		{
			rootPath = serv._uploadPath;
			rootPath.append(serv._defaultDir);
		}
	}
	else
	{
		if (!serv._loc[locIndx]._post)
		{
			ret.append("405 KO\n\n");
			return ret;
		}
		else if(req.contentLength > serv._maxBodySize)
		{
			ret.append("413 KO\n\n");
			return ret;
		}
		
		if (serv._loc[locIndx]._rootPath.length() != 0)
			rootPath = serv._loc[locIndx]._rootPath;
		else
			rootPath = serv._rootPath;
		if (serv._loc[locIndx]._uploadPath.length() != 0)
			req.path.replace(req.path.find(serv._loc[locIndx]._name), serv._loc[locIndx]._name.length(), serv._loc[locIndx]._uploadPath);
		else
			req.path.replace(req.path.find(serv._loc[locIndx]._name), serv._loc[locIndx]._name.length(), serv._uploadPath);
		rootPath.append("/");
		rootPath.append(req.path);
	}
	file.open(rootPath.c_str());
	if (file.is_open() == false)
	{
		ret.append("404 KO\n\n");
		std::string tmpPath;
		tmpPath = serv._rootPath;
		if ((it = serv._error.find(404)) != serv._error.end())
		{
			tmpPath.append(it->second.c_str());
			file.open(tmpPath.c_str());
		}
		else
			file.open("www/html/Error404.html");
	}
	else
	{
		ret.append("200 OK\n\n");
		if (req.contentLength != 0)
			req.body = req.body.substr(0, req.contentLength);
		file << req.body;
		// std::cout << req.body <<std::endl;
	}

	return (ret);
}

std::string		res_delete(request &req, server &serv)
{
	std::string ret;
	std::ifstream file;
	std::string fileString;
	std::map<int, std::string>::iterator it;
	int locIndx = 0;
	std::string rootPath;

	ret.append("HTTP/1.1 ");
	if (req.err400 == true)
	{
		ret.append("400 KO\n\n");
		return ret;
	}
	locIndx = check_path(req, serv);
	if (locIndx == -1)
	{
		rootPath = serv._rootPath;
		rootPath.append(req.path);
		if (!serv._delete)
		{
			ret.append("405 KO\n\n");
			return ret;
		}
	}
	else
	{
		if (!serv._loc[locIndx]._delete)
		{
			ret.append("405 KO\n\n");
			return ret;
		}
		if (serv._loc[locIndx]._rootPath.length() != 0)
			req.path.replace(req.path.find(serv._loc[locIndx]._name), serv._loc[locIndx]._name.length(), serv._loc[locIndx]._rootPath);
		else
			req.path.replace(req.path.find(serv._loc[locIndx]._name), serv._loc[locIndx]._name.length(), serv._rootPath);
		rootPath = req.path;
	}
	if (remove(rootPath.c_str()) != 0)
		ret.append("404 KO\n\n");
	else
	{
		ret.append("200 OK\n\n");
		remove(rootPath.c_str());
	}
	return (ret);
}

static std::string resError()
{
	std::string ret;

	ret.append("HTTP/1.1 ");
	ret.append("400 KO\n\n");
	return ret;
}

std::string		server::response(request &req)
{
	// std::cout << "serv Name: " << _name << std::endl; 
	std::string ret;
	switch (req.method)
	{
		case 3:
			ret = res_get(req, *this);
			break;
		case 4:
			ret = res_post(req, *this);
			break;
		case 6:
			ret = res_delete(req, *this);
			break;
		default:
			ret = resError();
			break;
	}
	// ret.insert(ret.find("\n"), "\nConnection: close");
	// ret.push_back('\0');
	return ret;
}
