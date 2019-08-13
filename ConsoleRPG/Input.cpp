#include "Input.h"
#include <iostream>
#include "StringUtils.h"

using namespace std;

string Input::promptUser(string message)
{
	cout << message;
	string input;
	do {
		getline(cin, input);
	} while (input == "");
	return input;
}

Input::FormattedInput Input::formatInput(string input)
{
	StringUtils::trim(input);
	StringUtils::toLowerCase(input);

	vector<string> args = StringUtils::parse(input);

	string command = args[0];
	args.erase(args.begin());

	return FormattedInput(command, args);
}