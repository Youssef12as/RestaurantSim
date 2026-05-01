#include "Restaurant.h"
#include <cstdlib>   // rand, srand
#include <ctime>     // time
#include <random>
#include"../actions/RequestAction.h"
#include"../actions/CancelAction.h"
Restaurant::Restaurant()
{
	pUI = new UI;
    currentTime = 1;
    num_CS = num_CN = Scooter_Count = total_Table = Main_Ords = TH = orderCount = 0;
    numODG = numODN = numOT = numOVC = numOVG = numOVN = numCombo = 0;
    overwaitCount = 0;
    totalChefBusyTime = totalScooterBusyTime = 0;
}

Restaurant::~Restaurant()
{
	delete pUI;
}


void Restaurant::main_simulation()
{
    string inFile = "input_combo.txt";   // get input file
    string outFile = "output_combo.txt"; // get output file
    ProgramMode currentMode = pUI->ReadMode();   //get mode
    if (currentMode == ProgramMode::Silent) pUI->PrintStartSilent();

    //load input file
    LoadInputFile(inFile);
    cout << "orders:" << orderCount << endl;
    if (currentMode == ProgramMode::Interactive) {
        pUI->PrintCurrentState(0, actions, pendODG, pendODN, pendOT, pendOVN, pendOVC, pendOVG, pendCombo,
            availCS, availCN, cooking, readyOD, readyOT, readyOV, overWaitOVG, readyCombo,
            freeScooters, maintScooters, backScooters, freeTables, busySharable,
            inServOrders, cancelledOrders, finishedOrders);

        pUI->WaitForNextStep();
    }
    pUI->WaitForNextStep();
    while (finishedOrders.GetCount() + cancelledOrders.GetCount() < orderCount) {
        
        check_action_list();        // execute the actions in the current time step

        check_inservice_orders();    // finish the orders in the inservice to free tables and scooters

        check_scooters_lists();      // recover the the back scooters

        check_cooking_orders();      // get the cooking orders ready to free up chefs

        check_overwait_orders();    // check the over wait orders

        check_ready_orders();        // get the ready order to inservice

        AssignPendingToChefs();     //assign pending orders to the chefs

        if (currentMode == ProgramMode::Interactive) {
            pUI->PrintCurrentState(currentTime, actions, pendODG, pendODN, pendOT, pendOVN, pendOVC, pendOVG, pendCombo,
                availCS, availCN, cooking, readyOD, readyOT, readyOV, overWaitOVG, readyCombo,
                freeScooters, maintScooters, backScooters, freeTables, busySharable,
                inServOrders, cancelledOrders, finishedOrders);
            pUI->WaitForNextStep();
        }
        currentTime++;

    }
    GenerateOutputFile(outFile);
    if (currentMode == ProgramMode::Silent) pUI->PrintEndSilent();
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
            int seats = rand() % 5 +1;
            int duration = rand() % 120 + 10;
            bool share = rand() % 2;

            Order* o = new DineInOrder(id, "ODG", TQ, size, price, seats, duration, share);
            pendODG.enqueue(o);
            break;
        }

        case 1: // Dine-In Normal
        {
            int seats = rand() % 5 +1;
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
    for (size_t i = 0; i < 10; i++)
    {
        Table* temptable = new Table(4);
        freeTables.enqueue(temptable, -temptable->GetFreeSeats());
    }

    pUI->PrintCurrentState(0, actions, pendODG, pendODN, pendOT, pendOVN, pendOVC, pendOVG, pendCombo,
        availCS, availCN, cooking, readyOD, readyOT, readyOV, overWaitOVG, readyCombo,
        freeScooters, maintScooters, backScooters, freeTables, busySharable, inServOrders, cancelledOrders, finishedOrders);

    pUI->WaitForNextStep();
    //-------------------------while there are processing orders------------
    while (finishedOrders.GetCount() + cancelledOrders.GetCount() < 500)
    {
        //----------- 3.1 --------------
        for (size_t i = 0; i < 30;i++ )
        {
            if (pendODG.isEmpty() && pendODN.isEmpty() && pendOT.isEmpty()          // checks the lists
                && pendOVC.isEmpty() && pendOVG.isEmpty() && pendOVN.isEmpty()) {
                break;
            }
            int category = rand() % 6;
            // 0: ODG, 1: ODN, 2: OT, 3: OVC, 4: OVG, 5: OVN
            if (availCN.GetCount() == 0 && availCS.GetCount() == 0) break;     // check if there are chefs available
            switch (category)
            {
                case 0: // Dine-In Grilled
                {
                    Order* o = nullptr;
                    if (pendODG.peek(o))        //check the list
                    {
                        if (assignToChef(o)) {      // if the order is assigned to a chef the order will automatically moved to cooking
                            pendODG.dequeue(o);        // dequeue that order
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
            if (r <= 0.75) {        //with propability of 75%

                if (cooking.isEmpty()) {    // check the cooking list
                    break;
                }
                int pri;
                Order* od = nullptr;
                if (cooking.dequeue(od, pri)) {
                    DeliveryOrder* deliv = dynamic_cast<DeliveryOrder*>(od);    //check the type of the order by dynamic cast
                    if (deliv != nullptr) {
                        freeOrderChef(od);      //free the order chef
                        readyOV.enqueue(od);    //move the order to its ready queue
                        continue;
                    }
                    DineInOrder* dinein = dynamic_cast<DineInOrder*>(od);
                    if (dinein != nullptr) {
                        freeOrderChef(od);
                        readyOD.enqueue(od);
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
        for (size_t i = 0; i < 10;i++ )
        {
            if (readyOD.isEmpty() && readyOT.isEmpty() && readyOV.isEmpty()) {  //check the ready lists
                break;
            }
            int ready = rand() % 3; // randomly choose a list
            Order* od = nullptr;
            switch (ready)
            {
            case 0:
            {
                if (readyOD.peek(od))   //check the list
                {
                    if (assignToTable(od)) {    //if the order is assigned to table, it will move to service list
                        readyOD.dequeue(od);    //dequeue it from the ready list
                    }
                }
                break;
            }
            case 1:
            {
                if (readyOT.dequeue(od)) {
                    finishedOrders.push(od);
                }
                break;
            }
            case 2:
            {
                if (readyOV.peek(od))
                {
                    if (assignToScooter(od)) {
                        readyOV.dequeue(od);
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
        if (r2 <= 0.25) {   //with probapility of 25 
            Order* servedorder = nullptr;
            int pri;
            if (inServOrders.dequeue(servedorder, pri)) {   //check the inservice list
                finishedOrders.push(servedorder);   //push the order to the finished list
                DineInOrder* dinein = dynamic_cast<DineInOrder*>(servedorder);  //if t is a dinein free its table
                if (dinein != nullptr) {
                    freeOrderTable(dinein);
                }
                else {      //if not then it is delivery free its scooter
                    DeliveryOrder* deliv = (DeliveryOrder*)servedorder;
                    freeOrderScooter(deliv);        //return the scooter to back_scooters
                }
            }
        }


        //---------------- 3.8 -------------------
        double r3 = dist(gen);
        if (r3 <= 0.50) {       //with probability of 50
            Scooter* backscooter = nullptr;
            int pri;
            if (backScooters.dequeue(backscooter, pri)) {   //check the back scooters
                int distination = rand() % 2;   //random 0 or 1
                switch (distination) {      //move it to , maintenence or free scooters
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
        if (r4 <= 0.50) {       //with probability of 50
            Scooter* backscooter = nullptr;
            int pri;
            if (maintScooters.dequeue(backscooter)) {   //check the maintenence
                freeScooters.enqueue(backscooter, -backscooter->GetDistance());     //move it to the free scooters
            }
        }
        

        //---------------- 3.10 -------------------
        pUI->PrintCurrentState(currentTime, actions, pendODG, pendODN, pendOT, pendOVN, pendOVC, pendOVG, pendCombo,
            availCS, availCN, cooking, readyOD, readyOT, readyOV, overWaitOVG, readyCombo,
            freeScooters, maintScooters, backScooters, freeTables, busySharable, inServOrders, cancelledOrders, finishedOrders);
        currentTime++;
        pUI->WaitForNextStep();
    }
}

//----------------------------------------------------------------------------------//
//------------------------- Chefs functions -----------------------------------------//
//----------------------------------------------------------------------------------//
bool Restaurant::assignToChef(Order* od)   // need to update the chefs busy time
{
    // if the order is dinein
    DineInOrder* dinein = dynamic_cast<DineInOrder*>(od);
    if (dinein != nullptr) {        
        char type = od->getType()[2];
        bool assigned = false;
        Chef* tempChef = nullptr;
        if (type == 'G') {          //check the exact type
            if (availCS.GetCount() > 0) {
                assigned = availCS.dequeue(tempChef);
                dinein->setAssignedChef(tempChef);
            }
        }
        else
        {
            if (availCN.GetCount() > 0) {
                assigned = availCN.dequeue(tempChef);
            }
            else if (availCS.GetCount() > 0) {
                assigned = availCS.dequeue(tempChef);
            }
            dinein->setAssignedChef(tempChef);
        }
        if (assigned) {
            od->setTA(currentTime);
            cooking.enqueue(od, -od->getExpectedReadyTime(od->getAssignedChef()->getSpeed())); // put the order in the cooking list
        }
        return assigned;
    }

    //if the order is delivery
    DeliveryOrder* deliv = dynamic_cast<DeliveryOrder*>(od);
    if (deliv != nullptr) {
        char type = od->getType()[2];
        Chef* tempChef = nullptr;
        bool assigned = false;
        if (type == 'G') {
            if (availCS.GetCount() > 0) {
                assigned = availCS.dequeue(tempChef);
                deliv->setAssignedChef(tempChef);
            }
        }
        else if(type == 'N')
        {
            if (availCN.GetCount() > 0) {
                assigned = availCN.dequeue(tempChef);
                deliv->setAssignedChef(tempChef);
            }
        }
        else {
            if (availCN.GetCount() > 0) {
                assigned = availCN.dequeue(tempChef);
            }
            else if (availCS.GetCount() > 0) {
                assigned = availCS.dequeue(tempChef);
            }
            deliv->setAssignedChef(tempChef);
        }
        if (assigned) {
            od->setTA(currentTime);
            cooking.enqueue(od, -od->getExpectedReadyTime(od->getAssignedChef()->getSpeed()));
        }

        return assigned;
    }

    // if the order is takeaway
    TakeawayOrder* take = dynamic_cast<TakeawayOrder*>(od);
    if (take != nullptr) {
        char type = od->getType()[2];
        bool assigned = false;
        Chef* tempChef = nullptr;

        if (availCN.GetCount() > 0) {
            assigned = availCN.dequeue(tempChef);
        }
        take->setAssignedChef(tempChef);

        if (assigned) {
            od->setTA(currentTime);
            cooking.enqueue(od, -od->getExpectedReadyTime(od->getAssignedChef()->getSpeed()));
        }

        return assigned;
    }

    return false;
}


bool Restaurant::freeOrderChef(Order* od)
{
    if (od->getAssignedChef() != nullptr) {
        // Accumulate chef busy time.
        if (od->getTA() != -1 && od->getTR() != -1) totalChefBusyTime += od->getTR() - od->getTA();
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

//----------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------//



//----------------------------------------------------------------------------------//
//------------------------- Tables functions ---------------------------------------//
//----------------------------------------------------------------------------------//
bool Restaurant::assignToTable(Order* od)
{
    DineInOrder* dinein = (DineInOrder*)od;
    if (!dinein)     return false;
    Table* temptable = nullptr;
    if (dinein->getCanShare()== true)    
    {
        temptable = busySharable.GetBest(dinein->getSeats());// confirm if the it allow sharing
        if (temptable) RemoveTable(busySharable, temptable->GetId()); //if we found in busysharable remove it 
    }
    if (temptable == nullptr) {
        temptable = freeTables.GetBest(dinein->getSeats());
        if (temptable) RemoveTable(freeTables, temptable->GetId());
    }
    if (temptable != nullptr) {     // if i got a table
        dinein->setAssignedTable(temptable);    // assign it
        temptable->setBusySeats(dinein->getSeats()); // update free seats
        if (temptable->GetFreeSeats() > 0 && dinein->getCanShare()==true) {        // put it in the proper list
            busySharable.enqueue(temptable, -temptable->GetFreeSeats());// if there is some empty seat and the customer can share
        }
        else busyNoShare.enqueue(temptable,-temptable->GetFreeSeats());//if it is completely full or  the customer refused to share

        od->setTS(currentTime); // update the TS
        inServOrders.enqueue(od, -dinein->getExpectedFinishTime());   // move the order to inservice
        return true;
    }
    return false;
}

bool Restaurant::freeOrderTable(DineInOrder* dinorder)
{
    Table* temptable = dinorder->getAssignedTable();    // get the order table
    if (temptable) {// the table is possible in either busySharable or busyNoshare so remove it in both
        RemoveTable(busySharable, temptable->GetId());
        RemoveTable(busyNoShare, temptable->GetId());
        temptable->freeSeats(dinorder->getSeats());
        dinorder->setAssignedTable(nullptr);
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

bool Restaurant::RemoveTable(Fit_Tables& t, int id)
{
    Table* tempTable;
    int pri;
    Fit_Tables temp;
    bool found = false;
    while (t.dequeue(tempTable, pri))
    {
        if (!found && tempTable->GetId() == id)
        {
            found = true;
            continue;
        }
        temp.enqueue(tempTable, pri);
    }
    while (temp.dequeue(tempTable, pri)) t.enqueue(tempTable, pri);
    return found;
}
//----------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------//



//----------------------------------------------------------------------------------//
//------------------------- Scooters functions -------------------------------------//
//----------------------------------------------------------------------------------//
bool Restaurant::assignToScooter(Order* od)
{
    DeliveryOrder* deliv = (DeliveryOrder*)od;
    Scooter* tempscooter = nullptr;
    int pri;
    freeScooters.dequeue(tempscooter, pri);
    if (tempscooter != nullptr) {       // if i got a scooter
        deliv->setAssignedScooter(tempscooter);
        od->setTS(currentTime);
        inServOrders.enqueue(od, -deliv->getExpectedFinishTime());
        return true;
    }
    return false;
}

bool Restaurant::freeOrderScooter(DeliveryOrder* deliorder) // this is called before the check scooters
{
    Scooter* tempscooter = deliorder->getAssignedScooter();     // get the order scooter
    if (tempscooter) {
        int tripTime = (int)ceil(deliorder->getDistance() / tempscooter->getSpeed());// Time needed to reach customer
        totalScooterBusyTime += 2 * tripTime;  // going to customer + returning to restaurant
        tempscooter->incDist(deliorder->getDistance() * 2);     // increase total distance (it *2 because it needs to go and comeback)
        tempscooter->incrementDeliOreders();       // increase the orders delivered
        backScooters.enqueue(tempscooter, -(ceil(deliorder->getDistance() / tempscooter->getSpeed()) + currentTime)); // put it in scooters back ordered by the time step they will comeback they will cut back
        deliorder->setAssignedScooter(nullptr);     // free the pointer
        return true;
    }

    return false;
}
void Restaurant::check_scooters_lists()
{
    Scooter* tempscooter = nullptr;
    int pri;
    while (backScooters.peek(tempscooter, pri)) {   //check the back scooters
        if (-pri > currentTime) break;
        backScooters.dequeue(tempscooter, pri);
        if (tempscooter->getDeliOrders() >= Main_Ords) {
            tempscooter->setTmain(currentTime);
            maintScooters.enqueue(tempscooter);
        }
        else freeScooters.enqueue(tempscooter, -tempscooter->GetDistance());
    }

    tempscooter = nullptr;
    while (maintScooters.peek(tempscooter)) {       //check the maint scooter
        if (tempscooter->getFinishMaint() > currentTime) break;
        maintScooters.dequeue(tempscooter);
        tempscooter->resetTmaint();     //reset the maint finish time
        tempscooter->resetDeliOreders();    // reset the delivered orders
        freeScooters.enqueue(tempscooter, -tempscooter->GetDistance());
    }
}
//----------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------//


//----------------------------------------------------------------------------------//
//------------------------- Order functions ---------------------------------------//
//----------------------------------------------------------------------------------//
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
        // The order was cancelled while cooking.
        cancelledorder->setTR(currentTime);// Set TR to currentTime so freeOrderChef() can count chef busy time.
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

bool Restaurant::AddOrderToPending(Order* o)
{
    string typ = o->getType();
    if (typ == "ODN") {
        pendODN.enqueue(o);
        numODN++;
        return true;
    }
    else if (typ == "ODG") {
        pendODG.enqueue(o);
        numODG++;
        return true;
    }
    else if (typ == "OT") {
        pendOT.enqueue(o);
        numOT++;
        return true;
    }
    else if (typ == "OVC") {
        pendOVC.enqueue(o);
        numOVC++;
        return true;
    }
    else if (typ == "OVN") {
        pendOVN.enqueue(o);
        numOVN++;
        return true;
    }
    else if (typ == "OVG") {
        DeliveryOrder* deliv = (DeliveryOrder*)o;
        int pri = deliv->getOVGPriority(2, 2, 1);
        pendOVG.enqueue(o, pri);
        numOVG++;
        return true;
    }
    else if (typ == "OC") {
        pendCombo.enqueue(o);
        numCombo++;
        return true;
    }
    else return false;
}

void Restaurant::check_inservice_orders()
{
    Order* temporder = nullptr;
    int pri;
    while (inServOrders.peek(temporder, pri)) {
        if (-pri > currentTime) break;
        inServOrders.dequeue(temporder, pri);

        // if the order is COMBO
        ComboOrder* combo = dynamic_cast<ComboOrder*>(temporder);
        if (combo != nullptr) {
            freeComboScooters(combo);
            temporder->setTF(currentTime);
            finishedOrders.push(temporder);
            continue;
        }

        // if the order is dinein
        DineInOrder* dinein = dynamic_cast<DineInOrder*>(temporder);
        if (dinein != nullptr) {
            freeOrderTable(dinein);
            temporder->setTF(currentTime);
            finishedOrders.push(temporder);
            continue;
        }

        //if the order is delivery
        DeliveryOrder* deliv = dynamic_cast<DeliveryOrder*>(temporder);
        if (deliv != nullptr) {
            freeOrderScooter(deliv);
            temporder->setTF(currentTime);
            finishedOrders.push(temporder);
            continue;
        }

    }
}

void Restaurant::check_overwait_orders()
{
    LinkedQueue<Order*> tempqueue;
    Order* temporder;
    while (readyOV.dequeue(temporder)) {
        if (temporder->getType() == "OVG" && temporder->getTR() != -1 && (currentTime - temporder->getTR() > TH)) {
            overWaitOVG.enqueue(temporder, -temporder->getTQ());
            overwaitCount++;
            continue;
        }
        tempqueue.enqueue(temporder);
    }
    while (tempqueue.dequeue(temporder)) {
        readyOV.enqueue(temporder);
    }
}

void Restaurant::check_cooking_orders()
{
    Order* temporder = nullptr;
    int pri;
    while (cooking.peek(temporder, pri)) {
        if (-pri > currentTime) break;
        cooking.dequeue(temporder, pri);
        temporder->setTR(currentTime);

        // if the order is COMBO - free all chefs, move to readyCombo
        ComboOrder* combo = dynamic_cast<ComboOrder*>(temporder);
        if (combo != nullptr) {
            freeComboChefs(combo);
            readyCombo.enqueue(temporder);
            continue;
        }

        freeOrderChef(temporder);
        // if the order is dinein
        DineInOrder* dinein = dynamic_cast<DineInOrder*>(temporder);
        if (dinein != nullptr) {
            readyOD.enqueue(temporder);
            continue;
        }

        //if the order is delivery
        DeliveryOrder* deliv = dynamic_cast<DeliveryOrder*>(temporder);
        if (deliv != nullptr) {
            readyOV.enqueue(temporder);
            continue;
        }

        // if the order is takeaway
        TakeawayOrder* take = dynamic_cast<TakeawayOrder*>(temporder);
        if (take != nullptr) {
            readyOT.enqueue(temporder);
            continue;
        }
    }
}

void Restaurant::check_ready_orders()
{
    // COMBO orders have highest priority for scooter assignment
    Order* comboTemp = nullptr;
    while (readyCombo.peek(comboTemp)) {
        ComboOrder* combo = dynamic_cast<ComboOrder*>(comboTemp);
        if (combo && assignComboToScooters(combo)) {
            readyCombo.dequeue(comboTemp);
        } else {
            break;
        }
    }

    //first OD order
    Order* od = nullptr;
    int pri;
    while(readyOD.peek(od))   //check the list
    {
        if (assignToTable(od)) {    //if the order is assigned to table, it will move to service list
            readyOD.dequeue(od);    //dequeue it from the ready list
        }
        else break;
    }
   
    while(readyOT.peek(od)) {
        if (od->getTR() == -1 || currentTime - od->getTR() < 1) break;
        readyOT.dequeue(od);
        od->setTS(currentTime);//to avoid making TS = -1
        od->setTF(currentTime);
        finishedOrders.push(od);
    }
    while (overWaitOVG.peek(od,pri)) {      //first assign all overwait OVG
        if (assignToScooter(od)) {
            overWaitOVG.dequeue(od, pri);
        }
        else break;
    }
    // Assign OVC orders first from readyOV.
    int n = readyOV.GetCount();
    for (int i = 0; i < n; i++)
    {
        readyOV.dequeue(od);
        if (od->getType() == "OVC" && !freeScooters.isEmpty())  assignToScooter(od);
        else  readyOV.enqueue(od);// if not OVC, or no scooter available return order back to readyOV
    }
    //Assign remaining readyOV orders by FCFS.
    while(readyOV.peek(od)) {
        if (assignToScooter(od)) {      // if order is assigned to scooter, it will be moved to servise list
            readyOV.dequeue(od);
        }
        else break;
    }
}

void Restaurant::AssignPendingToChefs()
{
    // COMBO orders have highest priority - assign them first
    Order* comboTemp = nullptr;
    while (pendCombo.peek(comboTemp)) {
        ComboOrder* combo = dynamic_cast<ComboOrder*>(comboTemp);
        if (combo && assignComboToChefs(combo)) {
            pendCombo.dequeue(comboTemp);
        } else {
            break;
        }
    }

    while (true) {
        bool hasCN = (availCN.GetCount() > 0);
        bool hasCS = (availCS.GetCount() > 0);
        if (!hasCN && !hasCS) {
            break;
        }

        Order* oldestOrder = nullptr;
        int minTime = INT16_MAX;

        Order* temp = nullptr;
        int pri = 0;

        if (hasCS && pendODG.peek(temp)) {
            if (temp->getTQ() < minTime) { minTime = temp->getTQ(); oldestOrder = temp; }
        }

        if ((hasCN || hasCS) && pendODN.peek(temp)) {
            if (temp->getTQ() < minTime) { minTime = temp->getTQ(); oldestOrder = temp; }
        }

        if (hasCN && pendOT.peek(temp)) {
            if (temp->getTQ() < minTime) { minTime = temp->getTQ(); oldestOrder = temp; }
        }

        if (hasCS && pendOVG.peek(temp, pri)) {
            if (temp->getTQ() < minTime) { minTime = temp->getTQ(); oldestOrder = temp; }
        }

        if ((hasCN || hasCS) && pendOVC.peek(temp)) {
            if (temp->getTQ() < minTime) { minTime = temp->getTQ(); oldestOrder = temp; }
        }

        if (hasCN && pendOVN.peek(temp)) {
            if (temp->getTQ() < minTime) { minTime = temp->getTQ(); oldestOrder = temp; }
        }

        if (oldestOrder == nullptr) {
            break;
        }

        bool assigned = assignToChef(oldestOrder);

        if (assigned) {
            string type = oldestOrder->getType();

            if (type == "ODG") {
                pendODG.dequeue(temp);
            }
            else if (type == "ODN") {
                pendODN.dequeue(temp);
            }
            else if (type == "OT") {
                pendOT.dequeue(temp);
            }
            else if (type == "OVC") {
                pendOVC.dequeue(temp);
            }
            else if (type == "OVG") {
                pendOVG.dequeue(temp, pri);
            }
            else if (type == "OVN") {
                pendOVN.dequeue(temp);
            }


            else {
                break;
            }
        }
    }
}
void Restaurant::check_action_list()
{
    Action* tempaction;
    while (actions.peek(tempaction)) {
        RequestAction* req = dynamic_cast<RequestAction*>(tempaction);
        if (req != nullptr) {
            if (req->getTQ() > currentTime) return;
            actions.dequeue(tempaction);
            tempaction->Act();
        }
        else {
            CancelAction* cancel = dynamic_cast<CancelAction*>(tempaction);
            if (cancel != nullptr) {
                if (cancel->getTcancel() > currentTime) return;
                actions.dequeue(tempaction);
                tempaction->Act();
            }
        }
    }
}
//----------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------//



//----------------------------------------------------------------------------------//
//------------------------- COMBO functions ----------------------------------------//
//----------------------------------------------------------------------------------//
bool Restaurant::assignComboToChefs(ComboOrder* combo)
{
    int needed = combo->getNumChefsNeeded();
    // Must have at least 1 CS
    if (availCS.GetCount() < 1) return false;
    // Must have enough total chefs
    int totalAvail = availCN.GetCount() + availCS.GetCount();
    if (totalAvail < needed) return false;

    // Assign 1 CS first (required)
    Chef* cs = nullptr;
    availCS.dequeue(cs);
    combo->addChef(cs);
    cs->assignOrder();

    // Assign remaining from CN first, then CS
    for (int i = 1; i < needed; i++) {
        Chef* chef = nullptr;
        if (availCN.GetCount() > 0) {
            availCN.dequeue(chef);
        }
        else if (availCS.GetCount() > 0) {
            availCS.dequeue(chef);
        }
        if (chef) {
            combo->addChef(chef);
            chef->assignOrder();
        }
    }

    // Set primary chef for base class compatibility
    combo->setAssignedChef(combo->getChefAt(0));
    combo->setTA(currentTime);

    // Cooking time based on combined speed of all assigned chefs
    float totalSpeed = combo->getTotalChefSpeed();
    int readyTime = combo->getTA() + (int)ceil((float)combo->getSize() / totalSpeed);
    cooking.enqueue(combo, -readyTime);

    return true;
}

void Restaurant::freeComboChefs(ComboOrder* combo)
{
    // Accumulate chef busy time
    int busyTime = 0;
    if (combo->getTA() != -1 && combo->getTR() != -1)
        busyTime = combo->getTR() - combo->getTA();

    for (int i = 0; i < combo->getChefCount(); i++) {
        Chef* chef = combo->getChefAt(i);
        if (chef) {
            totalChefBusyTime += busyTime;
            chef->releaseOrder();
            if (chef->getType() == "CS")
                availCS.enqueue(chef);
            else
                availCN.enqueue(chef);
        }
    }
    combo->clearAllChefs();
}

bool Restaurant::assignComboToScooters(ComboOrder* combo)
{
    int needed = combo->getNumScootersNeeded();
    if (freeScooters.GetCount() < needed) return false;

    for (int i = 0; i < needed; i++) {
        Scooter* sco = nullptr;
        int pri;
        freeScooters.dequeue(sco, pri);
        if (sco) {
            combo->addScooter(sco);
        }
    }

    combo->setTS(currentTime);
    inServOrders.enqueue(combo, -combo->getExpectedFinishTime());
    return true;
}

void Restaurant::freeComboScooters(ComboOrder* combo)
{
    for (int i = 0; i < combo->getScooterCount(); i++) {
        Scooter* sco = combo->getScooterAt(i);
        if (sco) {
            int tripTime = (int)ceil(combo->getDistance() / sco->getSpeed());
            totalScooterBusyTime += 2 * tripTime;
            sco->incDist((int)(combo->getDistance() * 2));
            sco->incrementDeliOreders();
            backScooters.enqueue(sco, -(int)(ceil(combo->getDistance() / sco->getSpeed()) + currentTime));
        }
    }
    combo->clearAllScooters();
}

//----------------------------------------------------------------------------------//
//------------------------- Files functions ----------------------------------------//
//----------------------------------------------------------------------------------//
bool Restaurant::LoadInputFile(const string& filename)
{
    ifstream inputFile(filename);
    if (!inputFile.is_open())   return false;

    float CN_Speed, CS_Speed;
    int Scooter_Speed, Main_Dur, M;// M for the number of orders action (i changed it to actionscount)
    inputFile >> num_CN >> num_CS;
    inputFile >> CN_Speed >> CS_Speed;
    for (int i = 0; i < num_CN; i++)    availCN.enqueue(new Chef("CN", CN_Speed));
    for (int i = 0; i < num_CS; i++)    availCS.enqueue(new Chef("CS", CS_Speed));
    inputFile >> Scooter_Count >> Scooter_Speed;
    inputFile >> Main_Ords >> Main_Dur;
    for (int i = 0; i < Scooter_Count; i++)
    {
        Scooter* sco = new Scooter(Scooter_Speed, Main_Dur);
        freeScooters.enqueue(sco,-sco->GetDistance());
    }
    inputFile >> total_Table;
    int createdTable = 0;
    while (createdTable < total_Table)
    {
        int Count, capacity;
        inputFile >> Count >> capacity;
        for (int i = 0; i < Count && createdTable < total_Table; i++)
        {
            Table* table = new Table(capacity);
            freeTables.enqueue(table, -table->GetFreeSeats());
            createdTable++;
        }
    }
    inputFile >> TH;
    inputFile >> M;
    for (int i = 0; i < M; i++)
    {
        char letter;
        inputFile >> letter;
        if (letter == 'Q')
        {
            string TYP;
            int TQ, ID, SIZE;
            float Price;
            inputFile >> TYP >> TQ >> ID >> SIZE >> Price;
            Action* action = nullptr;
            if (TYP == "ODG" || TYP == "ODN")
            {
                int seats, duration;
                bool canShare;
                char chshare;
                inputFile >> seats >> duration >> chshare;
                canShare = chshare == 'Y';
                action = new RequestAction(this, TYP, TQ, ID, SIZE, Price, seats, duration, canShare);
                actions.enqueue(action);
            }
            else if (TYP == "OVC" || TYP == "OVG" || TYP == "OVN")
            {
                float distance;
                inputFile >> distance;
                action = new RequestAction(this,TYP, TQ, ID, SIZE, Price, distance);
                actions.enqueue(action);
            }
            else if (TYP == "OT")
            {
                action = new RequestAction(this,TYP, TQ, ID, SIZE, Price);
                actions.enqueue(action);
            }
            else if (TYP == "OC")
            {
                float distance;
                int numChefs, numScooters;
                inputFile >> distance >> numChefs >> numScooters;
                action = new RequestAction(this, TYP, TQ, ID, SIZE, Price, distance, numChefs, numScooters);
                actions.enqueue(action);
            }
            orderCount++;
        }
        else if (letter == 'X')
        {
            int Tcancel, ID;
            inputFile >> Tcancel >> ID;
            Action* act = new CancelAction(this,Tcancel, ID);
            actions.enqueue(act);
        }
    }
    inputFile.close();
    return true;
}

bool Restaurant::GenerateOutputFile(const string& filename)
{
    ofstream outputFile(filename);
    if (!outputFile.is_open())  return false;

    // These sums are used to calculate averages for finished orders
    double sumTi = 0, sumTc = 0, sumTw = 0, sumTserv = 0;

    // Required output header for each finished order
    outputFile << "TF \tID \tTQ \tTA \tTR \tTS \tTi \tTC \tTw \tTserv\n";

    ArrayStack<Order*> tempFinish;    // Temporary stack to restore finishedOrders
    Order* o = nullptr;
    while (finishedOrders.pop(o))
    {
        outputFile << o->getTF() << "\t"<< o->getID() << "\t"<< o->getTQ() << "\t"<< o->getTA() << "\t"<< o->getTR() << "\t" << o->getTS() << "\t"<< o->getTi() << "\t"<< o->getTc() << "\t"<< o->getTw() << "\t"<< o->getTserv() << "\n";

        // The checks are only safety checks.In a correct finished order, these values should never be -1
        if (o->getTi() != -1)    sumTi += o->getTi();
        if (o->getTc() != -1)    sumTc += o->getTc();
        if (o->getTw() != -1)    sumTw += o->getTw();
        if (o->getTserv() != -1) sumTserv += o->getTserv();

        tempFinish.push(o);
    }
    while (tempFinish.pop(o))   finishedOrders.push(o); // Restore finishedOrders exactly as it was

    int fi = finishedOrders.GetCount();// Number of finished orders
    // Averages for all finished orders. // If fi == 0, keep averages as 0 to avoid division by zero.
    double avgTi = fi ? sumTi / fi : 0.0;
    double avgTc = fi ? sumTc / fi : 0.0;
    double avgTw = fi ? sumTw / fi : 0.0;
    double avgTserv = fi ? sumTserv / fi : 0.0;

    // Percentages// If orderCount  == 0, keep percentages as 0 to avoid division by zero.
    double finishedPercent = orderCount ? (fi * 100.0) / orderCount : 0.0;
    double cancelledPercent = orderCount ? (cancelledOrders.GetCount() * 100.0) / orderCount : 0.0;
    double overwaitPercent = orderCount ? (overwaitCount * 100.0) / orderCount : 0.0;

    int simTime = currentTime - 1;// Simulation time.

    // Chef utilization:
    int numChefs = num_CN + num_CS;
    double chefUtil = (simTime > 0 && numChefs > 0) ? (100.0 * totalChefBusyTime) / (simTime * numChefs) : 0.0;

    // Scooter utilization:
    double scooterUtil = (simTime > 0 && Scooter_Count > 0) ? (100.0 * totalScooterBusyTime) / (simTime * Scooter_Count): 0.0;

    outputFile << "\n================ Statistics ================\n\n";
    outputFile << "1- Total number of orders and total number of each order type = "<< orderCount << "\n\tODG = " << numODG<< "\n\tODN = " << numODN<< "\n\tOT  = " << numOT<< "\n\tOVC = " << numOVC<< "\n\tOVG = " << numOVG<< "\n\tOVN = " << numOVN << "\n\tCOMBO = " << numCombo << "\n";
    outputFile << "2- Total number of chefs and total number of each type = "<< numChefs << "\n\tCN = " << num_CN<< "\n\tCS = " << num_CS << "\n";
    outputFile << "3- Total number of scooters = " << Scooter_Count << "\n";
    outputFile << "4- Percentage of finished orders and percentage of cancelled orders"<< "\n\tFinished Orders % = " << finishedPercent<< "%\n\tCancelled Orders % = " << cancelledPercent << "%\n";
    outputFile << "5- Overwait Orders % = " << overwaitPercent << "%\n";
    outputFile << "6- Average for Ti TC Tw Tserv for all finished orders"<< "\n\tAverage Ti = " << avgTi<< "\n\tAverage TC = " << avgTc<< "\n\tAverage Tw = " << avgTw<< "\n\tAverage Tserv = " << avgTserv << "\n";
    outputFile << "7- Scooters Utilization % = " << scooterUtil << "%\n";
    outputFile << "8- Chefs Utilization % = " << chefUtil << "%\n";
    outputFile.close();
    return true;
}