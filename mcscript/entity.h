#ifndef ENTITY_H
#define ENTITY_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

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
#define DISPLAY_MOT_DIGITS 5 // 17 normal

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
    double getX() const;
    double getY() const;
    double getZ() const;
    double getU() const;
    double getV() const;
    double getW() const;
    unsigned int getAmount() const;
    unsigned int getOrder() const;
    unsigned int getAge() const;
    bool isAlive();
    std::string getName() const;

    void setX(double x);
    void setY(double y);
    void setZ(double z);
    void setU(double u);
    void setV(double v);
    void setW(double w);

    void addX(double x);
    void addY(double y);
    void addZ(double z);
    void addU(double u);
    void addV(double v);
    void addW(double w);
    void setAmount(unsigned int n);
    void setOrder(unsigned int n);
    void setAge(unsigned int num);
    void kill();
    void setName(std::string myName);
private:
    bool onGround = false;
public:
    Entity();
    Entity(double x, double y, double z);
    Entity(double x, double y, double z, double vx, double vy, double vz);
    Entity(std::string inputString); // To create the entity from a copy-paste of text
    Entity(const Entity& other); // create a shallow copy of another entity
    virtual ~Entity() = default;

    void printHistory();
    // Abstract class
    virtual void checkCollision(const std::vector<Block>& blocksInWorld) = 0;
    virtual void explosionFrom(const Entity& booster, double exposureFraction, const std::vector<Block>& blocksInWorld) = 0;
    virtual void explosion(const Entity& booster, double fraction, const std::vector<Block>& blocksInWorld) = 0;
    virtual void swing(const Entity& booster, const std::vector<Block>& blocksInWorld) = 0;
    virtual void freefall(unsigned int ticks, const std::vector<Block>& blocksInWorld) = 0;
    virtual void print() = 0;
};

class Tnt : public Entity
{
public:
    static const double explosion_height;
    const double halfWidth = 0.98f / 2.0;
    const double height = 0.98f;
private:
    const double drag = 0.98, gravity = -0.04; // eye height
    unsigned int fuse = 80;
public:
    Tnt();
    Tnt(double x, double y, double z);
    Tnt(double x, double y, double z, double vx, double vy, double vz);
    Tnt(std::string inputString);
    Tnt(const Entity& other);
    Tnt(const Tnt& other);
    Tnt& operator=(const Tnt& other);

    unsigned int getFuse() const;
    void setFuse(unsigned int n);

    void checkCollision(const std::vector<Block>& blocksInWorld);
    void explosionFrom(const Entity& booster, double exposureFraction, const std::vector<Block>& blocksInWorld) override;
    void explosion(const Entity& booster, double exposureFraction, const std::vector<Block>& blocksInWorld) override;
    void swing(const Entity& booster, const std::vector<Block>& blocksInWorld) override;
    void freefall(unsigned int ticks, const std::vector<Block>& blocksInWorld) override;
    void print() override;
};
class FallingBlock : public Entity
{
public:
    const double halfWidth = 0.98f / 2.0;
    const double height = 0.98f;
private:
    const double drag = 0.98, gravity = -0.04;
public:
    FallingBlock();
    FallingBlock(double x, double y, double z);
    FallingBlock(double x, double y, double z, double vx, double vy, double vz);
    FallingBlock(std::string inputString);
    FallingBlock(const Entity& other);
    FallingBlock(const FallingBlock& other);
    FallingBlock& operator=(const FallingBlock& other);

    void checkCollision(const std::vector<Block>& blocksInWorld);
    void explosionFrom(const Entity& booster, double exposureFraction, const std::vector<Block>& blocksInWorld) override;
    void explosion(const Entity& booster, double exposureFraction, const std::vector<Block>& blocksInWorld) override;
    void swing(const Entity& booster, const std::vector<Block>& blocksInWorld) override;
    void freefall(unsigned int ticks, const std::vector<Block>& blocksInWorld) override;
    void print() override;
};

/*
*   World class is meant to simulate the world with blocks and entities runnin at the same time
*/
class World {
public:
    // Default constructor
    World() = default;

    // Copy constructor
    World(const World& other);

    // Assignment operator
    World& operator=(const World& other);
private:
    // Active:
    unsigned int worldTick = 0;
    std::vector<Block> blocksInWorld;
    std::vector<Entity*> entitiesInWorld; // is the adress of the
    std::vector<Tnt> tntsInWorld; // is a copy to f with

    // Backups:
    unsigned int backupWorldTick = 0;
    std::vector<Block> backupBlocksInWorld; 
    std::vector<Entity*> backupEntitiesInWorld;
public:
    // Blocks
    void addBlock(Block blockToAdd);
    void addBlocks(const std::vector<Block> blocksToAdd);

    // Entities
    void addEntity(Entity* entity);
    void addEntities(const std::vector<Entity*>& entitiesToAdd);
    /*
    * They should only write to the actives (Tnts InWorld, entitiesInWorld) and create new instances
    */

    void run(unsigned int ticks);

    // Save States
    void backup();
    /* Backup should
    * 1. clear the backups
    * 2. read from the actives and store into backups
    */
    void clear();
    void reload();
    /*
    * 1. clear the active ones
    * 2. It should read from backups
    * 3. It shoudl load into the actives
    */

    // Print/Debug Functions
    void printInformation();  

    // Prints out the current Blocks in the world
    void printBlocksInWorld();
    ~World();
};
#endif // ENTITY_H
