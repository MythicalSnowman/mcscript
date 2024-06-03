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
#define explosion_height 0.061250001192092896 //.06125000119210 0.98F*0.0625D 0.061250001192092896 //0.06125f
// 0.98F * 0.85D? // experiment: 0.8330000638962
// 302.1387850601281
#define add "add"
#define multiply "multiply"
#define STANDARD_PRECISION 17 // 17 normal
#define DISPLAY_LOC_DIGITS 17 // 17 normal
#define DISPLAY_MOT_DIGITS 17
#define log(x) cout << x << endl;

using namespace std;

class Entity
{
public:
    double locX = 0.0, locY = 0.0, locZ = 0.0;
    double motX = 0.0, motY = 0.0, motZ = 0.0;
    unsigned int amount = 1;
    unsigned int age = 0;
    string name = "";
    vector<vector<double>> history;
    vector<vector<int>> blocksInWorld;

private:
    bool onGround = false;

public:
    Entity()
    {
    }
    Entity(double x, double y, double z) : locX(x), locY(y), locZ(z)
    {
        history.push_back(vector<double>{locX, locY, locZ, motX, motY, motZ, 0});
    }
    Entity(double x, double y, double z, double vx, double vy, double vz) : locX(x), locY(y), locZ(z), motX(vx), motY(vy), motZ(vz)
    {
        history.push_back(vector<double>{locX, locY, locZ, motX, motY, motZ, 0});
    }
    Entity(string inputString)
    {
        stringstream ss(inputString);
        ss >> locX;
        ss >> locY;
        ss >> locZ;
        ss >> motX;
        ss >> motY;
        ss >> motZ;
        history.push_back(vector<double>{locX, locY, locZ, motX, motY, motZ, 0});
    }
    Entity(const Entity& other) : locX(other.locX), locY(other.locY), locZ(other.locZ), motX(other.motX), motY(other.motY), motZ(other.motZ),
        amount(other.amount), age(other.age), name(other.name), history(other.history)
    {
        blocksInWorld = other.blocksInWorld; // Same as adding in the header
    }

    void printHistory()
    {
        short int cellWidthLoc = DISPLAY_LOC_DIGITS + 4; // +2 for '-' and '.'
        short int cellWidthMot = DISPLAY_MOT_DIGITS + 4;
        for (long unsigned int i = 0; i < history.size(); i++)
        {
            // Age
            cout << setw(10) << left << "Age " + to_string(i) + ":";
            // Position
            cout << setprecision(DISPLAY_LOC_DIGITS);
            cout << setw(cellWidthLoc) << left << history[i][0] << " " << setw(cellWidthLoc) << left << history[i][1] << " " << setw(cellWidthLoc) << left << history[i][2];
            cout << "   ";

            // Velocity
            cout << setprecision(DISPLAY_MOT_DIGITS);
            cout << setw(cellWidthMot) << left << history[i][3] << " " << setw(cellWidthMot) << left << history[i][4] << " " << setw(cellWidthMot) << left << history[i][5];
            if (history[i][6] == 1)
            {
                cout << " X";
            }
            cout << endl;
        }
        cout << setprecision(STANDARD_PRECISION);
    }

    // pure virtual functions
    virtual void explosionFrom(const Entity& booster, double fraction) = 0;
    virtual void swingFrom(const Entity& booster) = 0;
    virtual void freefall(int ticks, bool TF) = 0;
    virtual void print() = 0;
};
class Tnt : public Entity
{
public:
    const double drag = 0.98, gravity = -0.04;
    Tnt() : Entity() {}
    Tnt(double x, double y, double z) : Entity(x, y, z) {}
    Tnt(double x, double y, double z, double vx, double vy, double vz) : Entity(x, y, z, vx, vy, vz) {}
    Tnt(string inputString) : Entity(inputString) {}
    Tnt(const Entity& other) : Entity(other) {}
    Tnt(const Tnt& other) : Entity(other) {} // idk if this works
    Tnt& operator=(const Tnt& other) // Assignment operator for Subclass
    {
        if (this != &other) {
            Entity::operator=(other); // Call base class assignment operator
        }
        return *this;
    }
    void explosionFrom(const Entity& booster, double fraction) override
    {
        double dx = locX - booster.locX, dy = locY - (booster.locY + explosion_height), dz = locZ - booster.locZ;
        double dist = sqrt(dx * dx + dy * dy + dz * dz);

        if (dist > 8.0 || dist == 0.0)
        {
            return;
        }

        double f = (1.0 - dist / 8.0) * fraction;
        double directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);

        motX += dx / directionMagnitude * f * booster.amount;
        motY += dy / directionMagnitude * f * booster.amount;
        motZ += dz / directionMagnitude * f * booster.amount;
    }
    void swingFrom(const Entity& booster) override
    {
        Tnt origin(booster);
        vector<vector<double>> locations;
        double dx, dy, dz, dist, f, directionMagnitude;
        // generate explosion locations
        for (int i = 0; i < booster.amount; i++)
        {
            Tnt next = origin;
            next.amount = 1;
            // freefall (optimized)
            next.motY += gravity;

            for (int blocks = 0; blocks < blocksInWorld.size(); blocks++)
            {
                if (next.locY + 0.98 + next.motY > blocksInWorld[blocks][1])
                {

                    // cout << "Hit!" << endl;
                    //  doenst check for multiple blocks
                    next.locY = blocksInWorld[blocks][1] - 0.98;
                    next.motY = 0;
                }
            }
            next.locY += next.motY;
            next.locX += next.motX;
            next.locZ += next.motZ;
            vector<double> loc;
            loc.push_back(next.locX);
            loc.push_back(next.locY);
            loc.push_back(next.locZ);
            locations.push_back(loc);
            // optimized explosion
            dx = origin.locX - locations[i][0], dy = origin.locY - (locations[i][1] + explosion_height), dz = origin.locZ - locations[i][2];
            dist = sqrt(dx * dx + dy * dy + dz * dz);
            if (dist > 8.0 || dist == 0.0)
            {
                continue;
            }
            f = (1.0 - dist / 8.0);
            directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);
            origin.motX += dx / directionMagnitude * f;
            origin.motY += dy / directionMagnitude * f;
            origin.motZ += dz / directionMagnitude * f;
        }
        // calculate explosions
        for (int i = 0; i < booster.amount; i++)
        {
            // cout << locations[i][1] << ",";
            //   explosion (optimized)
            dx = locX - locations[i][0], dy = locY - (locations[i][1] + explosion_height), dz = locZ - locations[i][2];
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
    void freefall(int ticks, bool TF) override
    {
        if (ticks > 0)
        {
            if (TF == 1)
            {
                short int cellWidthLoc = DISPLAY_LOC_DIGITS + 4; // +2 for '-' and '.'
                short int cellWidthMot = DISPLAY_MOT_DIGITS + 4;
                // Header
                cout << "\t(tnt freefall printout)" << endl;
                cout << "\t" << setw(static_cast<int>(cellWidthLoc * 1.5 + 10)) << "Postion" << setw(static_cast<int>(cellWidthMot * 1.5 + cellWidthLoc * 1.5)) << "Velocity" << endl;
                cout << "\t" << setw((cellWidthLoc * 3) + (cellWidthMot * 3) + 16) << setfill('-') << "" << setfill(' ') << endl;
                cout << setw(10) << left << "\tgt 0:";
                // Position
                cout << setprecision(DISPLAY_LOC_DIGITS);
                cout << setw(cellWidthLoc) << left << locX << " " << setw(cellWidthLoc) << left << locY << " " << setw(cellWidthLoc) << left << locZ;
                cout << "   ";

                // Velocity
                cout << setprecision(DISPLAY_MOT_DIGITS);
                cout << setw(cellWidthMot) << left << motX << " " << setw(cellWidthMot) << left << motY << " " << setw(cellWidthMot) << left << motZ;
                cout << endl;
            }
            for (int i = 1; i <= ticks; i++)
            {
                motY += gravity;

                // cout <<"blocksin world: " << blocksInWorld.size()<<endl;
                for (int blocks = 0; blocks < blocksInWorld.size(); blocks++)
                {
                    if (locY + 0.98 + motY > blocksInWorld[blocks][1])
                    {
                        // cout << "freefall hit" << endl;
                        //  doenst check for multiple blocks
                        locY = blocksInWorld[blocks][1] - 0.98;
                        motY = 0;
                    }
                }
                locY += motY;
                locX += motX;
                locZ += motZ;

                motX *= drag;
                motY *= drag;
                motZ *= drag;
                age++;
                if (TF == 1)
                {
                    short int cellWidthLoc = DISPLAY_LOC_DIGITS + 4; // +2 for '-' and '.'
                    short int cellWidthMot = DISPLAY_MOT_DIGITS + 4;
                    cout << setw(10) << left << "\tgt " + to_string(i) + ":";
                    // Position
                    cout << setprecision(DISPLAY_LOC_DIGITS); // +2 for - and .
                    cout << setw(cellWidthLoc) << left << locX << " " << setw(cellWidthLoc) << left << locY << " " << setw(cellWidthLoc) << left << locZ;
                    cout << "   ";

                    // Velocity
                    cout << setprecision(DISPLAY_MOT_DIGITS); // +2 for - and .
                    cout << setw(cellWidthMot) << left << motX << " " << setw(cellWidthMot) << left << motY << " " << setw(cellWidthMot) << left << motZ;
                    cout << endl;
                }
                history.push_back(vector<double>{locX, locY, locZ, motX, motY, motZ, 0});
            }
            cout << setprecision(STANDARD_PRECISION);
        }
    }
    void print() override
    {
        if (name != "")
            cout << name << endl;
        cout << "LOC: " << locX << " " << locY << " " << locZ;
        cout << "\tMOT: " << motX << " " << motY << " " << motZ << endl;
    }
};

class Sand : public Entity
{
public:
    const double drag = 0.98, gravity = -0.04;
    ;
    double eye_height = 0.8330000638962;
    Sand() : Entity() {}
    Sand(double x, double y, double z) : Entity(x, y, z) {}
    Sand(double x, double y, double z, double vx, double vy, double vz) : Entity(x, y, z, vx, vy, vz) {}
    Sand(string inputString) : Entity(inputString) {}
    void explosionFrom(const Entity& booster, double fraction) override
    {
        double dx = locX - booster.locX, dy = locY - (booster.locY + explosion_height), dz = locZ - booster.locZ;
        double dist = sqrt(dx * dx + dy * dy + dz * dz);

        if (dist > 8.0 || dist == 0.0)
        {
            return;
        }

        double f = (1.0 - dist / 8.0) * fraction;
        dy = (locY + eye_height) - (booster.locY + explosion_height);
        double directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);
        motX += dx / directionMagnitude * f * booster.amount;
        motY += dy / directionMagnitude * f * booster.amount;
        motZ += dz / directionMagnitude * f * booster.amount;
    }
    void swingFrom(const Entity& booster) override
    {
        Tnt origin(booster);
        vector<vector<double>> locations;
        double dx, dy, dz, dist, f, directionMagnitude;
        // generate explosion locations
        for (int i = 0; i < booster.amount; i++)
        {
            Tnt next = origin;
            next.amount = 1;
            // freefall (optimized)
            next.motY += gravity;

            // check if any blocks clip on the Y direction
            for (int blocks = 0; blocks < blocksInWorld.size(); blocks++)
            {
                if (next.locY + 0.98 + next.motY > blocksInWorld[blocks][1])
                {
                    // cout << "Hit!" << endl;
                    //  doenst check for multiple blocks
                    next.locY = blocksInWorld[blocks][1] - 0.98;
                    next.motY = 0;
                }
            }

            next.locY += next.motY;
            next.locX += next.motX;
            next.locZ += next.motZ;
            vector<double> loc;
            loc.push_back(next.locX);
            loc.push_back(next.locY);
            loc.push_back(next.locZ);
            locations.push_back(loc);
            // optimized explosion
            dx = origin.locX - locations[i][0], dy = origin.locY - (locations[i][1] + explosion_height), dz = origin.locZ - locations[i][2];
            dist = sqrt(dx * dx + dy * dy + dz * dz);
            if (dist > 8.0 || dist == 0.0)
            {
                continue;
            }
            f = (1.0 - dist / 8.0);
            directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);
            origin.motX += dx / directionMagnitude * f;
            origin.motY += dy / directionMagnitude * f;
            origin.motZ += dz / directionMagnitude * f;
        }
        // calculate explosions
        for (int i = 0; i < booster.amount; i++)
        {
            // cout << "exp: " << locations[i][1] << endl;
            //   explosion (optimized)
            dx = locX - locations[i][0], dy = locY - (locations[i][1] + explosion_height), dz = locZ - locations[i][2];
            dist = sqrt(dx * dx + dy * dy + dz * dz);
            if (dist > 8.0 || dist == 0.0)
            {
                continue;
            }

            f = (1.0 - dist / 8.0);
            // dy = (locY + eye_height) - (locations[i][1] + 0.06125F); SAND PARITY
            directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);
            motX += dx / directionMagnitude * f;
            motY += dy / directionMagnitude * f;
            motZ += dz / directionMagnitude * f;
        }
    }
    void freefall(int ticks, bool TF) override
    {
        if (ticks > 0)
        {
            if (TF == 1)
            {
                short int cellWidthLoc = DISPLAY_LOC_DIGITS + 4; // +2 for '-' and '.'
                short int cellWidthMot = DISPLAY_MOT_DIGITS + 4;
                // Header
                cout << "\t(sand freefall printout)" << endl;
                cout << "\t" << setw(static_cast<int>(cellWidthLoc * 1.5 + 10)) << "Postion" << setw(static_cast<int>(cellWidthMot * 1.5 + cellWidthLoc * 1.5)) << "Velocity" << endl;
                cout << "\t" << setw((cellWidthLoc * 3) + (cellWidthMot * 3) + 16) << setfill('-') << "" << setfill(' ') << endl;
                cout << setw(10) << left << "\tgt 0:";
                // Position
                cout << setprecision(DISPLAY_LOC_DIGITS);
                cout << setw(cellWidthLoc) << left << locX << " " << setw(cellWidthLoc) << left << locY << " " << setw(cellWidthLoc) << left << locZ;
                cout << "   ";

                // Velocity
                cout << setprecision(DISPLAY_MOT_DIGITS);
                cout << setw(cellWidthMot) << left << motX << " " << setw(cellWidthMot) << left << motY << " " << setw(cellWidthMot) << left << motZ;
                cout << endl;
            }
            for (int i = 1; i <= ticks; i++)
            {
                motY += gravity;

                // cout <<"blocksin world: " << blocksInWorld.size()<<endl;
                for (int blocks = 0; blocks < blocksInWorld.size(); blocks++)
                {
                    if (locY + 0.98 + motY > blocksInWorld[blocks][1])
                    {
                        // cout << "freefall hit" << endl;
                        //  doenst check for multiple blocks
                        locY = blocksInWorld[blocks][1] - 0.98;
                        motY = 0;
                    }
                }
                locY += motY;
                locX += motX;
                locZ += motZ;

                motX *= drag;
                motY *= drag;
                motZ *= drag;
                age++;
                if (TF == 1)
                {
                    short int cellWidthLoc = DISPLAY_LOC_DIGITS + 4; // +2 for '-' and '.'
                    short int cellWidthMot = DISPLAY_MOT_DIGITS + 4;
                    cout << setw(10) << left << "\tgt " + to_string(i) + ":";
                    // Position
                    cout << setprecision(DISPLAY_LOC_DIGITS); // +2 for - and .
                    cout << setw(cellWidthLoc) << left << locX << " " << setw(cellWidthLoc) << left << locY << " " << setw(cellWidthLoc) << left << locZ;
                    cout << "   ";

                    // Velocity
                    cout << setprecision(DISPLAY_MOT_DIGITS); // +2 for - and .
                    cout << setw(cellWidthMot) << left << motX << " " << setw(cellWidthMot) << left << motY << " " << setw(cellWidthMot) << left << motZ;
                    cout << endl;
                }
                history.push_back(vector<double>{locX, locY, locZ, motX, motY, motZ, 0});
            }
            cout << setprecision(STANDARD_PRECISION);
        }
    }
    void print() override
    {
        if (name != "")
            cout << name << endl;
        cout << "LOC: " << locX << " " << locY << " " << locZ;
        cout << "\tMOT: " << motX << " " << motY << " " << motZ << endl;
    }
};

class Arrow : public Entity
{
public:
    const double drag = 0.99F, gravity = -0.05F;
    const double eye_height = 0.13F;
    Arrow() : Entity() {}
    Arrow(double x, double y, double z) : Entity(x, y, z) {}
    Arrow(double x, double y, double z, double vx, double vy, double vz) : Entity(x, y, z, vx, vy, vz) {}
    Arrow(string inputString) : Entity(inputString) {}

    void explosionFrom(const Entity& booster, double fraction) override
    {
        double dx = locX - booster.locX, dy = locY - (booster.locY + explosion_height), dz = locZ - booster.locZ;
        double dist = sqrt(dx * dx + dy * dy + dz * dz);

        if (dist > 8.0 || dist == 0.0)
        {
            return;
        }

        double f = (1.0 - dist / 8.0) * fraction;
        dy = (locY + eye_height) - (booster.locY + explosion_height);
        double directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);
        motX += dx / directionMagnitude * f * booster.amount;
        motY += dy / directionMagnitude * f * booster.amount;
        motZ += dz / directionMagnitude * f * booster.amount;
    }
    void swingFrom(const Entity& booster) override
    {
        Tnt origin(booster);
        vector<vector<double>> locations;
        double dx, dy, dz, dist, f, directionMagnitude;
        // generate explosion locations
        for (int i = 0; i < booster.amount; i++)
        {
            Tnt next = origin;
            next.amount = 1;
            // freefall (optimized)
            next.motY += gravity;

            // check if any blocks clip on the Y direction
            for (int blocks = 0; blocks < blocksInWorld.size(); blocks++)
            {
                if (next.locY + 0.98 + next.motY > blocksInWorld[blocks][1])
                {
                    // cout << "Hit!" << endl;
                    //  doenst check for multiple blocks
                    next.locY = blocksInWorld[blocks][1] - 0.98;
                    next.motY = 0;
                }
            }

            next.locY += next.motY;
            next.locX += next.motX;
            next.locZ += next.motZ;
            vector<double> loc;
            loc.push_back(next.locX);
            loc.push_back(next.locY);
            loc.push_back(next.locZ);
            locations.push_back(loc);
            // optimized explosion
            dx = origin.locX - locations[i][0], dy = origin.locY - (locations[i][1] + explosion_height), dz = origin.locZ - locations[i][2];
            dist = sqrt(dx * dx + dy * dy + dz * dz);
            if (dist > 8.0 || dist == 0.0)
            {
                continue;
            }
            f = (1.0 - dist / 8.0);
            directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);
            origin.motX += dx / directionMagnitude * f;
            origin.motY += dy / directionMagnitude * f;
            origin.motZ += dz / directionMagnitude * f;
        }
        // calculate explosions
        for (int i = 0; i < booster.amount; i++)
        {
            // cout << "exp: " << locations[i][1] << endl;
            //   explosion (optimized)
            dx = locX - locations[i][0], dy = locY - (locations[i][1] + explosion_height), dz = locZ - locations[i][2];
            dist = sqrt(dx * dx + dy * dy + dz * dz);
            if (dist > 8.0 || dist == 0.0)
            {
                continue;
            }

            f = (1.0 - dist / 8.0);
            // dy = (locY + eye_height) - (locations[i][1] + 0.06125F); SAND PARITY
            directionMagnitude = sqrt(dx * dx + dy * dy + dz * dz);
            motX += dx / directionMagnitude * f;
            motY += dy / directionMagnitude * f;
            motZ += dz / directionMagnitude * f;
        }
    }
    void freefall(int ticks, bool TF) override
    {
        if (ticks > 0)
        {
            if (TF == 1)
            {
                short int cellWidthLoc = DISPLAY_LOC_DIGITS + 4; // +2 for '-' and '.'
                short int cellWidthMot = DISPLAY_MOT_DIGITS + 4;
                // Header
                cout << "\t(arrow freefall printout)" << endl;
                cout << "\t" << setw(static_cast<int>(cellWidthLoc * 1.5 + 10)) << "Postion" << setw(static_cast<int>(cellWidthMot * 1.5 + cellWidthLoc * 1.5)) << "Velocity" << endl;
                cout << "\t" << setw((cellWidthLoc * 3) + (cellWidthMot * 3) + 16) << setfill('-') << "" << setfill(' ') << endl;
                cout << setw(10) << left << "\tgt 0:";
                // Position
                cout << setprecision(DISPLAY_LOC_DIGITS);
                cout << setw(cellWidthLoc) << left << locX << " " << setw(cellWidthLoc) << left << locY << " " << setw(cellWidthLoc) << left << locZ;
                cout << "   ";

                // Velocity
                cout << setprecision(DISPLAY_MOT_DIGITS);
                cout << setw(cellWidthMot) << left << motX << " " << setw(cellWidthMot) << left << motY << " " << setw(cellWidthMot) << left << motZ;
                cout << endl;
            }
            for (int i = 1; i <= ticks; i++)
            {

                //  cout <<"blocksin world: " << blocksInWorld.size()<<endl;
                for (int blocks = 0; blocks < blocksInWorld.size(); blocks++)
                {
                    if (locY + 0.98 + motY > blocksInWorld[blocks][1])
                    {
                        //  cout << "freefall hit" << endl;
                        //  doenst check for multiple blocks
                        locY = blocksInWorld[blocks][1] - 0.98;
                        motY = 0;
                    }
                }

                locY += motY;
                locX += motX;
                locZ += motZ;

                motX *= drag;
                motY *= drag;
                motZ *= drag;
                motY += gravity;

                age++;
                if (TF == 1)
                {
                    short int cellWidthLoc = DISPLAY_LOC_DIGITS + 4; // +2 for '-' and '.'
                    short int cellWidthMot = DISPLAY_MOT_DIGITS + 4;
                    cout << setw(10) << left << "\tgt " + to_string(i) + ":";
                    // Position
                    cout << setprecision(DISPLAY_LOC_DIGITS); // +2 for - and .
                    cout << setw(cellWidthLoc) << left << locX << " " << setw(cellWidthLoc) << left << locY << " " << setw(cellWidthLoc) << left << locZ;
                    cout << "   ";

                    // Velocity
                    cout << setprecision(DISPLAY_MOT_DIGITS); // +2 for - and .
                    cout << setw(cellWidthMot) << left << motX << " " << setw(cellWidthMot) << left << motY << " " << setw(cellWidthMot) << left << motZ;
                    cout << endl;
                }
                history.push_back(vector<double>{locX, locY, locZ, motX, motY, motZ, 0});
            }
            cout << setprecision(STANDARD_PRECISION);
        }
    }
    void print() override
    {
        if (name != "")
            cout << name << endl;
        cout << "LOC: " << locX << " " << locY << " " << locZ;
        cout << "\tMOT: " << motX << " " << motY << " " << motZ << endl;
    }
};

class Gain
{
public:
    double x = 0.0, y = 0.0, z = 0.0;
};

#endif // ENTITY_H