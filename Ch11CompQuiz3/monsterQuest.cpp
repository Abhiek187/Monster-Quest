/* The player must fight monsters until he or she reaches level 20.
 * monsterQuest.cpp
 *
 *  Created on: Jun 9, 2017
 *      Author: Abhiek187
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

int getRandomNumber(int min, int max)
	{
		static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
		return static_cast<int>(rand() * fraction * (max - min + 1) + min);
	}

// information shared between the player and enemy
class Creature
{
protected: // so Player and Monster can access these variables
	std::string m_name;
	char m_symbol;
	int m_health;
	int m_power;
	int m_gold;

public:
	Creature(std::string name, char symbol, int health, int power, int gold)
		: m_name(name), m_symbol(symbol), m_health(health), m_power(power), m_gold(gold)
	{
	}

	const std::string& getName() {return m_name;}
	char getSymbol()             {return m_symbol;}
	int getHealth()              {return m_health;}
	int getPower()               {return m_power;}
	int getGold()                {return m_gold;}

	void reduceHealth(int damage) {m_health -= damage;}
	bool isDead() {return m_health <= 0;}
	void addGold(int value) {m_gold += value;}
};

// information about the player
class Player: public Creature
{
	int m_level; // player starts at level 1

public:
	Player(std::string name)
		: Creature(name, '@', 30, 1, 0), m_level(1) // HP: 30, Pow: 1, Gold: 0
	{
	}

	void levelUp() {++m_level; ++m_power;} // attack increases by 1 each level
	int getLevel() {return m_level;}
	bool hasWon() {return m_level >= 20;} // At level 20, the player wins.
};

// information about the enemy the player is fighting
class Monster: public Creature
{
public:
	enum Type
	{
		DRAGON,
		VAMPIRE,
		ORC,
		SLIME,
		MAX_TYPES,
	};

	struct MonsterData
	{
		const char* name; // faster than std::string in this case
		char symbol;
		int health;
		int power;
		int gold;
	};

	static MonsterData monsterData[MAX_TYPES];

	Monster(Type type)
		: Creature(monsterData[type].name, monsterData[type].symbol,
		monsterData[type].health, monsterData[type].power, monsterData[type].gold)
	{
	}

	static Monster getRandomMonster() // monsters are random each encounter
	{
		return Monster(static_cast<Type>(getRandomNumber(0, MAX_TYPES - 1)));
	}
};

// lookup table for each monster type
Monster::MonsterData Monster::monsterData[Monster::MAX_TYPES] =
{
	{"dragon", 'D', 20, 4, 100}, // Dragon: HP: 20, Pow: 4, Gold: 100
	{"vampire", 'V', 10, 2, 50}, // Vampire: HP: 10, Pow: 2, Gold: 50
	{"orc", 'o', 4, 2, 25},      // Orc: HP: 4, Pow: 2, Gold: 25
	{"slime", 's', 1, 1, 10},    // Slime: HP: 1, Pow: 1, Gold: 10
};

void attackPlayer(Player &p, Monster &m)
{
	p.reduceHealth(m.getPower());
	std::cout << "The " << m.getName() << " hit you for " << m.getPower() << " damage.\n";
}

void attackMonster(Player &p, Monster &m)
{
	m.reduceHealth(p.getPower());
	std::cout << "You hit the " << m.getName() << " for " << p.getPower() << " damage.\n";

	if (m.isDead())
	{
		std::cout << "You defeated the " << m.getName() << ".\n";
		p.levelUp();
		std::cout << "You are now at level " << p.getLevel() << ".\n";
		p.addGold(m.getGold());
		std::cout << "The " << m.getName() << " dropped " << m.getGold() << " gold.\n";
	}
	else
		attackPlayer(p, m);
}

void fightMonster(Player &p)
{
	Monster m(Monster::getRandomMonster());
	std::cout << "A(n) " << m.getName() << " has appeared! (" << m.getSymbol() << ")\n";
	bool hasRun(false);

	// battle continues until monster or player dies or player has run away
	while (!m.isDead() && !p.isDead() && !hasRun)
	{
		std::cout << "Lv: " << p.getLevel() << ", HP: " << p.getHealth() << "/30, (R)un or (F)ight: ";
		char choice;
		std::cin >> choice;

		if (choice == 'r' || choice == 'R')
		{
			hasRun = getRandomNumber(0,1); // player has a 50% chance of running away

			if (hasRun)
				std::cout << "You successfully fled.\n";
			else
			{
				std::cout << "You failed to flee.\n";
				attackPlayer(p, m);
			}
		}
		else if (choice == 'f' || choice == 'F')
			attackMonster(p, m); // player always attacks first
	}
}

int main()
{
	srand(static_cast<unsigned int>(time(0)));
	rand(); // first result isn't random

	std::cout << "Enter your name, brave warrior: ";
	std::string name;
	std::cin >> name;
	Player p(name);
	std::cout << "Welcome, " << p.getName() << ". Can you survive for 20 levels?\n";

	// game continues until player reaches level 20 or has lost all HP
	while (!p.hasWon() && !p.isDead())
		fightMonster(p);

	if (p.hasWon())
		std::cout << p.getName() << " has won! (w/ " << p.getGold() << " gold to spare!)\n";
	else
		std::cout << p.getName() << " vanquished at level " << p.getLevel() <<
			" and lost " << p.getGold() << " gold.\n";

	return 0;
}
