#pragma once

using namespace std;

class Order;
class Chef;
class Scooter;
class Table;
class Action;
class UI;

#include <iostream>
#include <string>
#include "../ds/LinkedQueue.h"
#include "../ds/PriQueue.h"
#include "../Derived DS/CookingOrders.h"
#include "../Derived DS/Fit_Table.h"
#include "../ds/ArrayStack.h"

#include "../entities/Order.h"
#include "../entities/Chef.h"
#include "../entities/Scooter.h"
#include "../entities/Table.h"
#include "../Derived DS/derivedQueue.h"




class Restaurant {
private:

    LinkedQueue<Order*> pendODG;
    LinkedQueue<Order*> pendODN;
    LinkedQueue<Order*> pendOT;
    derivedQueue pendOVC;
    priQueue<Order*>    pendOVG;
    LinkedQueue<Order*> pendOVN;

    CookingOrders       cooking;

    LinkedQueue<Order*> readyOT;
    LinkedQueue<Order*> readyOD;
    derivedQueue readyOVC;
    LinkedQueue<Order*> readyOVG;
    LinkedQueue<Order*> readyOVN;


    priQueue<Order*>    inServOrders;
    ArrayStack<Order*>    finishedOrders;
    LinkedQueue<Order*> cancelledOrders;

    LinkedQueue<Chef*>  availCN;
    LinkedQueue<Chef*>  availCS;

    priQueue<Scooter*>  freeScooters;
    priQueue<Scooter*>  backScooters;
    LinkedQueue<Scooter*> maintScooters;

    Fit_Tables  freeTables;
    Fit_Tables  busySharable;
  //  priQueue<Table*>  busyNoShare;

    int currentTime;

public:
    Restaurant();
    ~Restaurant();

   void randomSimulate(UI& ui);

};