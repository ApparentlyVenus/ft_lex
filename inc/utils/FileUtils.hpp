#ifndef FILEUTILS_HPP
# define FILEUTILS_HPP

# include "StringUtils.hpp"
# include <sys/stat.h>
# include <unistd.h>
# include <fstream>

bool		fileExists(const std::string& path);
bool		isReadable(const std::string& path);
std::string	readFile(const std::string& path);

#endif