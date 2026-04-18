#include "Restaurant.h"
#include <cstdlib>   // rand, srand
#include <ctime>     // time
#include <random>

Restaurant::Restaurant()
{
	pUI = new UI;
    currentTime = 1;
}

Restaurant::~Restaurant()
{
	delete pUI;
}

void Restaurant::randomSimulate()
{

    //-------------------genrerate 500 random orders--------------------
    srand(time(0));

    const int NUM_ORDERS = 500;

    for (int i = 0; i < NUM_ORDERS; i++)
    {
        int id = i + 1;

        // Random common attributes
        int TQ = rand() % 100 + 1;
        int size = rand() % 10 + 1;
        float price = (rand() % 2000) / 10.0f + 10; 

        int category = rand() % 6;
        // 0: ODG, 1: ODN, 2: OT, 3: OVC, 4: OVG, 5: OVN

        switch (category)
        {
        case 0: // Dine-In Grilled
        {
            int seats = rand() % 6 + 1;
            int duration = rand() % 120 + 10;
            bool share = rand() % 2;

            Order* o = new DineInOrder(id, "ODG", TQ, size, price, seats, duration, share);
            pendODG.enqueue(o);
            break;
        }

        case 1: // Dine-In Normal
        {
            int seats = rand() % 6 + 1;
            int duration = rand() % 120 + 10;
            bool share = rand() % 2;

            Order* o = new DineInOrder(id, "ODN", TQ, size, price, seats, duration, share);
            pendODN.enqueue(o);
            break;
        }

        case 2: // Takeaway
        {
            Order* o = new TakeawayOrder(id, "OT", TQ, size, price);
            pendOT.enqueue(o);
            break;
        }

        case 3: // cold delivery order 
        {
            float dist = (rand() % 1000) / 10.0f;

            DeliveryOrder* o = new DeliveryOrder(id, "OVC", TQ, size, price, dist);

            //float priority = o->getOVGPriority(1.0, 1.0, 1.0);// There is no meaning for this since it is queue not priQueue
            pendOVC.enqueue(o);

            break;
        }

        case 4: // grilled delivery order
        {
            float dist = (rand() % 1000) / 10.0f;

            DeliveryOrder* o = new DeliveryOrder(id, "OVG", TQ, size, price, dist);

            float priority = o->getOVGPriority(1.0, 1.0, 1.0);
            pendOVG.enqueue(o, priority);

            break;
        }

        case 5: // Normal Delivery
        {
            float dist = (rand() % 1000) / 10.0f;

            Order* o = new DeliveryOrder(id, "OVN", TQ, size, price, dist);
            pendOVN.enqueue(o);
            break;
        }
        }
    }
    //------------------end of generation--------------------

    //-------------- add chefs, scooters and tables
    for (size_t i = 0; i < 25; i++)
    {
        Chef* tempchef = new Chef("CN", 3);
        availCN.enqueue(tempchef);
    }
    for (size_t i = 0; i < 13; i++)
    {
        Chef* tempchef = new Chef("CS", 2);
        availCS.enqueue(tempchef);
    }

    for (size_t i = 0; i < 20; i++)
    {
        Scooter* tempscooter = new Scooter(150, 23);
        int pri = 3;
        freeScooters.enqueue(tempscooter, pri);
    }

    for (size_t i = 0; i < 13; i++)
    {
        Table* temptable = new Table(7);
        freeTables.enqueue(temptable, -temptable->GetFreeSeats());
    }
    for (size_t i = 0; i < 8; i++)
    {
        Table* temptable = new Table(4);
        freeTables.enqueue(temptable, -temptable->GetFreeSeats());
    }

    pUI->PrintCurrentState(0, actions, pendODG, pendODN, pendOT, pendOVN, pendOVC, pendOVG,
        availCS, availCN, cooking, readyOD, readyOT, readyOV,
        freeScooters, maintScooters, backScooters, freeTables, inServOrders, cancelledOrders, finishedOrders);

    pUI->WaitForNextStep();
    //-------------------------while there are processing orders------------
    while (finishedOrders.GetCount() + cancelledOrders.GetCount() != 500)
    {
        //----------- 3.1 --------------
        for (size_t i = 0; i < 30; )
        {
            if (pendODG.isEmpty() && pendODN.isEmpty() && pendOT.isEmpty()
                && pendOVC.isEmpty() && pendOVG.isEmpty() && pendOVN.isEmpty()) {
                break;
            }
            int category = rand() % 6;
            // 0: ODG, 1: ODN, 2: OT, 3: OVC, 4: OVG, 5: OVN

            switch (category)
            {
                case 0: // Dine-In Grilled
                {
                    Order* o = nullptr;
                    if (pendODG.peek(o))
                    {
                        if (assignToChef(o)) {
                            pendODG.dequeue(o);
                            i++;
                        }
                    }
                    break;
                }

                case 1: // Dine-In Normal
                {
                    Order* o= nullptr;
                    if (pendODN.peek(o))
                    {
                        if (assignToChef(o)) {
                            pendODN.dequeue(o);
                            i++;
                        }
                    }
                    break;
                }

                case 2: // Takeaway
                {
                    Order* o = nullptr;
                    if (pendOT.peek(o))
                    {
                        if (assignToChef(o)) {
                            pendOT.dequeue(o);
                            i++;
                        }
                    }
                    break;
                }

                case 3: // cold delivery order 
                {
                    Order* o = nullptr;
                    if (pendOVC.peek(o))
                    {
                        if (assignToChef(o)) {
                            pendOVC.dequeue(o);
                            i++;
                        }
                    }
                    break;
                }

                case 4: // grilled delivery order
                {
                    Order* o = nullptr;
                    int pri;
                    if (pendOVG.peek(o,pri))
                    {
                        if (assignToChef(o)) {
                            pendOVG.dequeue(o, pri);
                            i++;
                        }
                    }
                    break;
                }

                case 5: // Normal Delivery
                {
                    Order* o = nullptr;
                    if (pendOVN.peek(o))
                    {
                        if (assignToChef(o)) {
                            pendOVN.dequeue(o);
                            i++;
                        }
                    }
                    break;
                }
            }

        }
        //-------------- 3.2 ------------------------

        std::random_device rd;          // seed source
        std::mt19937 gen(rd());         // random engine
        std::uniform_real_distribution<> dist(0.0, 1.0); 

        for (size_t i = 0; i < 15; i++)
        {
            double r = dist(gen);   //this gives random number from 0 to 1 with equal propapilities
            if (r <= 0.75) {

                if (cooking.isEmpty()) {
                    break;
                }
                int pri;
                Order* od;
                if (cooking.dequeue(od, pri)) {
                    DineInOrder* dinein = dynamic_cast<DineInOrder*>(od);
                    if (dinein != nullptr) {
                        freeOrderChef(od);
                        readyOD.enqueue(od);
                        continue;
                    }
                    DeliveryOrder* deliv = dynamic_cast<DeliveryOrder*>(od);
                    if (deliv != nullptr) {
                        freeOrderChef(od);
                        readyOV.enqueue(od);
                        continue;
                    }
                    TakeawayOrder* take = dynamic_cast<TakeawayOrder*>(od);
                    if (take != nullptr) {
                        freeOrderChef(od);
                        readyOT.enqueue(od);
                        continue;
                    }
                    
                }

            }
        }

        //--------------- 3.3 --------------
        for (size_t i = 0; i < 10; )
        {
            if (readyOD.isEmpty() && readyOT.isEmpty() && readyOV.isEmpty()) {
                break;
            }
            int ready = rand() % 3;
            Order* od = nullptr;
            switch (ready)
            {
            case 0:
            {
                if (readyOD.peek(od))
                {
                    if (assignToTable(od)) {
                        readyOD.dequeue(od);
                        i++;
                    }
                }
                break;
            }
            case 1:
            {
                if (readyOT.dequeue(od)) {
                    finishedOrders.push(od);
                    i++;
                }
                break;
            }
            case 2:
            {
                if (readyOV.peek(od))
                {
                    if (assignToScooter(od)) {
                        readyOV.dequeue(od);
                        i++;
                    }
                }
                break;
            }
            }
        }
        //-------------------- 3.4 ----------------------
        int randomId = rand() % 500 + 1;
        cancelOrderFromPending(randomId);
        //-------------------- 3.5 ----------------------
        randomId = rand() % 500 + 1;
        cancelOrderFromCooking(randomId);
        //-------------------- 3.6 ----------------------
        randomId = rand() % 500 + 1;
        cancelOrderFromReady(randomId);


        //---------------- 3.7 -----------------------
        double r2 = dist(gen);
        if (r2 <= 0.25) {
            Order* servedorder = nullptr;
            int pri;
            if (inServOrders.dequeue(servedorder, pri)) {
                finishedOrders.push(servedorder);
                DineInOrder* dinein = dynamic_cast<DineInOrder*>(servedorder);
                if (dinein != nullptr) {
                    freeOrderTable(dinein);
                }
                else {
                    DeliveryOrder* deliv = (DeliveryOrder*)servedorder;
                    freeOrderScooter(deliv);
                }
            }
        }


        //---------------- 3.8 -------------------
        double r3 = dist(gen);
        if (r3 <= 0.50) {
            Scooter* backscooter = nullptr;
            int pri;
            if (backScooters.dequeue(backscooter, pri)) {
                int distination = rand() % 2;
                switch (distination) {
                case 0: {
                    maintScooters.enqueue(backscooter);
                    break;
                }
                case 1: {
                    freeScooters.enqueue(backscooter, -backscooter->GetDistance());
                    break;
                }
                }
            }
        }

        //---------------- 3.9 -------------------
        double r4 = dist(gen);
        if (r4 <= 0.50) {
            Scooter* backscooter = nullptr;
            int pri;
            if (maintScooters.dequeue(backscooter)) {
                freeScooters.enqueue(backscooter, -backscooter->GetDistance());
            }
        }
        

        //---------------- 3.10 -------------------
        pUI->PrintCurrentState(currentTime, actions, pendODG, pendODN, pendOT, pendOVN, pendOVC, pendOVG,
            availCS, availCN, cooking, readyOD, readyOT, readyOV,
            freeScooters, maintScooters, backScooters, freeTables, inServOrders, cancelledOrders, finishedOrders);
        currentTime++;
        pUI->WaitForNextStep();
    }
}

bool Restaurant::assignToChef(Order* od)   // need to update the chefs busy time
{
    // if the order is dinein
    DineInOrder* dinein = dynamic_cast<DineInOrder*>(od);
    if (dinein != nullptr) {        
        char type = od->getType()[2];
        bool assigned = false;
        Chef* tempChef = nullptr;
        if (type == 'G') {          //check the exact type
            assigned = availCS.dequeue(tempChef);
            dinein->setAssignedChef(tempChef);
        }
        else
        {
            assigned = availCN.dequeue(tempChef);
            if (!assigned) {
                assigned = availCS.dequeue(tempChef);
            }
            dinein->setAssignedChef(tempChef);
        }
        if(assigned) cooking.enqueue(od, -od->getExpectedFinishTime(od->getAssignedChef()->getSpeed())); // put the order in the cooking list
        return assigned;
    }

    //if the order is delivery
    DeliveryOrder* deliv = dynamic_cast<DeliveryOrder*>(od);
    if (deliv != nullptr) {
        char type = od->getType()[2];
        Chef* tempChef = nullptr;
        bool assigned = false;
        if (type == 'G') {
            assigned = availCS.dequeue(tempChef);
            deliv->setAssignedChef(tempChef);
        }
        else if(type == 'N')
        {
            assigned = availCN.dequeue(tempChef);
            deliv->setAssignedChef(tempChef);
        }
        else {
            assigned = availCN.dequeue(tempChef);
            if (!assigned) {
                assigned = availCS.dequeue(tempChef);
                
            }
            deliv->setAssignedChef(tempChef);
        }
        if (assigned) cooking.enqueue(od, -ceil(od->getAssignedChef()->getSpeed()));
        return assigned;
    }

    // if the order is takeaway
    TakeawayOrder* take = dynamic_cast<TakeawayOrder*>(od);
    if (take != nullptr) {
        char type = od->getType()[2];
        bool assigned = false;
        Chef* tempChef = nullptr;

        assigned = availCN.dequeue(tempChef);
        take->setAssignedChef(tempChef);

        if (assigned) cooking.enqueue(od, -ceil(od->getAssignedChef()->getSpeed()));
        return assigned;
    }

    return false;
}

bool Restaurant::assignToTable(Order* od)
{
    DineInOrder* dinein = (DineInOrder*)od;
    Table* temptable = busySharable.GetBest(dinein->getSeats());
    if (temptable == nullptr) {
        temptable = freeTables.GetBest(dinein->getSeats());
    }
    if (temptable != nullptr) {     // if i got a table
        dinein->setAssignedTable(temptable);    // assign it
        temptable->setBusySeats(dinein->getSeats()); // update free seats
        if (temptable->GetFreeSeats() > 0) {        // put it in the proper list
            busySharable.enqueue(temptable, -temptable->GetFreeSeats());
        }
        else busyNoShare.enqueue(temptable);

        inServOrders.enqueue(od, -dinein->getDuration());   // move the order to inservice
        return true;
    }
    return false;
}

bool Restaurant::assignToScooter(Order* od)
{
    DeliveryOrder* deliv = (DeliveryOrder*)od;
    Scooter* tempscooter = nullptr;
    int pri;
    freeScooters.dequeue(tempscooter, pri);
    if (tempscooter != nullptr) {       // if i got a scooter
        deliv->setAssignedScooter(tempscooter);
        int pri = deliv->getDistance() / deliv->getAssignedScooter()->getSpeed();
        inServOrders.enqueue(od, -pri);
        return true;
    }
    return false;
}

bool Restaurant::cancelOrderFromPending(int id)
{
    // i check if it is ovc in the cancel function
    Order* cancelledorder = nullptr;
    if (pendOVC.CancelOrder(id, cancelledorder)) {
         cancelledOrders.enqueue(cancelledorder);
         return true;
    }
    return false;
}

bool Restaurant::cancelOrderFromCooking(int id)
{
    Order* cancelledorder = nullptr;
    if (cooking.CancelOrder(id, cancelledorder)) {
        freeOrderChef(cancelledorder);
        cancelledOrders.enqueue(cancelledorder);
        return true;
    }
    return false;
}

bool Restaurant::cancelOrderFromReady(int id)
{
    Order* cancelledorder = nullptr;
    if (readyOV.CancelOrder(id, cancelledorder)) {
        cancelledOrders.enqueue(cancelledorder);
        return true;
    }
    return false;
}

bool Restaurant::freeOrderChef(Order* od)
{
    if (od->getAssignedChef() != nullptr) {
        Chef* tempchef = od->getAssignedChef();
        tempchef->releaseOrder();
        if (tempchef->getType() == "CS") {
            availCS.enqueue(tempchef);
        }
        else {
            availCN.enqueue(tempchef);
        }
        od->setAssignedChef(nullptr);
        return true;
    }
    return false;
}

bool Restaurant::freeOrderTable(DineInOrder* dinorder)
{
    Table* temptable = dinorder->getAssignedTable();    // get the order table
    if (temptable) {
        temptable->freeSeats(dinorder->getSeats());
        if (temptable->getBusySeats() == 0) {           // place it in the right list
            freeTables.enqueue(temptable, -temptable->GetFreeSeats());
        }
        else {
            busySharable.enqueue(temptable, -temptable->GetFreeSeats());
        }
        dinorder->setAssignedTable(nullptr);
        return true;
    }
    return false;
}

bool Restaurant::freeOrderScooter(DeliveryOrder* deliorder) // missing : check if it needs to go to the maintenence
{
    Scooter* tempscooter = deliorder->getAssignedScooter();     // get the order scooter
    if (tempscooter) {
        tempscooter->incDist(deliorder->getDistance());     // increase total distance
        tempscooter->incrementDeliOreders();       // increase the orders delivered
        backScooters.enqueue(tempscooter, -deliorder->getDistance()) ; // put it in scooters back ordered by the distace they will cut back
        deliorder->setAssignedScooter(nullptr);     // free the pointer
        return true;
    }
    
    return false;
}

