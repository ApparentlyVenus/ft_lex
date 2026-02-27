#include "../inc/utils/StringUtils.hpp"

std::string toUpper(const std::string &str)
{
	std::string result = str;
	for (size_t i = 0; i < result.size(); i++)
		result[i] = std::toupper(result[i]);
	return (result);
}

std::string toLower(const std::string &str)
{
	std::string result = str;
	for (size_t i = 0; i < result.size(); i++)
		result[i] = std::tolower(result[i]);
	return result;
}

std::string trim(const std::string &str)
{
	size_t start = 0;
	size_t end = str.size();

	while (start < end && std::isspace(str[start]))
		start++;

	while (end > start && std::isspace(str[end - 1]))
		end--;

	return str.substr(start, end - start);
}

std::vector<std::string> split(const std::string &str, char delim)
{
	std::vector<std::string> container;
	size_t pos = 0;
	size_t start = 0;

	while (pos != std::string::npos)
	{
		pos = str.find(delim, start);
		if (pos != std::string::npos)
		{
			container.push_back(str.substr(start, pos - start));
			start = pos + 1;
		}
		else
			container.push_back(str.substr(start));
	}
	return (container);
}
