#include "robot.h"

bool chance(int percent) { // chance function
	return rand() % 100 < percent;
}

Robot::Robot(int newType, int newStrength, int newHit, std::string name) : type(newType), strength(newStrength), hitPoints(newHit), name(name), fought_flag(false)
{

}

Robot::~Robot()
{
}

std::string Robot::getName()
{
	return name;
}

void Robot::setName(std::string name)
{
	this->name = name;
}

void Robot::setType(int newType)
{
	type = newType;
}

int Robot::getStrength()
{
	return strength;
}

void Robot::setStrength(int val)
{
	strength = val;
}

int Robot::getHitPoints()
{
	return hitPoints;
}

void Robot::setHitPoints(int val)
{
	this->hitPoints = val;
}

int Robot::getDamage()
{
	return (rand() % strength) + 1;
}

void Robot::takeDamage(int val)
{
	hitPoints -= val;
}

void Robot::setFought(bool isFought)
{
	fought_flag = isFought;
}

bool Robot::hasFought()
{
	return fought_flag;
}

bool Robot::isDead()
{
	return hitPoints <= 0;
}

bulldozer::bulldozer(std::string name) : Robot(3, 50, 200, name)
{
}

std::string bulldozer::getType()
{
	return "bulldozer";
}

roomba::roomba(std::string name) : Robot(2, 3, 10, name)
{
}

std::string roomba::getType()
{
	return "roomba";
}

int roomba::getDamage()
{
	return Robot::getDamage() * 2;
}

humanic::humanic(int newType, int newStrength, int newHit, std::string name) : Robot(newType, newStrength, newHit, name)
{
	
}

humanic::~humanic()
{
}

int humanic::getDamage()
{
	int damage = Robot::getDamage();

	if (chance(10))
		damage += 50;

	return damage;
}

optimusprime::optimusprime(std::string name) : humanic(0, 100, 100, name)
{
}

std::string optimusprime::getType()
{
	return "optimusprime";
}

int optimusprime::getDamage()
{
	int damage = humanic::getDamage();

	if (chance(15))
		damage *= 2;

	return damage;
}

robocop::robocop(std::string name) : humanic(0, 30, 40, name)
{
	
}

std::string robocop::getType()
{
	return "robocop";
}
