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
    priQueue<Order*>    overWaitOVG;
    
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
    Fit_Tables  freeTables;     //ordered by the least number of seats
    Fit_Tables  busySharable;   //ordered by the least number of seats
    Fit_Tables  busyNoShare;   //doesnt need an order this is just a container (the tables are free when order is done)
    //-------------------------- bonus lists ---------------------------//
    priQueue<Order*> overwaitOVG;   //order by highest Current time - TQ

    int currentTime;        // current time indicator

    UI* pUI;                // pointer for the ui class


    // for statistic // maybe I will add more later 
    int num_CS, num_CN, Scooter_Count, total_Table, Main_Ords, TH, orderCount;
public:
    Restaurant();
    ~Restaurant();
    void randomSimulate();

    //Chefs
    bool assignToChef(Order* od);    // assign an order to a chef
    bool freeOrderChef(Order* od);   // free the chef of an order
    
    
   //Tables
   bool assignToTable(Order* od);   // assign an order to a table and put the order in inservice list
   bool freeOrderTable(DineInOrder* dinorder);     // free a table
   bool RemoveTable(Fit_Tables& t, int  id);     // remove a table from a list


   //Scooter
   bool assignToScooter(Order* od); // assign an order to scooter and put the order in inservice list
   bool freeOrderScooter(DeliveryOrder* deliorder);    // free a scooter from an order then send it back or to the res
   void check_scooters_lists();//done         // check maintscooter and back scooters to the free scooters


   //Orders
   bool cancelOrderFromPending(int id);  
   bool cancelOrderFromCooking(int id);
   bool cancelOrderFromReady(int id);   
   bool AddOrderToPending(Order* o);    //add order to a list

   void check_cooking_orders(); //done        //move order from cooking to ready
   void check_ready_orders();  //done        //move oreders from ready to service
   void check_inservice_orders();//done       // move orders from service to finished
   void check_overwait_orders();// done        // move the overwait order to the overwait list
   void AssignPendingToChefs();//done         // move the orders from pending to cooking

   //action
   void check_action_list(); //done

   
   // The input file
   bool LoadInputFile(const string& filename);



   
   void main_simulation();
};