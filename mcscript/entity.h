#ifndef ENTITY_H
#define ENTITY_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <string>
#include <vector>
#include <sstream>
#define pi 3.14159265358979

#define air 1.0f
#define slab_bottom float(1.0 / 3.0)
#define slab_top float(2.0 / 3.0)
#define two_three float(2.0 / 3.0)
#define seven_nine float(7.0 / 9.0)
#define five_nine float(5.0 / 9.0)
#define one_twenty float(1.0 / 20.0)
#define STANDARD_PRECISION 17 // 17 normal
#define DISPLAY_LOC_DIGITS 17 // 17 normal
#define DISPLAY_MOT_DIGITS 17

// Class that contains the current blocks in the world
// We do this because we might want to create multiple worlds in multithreading applications
struct Block {
	int x = 0, y = 0, z = 0;
	short unsigned int blockType = 0; // Stone
	Block() = default;
	Block(int x, int y, int z) : x(x), y(y), z(z) {};
};

class Entity
{
protected:
	double locX = 0.0, locY = 0.0, locZ = 0.0;
	double motX = 0.0, motY = 0.0, motZ = 0.0;
	unsigned int amount = 1;
	unsigned int order = 0;
	unsigned int age = 0;
	bool aliveState = true;
	std::string name = "";
	std::vector<std::vector<double>> history;
public:
	double getX() const { return locX; }
	double getY() const { return locY; }
	double getZ() const { return locZ; }
	double getU() const { return motX; }
	double getV() const { return motY; }
	double getW() const { return motZ; }
	unsigned int getAmount() const { return amount; }
	unsigned int getOrder() const { return order; }
	unsigned int getAge() const { return age; }
	bool isAlive() { return aliveState; }
	std::string getName() const { return name; }

	void setX(double x) { locX = x; }
	void setY(double y) { locY = y; }
	void setZ(double z) { locZ = z; }
	void setU(double u) { motX = u; }
	void setV(double v) { motY = v; }
	void setW(double w) { motZ = w; }

	void addX(double x) { locX += x; }
	void addY(double y) { locY += y; }
	void addZ(double z) { locZ += z; }
	void addU(double u) { motX += u; }
	void addV(double v) { motY += v; }
	void addW(double w) { motZ += w; }
	void setAmount(unsigned int n) { amount = n; }
	void setOrder(unsigned int n) { order = n; }
	void setAge(unsigned int num) { age = num; }
	void kill() { aliveState = false; }
	void setName(std::string myName) { name = myName; }
private:
	bool onGround = false;
public:
	Entity() {}
	Entity(double x, double y, double z) : locX(x), locY(y), locZ(z)
	{
		history.push_back(std::vector<double>{locX, locY, locZ, motX, motY, motZ, 0});
	}
	Entity(double x, double y, double z, double vx, double vy, double vz) : locX(x), locY(y), locZ(z), motX(vx), motY(vy), motZ(vz)
	{
		history.push_back(std::vector<double>{locX, locY, locZ, motX, motY, motZ, 0});
	}
	Entity(std::string inputString)
	{
		std::stringstream ss(inputString);
		ss >> locX;
		ss >> locY;
		ss >> locZ;
		ss >> motX;
		ss >> motY;
		ss >> motZ;
		history.push_back(std::vector<double>{locX, locY, locZ, motX, motY, motZ, 0});
	}
	Entity(const Entity& other) : locX(other.locX), locY(other.locY), locZ(other.locZ), motX(other.motX), motY(other.motY), motZ(other.motZ),
		amount(other.amount), age(other.age), name(other.name), history(other.history)
	{
	}

	void printHistory()
	{
		short int cellWidthLoc = DISPLAY_LOC_DIGITS + 4; // +2 for '-' and '.'
		short int cellWidthMot = DISPLAY_MOT_DIGITS + 4;
		for (long unsigned int i = 0; i < history.size(); i++)
		{
			// Age
			std::cout << std::setw(10) << std::left << "Age " + std::to_string(i) + ":";
			// Position
			std::cout << std::setprecision(DISPLAY_LOC_DIGITS);
			std::cout << std::setw(cellWidthLoc) << std::left << history[i][0] << " " << std::setw(cellWidthLoc) << std::left << history[i][1] << " " << std::setw(cellWidthLoc) << std::left << history[i][2];
			std::cout << "   ";

			// Velocity
			std::cout << std::setprecision(DISPLAY_MOT_DIGITS);
			std::cout << std::setw(cellWidthMot) << std::left << history[i][3] << " " << std::setw(cellWidthMot) << std::left << history[i][4] << " " << std::setw(cellWidthMot) << std::left << history[i][5];
			if (history[i][6] == 1)
			{
				std::cout << " X"; // TODO: last index would probably mark what happened to it, add multiple cases
			}
			std::cout << std::endl;
		}
		std::cout << std::setprecision(STANDARD_PRECISION);
	}
	// pure virtual functions
	// virtual void getEyeHeight() = 0;
	virtual void explosionFrom(const Entity& booster, double fraction) = 0;
	virtual void swingFrom(const Entity& booster) = 0;
	virtual void freefall(unsigned int ticks, const std::vector<Block>& blocksInWorld) = 0;
	virtual void print() = 0;
};

class Tnt : public Entity
{
public:
	static const double explosion_height;
	const double halfWidth = 0.49;
	const double height = 0.98;
private:
	const double drag = 0.98, gravity = -0.04; // eye height
	unsigned int fuse = 80;
public:
	// Constructors and setters/getters
	Tnt() : Entity() {}
	Tnt(double x, double y, double z) : Entity(x, y, z) {}
	Tnt(double x, double y, double z, double vx, double vy, double vz) : Entity(x, y, z, vx, vy, vz) {}
	Tnt(std::string inputString) : Entity(inputString) {}
	Tnt(const Entity& other) : Entity(other) {}
	Tnt(const Tnt& other) : Entity(other) {} // idk if this works
	Tnt& operator=(const Tnt& other) // Assignment operator for Subclass
	{
		if (this != &other) {
			Entity::operator=(other); // Call base class assignment operator
		}
		return *this;
	}

	unsigned int getFuse() const { return fuse; };
	void setFuse(unsigned int n) { fuse = n; }

public:
	void checkCollision(const std::vector<Block>& blocksInWorld) {

		double newX = locX + motX;
		double newY = locY + motY;
		double newZ = locZ + motZ;

		// Calculate the boundaries of the TNT block's new position
		double newTntMinX = locX - halfWidth;
		double newTntMaxX = locX + halfWidth;
		double newTntMinY = locY;
		double newTntMaxY = locY + height;
		double newTntMinZ = locZ - halfWidth;
		double newTntMaxZ = locZ + halfWidth;

		// Checks along the Y direciton
		for (const Block& block : blocksInWorld) {
			// Calculate the boundaries of the block
			double blockMinX = block.x;
			double blockMaxX = block.x + 1.0;
			double blockMinY = block.y;
			double blockMaxY = block.y + 1.0;
			double blockMinZ = block.z;
			double blockMaxZ = block.z + 1.0;

			// Check along the Y direction
			if (newTntMaxX > blockMinX && newTntMinX < blockMaxX &&
				newTntMaxZ > blockMinZ && newTntMinZ < blockMaxZ) {

				if (motY > 0 && newY + height > blockMinY) {
					newY = blockMinY - height;
				}
				else if (motY < 0 && newY < blockMaxY) {
					newY = blockMaxY; // not tested
				}
			}
		} // update Y location/velocity
		if (locY + motY != newY) {
			locY = newY;
			motY = 0;
		}
		else locY = newY;

		
		// check the dirction of the triangle
		if (motX > motZ)
		{
			// Calculate the boundaries of the TNT block's new position
			double newTntMinX = locX - halfWidth;
			double newTntMaxX = locX + halfWidth;
			double newTntMinY = locY;
			double newTntMaxY = locY + height;
			double newTntMinZ = locZ - halfWidth;
			double newTntMaxZ = locZ + halfWidth;
			for (const Block& block : blocksInWorld) {
				// Calculate the boundaries of the block
				double blockMinX = block.x;
				double blockMaxX = block.x + 1.0;
				double blockMinY = block.y;
				double blockMaxY = block.y + 1.0;
				double blockMinZ = block.z;
				double blockMaxZ = block.z + 1.0;

				// Check along the Y direction
				if (newTntMaxZ > blockMinZ && newTntMinZ < blockMaxZ &&
					newTntMaxY > blockMinY && newTntMinY < blockMaxY) {

					if (motX > 0 && newX + halfWidth > blockMinX) {
						newX = blockMinX - halfWidth;

					}
					else if (motX < 0 && newX - halfWidth < blockMaxX) {
						newX = blockMaxX + halfWidth;

					}
				}
			}
		} else {

		}
		
		if (locX + motX != newX) {
			locX = newX;
			motX = 0;
		} else locX = newX;
		
		

		if (locZ + motZ != newZ) {
			locZ = newZ;
			motX = 0;
		} else locZ = newZ;
		
	}


	void explosionFrom(const Entity& booster, double fraction) override
	{
		double dx = locX - booster.getX(), dy = locY - (booster.getY() + Tnt::explosion_height), dz = locZ - booster.getZ();
		double dist = sqrt(dx * dx + dy * dy + dz * dz);
		// check range if out of range
		if (dist > 8.0 || dist == 0.0)
		{
			return;
		}
		double f = (1.0 - dist / 8.0) * fraction;
		double directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);

		motX += dx / directionMagnitude * f * booster.getAmount();
		motY += dy / directionMagnitude * f * booster.getAmount();
		motZ += dz / directionMagnitude * f * booster.getAmount();
	}
	void swingFrom(const Entity& booster) override
	{
		Tnt origin(booster);
		std::vector<std::vector<double>> locations;
		double dx, dy, dz, dist, f, directionMagnitude;
		// generate explosion locations
		for (int i = 0; i < booster.getAmount(); i++)
		{
			Tnt next = origin;
			// freefall (optimized)
			next.addV(gravity);

			// INTERSECTION
			// Probably Freefall?

			next.addX(next.getU());
			next.addY(next.getV());
			next.addZ(next.getZ());
			// initializes the location of the epxlosions
			std::vector<double> loc(3);
			loc.push_back(next.getX());
			loc.push_back(next.getY());
			loc.push_back(next.getZ());
			locations.push_back(loc);
			// optimized explosion
			dx = origin.getX() - locations[i][0], dy = origin.getY() - (locations[i][1] + Tnt::explosion_height), dz = origin.getZ() - locations[i][2];
			dist = sqrt(dx * dx + dy * dy + dz * dz);
			if (dist > 8.0 || dist == 0.0)
			{
				continue;
			}
			f = (1.0 - dist / 8.0);
			directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);
			origin.addU(dx / directionMagnitude * f);
			origin.addV(dy / directionMagnitude * f);
			origin.addW(dz / directionMagnitude * f);
		}
		// calculate explosions
		for (int i = 0; i < booster.getAmount(); i++)
		{
			// cout << locations[i][1] << ",";
			// explosion (optimized)
			dx = locX - locations[i][0], dy = locY - (locations[i][1] + Tnt::explosion_height), dz = locZ - locations[i][2];
			dist = sqrt(dx * dx + dy * dy + dz * dz);
			if (dist > 8.0 || dist == 0.0)
			{
				continue;
			}
			f = (1.0 - dist / 8.0);
			directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);
			motX += dx / directionMagnitude * f;
			motY += dy / directionMagnitude * f;
			motZ += dz / directionMagnitude * f;
		}
	}
	void freefall(unsigned int ticks, const std::vector<Block>& blocksInWorld) override
	{
		for (unsigned int i = 1; i <= ticks; i++) {
			motY += gravity;

			// Position update with obstacle check
			checkCollision(blocksInWorld);

			motX *= drag;
			motY *= drag;
			motZ *= drag;
			age++;
			fuse--;
			history.push_back(std::vector<double>{locX, locY, locZ, motX, motY, motZ, 0});
		}
	}
	void print() override
	{
		if (name != "")
			std::cout << name << std::endl;
		std::cout << "LOC: " << locX << " " << locY << " " << locZ;
		std::cout << "\tMOT: " << motX << " " << motY << " " << motZ << std::endl;
	}
};

const double Tnt::explosion_height = 0.061250001192092896; // something is off
/*
class FallingEntity : public Entity
{

private:
	const double drag = 0.98, gravity = -0.04; // eye height
public:
	FallingEntity() : Entity() {}
	FallingEntity(double x, double y, double z) : Entity(x, y, z) {}
	FallingEntity(double x, double y, double z, double vx, double vy, double vz) : Entity(x, y, z, vx, vy, vz) {}
	FallingEntity(std::string inputString) : Entity(inputString) {}
	FallingEntity(const Entity& other) : Entity(other) {}
	FallingEntity(const FallingEntity& other) : Entity(other) {} // idk if this works
	FallingEntity& operator=(const FallingEntity& other) // Assignment operator for Subclass
	{
		if (this != &other) {
			Entity::operator=(other); // Call base class assignment operator
		}
		return *this;
	}
	void explosionFrom(const Entity& booster, double fraction) override
	{
		double dx = locX - booster.getX(), dy = locY - (booster.getY() + Tnt::explosion_height), dz = locZ - booster.getZ();
		double dist = sqrt(dx * dx + dy * dy + dz * dz);
		// check range if out of range
		if (dist > 8.0 || dist == 0.0)
		{
			return;
		}
		double f = (1.0 - dist / 8.0) * fraction;
		double directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);

		motX += dx / directionMagnitude * f * booster.getAmount();
		motY += dy / directionMagnitude * f * booster.getAmount();
		motZ += dz / directionMagnitude * f * booster.getAmount();
	}
	void swingFrom(const Entity& booster) override
	{
		Tnt origin(booster);
		std::vector<std::vector<double>> locations;
		double dx, dy, dz, dist, f, directionMagnitude;
		// generate explosion locations
		for (int i = 0; i < booster.getAmount(); i++)
		{
			Tnt next = origin;
			// freefall (optimized)
			next.addV(gravity);

			// INTERSECTION
			// Probably Freefall?

			next.addX(next.getU());
			next.addY(next.getV());
			next.addZ(next.getZ());
			// initializes the location of the epxlosions
			std::vector<double> loc(3);
			loc.push_back(next.getX());
			loc.push_back(next.getY());
			loc.push_back(next.getZ());
			locations.push_back(loc);
			// optimized explosion
			dx = origin.getX() - locations[i][0], dy = origin.getY() - (locations[i][1] + Tnt::explosion_height), dz = origin.getZ() - locations[i][2];
			dist = sqrt(dx * dx + dy * dy + dz * dz);
			if (dist > 8.0 || dist == 0.0)
			{
				continue;
			}
			f = (1.0 - dist / 8.0);
			directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);
			origin.addU(dx / directionMagnitude * f);
			origin.addV(dy / directionMagnitude * f);
			origin.addW(dz / directionMagnitude * f);
		}
		// calculate explosions
		for (int i = 0; i < booster.getAmount(); i++)
		{
			// cout << locations[i][1] << ",";
			// explosion (optimized)
			dx = locX - locations[i][0], dy = locY - (locations[i][1] + Tnt::explosion_height), dz = locZ - locations[i][2];
			dist = sqrt(dx * dx + dy * dy + dz * dz);
			if (dist > 8.0 || dist == 0.0)
			{
				continue;
			}
			f = (1.0 - dist / 8.0);
			directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);
			motX += dx / directionMagnitude * f;
			motY += dy / directionMagnitude * f;
			motZ += dz / directionMagnitude * f;
		}
	}
	void freefall(unsigned int ticks, std::vector<Block>& blocksInWorld) override
	{
		for (int i = 1; i <= ticks; i++)
		{
			motY += gravity;

			//	position update
			locY += motY; // add a check before updating position to check if any obstacles exist along the Y direction
			if (motX > motZ) {
				locX += motX; // add a check before updating position to check if any obstacles exist along the X direction
				locZ += motZ; // add a check before updating position to check if any obstacles exist along the Z direction
			}
			else {
				locZ += motZ; // add a check before updating position to check if any obstacles exist along the Z direction
				locX += motX; // add a check before updating position to check if any obstacles exist along the X direction
			}

			motX *= drag;
			motY *= drag;
			motZ *= drag;
			age++;
			history.push_back(std::vector<double>{locX, locY, locZ, motX, motY, motZ, 0});
		}
	}
	void print() override
	{
		if (name != "")
			std::cout << name << std::endl;
		std::cout << "LOC: " << locX << " " << locY << " " << locZ;
		std::cout << "\tMOT: " << motX << " " << motY << " " << motZ << std::endl;
	}
};
*/
// Comparator function for sorting entities by order
bool compareEntities(Entity* a, Entity* b) {
	return a->getOrder() < b->getOrder();
}
class World {
public:
	// Default constructor
	World() = default;

	// Copy constructor
	World(const World& other) : blocksInWorld(other.blocksInWorld) {}

	// Assignment operator
	World& operator=(const World& other) {
		if (this != &other)  // Self-assignment check
			blocksInWorld = other.blocksInWorld;
		return *this;
	}
private:
	unsigned int worldTick = 0;
	std::vector<Block> blocksInWorld;
	std::vector<Block> backupBlocksInWorld;

	unsigned int backupWorldTick = 0;
	std::vector<Entity*> entitiesInWorld;
	std::vector<Entity*> backupEntitiesInWorld;

public:
	// Blocks
	void addBlocks(Block blockToAdd) {
		blocksInWorld.push_back(blockToAdd);
	};
	void addBlocks(std::vector<Block> blocksToAdd) {
		for (Block block : blocksToAdd)
			blocksInWorld.push_back(block);
	};

	// Entities
	void addEntity(Entity* entity) {
		entitiesInWorld.push_back(entity);
		std::sort(entitiesInWorld.begin(), entitiesInWorld.end(), compareEntities);
	}

	void addEntities(std::vector<Entity*> entitiesToAdd) {
		for (auto entity : entitiesToAdd)
			entitiesInWorld.push_back(entity);
		std::sort(entitiesInWorld.begin(), entitiesInWorld.end(), compareEntities);
	}

	void run(unsigned int ticks) {
		if (ticks == 0)
			return;
		/*
		* Minecraft normally computes in each gametick
		*
		* while(true) {
		*    Freefall
		*    Explosions
		* }
		*
		* This code will perform:
		*
		* Explosions
		* while(true) {
		*    Freefall
		*    Explosions
		* }
		*/

		//	Explosions
		for (Entity* entity : entitiesInWorld) {
			// Use dynamic_cast to check if entity is a Tnt
			Tnt* tntEntity = dynamic_cast<Tnt*>(entity);

			// If the cast is successful, check the fuse value
			if (tntEntity && tntEntity->isAlive() && tntEntity->getFuse() == 0) {
				std::cout << "Tnt entity with fuse 0 found" << std::endl;

				// Loop through all entities except the current one
				for (Entity* otherEntity : entitiesInWorld) {
					if (otherEntity != tntEntity) {
						// Performs explosions functions on other entities
						otherEntity->explosionFrom(*tntEntity, air);
					}
				}
				tntEntity->kill();
			}
		}
		for (int i = 0; i < ticks; i++) {
			//	Freefall
			for (Entity* entity : entitiesInWorld) {
				if (entity->isAlive()) {
					entity->freefall(1, blocksInWorld);
				}
			}


			/*
			//	Explosions
			for (Entity* entity : entitiesInWorld) {
				// Use dynamic_cast to check if entity is a Tnt
				Tnt* tntEntity = dynamic_cast<Tnt*>(entity);
				if (tntEntity) {
					// If the cast is successful, check the fuse value
					if (tntEntity->getFuse() == 0) {
						std::cout << "Tnt entity with fuse 0 found" << std::endl;

						// Loop through all entities except the current one
						for (Entity* otherEntity : entitiesInWorld) {
							if (otherEntity != tntEntity) {
								// Performs explosions functions on other entities
								otherEntity->explosionFrom(*tntEntity, air);
							}
						}
					}
				}
			}*/
		}
	}

	// Save States
	void backup() {
		for (Block block : blocksInWorld) {
			backupBlocksInWorld.push_back(block);
		}
		for (Entity* entity : entitiesInWorld) {
			backupEntitiesInWorld.push_back(entity);
		}
		backupWorldTick = worldTick;
	}
	void clear() {
		blocksInWorld.clear();
		entitiesInWorld.clear();
	}
	void reload() {
		blocksInWorld = backupBlocksInWorld;
	}

	// Print/Debug Functions
	void printInformation() {
		std::cout << "  World information: " << std::endl;
		std::cout << "\tWorld Tick: " << worldTick << std::endl;
		std::cout << "  Block information: " << std::endl;
		for (Block block : blocksInWorld)
			std::cout << "\t" << block.x << ", " << block.y << ", " << block.z << std::endl;
		std::cout << "  Entity information: " << std::endl;
		for (Entity* entity : entitiesInWorld) {
			std::cout << "\t" << entity->getX() << ", " << entity->getY() << ", " << entity->getZ() << "\t"
				<< entity->getU() << ", " << entity->getV() << ", " << entity->getW() << std::endl;
		}
	}

};
#endif // ENTITY_H