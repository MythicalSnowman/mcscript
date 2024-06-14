#include "entity.h"

const double Tnt::explosion_height = 0.061250001192092896; // something might be off

double Entity::getX() const { return locX; }
double Entity::getY() const { return locY; }
double Entity::getZ() const { return locZ; }
double Entity::getU() const { return motX; }
double Entity::getV() const { return motY; }
double Entity::getW() const { return motZ; }
unsigned int Entity::getAmount() const { return amount; }
unsigned int Entity::getOrder() const { return order; }
unsigned int Entity::getAge() const { return age; }
bool Entity::isAlive() { return aliveState; }
std::string Entity::getName() const { return name; }

void Entity::setX(double x) { locX = x; }
void Entity::setY(double y) { locY = y; }
void Entity::setZ(double z) { locZ = z; }
void Entity::setU(double u) { motX = u; }
void Entity::setV(double v) { motY = v; }
void Entity::setW(double w) { motZ = w; }

void Entity::addX(double x) { locX += x; }
void Entity::addY(double y) { locY += y; }
void Entity::addZ(double z) { locZ += z; }
void Entity::addU(double u) { motX += u; }
void Entity::addV(double v) { motY += v; }
void Entity::addW(double w) { motZ += w; }
void Entity::setAmount(unsigned int n) { amount = n; }
void Entity::setOrder(unsigned int n) { order = n; }
void Entity::setAge(unsigned int num) { age = num; }
void Entity::kill() { aliveState = false; }
void Entity::setName(std::string myName) { name = std::move(myName); }

Entity::Entity() {
	history.push_back(std::vector<double>{locX, locY, locZ, motX, motY, motZ, 0}); // this should be empty
};
Entity::Entity(double x, double y, double z) : locX(x), locY(y), locZ(z) {
	history.push_back(std::vector<double>{locX, locY, locZ, motX, motY, motZ, 0});
}
Entity::Entity(double x, double y, double z, double vx, double vy, double vz)
    : locX(x), locY(y), locZ(z), motX(vx), motY(vy), motZ(vz) {
	history.push_back(std::vector<double>{locX, locY, locZ, motX, motY, motZ, 0});
}
Entity::Entity(std::string inputString) {
	std::istringstream(inputString) >> locX >> locY >> locZ >> motX >> motY >> motZ;
	history.push_back({ locX, locY, locZ, motX, motY, motZ, 0 });
}
Entity::Entity(const Entity& other)
    : locX(other.locX), locY(other.locY), locZ(other.locZ),
    motX(other.motX), motY(other.motY), motZ(other.motZ),
    amount(other.amount), order(other.order), age(other.age),
    aliveState(other.aliveState), name(other.name), history(other.history) {}

void Entity::printHistory() {
	short int cellWidthLoc = DISPLAY_LOC_DIGITS + 2; // +2 for '-' and '.'
	short int cellWidthMot = DISPLAY_MOT_DIGITS + 2;
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
		switch (static_cast<int>(history[i][6])) {
		case 0:
			break;
		case 1:
			std::cout << " X";
			break;
		case 2:
			std::cout << " S";
			break;
		default:
			break;
		}
		std::cout << std::endl;
	}
	std::cout << std::setprecision(STANDARD_PRECISION);
}

/*
* This is the Tnt class derived from the Entity Class
*/
Tnt::Tnt() : Entity() {}
Tnt::Tnt(double x, double y, double z) : Entity(x, y, z) {}
Tnt::Tnt(double x, double y, double z, double vx, double vy, double vz)
    : Entity(x, y, z, vx, vy, vz) {}
Tnt::Tnt(std::string inputString) : Entity(inputString) {}
Tnt::Tnt(const Entity& other) : Entity(other) {}
Tnt::Tnt(const Tnt& other) : Entity(other), fuse(other.fuse) {}
Tnt& Tnt::operator=(const Tnt& other) {
    if (this == &other) return *this;
    Entity::operator=(other);
    fuse = other.fuse;
    return *this;
}

unsigned int Tnt::getFuse() const { return fuse; }
void Tnt::setFuse(unsigned int n) { fuse = n; }

//	These are the implementations of the abstract functions
void Tnt::checkCollision(const std::vector<Block>& blocksInWorld) {
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
	double blockMinX;
	double blockMaxX;
	double blockMinY;
	double blockMaxY;
	double blockMinZ;
	double blockMaxZ;

	// Checks along the Y direciton
	for (const Block& block : blocksInWorld) {
		// Calculate the boundaries of the block
		blockMinX = block.x;
		blockMaxX = block.x + 1.0;
		blockMinY = block.y;
		blockMaxY = block.y + 1.0;
		blockMinZ = block.z;
		blockMaxZ = block.z + 1.0;

		// Check along the Y direction
		if (newTntMaxX > blockMinX && newTntMinX < blockMaxX &&
			newTntMaxZ > blockMinZ && newTntMinZ < blockMaxZ) {

			if (motY > 0 && newY + height > blockMinY && locY <= blockMinY) { // check inclusive
				newY = blockMinY - height;
			}
			else if (motY < 0 && newY < blockMaxY && locY >= blockMaxY) {
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
	if (abs(motX) > abs(motZ))
	{
		// Calculate the boundaries of the TNT block's new position
		newTntMinX = locX - halfWidth;
		newTntMaxX = locX + halfWidth;
		newTntMinY = locY;
		newTntMaxY = locY + height;
		newTntMinZ = locZ - halfWidth;
		newTntMaxZ = locZ + halfWidth;
		for (const Block& block : blocksInWorld) {
			// Calculate the boundaries of the block
			blockMinX = block.x;
			blockMaxX = block.x + 1.0;
			blockMinY = block.y;
			blockMaxY = block.y + 1.0;
			blockMinZ = block.z;
			blockMaxZ = block.z + 1.0;
			// Check along the X direction
			if (newTntMaxZ > blockMinZ && newTntMinZ < blockMaxZ &&
				newTntMaxY > blockMinY && newTntMinY < blockMaxY) {
				if (motX > 0 && newX + halfWidth > blockMinX && locX <= blockMinX - halfWidth)
					newX = blockMinX - halfWidth;
				else if (motX < 0 && newX - halfWidth < blockMaxX && locX >= blockMaxX + halfWidth)
					newX = blockMaxX + halfWidth;	// untested
			}
		}
		if (locX + motX != newX) {
			locX = newX;
			motX = 0;
		}
		else locX = newX;

		// Calculate the boundaries of the TNT block's new position
		newTntMinX = locX - halfWidth;
		newTntMaxX = locX + halfWidth;
		newTntMinY = locY;
		newTntMaxY = locY + height;
		newTntMinZ = locZ - halfWidth;
		newTntMaxZ = locZ + halfWidth;
		for (const Block& block : blocksInWorld) {
			// Calculate the boundaries of the block
			blockMinX = block.x;
			blockMaxX = block.x + 1.0;
			blockMinY = block.y;
			blockMaxY = block.y + 1.0;
			blockMinZ = block.z;
			blockMaxZ = block.z + 1.0;

			// Check along the Y direction
			if (newTntMaxX > blockMinX && newTntMinX < blockMaxX &&
				newTntMaxY > blockMinY && newTntMinY < blockMaxY) {
				if (motZ > 0 && newZ + halfWidth > blockMinZ && locZ <= blockMinZ - halfWidth)
					newZ = blockMinZ - halfWidth;
				else if (motZ < 0 && newZ - halfWidth < blockMaxZ && locZ >= blockMaxZ + halfWidth)
					newZ = blockMaxZ + halfWidth;	// untested
			}
		}
		if (locZ + motZ != newZ) {
			locZ = newZ;
			motZ = 0;
		}
		else locZ = newZ;
	}
	else {
		// Calculate the boundaries of the TNT block's new position
		newTntMinX = locX - halfWidth;
		newTntMaxX = locX + halfWidth;
		newTntMinY = locY;
		newTntMaxY = locY + height;
		newTntMinZ = locZ - halfWidth;
		newTntMaxZ = locZ + halfWidth;
		for (const Block& block : blocksInWorld) {
			// Calculate the boundaries of the block
			blockMinX = block.x;
			blockMaxX = block.x + 1.0;
			blockMinY = block.y;
			blockMaxY = block.y + 1.0;
			blockMinZ = block.z;
			blockMaxZ = block.z + 1.0;

			// Check along the Z direction
			if (newTntMaxX > blockMinX && newTntMinX < blockMaxX &&
				newTntMaxY > blockMinY && newTntMinY < blockMaxY) {
				if (motZ > 0 && newZ + halfWidth > blockMinZ && locZ <= blockMinZ - halfWidth)
					newZ = blockMinZ - halfWidth;
				else if (motZ < 0 && newZ - halfWidth < blockMaxZ && locZ >= blockMaxZ + halfWidth)
					newZ = blockMaxZ + halfWidth;	// untested
			}
		}
		if (locZ + motZ != newZ) {
			locZ = newZ;
			motZ = 0;
		}
		else locZ = newZ;

		// Calculate the boundaries of the TNT block's new position
		newTntMinX = locX - halfWidth;
		newTntMaxX = locX + halfWidth;
		newTntMinY = locY;
		newTntMaxY = locY + height;
		newTntMinZ = locZ - halfWidth;
		newTntMaxZ = locZ + halfWidth;
		for (const Block& block : blocksInWorld) {
			// Calculate the boundaries of the block
			blockMinX = block.x;
			blockMaxX = block.x + 1.0;
			blockMinY = block.y;
			blockMaxY = block.y + 1.0;
			blockMinZ = block.z;
			blockMaxZ = block.z + 1.0;

			// Check along the X direction
			if (newTntMaxZ > blockMinZ && newTntMinZ < blockMaxZ &&
				newTntMaxY > blockMinY && newTntMinY < blockMaxY) {
				if (motX > 0 && newX + halfWidth > blockMinX && locX <= blockMinX - halfWidth)
					newX = blockMinX - halfWidth;
				else if (motX < 0 && newX - halfWidth < blockMaxX && locX >= blockMaxX + halfWidth)
					newX = blockMaxX + halfWidth;	// untested
			}
		}
		if (locX + motX != newX) {
			locX = newX;
			motX = 0;
		}
		else locX = newX;

	}
}

void Tnt::explosionFrom(const Entity& booster, double exposureFraction, const std::vector<Block>& blocksInWorld) {
	// Check which case of explosion is needed, bercause one is faster than the other
	// Might need to specify gravity check
	if (booster.getU() * booster.getU() + booster.getV() * booster.getV() + booster.getW() * booster.getW() > 64 || booster.getU() * booster.getU() + booster.getV() * booster.getV() + booster.getW() * booster.getW() == 0) {
		// if no swing
		explosion(booster, exposureFraction, blocksInWorld);
	}
	else {
		swing(booster, blocksInWorld);
	}
	
}

void Tnt::explosion(const Entity& booster, double exposureFraction, const std::vector<Block>& blocksInWorld) {
    // Implementation of explosion
	double dx = locX - booster.getX(), dy = locY - (booster.getY() + Tnt::explosion_height), dz = locZ - booster.getZ();
	double dist = sqrt(dx * dx + dy * dy + dz * dz);
	// check range if out of range
	if (dist > 8.0 || dist == 0.0)
	{
		return;
	}
	double f = (1.0 - dist / 8.0) * exposureFraction;
	double directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);

	motX += dx / directionMagnitude * f * booster.getAmount();
	motY += dy / directionMagnitude * f * booster.getAmount();
	motZ += dz / directionMagnitude * f * booster.getAmount();
	history[history.size() - 1][6] = 1;
}

void Tnt::swing(const Entity& booster, const std::vector<Block>& blocksInWorld) {
	Tnt origin(booster);
	std::vector<std::vector<double>> locations;
	double dx, dy, dz, dist, f, directionMagnitude;
	double wasSwung = false;
	// generate explosion locations
	for (int i = 0; i < booster.getAmount(); i++)
	{
		Tnt next = origin;
		next.freefall(1, blocksInWorld);
		// initializes the location of the epxlosions
		std::vector<double> loc(3);
		loc[0] = next.getX();
		loc[1] = next.getY();
		loc[2] = next.getZ();
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
		wasSwung = true;
	}
	// calculate explosions
	for (int i = 0; i < booster.getAmount(); i++)
	{
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
	history[history.size() - 1][6] = 2;
}

void Tnt::freefall(unsigned int ticks, const std::vector<Block>& blocksInWorld) {
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

void Tnt::print() {
    // Print implementation
}


/*
*	This is where the World Class definitions start
*/
// Comparator function for sorting entities by order
bool compareEntities(Entity* a, Entity* b) {
	return a->getOrder() < b->getOrder();
}

// Copy constructor
World::World(const World& other) : blocksInWorld(other.blocksInWorld) {}

// Assignment operator
World& World::operator=(const World& other) {
	if (this != &other) { // Self-assignment check
		blocksInWorld = other.blocksInWorld;
	}
	return *this;
}

// Blocks
void World::addBlock(Block blockToAdd) {
	blocksInWorld.push_back(blockToAdd);
}

void World::addBlocks(const std::vector<Block> blocksToAdd) {
	for (const Block block : blocksToAdd) {
		blocksInWorld.push_back(block);
	}
}

// Entities
void World::addEntity(Entity* entity) {
	entitiesInWorld.push_back(entity);
	std::sort(entitiesInWorld.begin(), entitiesInWorld.end(), compareEntities);
}

void World::addEntities(const std::vector<Entity*>& entitiesToAdd) {
	for (Entity* entity : entitiesToAdd) {
		entitiesInWorld.push_back(entity);
	}
	std::sort(entitiesInWorld.begin(), entitiesInWorld.end(), compareEntities);
}

void World::run(unsigned int ticks) {
	if (ticks == 0) {
		return;
	}

	// Explosions
	for (Entity* entity : entitiesInWorld) {
		Tnt* tntEntity = dynamic_cast<Tnt*>(entity);
		if (tntEntity && tntEntity->isAlive() && tntEntity->getFuse() == 0) {
			std::cout << "Tnt entity with fuse 0 found" << std::endl;
			for (Entity* otherEntity : entitiesInWorld) {
				if (otherEntity != tntEntity) {
					// otherEntity->explosionFrom(*tntEntity, air); TODO: FIXME
				}
			}
			tntEntity->kill();
		}
	}

	for (unsigned int i = 0; i < ticks; ++i) {
		// Freefall
		for (Entity* entity : entitiesInWorld) {
			if (entity->isAlive()) {
				entity->freefall(1, blocksInWorld);
			}
		}

		// Explosions
		for (Entity* entity : entitiesInWorld) {
			Tnt* tntEntity = dynamic_cast<Tnt*>(entity);
			if (tntEntity && tntEntity->isAlive() && tntEntity->getFuse() == 0) {
				std::cout << "Tnt entity with fuse 0 found" << std::endl;
				for (Entity* otherEntity : entitiesInWorld) {
					if (otherEntity != tntEntity) {
						// otherEntity->swingFrom(*tntEntity, blocksInWorld); TODO FIXME
					}
				}
				tntEntity->kill();
			}
		}
	}
}

// Save States
void World::backup() {
	for (const Block& block : blocksInWorld) {
		backupBlocksInWorld.push_back(block);
	}
	for (Entity* entity : entitiesInWorld) {
		backupEntitiesInWorld.push_back(entity);
	}
	backupWorldTick = worldTick;
}

void World::clear() {
	blocksInWorld.clear();
	entitiesInWorld.clear();
}

void World::reload() {
	blocksInWorld = backupBlocksInWorld;
}

// Print/Debug Functions
void World::printInformation() {
	std::cout << "  World information: " << std::endl;
	std::cout << "\tWorld Tick: " << worldTick << std::endl;
	std::cout << "  Block information: " << std::endl;
	for (const Block& block : blocksInWorld) {
		std::cout << "\t" << block.x << ", " << block.y << ", " << block.z << std::endl;
	}
	std::cout << "  Entity information: " << std::endl;
	for (Entity* entity : entitiesInWorld) {
		if (!entity->getName().empty()) {
			std::cout << entity->getName();
		}
		std::cout << "\tAlive?: " << entity->isAlive() << std::endl;
		std::cout << "\t" << entity->getX() << ", " << entity->getY() << ", " << entity->getZ() << "\t"
			<< entity->getU() << ", " << entity->getV() << ", " << entity->getW() << std::endl;
	}
}