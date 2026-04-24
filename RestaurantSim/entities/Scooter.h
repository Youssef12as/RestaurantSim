#pragma once
using namespace std;
#include <iostream>

class Scooter {
private:
    int id;
    int Speed;
    int Main_Dur;
    int Distance_Cut;
    int Tmaint;     //time when the scooter back from maint

    int DeliveredOrders;
    static int ScooterCount;

public:
    Scooter() :
        Speed(0), Main_Dur(0), Distance_Cut(0), DeliveredOrders(0) {
        id = ScooterCount++;
    }

    Scooter(int speed, int maind) :
        Speed(speed), Main_Dur(maind), Distance_Cut(0), DeliveredOrders(0) {
        id = ScooterCount++;
    }

    int getSpeed() const { return Speed; }

    int getMainD() const { return Main_Dur; }

    int GetDistance() const { return Distance_Cut; }

    int getID() const { return id; }

    void incrementDeliOreders() {       // i will increent this to compare it with main_ord in restaurant
        DeliveredOrders++;      
    }

    void resetDeliOreders() {          // this is resetted when the scooter back from mentainence
        DeliveredOrders = 0;
    }

    int getDeliOrders() const {
        return DeliveredOrders;
    }

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

    void setTmain(int current) {
        Tmaint = current + Main_Dur;
    }

    int getFinishMaint() {
        return Tmaint;
    }

    void resetTmaint() {
        Tmaint = 0;
    }
   

};
