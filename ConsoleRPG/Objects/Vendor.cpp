#include "Vendor.h"
#include "Input.h"
#include <iostream>
#include "Game.h"
#include "Gear.h"
#include "Junk.h"
#include "EquipmentSlot.h"
#include "StringUtils.h"

using namespace std;

Vendor::Vendor(string name) : Object(name)
{
	addItem(make_shared<Gear>("Bow", 7, 43, "Ranged weapon. Usually used to hunt on animals.", Slot::MainHand), 2);
	addItem(make_shared<Item>("Book", 12, 15, "This book describes some sort of a fantastic deviant creature... some sort of a god."), 3);
	addItem(make_shared<Junk>("Basket", 2, 3, "A wooden basket."), 5);
	addItem(make_shared<Gear>("Legplates of Truth", 17, 67, "Pants made of plate. Good for protection.", Slot::Legs), 4);
}

Vendor::~Vendor() {}

bool Vendor::hasInStock(string itemName)
{
	return inventory.getItem(itemName) != nullptr;
}

void Vendor::addItem(shared_ptr<Item> item, int count)
{
	inventory.addItem(item, count);
}

void Vendor::removeItem(string name, int count)
{
	inventory.removeItem(name, count);
}

void Vendor::onInteract(Game* context)
{
	cout << "Hello! My name is " << name << ". I am a vendor at this lovely place! Would you like to take a look at our goods, kind sir?\n";
	string response = Input::promptUser();
	if (response == "yes")
	{
		Inventory& playerInventory = context->player.inventory;
		cout << "Ah, great! I am sure you will find my wares to be of use!\n";
		cout << "How can I help you?\n";
		while (true) {
			response = Input::promptUser();
			Input::FormattedInput input = Input::formatInput(response);
			if (input.command == "print" && !input.args.empty())
			{
				if (input.args[0] == "inventory")
				{
					playerInventory.print();
				}
				else if (input.args[0] == "shop")
				{
					inventory.print();
				}
			}
			else if (input.command == "buy" && !input.args.empty())
			{
				if (hasInStock(input.args[0]))
				{
					int count = 1;
					if (input.args.size() >= 2 && StringUtils::isNumber(input.args[1]))
					{
						count = stoi(input.args[1]);
					}
					shared_ptr<Item>& item = inventory.getItem(input.args[0]);

					int estimatedCost = item->estimatedCost * count;
					int availableGold = playerInventory.getGold();

					int estimatedWeight = item->weight * count;
					int availableSpace = playerInventory.getCapacity() - estimatedWeight;

					if (estimatedCost > availableGold)
						cout << "Wait, you don't have enough gold to buy this. In fact you need " << (estimatedCost - availableGold) << " more gold to pay.\n";
					else if (estimatedWeight > availableSpace)
						cout << "You don't have enough space for this, I'd rather make sure you get everything you paid for. How about if you free " << (estimatedWeight - availableSpace) << "kg first?\n";
					else
					{
						inventory.transferItem(playerInventory, item, count);
						playerInventory.transferGold(inventory, estimatedCost);
						cout << "Deal!\n";
					}
				}
				else
					cout << "I don't have " << input.args[0] << " in stock.\n";
			}
			else if (input.command == "sell" && !input.args.empty())
			{
				if (playerInventory.hasItemByName(input.args[0]))
				{
					int count = 1;
					if (input.args.size() >= 2 && StringUtils::isNumber(input.args[1]))
					{
						count = stoi(input.args[1]);
					}
					shared_ptr<Item>& item = playerInventory.getItem(input.args[0]);
					
					int estimatedCost = item->estimatedCost * count;
					int availableGold = inventory.getGold();

					int estimatedWeight = item->weight * count;
					int availableSpace = inventory.getCapacity() - estimatedWeight;

					if (estimatedCost > availableGold)
						cout << "I don't have enough money to pay for this, pal. I need precisely " << (estimatedCost - availableGold) << " more gold to pay.\n";
					else if (estimatedWeight > availableSpace)
						cout << "I don't have enough space for this, and I'd rather make sure I get everything I paid for. Come visit me later, maybe I'll free some space by then.\n";
					else
					{
						playerInventory.transferItem(inventory, item, count);
						inventory.transferGold(playerInventory, (item->estimatedCost * count));
						cout << "Deal!\n";
					}
				}
				else
					cout << "I don't see anything like that in your inventory.\n";
			}
			else
			{
				cout << "Pardon?\n";
			}
		}
	}
	else
	{
		cout << "Hm? Oh well, maybe next time then. May your path be a safe one.\n";
	}
}
