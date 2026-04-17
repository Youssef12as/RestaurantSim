#pragma once
#include <iostream>
#include <string>
#include <cmath>

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

    virtual ~Order() {}

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

    int getExpectedFinishTime(float chefSpeed) const {
        if (TA == -1) {
            return -1;
        }
        else {
            return TA + (int)ceil((float)size / chefSpeed);
        }
    }

    virtual void print() const {
        cout << id << ", ";
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

};

class TakeawayOrder : public Order {
public:
    TakeawayOrder(int id, string type, int TQ, int size, float price)
        : Order(id, type, TQ, size, price) {
    }

};