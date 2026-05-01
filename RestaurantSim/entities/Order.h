#pragma once
#include <iostream>
#include <string>
#include <cmath>
#include "Scooter.h"
#include "Chef.h"

using namespace std;

class Chef;
class Table;
class Scooter;

class Order {
protected:
    int id;
    string type;
    int size;
    float price;

    int TQ, TA, TR, TS, TF;
    Chef* assignedChef;

public:
    Order(int id, string type, int TQ, int size, float price)
        : id(id), type(type), TQ(TQ), size(size), price(price),
        TA(-1), TR(-1), TS(-1), TF(-1), assignedChef(nullptr) {
    }

    virtual ~Order() = default;

    int getID() const { return id; }
    string getType() const { return type; }
    int getSize() const { return size; }
    float getPrice() const { return price; }
    int getTQ() const { return TQ; }
    int getTA() const { return TA; }
    int getTR() const { return TR; }
    int getTS() const { return TS; }
    int getTF() const { return TF; }
    Chef* getAssignedChef() const { return assignedChef; }

    void setTA(int t) { TA = t; }
    void setTR(int t) { TR = t; }
    void setTS(int t) { TS = t; }
    void setTF(int t) { TF = t; }
    void setAssignedChef(Chef* c) { assignedChef = c; }


    int getTi() const {
        if (TA == -1 || TS == -1 || TR == -1) {
            return -1;
        }
        else {
            return (TA - TQ) + (TS - TR);
        }
    }

    int getTc() const {
        if (TA == -1 || TR == -1) {
            return -1;
        }
        else {
            return TR - TA;
        }
    }

    int getTw() const {
        if (getTi() == -1 || getTc() == -1) {
            return -1;
        }
        else {
            return getTi() + getTc();
        }
    }

    int getTserv() const {
        if (TS == -1 || TF == -1) {
            return -1;
        }
        else {
            return TF - TS;
        }
    }

    int getExpectedReadyTime(float chefSpeed) const {
        if (TA == -1) {
            return -1;
        }
        else {
            return TA + (int)ceil((float)size / chefSpeed);
        }
    }

    virtual int getExpectedFinishTime() const { return -1; }

    virtual void print() const {
        cout << id;
    }

    friend ostream& operator<<(ostream& out, const Order* o) {
        if (!o) { out << "[null order]"; return out; }
        o->print();
        return out;
    }
};

class DineInOrder : public Order {
private:
    int seats;
    int duration;
    bool canShare;
    Table* assignedTable;

public:
    DineInOrder(int id, string type, int TQ, int size, float price, int seats, int duration, bool canShare)
        : Order(id, type, TQ, size, price), seats(seats), duration(duration),
        canShare(canShare), assignedTable(nullptr) {
    }

    int getSeats() const { return seats; }
    int getDuration() const { return duration; }
    bool getCanShare() const { return canShare; }
    Table* getAssignedTable() const { return assignedTable; }
    void setAssignedTable(Table* t) { assignedTable = t; }

    int getExpectedFinishTime() const {
        if (TS == -1) {
            return -1;
        }
        else {
            return TS + duration;
        }
    }


};

class DeliveryOrder : public Order {
private:
    float distance;
    Scooter* assignedScooter;

public:
    DeliveryOrder(int id, string type, int TQ, int size, float price, float distance)
        : Order(id, type, TQ, size, price), distance(distance), assignedScooter(nullptr) {
    }

    float getDistance() const { return distance; }
    Scooter* getAssignedScooter() const { return assignedScooter; }
    void setAssignedScooter(Scooter* s) { assignedScooter = s; }

    float getOVGPriority(float w1, float w2, float w3) const {
        return w1 * price + w2 * size - w3 * distance;
    }

    int getExpectedFinishTime() const {
        if (TS == -1) {
            return -1;
        }
        if (assignedScooter == nullptr) {
            return -1;
        }
        else {
            return TS + ceil(distance / assignedScooter->getSpeed());
        }
    }

};

class TakeawayOrder : public Order {
public:
    TakeawayOrder(int id, string type, int TQ, int size, float price)
        : Order(id, type, TQ, size, price) {
    }

};

class ComboOrder : public Order {
private:
    float distance;
    int numChefsNeeded;      // 1-4, at least 1 must be CS
    int numScootersNeeded;   // 2+

    Chef* chefs[4];
    int chefCount;

    Scooter* scooters[4];
    int scooterCount;

public:
    ComboOrder(int id, int TQ, int size, float price,
               float dist, int numChefs, int numScooters)
        : Order(id, "OC", TQ, size, price), distance(dist),
          numChefsNeeded(numChefs), numScootersNeeded(numScooters),
          chefCount(0), scooterCount(0) {
        for (int i = 0; i < 4; i++) { chefs[i] = nullptr; scooters[i] = nullptr; }
    }

    float getDistance() const { return distance; }
    int getNumChefsNeeded() const { return numChefsNeeded; }
    int getNumScootersNeeded() const { return numScootersNeeded; }
    int getChefCount() const { return chefCount; }
    int getScooterCount() const { return scooterCount; }

    Chef* getChefAt(int i) const { return (i >= 0 && i < chefCount) ? chefs[i] : nullptr; }
    Scooter* getScooterAt(int i) const { return (i >= 0 && i < scooterCount) ? scooters[i] : nullptr; }

    bool addChef(Chef* c) {
        if (chefCount >= numChefsNeeded || chefCount >= 4) return false;
        chefs[chefCount++] = c;
        return true;
    }

    bool addScooter(Scooter* s) {
        if (scooterCount >= numScootersNeeded || scooterCount >= 4) return false;
        scooters[scooterCount++] = s;
        return true;
    }

    float getTotalChefSpeed() const {
        float total = 0;
        for (int i = 0; i < chefCount; i++) {
            if (chefs[i]) total += chefs[i]->getSpeed();
        }
        return total;
    }

    int getComboReadyTime() const {
        if (TA == -1 || chefCount == 0) return -1;
        float totalSpeed = getTotalChefSpeed();
        if (totalSpeed <= 0) return -1;
        return TA + (int)ceil((float)size / totalSpeed);
    }

    int getExpectedFinishTime() const override {
        if (TS == -1 || scooterCount == 0 || scooters[0] == nullptr) return -1;
        return TS + (int)ceil(distance / scooters[0]->getSpeed());
    }

    void clearAllChefs() {
        for (int i = 0; i < 4; i++) chefs[i] = nullptr;
        chefCount = 0;
        setAssignedChef(nullptr);
    }

    void clearAllScooters() {
        for (int i = 0; i < 4; i++) scooters[i] = nullptr;
        scooterCount = 0;
    }
};
