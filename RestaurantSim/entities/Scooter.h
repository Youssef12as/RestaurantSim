#pragma once
using namespace std;
#include <iostream>

class Scooter {
private:
    int id;
    int Speed;
    int Main_Dur;
    int Distance_Cut;

    static int ScooterCount;

public:
    Scooter() :
        Speed(0), Main_Dur(0), Distance_Cut(0) {
        id = ScooterCount++;
    }

    Scooter(int speed, int maind) :
        Speed(speed), Main_Dur(maind), Distance_Cut(0) {
        id = ScooterCount++;
    }

    int getSpeed() const { return Speed; }

    int getMainD() const { return Main_Dur; }

    int GetDistance() const { return Distance_Cut; }

    void incDist(int distance) {
        Distance_Cut += distance;
    }

    void print() const {
        cout << id;
    }

    friend ostream& operator<<(ostream& out, const Scooter* o) {
        if (!o) { out << "[null scooter]"; return out; }
        o->print();
        return out;
    }
   

};

static int ScooterCount = 0;