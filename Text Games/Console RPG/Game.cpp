#include "Game.h"
#include <memory>
#include <string>
#include <iostream>
#include "Vendor.h"
#include "Sign.h"
#include "Enemy.h"
#include "Junk.h"
#include "HealthIncreasePotion.h"
#include "Equipment.h"
#include "EquipmentSlot.h"
#include "StringUtils.h"

using namespace std;

Game::Game() 
{
	unique_ptr<Zone> village = make_unique<Zone>("Village");
	village->addObject(make_unique<Vendor>("Mitchell"));
	village->addObject(make_unique<Vendor>("Alexander"));
	village->addObject(make_unique<Enemy>("Skeleton"));
	village->addObject(make_unique<Sign>(
		"Warning",
		"Skeletons claimed this land to be their home. If you wish to pass through this land, keep in mind that you will like encounter these vile creatures on your way. May your path be as safe as it can possibly be.\n"
	));
	village->removeObject("skeleton");
	world.addZone(move(village));

	unique_ptr<Zone> forest = make_unique<Zone>("Forest");
	forest->addObject(make_unique<Enemy>("Elf"));
	forest->addObject(make_unique<Enemy>("Skeleton"));
	forest->addObject(make_unique<Enemy>("Orc"));
	forest->addObject(make_unique<Sign>(
		"Beware: Elves",
		"Elves are proud of these forests. Since the most ancient times these forests are elves' only true home. But be careful, they aren't fond of trespassers...\n"
		));
	world.addZone(move(forest));

	world.switchZone("village");
	
	shared_ptr<Gear> dagger = make_shared<Gear>("Dagger", 5, 20, "Subtle weapon that is capable of killing even the most awake creatures.", Slot::OffHand);
	dagger->addModifier(Stats::AGILITY, "Agility", 3);
	dagger->addModifier(Stats::STRENGTH, "Strength", 1);

	shared_ptr<Gear> chestpiece = make_shared<Gear>("Chestpiece", 20, 50, "Armor capable of saving your life even on the verge of death.", Slot::Chest);
	chestpiece->addModifier(Stats::ARMOR, "Armor", 10);
	chestpiece->addModifier(Stats::STAMINA, "Stamina", 5);

	shared_ptr<Gear> crown = make_shared<Gear>("Crown", 10, 250, "Rumors have it that this crown belonged to the King of Skeletons...", Slot::Head);
	crown->addModifier(Stats::INTELLECT, "Intellect", 15);
	crown->addModifier(Stats::STAMINA, "Stamina", 10);
	crown->addModifier(Stats::ARMOR, "Armor", 2);

	shared_ptr<Gear> sword = make_shared<Gear>("Sword", 15, 76, "This sword may destroy up to a dozen enemies in a single swing.", Slot::MainHand);
	sword->addModifier(Stats::STRENGTH, "Strength", 10);
	sword->addModifier(Stats::AGILITY, "Agility", 1);

	player.inventory.addGold(567);
	player.inventory.addItem(make_shared<Junk>("Note", 1, 3, "A piece of paper with notes scrambled upon it."), 3);
	player.inventory.addItem(make_shared<Junk>("Healing Potion", 3, 10, "Even a single sip can majorly improve injured's health."), 20);
	player.inventory.addItem(make_shared<Junk>("Fang", 2, 1, "A fang obtained from a wolf's corpse."), 1);
	player.inventory.addItem(dagger, 3);
	player.inventory.addItem(chestpiece, 1);
	player.inventory.addItem(crown, 1);
	player.inventory.addItem(sword, 1);
}
Game::~Game() {}

void Game::evaluate(Input::FormattedInput input)
{
	if (input.command == "search")
	{
		world.getCurrentZone().search();
	}
	else if (input.command == "travel" && !input.args.empty())
	{
		world.switchZone(input.args[0]);
	}
	else if (input.command == "use" && !input.args.empty())
	{
		shared_ptr<Item>& item = player.inventory.getItem(input.args[0]);
		if (item != nullptr)
			item->onInteract(this);
		else
			cout << "No item " << input.args[0] << " found.\n";
	}
	else if (input.command == "drop" && !input.args.empty())
	{
		string& itemName = input.args[0];
		if (player.inventory.hasItemByName(itemName))
		{
			int count = 1;
			if (input.args.size() >= 2 && StringUtils::isNumber(input.args[1]))
			{
				count = stoi(input.args[1]);
			}
			player.inventory.removeItem(itemName, count);
			cout << "Item " << itemName << " removed from the inventory.\n";
		}
		else
			cout << "No item " << itemName << " found.\n";
	}
	else if (input.command == "unequip" && !input.args.empty())
	{
		shared_ptr<Gear>& item = player.equipment.getItem(input.args[0]);
		if (item != nullptr)
		{
			player.equipment.unequip(item->slot);
			cout << "Unequipped " << item->name << ".\n";
		}
		else
			cout << "No item called " << input.args[0] << " found.\n";
		
	}
	else if (input.command == "interact" && !input.args.empty())
	{
		world.getCurrentZone().interactWith(input.args[0], this);
	}
	else if (input.command == "print" && !input.args.empty())
	{
		if (input.args[0] == "inventory")
		{
			player.inventory.print();
		}
		else if (input.args[0] == "health")
		{
			cout << "Health: " << player.health << "/" << player.maxHealth << "\n";
		}
		else if (input.args[0] == "gear")
		{
			player.equipment.print();
		}
		else if (input.args[0] == "world")
		{
			world.print();
		}
		else if (input.args[0] == "stats")
		{
			for (auto &entry : player.stats)
			{
				cout << entry.second->name << ": " << entry.second->getValue() << "\n";
			}
		}
		else
		{
			cout << "Cannot print " << input.args[0] << "\n";
		}
	}
	else
	{
		cout << "What? I don't understand this nonsense!\n";
	}
}

void Game::start()
{
	while (!bGameOver)
	{
		string input = Input::promptUser();
		evaluate(Input::formatInput(input));
	}
}
