#pragma once
using namespace std;
#include <iostream>

class Table {
private:
    int id;
    int Capacity;
    int FreeSeats;

    static int TableCount;

public:
    Table() :
        Capacity(0), FreeSeats(0) {
        id = TableCount++;
    }

    Table(int cap) :
        Capacity(cap), FreeSeats(cap) {
        id = TableCount++;
    }

    int getCapacity() const { return Capacity; }

    int getBusySeats() const { return Capacity - FreeSeats; }

    int GetFreeSeats() const { return FreeSeats; }

    int GetId() const { return id; }

    bool setBusySeats(int seats) {
        if (seats <= FreeSeats && seats > 0) {
            FreeSeats -= seats;
            return true; 
        }
        return false;
    }

    void freeSeats(int seats) {  // this is for freeing seats when a order is finished (pass the order seats)
        FreeSeats += seats;
    }

    
    void print() const {
        cout << id;
    }

    friend ostream& operator<<(ostream& out, const Table* o) {
        if (!o) { out << "[null table]"; return out; }
        o->print();
        return out;
    }

};
