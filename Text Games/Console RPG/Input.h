#pragma once

#include <string>
#include <vector>

namespace Input
{
	struct FormattedInput
	{
		FormattedInput(std::string cmd, std::vector<std::string> args) : command(cmd), args(args) {}

		std::string command;
		std::vector<std::string> args;
	};

	std::string promptUser(std::string message = "");
	FormattedInput formatInput(std::string input);
	
}