#include "StringUtils.h"
#include <algorithm>
#include <cctype>
#include <sstream> 

using namespace std;

bool StringUtils::isWhitespaceString(const std::string &str)
{
	return str.empty() || std::all_of(str.begin(), str.end(), ::isspace);
}

void StringUtils::trim(string &str)
{
	str.erase(str.begin(), find_if(str.begin(), str.end(), [](int c) {
		return !isspace(c);
	}));

	str.erase(find_if(str.rbegin(), str.rend(), [](int c) {
		return !isspace(c);
	}).base(), str.end());
}

void StringUtils::toLowerCase(string &str)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
}

string StringUtils::toLowerCaseCopy(string input)
{
	transform(input.begin(), input.end(), input.begin(), ::tolower);
	return input;
}

vector<std::string> StringUtils::parse(const string &str, char delimeter, char strDelimeter)
{
	vector<string> result;
	string currentWord;
	for (auto it = str.begin(); it != str.end(); ++it)
	{
		const int index = it - str.begin();
		const char& c = (*it);
		if (c == strDelimeter)
		{
			int substringStart = str.find(strDelimeter, index);
			int substringEnd = str.find(strDelimeter, substringStart + 1);
			if (substringEnd != string::npos)
			{
				//Extract the substring and add it to the list
				int substringLength = substringEnd - substringStart;
				string substring = str.substr(substringStart + 1, substringLength - 1);
				result.push_back(substring);

				//Skipping through the substring
				it += substringLength;

				//Skip through the substring end's quote if it's not the end of the string yet
				if (it != str.end() - 1)
					it++;

				//Add and reset the current word (in this case it's the word before the substring)
				if (!isWhitespaceString(currentWord))
				{
					trim(currentWord);
					result.push_back(currentWord);
					currentWord.clear();
				}
			}
			else
				currentWord += c;
		}
		else if (c == delimeter)
		{
			if (!isWhitespaceString(currentWord))
			{
				trim(currentWord);
				result.push_back(currentWord);
				currentWord.clear();
			}
		}
		else
			currentWord += c;
	}
	if (!isWhitespaceString(currentWord))
	{
		trim(currentWord);
		result.push_back(currentWord);
	}
	return result;
}

bool StringUtils::isNumber(const string &str)
{
	return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}