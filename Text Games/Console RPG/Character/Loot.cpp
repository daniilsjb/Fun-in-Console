#include "Loot.h"
#include <iostream>
#include "Input.h"
#include "StringUtils.h"

using namespace std;

void Loot::startSession(Inventory& from, Inventory& to)
{
	cout << "You proceed to collect loot from this inventory:\n";
	from.print();
	while (true)
	{
		if (from.isEmpty() && from.getGold() <= 0)
		{
			cout << "Nothing left in the inventory.\n";
			break;
		}
		string response = Input::promptUser("What would you like to take?\n");
		if (response == "done")
			break;
		Input::FormattedInput input = Input::formatInput(response);
		if (input.command == "print" && !input.args.empty())
		{
			if (input.args[0] == "loot")
			{
				from.print();
			}
			else if (input.args[0] == "inventory")
			{
				to.print();
			}
		}
		else if (input.command == "take" && !input.args.empty())
		{
			if (input.args[0] == "all")
			{
				from.transferAll(to);
				cout << "Taken as much from the inventory as possible!\n";
				break;
			}
			else if (input.args[0] == "gold")
			{
				int amount = from.getGold();
				if (input.args.size() >= 2 && StringUtils::isNumber(input.args[1]))
				{
					amount = stoi(input.args[1]);
				}
				cout << "Amount of gold taken: " << from.transferGold(to, amount) << "\n";
			}
			else
			{
				shared_ptr<Item> item = from.getItem(input.args[0]);
				if (item != nullptr)
				{
					int count = 1;
					if (input.args.size() >= 2)
					{
						if (StringUtils::isNumber(input.args[1]))
							count = stoi(input.args[1]);
						else if (input.args[1] == "all")
							count = from.getAmount(item->name);
					}
					cout << from.transferItem(to, item, count) << " items taken from the inventory.\n";
				}
				else
				{
					cout << "No item called " << input.args[0] << " found in the inventory.\n";
				}
			}
		}
		else
		{
			cout << "Cannot do that with this inventory.\n";
		}
	}
	cout << "Finished collecting loot.\n";
}