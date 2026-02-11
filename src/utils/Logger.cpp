#include "../inc/utils/Logger.hpp"

static std::string	getCurrentTimestamp()
{
	time_t now = time(0);
	char buf[80];
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
	return std::string(buf);
}

void	logError(const std::string &message)
{
	std::cerr << "[" << getCurrentTimestamp() << "] [ERROR] "
			  << message << std::endl;
}

void	logInfo(const std::string &message)
{
	std::cout << "[" << getCurrentTimestamp() << "] [INFO] "
			  << message << std::endl;
}
