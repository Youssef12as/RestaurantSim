#pragma once
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

class Chef;
class Scooter;
class Table;

class Order {
private:
    int    id;
    string type;

    int    size;
    float  price;

    int    seats;
    int    duration;
    bool   canShare;

    float  distance;

    int    TQ;
    int    TA;
    int    TR;
    int    TS;
    int    TF;

    Chef* assignedChef;
    Scooter* assignedScooter;
    Table* assignedTable;

public:
    Order() : //default
        id(0), type(""), size(0), price(0),
        seats(0), duration(0), canShare(false),
        distance(0), TQ(0), TA(-1), TR(-1), TS(-1), TF(-1),
        assignedChef(nullptr), assignedScooter(nullptr), assignedTable(nullptr) {
    }

    Order(int id, string type, int TQ, int size, float price) :  //OT
        id(id), type(type), TQ(TQ), size(size), price(price),
        seats(0), duration(0), canShare(false),
        distance(0), TA(-1), TR(-1), TS(-1), TF(-1),
        assignedChef(nullptr), assignedScooter(nullptr), assignedTable(nullptr) {
    }

    Order(int id, string type, int TQ, int size, float price,
        int seats, int duration, bool canShare) :  //OD
        id(id), type(type), TQ(TQ), size(size), price(price),
        seats(seats), duration(duration), canShare(canShare),
        distance(0), TA(-1), TR(-1), TS(-1), TF(-1),
        assignedChef(nullptr), assignedScooter(nullptr), assignedTable(nullptr) {
    }

    Order(int id, string type, int TQ, int size, float price, float distance) :  //OV
        id(id), type(type), TQ(TQ), size(size), price(price),
        seats(0), duration(0), canShare(false),
        distance(distance), TA(-1), TR(-1), TS(-1), TF(-1),
        assignedChef(nullptr), assignedScooter(nullptr), assignedTable(nullptr) {
    }

    int    getID()       const { return id; }
    string getType()     const { return type; }
    int    getSize()     const { return size; }
    float  getPrice()    const { return price; }
    int    getSeats()    const { return seats; }
    int    getDuration() const { return duration; }
    bool   getCanShare() const { return canShare; }
    float  getDistance() const { return distance; }

    int    getTQ() const { return TQ; }
    int    getTA() const { return TA; }
    int    getTR() const { return TR; }
    int    getTS() const { return TS; }
    int    getTF() const { return TF; }

    Chef* getAssignedChef()    const { return assignedChef; }
    Scooter* getAssignedScooter() const { return assignedScooter; }
    Table* getAssignedTable()   const { return assignedTable; }

    void setTA(int t) { TA = t; }
    void setTR(int t) { TR = t; }
    void setTS(int t) { TS = t; }
    void setTF(int t) { TF = t; }

    void setAssignedChef(Chef* c) { assignedChef = c; }
    void setAssignedScooter(Scooter* s) { assignedScooter = s; }
    void setAssignedTable(Table* t) { assignedTable = t; }

    int getTi() const {
        if (TA == -1 || TS == -1 || TR == -1) return -1;
        return (TA - TQ) + (TS - TR);
    }

    int getTc() const {
        if (TA == -1 || TR == -1) return -1;
        return TR - TA;
    }

    int getTw() const {
        if (getTi() == -1 || getTc() == -1) return -1;
        return getTi() + getTc();
    }

    int getTserv() const {
        if (TS == -1 || TF == -1) return -1;
        return TF - TS;
    }

    int getExpectedFinishTime(float chefSpeed) const {
        if (TA == -1) return -1;
        return TA + (int)ceil((float)size / chefSpeed);
    }

    float getOVGPriority(float w1, float w2, float w3) const {
        return w1 * price + w2 * size - w3 * distance;
    }

    bool isDineIn()   const { return type == "ODG" || type == "ODN"; }
    bool isTakeaway() const { return type == "OT"; }
    bool isDelivery() const { return type == "OVC" || type == "OVG" || type == "OVN"; }
    bool isGold()     const { return type == "ODG" || type == "OVG"; }
    bool isNormal()   const { return type == "ODN" || type == "OVN"; }
    bool isVIP()      const { return type == "OVC"; }

    void print() const {
        cout << id << ", ";
    }

    friend ostream& operator<<(ostream& out, const Order* o) {
        if (!o) { out << "[null order]"; return out; }
        o->print();
        return out;
    }

    void printOutputLine() const {
        cout << TF << " " << id << " " << TQ << " "
            << TA << " " << TR << " " << TS << " "
            << getTi() << " " << getTc() << " "
            << getTw() << " " << getTserv() << "\n";
    }
};