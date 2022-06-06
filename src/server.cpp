#include "server.hpp"

server::server()
{
	_get = false;
	_post = false;
	_delete = false;
	_autoIndex = false;
}

server::~server()
{
	// uint32_t size = _listener.size();

	// for (uint32_t i = 0; i < size; i++)
	// {
	// 	close(_listener[i]->_socket);
	// 	free(_listener[i]);
	// }
}


void printall(std::vector<server> &serv)
{
	for(uint32_t i = 0; i < serv.size(); i++)
	{
		std::cout << "CONFIGURATION\n\n" << std::endl;
		std::cout << " serv n0:  : " << i << std::endl;
		std::cout << " IP : " << serv[i]._ip << std::endl;
		std::cout << " name : " << serv[i]._name << std::endl;
		std::cout << " rootPath : " << serv[i]._rootPath << std::endl;
		std::cout << " maxBodySize : " << serv[i]._maxBodySize << std::endl;
		// std::cout << serv[i]._error << std::endl;
		std::cout << " dDir : " << serv[i]._defaultDir << std::endl;
		std::cout << " cgiExt : " << serv[i]._cgiExtension << std::endl;
		std::cout << " cgiPath : " << serv[i]._cgiPath << std::endl;
		std::cout << " uploadPath : " << serv[i]._uploadPath << std::endl;
		for (uint32_t j = 0; j < serv[i]._listener.size(); j++)
		{
			std::cout << " port : " << serv[i]._listener[j]->_port << std::endl;
			std::cout << " ip : " << serv[i]._listener[j]->_ip << std::endl;
		}
		std::cout << "\n\nLOCATIONS\n\n" << std::endl;
		for (uint32_t j = 0; j < serv[i]._loc.size(); j++)
			std::cout << "redir: " << serv[i]._loc[j]._redir << std::endl;
		// for (uint32_t j = 0; j < serv[i]._error.size(); j++)
		// {
		// 	std::cout << " codeError : " << serv[i]._error[j].first << std::endl;
		// 	std::cout << " pathError : " << serv[i]._error[j].second << std::endl;
		// }
	}
}