#include "webserv.hpp"

static void initPoll(std::vector<server> &serv, std::vector<struct pollfd> &fds, std::vector<int> &fdsIndx)
{
	uint32_t sizeServ = serv.size();

	for (uint32_t i = 0; i < sizeServ; i++)
	{
		uint32_t sizeLis = serv[i]._listener.size();
		for (uint32_t j = 0; j < sizeLis; j++)
		{
			struct pollfd tmpPfd;
			tmpPfd.fd = serv[i]._listener[j]->_socket;
			tmpPfd.events = POLLIN;
			tmpPfd.revents = 0;
			fds.push_back(tmpPfd);
			fdsIndx.push_back(i);
		}
	}
}

static void closeFd(std::vector<struct pollfd> &fds, int start, int end)
{
	for (int i = start; i < end; i++)
		close(fds[i].fd);
}

#define WITH_TICTOC

void	delete_client(int i, std::vector<struct pollfd> &fds, std::vector<int> &fdsIndx)
{
	std::vector<struct pollfd>::iterator iterVec = fds.begin();
	while (iterVec != fds.end())
	{
		iterVec++;
		if (iterVec->fd == fds[i].fd)
		{ 
			struct pollfd tmp = fds[fds.size() - 1];
			fds[fds.size() - 1] = fds[i];
			fds[i] = tmp;
			close(fds[fds.size() - 1].fd);
			fds.pop_back();

			int temp = fdsIndx[fdsIndx.size() - 1];
			fdsIndx[fdsIndx.size() - 1] = fdsIndx[i];
			fdsIndx[i] = temp;
			fdsIndx.pop_back();
			break;
		}
	}
}

#include "tictoc.hpp"

static void loop(std::vector<server> &serv)
{
	std::vector<struct pollfd> fds;
	std::vector<int> fdsIndx;
	uint32_t indxServ = 0;

	initPoll(serv, fds, fdsIndx);

	indxServ = fds.size() - 1;

	int ret = 0;
	while(1)
	{
		if ((ret = poll(fds.data(), fds.size(), 100)) < 0)
		{
			std::cout << "Poll Error" << std::endl;
			exit(1);
		}
		else if (ret == 0)
		{
			continue;
		}

		for (uint32_t counter = 0; counter <= indxServ; counter++)
		{
			if ((fds[counter].revents & POLLHUP) == POLLHUP || (fds[counter].revents & POLLERR) == POLLERR || (fds[counter].revents & POLLNVAL) == POLLNVAL)
			{
				std::cout << "ERR ACCEPT counter: " << counter << std::endl;
				closeFd(fds, 0, counter + 1);
				return;
			}
			else if ((fds[counter].revents & POLLIN) == POLLIN)
			{
				sockaddr_in sockaddr;
				long int addrlen = sizeof(sockaddr);
				int connection = accept(fds[counter].fd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
				if (connection < 0)
				{
					std::cout << "Failed to grab connection." << std::endl;
					exit(EXIT_FAILURE);
				}
				fds[counter].revents = 0;
				struct pollfd tmpPfd;
				tmpPfd.fd = connection;
				tmpPfd.events = POLLIN | POLLOUT;
				tmpPfd.revents = 0;
				fds.push_back(tmpPfd);
				fdsIndx.push_back(fdsIndx[counter]);
			}
		}
		uint32_t sizeFds = fds.size();
		for (uint32_t i = indxServ + 1; i < sizeFds; i++)
		{
			if ((fds[i].revents & POLLHUP) == POLLHUP || (fds[i].revents & POLLERR) == POLLERR || (fds[i].revents & POLLNVAL) == POLLNVAL)
			{
				std::cout << "ERR" << std::endl;
				closeFd(fds, 0, sizeFds);
				return;
			}
			else if ((fds[i].revents & POLLIN) == POLLIN && (fds[i].revents & POLLOUT) == POLLOUT)
			{
				fds[i].revents = 0;
				char buffer[4096];
				memset(buffer, 0, 4096);
				ssize_t ret;
				if ((ret = recv(fds[i].fd, buffer, 4096, MSG_DONTWAIT)) == -1 || ret == 0)
				{
					delete_client(i, fds, fdsIndx);
					if (ret == -1)
						std::cout << "ERROR RECV" << std::endl;
					break ;
				}
				// TO CHECK IF RECV IS PROTECTED :
				// if (recv(42, buffer, 4096, MSG_DONTWAIT) == -1)
				// {
				// 	delete_client(i, fds, fdsIndx);
				// 	std::cout << "ERROR RECV" << std::endl;
				// 	break ;
				// }

				request parsedRequest = parseRequest(buffer);
				std::string resp = serv[fdsIndx[i]].response(parsedRequest);
				

				// REPLACING THE RESPONSE BY HAND :
				// std::string resp = "HTTP/1.1 200 OK\nConnection: close\n\n<!DOCTYPE html>\n<html>\n<head>\n<title>Basic Web Page</title>\n</head>\n<body>\nHello World!\n</body>\n</html>\n";
				
				if ((ret = send(fds[i].fd, resp.c_str(), resp.size(), 0)) == -1 || ret == 0)
				{
					delete_client(i, fds, fdsIndx);
					if (ret == -1)
						std::cout << "ERROR SEND" << std::endl;
					break ;
				}
				delete_client(i, fds, fdsIndx);
			}
		}
	}
}

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cout << "wrong number of argument" << std::endl;
		exit(1);
	}
	std::vector<server> serv;
	parseConfigFile(serv, av[1]);
	// printall(serv);

	loop(serv);

}