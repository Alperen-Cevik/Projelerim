#include <string>


class Robot
{
public:
	Robot(int newType, int newStrength, int newHit, std::string name);
	virtual ~Robot();

	std::string getName();
	void setName(std::string);
	
	virtual std::string getType() = 0;
	void setType(int newType);

	int getStrength();
	void setStrength(int);

	int getHitPoints();
	void setHitPoints(int);

	virtual int getDamage();
	void takeDamage(int);

	void setFought(bool);
	bool hasFought();

	bool isDead();

private:
	bool fought_flag;

	int type;
	int strength;
	int hitPoints;
	std::string name;
};


class bulldozer : public Robot {
public:
	bulldozer(std::string name);

	std::string getType();
};

class roomba : public Robot {
public:
	roomba(std::string name);

	std::string getType();
	int getDamage() override;
};

class humanic : public Robot {
public:
	humanic(int newType, int newStrength, int newHit, std::string name);
	virtual ~humanic();

	int getDamage() override;
};

class optimusprime : public humanic {
public:
	optimusprime(std::string name);

	std::string getType();
	int getDamage() override;
};

class robocop : public humanic {
public:
	robocop(std::string name);

	std::string getType();
};

