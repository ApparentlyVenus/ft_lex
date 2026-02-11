#include "../inc/utils/FileUtils.hpp"

bool	fileExists(const std::string& path)
{
	struct stat buff;

	if (stat(path.c_str(), &buff) != 0)
		return (false);
	return (true);
}

bool	isReadable(const std::string& path)
{
	if (access(path.c_str(), R_OK) != 0)
		return (false);
	return (true);
}

std::string readFile(const std::string& path)
{
	std::string content;
	char c;

	if (!fileExists(path) || !isReadable(path))
		return "";

	std::ifstream f(path.c_str(), std::ios::binary);
	if (!f.is_open())
		return "";

	while (f.get(c))
		content += c;

	return (content);
}