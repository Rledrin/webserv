#include "webserv.hpp"

static std::string getIndex(std::string path, std::map<int ,std::string> &_allFile)
{
	std::stringstream ss;
	std::string ret;
	ss << "<!DOCTYPE html>\n";
	if (path.compare("www/") != 0)
	{
		ss << "<p><a style=text-decoration:none href= .. > . . </a></p>\n";
	}
	std::map<int, std::string>::iterator it;
	it = _allFile.begin();
	while (it != _allFile.end())
	{
		if (it->first < 0)
			ss << "<p><a href=" + it->second + "/>" + it->second + "/</a></p>\n";
		else
			ss << "<p><a href=" + it->second + ">" + it->second + "</a></p>\n";
		it++; 
	}
	ret = ss.str();
	return ret;
}

static void readDirectory(std::map<int ,std::string> &_allFile, std::string &path)
{
	int first;
	int file = 1;
	int dir = -1;
	std::string second;
	struct dirent *_file;
	DIR *_dir;
	_dir = opendir(path.c_str());
	if (_dir == NULL)
		std::cout << "OpenDir Error" << std::endl;
	while ((_file = readdir(_dir)) != NULL)
	{
		if (_file->d_name[0] != '.')
		{
			if (_file->d_type == DT_DIR)
			{
				first = dir;
				dir--;
			}
			else
			{
				first = file;
				file++;
			}
			second = _file->d_name;
			std::pair<int, std::string> tmp;
			tmp.first = first;
			tmp.second = second;
			_allFile.insert(tmp);
		}
	}
	closedir(_dir);
}

std::string generateIndex(std::string path)
{
	std::map<int ,std::string> _allFile;
	std::string ret = "HTTP/1.1 200 OK\n\n";
	readDirectory(_allFile, path);
	ret.append(getIndex(path, _allFile));
	return ret;
}
