#include "listener.hpp"

listener::listener(const char *ip, int port)
{
	_port = port;
	_ip = ip;
	if (_ip.length() == 0)
	{
		std::cout << "Ip not defined" << std::endl;
		exit(EXIT_FAILURE);
	}
	_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (_socket == -1)
	{
		std::cout << "Failed to create socket ip: " << _ip << " port: " << _port << std::endl;
		exit(EXIT_FAILURE);
	}

	// if (fcntl(_socket, F_SETFL, O_NONBLOCK) < 0)
	if (fcntl(_socket, F_SETFL, fcntl(_socket, F_GETFL, 0) | O_NONBLOCK) < 0)
	{
		std::cout << "Failed to set the docket to nonblock with fcntl" << std::endl;
		close(_socket);
		exit(EXIT_FAILURE);
	}

	sockaddr_in sockaddr = {};
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(_port);
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	// inet_aton(ip, (in_addr*)&sockaddr.sin_addr.s_addr);

	int yes = 1;
	setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	// gethostname("JeSuisUnHost", 13);

	if (bind(_socket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cout << "Failed to bind to port " << _port << std::endl;
		close(_socket);
		exit(EXIT_FAILURE);
	}

	if (listen(_socket, 500) < 0)
	{
		std::cout << "Failed to listen on socket" << std::endl;
		close(_socket);
		exit(EXIT_FAILURE);
	}
}

listener::listener(const listener &op)
{
	if (op._ip.length() == 0)
		return ;
	//close(op._socket);
	_port = op._port;
	_ip = op._ip;
	if (_ip.length() == 0)
	{
		std::cout << "Ip not defined" << std::endl;
		exit(EXIT_FAILURE);
	}
	_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (_socket == -1)
	{
		std::cout << "Failed to create socket ip: " << _ip << " port: " << _port << std::endl;
		exit(EXIT_FAILURE);
	}

	// if (fcntl(_socket, F_SETFL, O_NONBLOCK) < 0)
	if (fcntl(_socket, F_SETFL, fcntl(_socket, F_GETFL, 0) | O_NONBLOCK) < 0)
	{
		std::cout << "Failed to set the docket to nonblock with fcntl" << std::endl;
		close(_socket);
		exit(EXIT_FAILURE);
	}

	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(_port);
	inet_aton(_ip.c_str(), (in_addr*)&sockaddr.sin_addr.s_addr);
	if (bind(_socket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cout << "Failed to bind to port " << _port << std::endl;
		close(_socket);
		exit(EXIT_FAILURE);
	}

	// Start listening. Hold at most 10 connections in the queue
	if (listen(_socket, 10) < 0)
	{
		std::cout << "Failed to listen on socket" << std::endl;
		close(_socket);
		exit(EXIT_FAILURE);
	}
}

listener::listener()
{
	std::cout << "tes sur que tes la ?" << std::endl;
}

listener::~listener()
{
	// FD_CLR(_socket);
	std::cout << "listener destroyed" << std::endl;
	close(_socket);
}
