#pragma once
#include "../ds/LinkedQueue.h"
#include "../ds/PriQueue.h"
#include "../Derived DS/CookingOrders.h"
#include "../Derived DS/Fit_Table.h"
#include "../ds/ArrayStack.h"
#include "../entities/Order.h"
#include "../Derived DS/derivedQueue.h"
#include "UI.h"
#include<fstream>
#include<string>
using namespace std;

class Order;
class Chef;
class Scooter;
class Table;
class Action;
class UI;

class Restaurant {
private:
    // --------------------Actions--------------
    LinkedQueue<Action*> actions;   //fcfs

    // --------------------pending orders--------------
    LinkedQueue<Order*> pendODG;    //fcfs
    LinkedQueue<Order*> pendODN;    //fcfs
    LinkedQueue<Order*> pendOT;     //fcfs
    derivedQueue        pendOVC;    //fcfs, cancelling an order
    priQueue<Order*>    pendOVG;    //ordered according to the priority of the ovg
    LinkedQueue<Order*> pendOVN;    // fcfs

    // --------------------coocking orders--------------
    CookingOrders       cooking;    //ordered by finish time to check the peek only, cancelling an order

    // -------------------ready orders----------------
    LinkedQueue<Order*> readyOT;    //fcfs
    LinkedQueue<Order*> readyOD;    //fcfs
    derivedQueue        readyOV;    //fcfs, cancelling an order
    
    // --------------------inservice orders--------------
    priQueue<Order*>    inServOrders;   //ordered by the end of service time 

    // --------------------finished orders--------------
    ArrayStack<Order*>    finishedOrders;   //stack to print from last finished to the first

    // --------------------cancelled orders--------------
    LinkedQueue<Order*> cancelledOrders;    //to be printed by their order of cancellation

    // -------------------- chefs --------------
    LinkedQueue<Chef*>  availCN;    //fcfs
    LinkedQueue<Chef*>  availCS;    //fcfs

    // -------------------- scooters --------------
    priQueue<Scooter*>  freeScooters;   //ordered by the least distance moved
    priQueue<Scooter*>  backScooters;   //order by the first to return to the restaurant (the delivered order distance)
    LinkedQueue<Scooter*> maintScooters;    //fcfs

    // -------------------- tables  --------------
    Fit_Tables  freeTables;
    Fit_Tables  busySharable;
    Fit_Tables  busyNoShare;
    //-------------------------- bonus lists ---------------------------//
    priQueue<Order*> overwaitOVG;   //order by highest Current time - TQ
    int currentTime;        // current time indicator

    UI* pUI;                // pointer for the ui class

    // for statistic // maybe I will add more later 
    int num_CS, num_CN, Scooter_Count, total_Table, Main_Ords, TH;
public:
    Restaurant();
    ~Restaurant();
    // most of these functions need to be updated for phase 2
   void randomSimulate();
   bool assignToChef(Order* od);    // assign an order to a chef
   bool assignToTable(Order* od);   // assign an order to a table and put the order in inservice list
   bool assignToScooter(Order* od); // assign an order to scooter and put the order in inservice list
   //for cancelling OVCs
   bool cancelOrderFromPending(int id);  
   bool cancelOrderFromCooking(int id);
   bool cancelOrderFromReady(int id);

   bool freeOrderChef(Order* od);   // free the chef of an order
   bool freeOrderTable(DineInOrder* dinorder);     // free a table
   bool freeOrderScooter(DeliveryOrder* deliorder);    // free a scooter
   

   bool RemoveTable(Fit_Tables& t,int  id);

   // The input file
   bool LoadInputFile(const string& filename);
};