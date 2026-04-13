#pragma once
using namespace std;

class Scooter {
private:
    int Speed;
    int Main_Dur;
    int Distance_Cut;

public:
    Scooter() :
        Speed(0), Main_Dur(0), Distance_Cut(0) {
    }

    Scooter(int speed, int maind) :
        Speed(speed), Main_Dur(maind), Distance_Cut(0) {
    }

    int getSpeed() const { return Speed; }

    int getMainD() const { return Main_Dur; }

    int GetDistance() const { return Distance_Cut; }

    void incDist(int distance) {
        Distance_Cut += distance;
    }
   

};