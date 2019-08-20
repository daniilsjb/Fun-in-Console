#include "Enemy.h"
#include "Game.h"
#include <iostream>
#include "Input.h"
#include "Loot.h"
#include "HealthIncreasePotion.h"

using namespace std;

Enemy::Enemy(string name) : Mob(name) 
{
	inventory.addGold(rand() % 50);
	inventory.addItem(make_shared<Gear>("Helmet of Truth", 30, 50, "Its wearer will always see the truth.", Slot::Head), 1);
	inventory.addItem(make_shared<HealthIncreasePotion>("Potion", 5, 30, "Permanently increases one's health capacity."), 3);
	shared_ptr<Gear> dagger = make_shared<Gear>("Dagger", 5, 20, "Subtle weapon that is capable of killing even the most awake creatures.", Slot::OffHand);
	inventory.addItem(dagger, 3);
	equipment.equip(dagger);
	stats[Stats::INTELLECT]->decrease(5);
}

Enemy::~Enemy() {}

void Enemy::onInteract(Game* context)
{
	Mob& player = context->player;
	cout << "Fight me, mortal.\n";
	while (!player.isDead() && !isDead())
	{
		string response;
		bool playerTurn = true;
		while (playerTurn)
		{
			response = Input::promptUser();
			Input::FormattedInput input = Input::formatInput(response);
			if (input.command == "cast")
			{
				if (!input.args.empty())
				{
					shared_ptr<Ability>& ability = player.spellbook.getAbility(input.args[0]);
					if (ability != nullptr)
					{
						if (ability->mayUse(player, *this))
						{
							ability->use(player, *this);
							playerTurn = false;
						}
					}
					else
						cout << "No such ability available.\n";
				}
				else
					cout << "You must provide the name of the ability.\n";
			}
		}

		shared_ptr<Ability>& ability = chooseAbility(player);
		if (ability != nullptr)
		{
			ability->use(*this, player);
		}
		else
			cout << name << " is unable to attack you right now.\n";
	}
	if (player.isDead())
	{
		cout << "You died.\n";
		context->bGameOver = true;
	}
	else
	{
		cout << name << " has died.\n";
		equipment.unequipAll();
		Loot::startSession(inventory, player.inventory);
		context->world.getCurrentZone().removeObject(name);
	}
}

shared_ptr<Ability> Enemy::chooseAbility(Mob& target)
{
	for (auto &ability : spellbook.getAbilities())
	{
		if (ability->mayUse(*this, target))
			return ability;
	}
	return nullptr;
}