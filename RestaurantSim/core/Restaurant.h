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
    LinkedQueue<Action*> actions;

    // --------------------pending orders--------------
    LinkedQueue<Order*> pendODG;
    LinkedQueue<Order*> pendODN;
    LinkedQueue<Order*> pendOT;
    derivedQueue        pendOVC;
    priQueue<Order*>    pendOVG;
    LinkedQueue<Order*> pendOVN;

    // --------------------coocking orders--------------
    CookingOrders       cooking;

    // -------------------ready orders----------------
    LinkedQueue<Order*> readyOT;
    LinkedQueue<Order*> readyOD;
    derivedQueue        readyOV;
    
    // --------------------inservice orders--------------
    priQueue<Order*>    inServOrders;

    // --------------------finished orders--------------
    ArrayStack<Order*>    finishedOrders;

    // --------------------cancelled orders--------------
    LinkedQueue<Order*> cancelledOrders;

    // -------------------- chefs --------------
    LinkedQueue<Chef*>  availCN;
    LinkedQueue<Chef*>  availCS;

    // -------------------- scooters --------------
    priQueue<Scooter*>  freeScooters;
    priQueue<Scooter*>  backScooters;
    LinkedQueue<Scooter*> maintScooters;

    // -------------------- tables  --------------
    Fit_Tables  freeTables;
    Fit_Tables  busySharable;
    Fit_Tables  busyNoShare;

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