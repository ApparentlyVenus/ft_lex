#ifndef STRINGUTILS_HPP
# define STRINGUTILS_HPP

# include <string>
# include <cstdlib>
# include <stdexcept>
# include <sstream>
# include <vector>

std::string					trim(const std::string& str);
std::vector<std::string>	split(const std::string& str, char delim);
std::string					toLower(const std::string& str);
std::string					toUpper(const std::string &str);

#endif