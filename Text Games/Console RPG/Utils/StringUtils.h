#pragma once

#include <string>
#include <vector>

namespace StringUtils
{

	bool isWhitespaceString(const std::string &str);

	void trim(std::string &str);

	void toLowerCase(std::string &str);
	std::string toLowerCaseCopy(std::string str);

	std::vector<std::string> parse(const std::string &str, char delimeter = ' ', char strDelimeter = '\'');

	bool isNumber(const std::string &str);

}