#pragma once
#include <iostream>
#include <string>

using namespace std;

class Order;

class Chef {
private:
    int    id;
    string type;
    float  speed;

    bool   isBusy;
    Order* currentOrder;

    int    busyTime;

public:
    Chef() :
        id(0), type(""), speed(0),
        isBusy(false), currentOrder(nullptr), busyTime(0) {
    }

    Chef(int id, string type, float speed) :
        id(id), type(type), speed(speed),
        isBusy(false), currentOrder(nullptr), busyTime(0) {
    }

    int    getID()           const { return id; }
    string getType()         const { return type; }
    float  getSpeed()        const { return speed; }
    bool   getIsBusy()       const { return isBusy; }
    Order* getCurrentOrder() const { return currentOrder; }
    int    getBusyTime()     const { return busyTime; }

    bool isCS() const { return type == "CS"; }
    bool isCN() const { return type == "CN"; }

    void assignOrder(Order* o) {
        currentOrder = o;
        isBusy = true;
    }

    void releaseOrder() {
        currentOrder = nullptr;
        isBusy = false;
    }

    void incrementBusyTime() {
        if (isBusy) busyTime++;
    }

    float getUtilization(int totalSimTime) const {
        if (totalSimTime == 0) return 0;
        return (float)busyTime / totalSimTime * 100.0f;
    }

    void print() const {
        cout << "[Chef #" << id << " | " << type
            << " | speed=" << speed
            << " | " << (isBusy ? "BUSY" : "FREE") << "]";
    }

    friend ostream& operator<<(ostream& out, const Chef* c) {
        if (!c) { out << "[null chef]"; return out; }
        c->print();
        return out;
    }
};