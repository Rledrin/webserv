#ifndef SERVER_HPP
#define SERVER_HPP

#include "listener.hpp"

#include <utility>

struct location
{
	std::string _name;
	std::string _defaultDir;
	std::string _rootPath;
	std::string _cgiExtension;
	std::string _cgiPath;
	std::string _uploadPath;
	std::string _redir;
	bool _get;
	bool _post;
	bool _delete;
	bool _autoIndex;

	location(std::string name, std::string defaultDir, std::string rootPath, std::string cgiExtension, std::string cgiPath, std::string uploadPath, std::string redir, bool _get = false, bool _post = false, bool _delete = false, bool autoIndex = false) :
	_name(name), _defaultDir(defaultDir), _rootPath(rootPath), _cgiExtension(cgiExtension), _cgiPath(cgiPath), _uploadPath(uploadPath), _redir(redir), _get(_get), _post(_post), _delete(_delete), _autoIndex(autoIndex)
	{};
};

struct request
{
	int method;
	int contentLength;
	std::string body;
	std::string path;
	std::string contentType;
	std::string host;
	std::string queryString;
	bool err400;


	request(int method, int contentLength, std::string body, std::string path, std::string contentType, std::string host, std::string queryString, bool err400) :
	method(method), contentLength(contentLength), body(body), path(path), contentType(contentType), host(host), queryString(queryString), err400(err400)
	{};
};

class server
{
public:
	std::string _ip;
	// std::vector<listener> _listener;
	std::vector<listener*> _listener;
	std::string _name;
	std::string _rootPath;
	int _maxBodySize;
	std::map<int, std::string> _error;


	std::string _defaultDir;
	std::string _cgiExtension;
	std::string _cgiPath;
	std::string _uploadPath;
	std::string _redir;
	bool _get;
	bool _post;
	bool _delete;
	bool _autoIndex;

	std::vector<location> _loc;

	server();
	~server();

	std::string response(request &reqParam);


};


#endif
