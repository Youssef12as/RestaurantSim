#pragma once
using namespace std;

class Table {
private:
    int Capacity;
    int BusySeats;

public:
    Table() :
        Capacity(0), BusySeats(0) {
    }

    Table(int cap) :
        Capacity(cap), BusySeats(0) {
    }

    int getCapacity() const { return Capacity; }

    int getBusySeats() const { return BusySeats; }

    int GetFreeSeats() const { return Capacity - BusySeats; }

    void setBusySeats(int seats) {
        BusySeats = Capacity - seats;
        if (BusySeats < 0) {
            BusySeats = Capacity;
        }
    }

    void freeSeats(int seats) {  // this is for freeing seats when a order is finished (pass the order seats)
        Capacity += BusySeats - seats;
    }

    
    

   

    
};