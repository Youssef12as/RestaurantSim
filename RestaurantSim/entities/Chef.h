#pragma once
#include <iostream>
#include <string>

using namespace std;

class Chef {
private:
    int id;
    string type;
    float speed;
    bool isBusy;
    int busyTime;

    static int ChefCount;

public:
    Chef() :
        type(""), speed(0),
        isBusy(false), busyTime(0) {
        id = ChefCount++;
    }

    Chef(string type, float speed) :
        type(type), speed(speed),
        isBusy(false), busyTime(0) {
        id = ChefCount++;
    }

    int getID() const { return id; }
    string getType() const { return type; }
    float getSpeed() const { return speed; }
    bool getIsBusy() const { return isBusy; }
    int getBusyTime() const { return busyTime; }

    bool isCS() const { return type == "CS"; }
    bool isCN() const { return type == "CN"; }

    void assignOrder() {
        isBusy = true;
    }

    void releaseOrder() {
        isBusy = false;
    }

    void incrementBusyTime() {
        if (isBusy) {
            busyTime++;
        }
    }

    float getUtilization(int totalSimTime) const {
        if (totalSimTime == 0) return 0.0f;
        return ((float)busyTime / totalSimTime) * 100.0f;
    }

    void print() const {
        cout << id;
    }

    friend ostream& operator<<(ostream& out, const Chef* c) {
        c->print();
        return out;
    }
};

