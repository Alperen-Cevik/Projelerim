#include <iostream>

#include "grid.h"

struct Direction {
	int offsetX;
	int offsetY;
};

void printHitMessage(Robot* attacker, Robot* victim, int damage) {
	printf("%s(%d) hits %s(%d) with %d\n",
		attacker->getName().c_str(), attacker->getHitPoints(),
		victim->getName().c_str(), victim->getHitPoints() + damage, damage);

	printf("The new hitpoints of %s is %d\n",
		victim->getName().c_str(), victim->getHitPoints());

	if (victim->isDead())
		printf("%s died\n", victim->getName().c_str());
}

void fight(Robot* attacker, Robot* victim)
{
	while (attacker->getHitPoints() > 0 && victim->getHitPoints() > 0) {
		int attackerDamage = attacker->getDamage();

		victim->takeDamage(attackerDamage);

		printHitMessage(attacker, victim, attackerDamage);

		if (victim->getHitPoints() <= 0)
			break;

		int victimDamage = victim->getDamage();

		attacker->takeDamage(victimDamage);

		printHitMessage(victim, attacker, victimDamage);
	}
}

#include <vector>

using namespace std;

class Solution {
public:
	static void rotate(vector<int>& nums, int k) {
		k %= nums.size();

		int num = nums[0], j = 0;
		for (int i = 0; i < nums.size(); i++) {
			j += k;
			j %= nums.size();
			int tmp = nums[j];
			nums[j] = num;
			num = tmp;
		}


	}
};

int main()
{
	vector<int> nums{ -1,-100,3,99 };
	Solution::rotate(nums, 2);

	system("pause");

	std::srand(time(nullptr));

	size_t grid_height = 10;
	size_t grid_width = 10;
	size_t initial_count_of_each_robot_type = 6;
	size_t total_robot_count = initial_count_of_each_robot_type * 4; // there are 4 types of robots

	Grid grid(grid_height, grid_width);

	Robot** robots = new Robot * [total_robot_count];
	size_t ctr = 0;

	// Create instances of each robot type and store them in the robots array
	for (size_t i = 0; i < initial_count_of_each_robot_type; i++) {
		robots[ctr++] = new robocop("robocop_" + std::to_string(i));
	}

	for (size_t i = 0; i < initial_count_of_each_robot_type; i++) {
		robots[ctr++] = new bulldozer("bulldozer_" + std::to_string(i));
	}

	for (size_t i = 0; i < initial_count_of_each_robot_type; i++) {
		robots[ctr++] = new roomba("roomba_" + std::to_string(i));
	}

	for (size_t i = 0; i < initial_count_of_each_robot_type; i++) {
		robots[ctr++] = new optimusprime("optimusprime_" + std::to_string(i));
	}

	// Place robots randomly on the grid
	for (size_t i = 0; i < total_robot_count; i++) {
		auto randX = rand() % grid.getHeight();
		auto randY = rand() % grid.getWidth();

		auto cell = grid.getCellAt(randX, randY);

		if (cell->robot) {
			i--;
			continue;
		}

		cell->robot = robots[i];
	}

	// create directions
	Direction directions[4];

	directions[0] = { 0, 1 }; // up
	directions[1] = { 0, -1 }; // down
	directions[2] = { -1, 0 }; // left
	directions[3] = { 1, 0 }; // right

	int aliveCount = 0;
	do {
		// Iterate through every cell in the grid
		for (size_t y = 0; y < grid.getHeight(); y++) {
			for (size_t x = 0; x < grid.getWidth(); x++) {
				auto cell = grid.getCellAt(x, y);

				// Skip if the cell has no robot or if the robot has already fought
				if (!cell->robot || cell->robot->hasFought())
					continue;

				Robot* attacker = cell->robot;

				Cell* victimCell = nullptr;
				size_t newX = x, newY = y;
				while (true) { // Move the attacker robot until it encounters a cell with a victim robot
					Direction& dir = directions[rand() % 4];
					size_t new_x = newX + dir.offsetX;
					size_t new_y = newY + dir.offsetY;

					victimCell = grid.getCellAt(new_x, new_y);

					if (victimCell) {
						newX = new_x;
						newY = new_y;

						if (victimCell->robot)
							break;

						cell->robot = nullptr;
						cell = victimCell; // move the attacker robot
						cell->robot = attacker;
					}
				}

				// Fight between the attacker and victim robots
				fight(cell->robot, victimCell->robot);

				if (cell->robot->isDead()) { // if attacker is dead
					cell->robot = nullptr; // remove it from the grid
					victimCell->robot->setFought(true); // set victim as fought
				}
				else if (victimCell->robot->isDead()) { // same procedure for victim
					victimCell->robot = nullptr;
					cell->robot->setFought(true);
				}
			}
		}

		aliveCount = 0;

		// Count the number of alive robots
		for (size_t i = 0; i < total_robot_count; i++) {
			robots[i]->setFought(false);

			aliveCount += !robots[i]->isDead();
		}

	} while (aliveCount != 1); // Loop until only one or zero robot stands.

	for (size_t i = 0; i < total_robot_count; i++) {
		if (!robots[i]->isDead())
			printf("%s stands alive\n", robots[i]->getName().c_str());
	}

	// Clean up the robots
	for (size_t i = 0; i < total_robot_count; i++)
		delete robots[i];

	delete[] robots;

	return 0;
}
