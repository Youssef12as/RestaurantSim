#include "Restaurant.h"
#include <cstdlib>   // rand, srand
#include <ctime>     // time
#include <random>
#include"../actions/RequestAction.h"
#include"../actions/CancelAction.h"
#include <sstream>
Restaurant::Restaurant()
{
	pUI = new UI;
    currentTime = 1;
    num_CS = num_CN = Scooter_Count = total_Table = Main_Ords = TH = orderCount = 0;
    numODG = numODN = numOT = numOVC = numOVG = numOVN = 0;
    overwaitCount = 0;
    totalChefBusyTime = totalScooterBusyTime = 0;
    numCombo = 0;
    Rescue_Count = 0;
    rescueMissionCount = 0;
}

Restaurant::~Restaurant()
{
	delete pUI;
}


void Restaurant::main_simulation()
{
    string inFile = pUI->ReadInputFileName();   // get input file
    string errorReason;
    //load input file
    while (!ValidateInputFile(inFile, errorReason))
    {
        cout << " Invalid input file.\n"<< "Reason: " << errorReason << "\n";
        cout << "Simulation did not start.try Again!\n";
        inFile = pUI->ReadInputFileName();
    }
    cout << "Input file validation succeeded.\n";
    LoadInputFile(inFile);
    string outFile = pUI->ReadOutputFileName(); // get output file
    ProgramMode currentMode = pUI->ReadMode();   //get mode
    if (currentMode == ProgramMode::Silent) pUI->PrintStartSilent();

    if (currentMode == ProgramMode::Interactive) {
        pUI->PrintCurrentState(0, actions, pendODG, pendODN, pendOT, pendOVN, pendOVC, pendOVG, pendCombo,
            availCS, availCN, cooking, readyOD, readyOT, readyOV, overWaitOVG, readyCombo,
            freeScooters,freeRescueScooters,maintScooters,backScooters,rescueBackScooters,failedBackScooters,rescueMissionCount,freeTables,busySharable,
            inServOrders, cancelledOrders, finishedOrders);
    }
    cin.get();
    pUI->WaitForNextStep();
    while (!isSimulationFinished()) {
        
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
                freeScooters, freeRescueScooters, maintScooters, backScooters, rescueBackScooters, failedBackScooters, rescueMissionCount, freeTables, busySharable,
                inServOrders, cancelledOrders, finishedOrders);
            pUI->WaitForNextStep();
        }
        currentTime++;
    }
    GenerateOutputFile(outFile);
    if (currentMode == ProgramMode::Silent) pUI->PrintEndSilent();
}

bool Restaurant::isSimulationFinished() const
{
    //all orders must be either finished or cancelled
    bool allOrdersDone = finishedOrders.GetCount() + cancelledOrders.GetCount() >= orderCount;

    // Normal scooters should not be returning or in maintenance they should be in freeScooters
    bool allNormalScootersBack = backScooters.isEmpty() && failedBackScooters.isEmpty() && 
        maintScooters.isEmpty();

    //Rescue scooters should not be returning
    bool allRescueScootersBack = rescueBackScooters.isEmpty();

    return allOrdersDone && allNormalScootersBack && allRescueScootersBack;
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
        freeScooters, freeRescueScooters, maintScooters, backScooters, rescueBackScooters, failedBackScooters, rescueMissionCount, freeTables, busySharable,
        inServOrders, cancelledOrders, finishedOrders);

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
        

        //---------------- 3.10 ------------------- //print the current step stats
        pUI->PrintCurrentState(0, actions, pendODG, pendODN, pendOT, pendOVN, pendOVC, pendOVG, pendCombo,
            availCS, availCN, cooking, readyOD, readyOT, readyOV, overWaitOVG, readyCombo,
            freeScooters, freeRescueScooters, maintScooters, backScooters, rescueBackScooters, failedBackScooters, rescueMissionCount, freeTables, busySharable,
            inServOrders, cancelledOrders, finishedOrders);
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
        // reset rescue state
        deliv->clearFailure();
        deliv->setDeliveredByRescue(false);

        int tripTime = (int)ceil(deliv->getDistance() / tempscooter->getSpeed());
        int finishTime = currentTime + tripTime;

        bool fail = (rand() % 100 < 20);// probablity of failure 
        if (fail && tripTime > 1)
        {
            int failureTimeAfterservice = 1 + rand() % (tripTime - 1); // from 1 to to tripTime -1 
            int failureTime = currentTime + failureTimeAfterservice;

            float FailureDistance = failureTimeAfterservice * tempscooter->getSpeed();
            
            if (FailureDistance >= deliv->getDistance())// If failure point reaches or passes the customer,the order finishes normally.
            {
                deliv->clearFailure();
                deliv->setDeliveredByRescue(false);
                inServOrders.enqueue(deliv, -finishTime);
            }
            else
            {//scooter failed
                deliv->setFailureDistance(FailureDistance);
                inServOrders.enqueue(deliv, -failureTime);
            }
        }
        else { inServOrders.enqueue(od, -deliv->getExpectedFinishTime()); }
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
    while (failedBackScooters.peek(tempscooter, pri))
    {
        if (-pri > currentTime) break;
        failedBackScooters.dequeue(tempscooter, pri);
        tempscooter->setTmain(currentTime);
        maintScooters.enqueue(tempscooter);// must enter maitScooter after failing 
    }
    while (rescueBackScooters.peek(tempscooter, pri))
    {
        if (-pri > currentTime) break;
        rescueBackScooters.dequeue(tempscooter, pri);
        freeRescueScooters.enqueue(tempscooter, -tempscooter->GetDistance()); // Rescue scooter becomes available again
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
    if (typ == "Combo")// COMBO order is  the highest assignment priority 
    {
        pendCombo.enqueue(o);
        numCombo++;
        return true;
    }
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
    else return false;
}

void Restaurant::check_inservice_orders()
{
    Order* temporder = nullptr;
    int pri;
    while (inServOrders.peek(temporder, pri)) {
        if (-pri > currentTime) break;
        inServOrders.dequeue(temporder, pri);

        //if the order is combo
        ComboOrder* combo = dynamic_cast<ComboOrder*>(temporder);
        if (combo != nullptr)
        {
            freeComboScooters(combo);              // Free all scooters assigned to this combo order
            temporder->setTF(currentTime);         // Combo order is completely finished now
            finishedOrders.push(temporder);        // Move order to finished orders
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
            if (deliv->isDeliveredByRescue())
            {
                Scooter* rescue = deliv->getAssignedScooter();
                deliv->setTF(currentTime);
                if (rescue)
                {
                    int rescueReturnTime = (int)ceil(deliv->getDistance() / rescue->getSpeed());
                    rescue->incDist((int)ceil(deliv->getDistance() * 2));
                    
                    rescueBackScooters.enqueue(rescue, -(currentTime + rescueReturnTime));// Rescue scooter is go to backRescueScooters

                    deliv->setAssignedScooter(nullptr);// Remove scooter from order
                }
                finishedOrders.push(deliv);
                continue;
            }
            if (deliv->hasFailure())// if  scooter failed.
            {
                Scooter* failedScooter = deliv->getAssignedScooter();
                Scooter* rescue = nullptr;
                int rescuePri = 0;
                if (!freeRescueScooters.dequeue(rescue, rescuePri))// if there is no freeRescue wait the next step
                {
                    inServOrders.enqueue(deliv, -(currentTime + 1));
                    continue;
                }
                rescueMissionCount++;// Rescue start
                float failureDistance = deliv->getFailureDistance();

                int rescueTravelTime = (int)ceil(failureDistance / rescue->getSpeed());

                int rescueArrivalTime = currentTime + rescueTravelTime;
                if (failedScooter)
                {
                    int failedReturnTime = (int)ceil(failureDistance / failedScooter->getSpeed());
                    //from the time of service until waiting for the rescue + return to restaurant
                    totalScooterBusyTime += (rescueArrivalTime - deliv->getTS()) + failedReturnTime;
                    failedScooter->incDist((int)ceil(failureDistance * 2));//update the distance 
                    failedBackScooters.enqueue(failedScooter, -(rescueArrivalTime + failedReturnTime));
                }
                deliv->setAssignedScooter(rescue);
                deliv->setDeliveredByRescue(true);
                deliv->clearFailure();// remove failue we don't allow to fail again
                float remainingDistance = deliv->getDistance() - failureDistance;
                if (remainingDistance < 0) remainingDistance = 0;

                //Rescue scooter completes the remaining distance   
                int rescueDeliveryTime = (int)ceil(remainingDistance / rescue->getSpeed());
                int rescueFinishTime = rescueArrivalTime + rescueDeliveryTime;

                inServOrders.enqueue(deliv, -rescueFinishTime);
                continue;
            }
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
        // if teh order is  COMBO order
        ComboOrder* combo = dynamic_cast<ComboOrder*>(temporder); // Check combo first
        if (combo != nullptr)
        {
            freeComboChefs(combo);// free all chefs assigned to this combo
            readyCombo.enqueue(temporder);// Move combo to ready combo list
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
    Order* od = nullptr;
    int pri;
    // Combo orders first
    while (readyCombo.peek(od))
    {
        if (assignComboToScooters(od))
            readyCombo.dequeue(od);
        else    break;
    }
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
        od->setTS(od->getTR()); // Packing starts when the order becomes ready
        od->setTF(od->getTR() + 1); // Customer picks it after 1 timestep
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
    while (true) {
        Order* comboOrder = nullptr;
        if (pendCombo.peek(comboOrder))// Check if there is any pending combo order
        {
            if (assignComboToChefs(comboOrder))// Try to assign all required chefs to combo
            {
                pendCombo.dequeue(comboOrder);// Remove combo from pending only after Assigning
                continue; // Continue loop to try assigning more Orders
            }
            else     break; // Combo exists but cannot get enough chefs so stop because combo has highest priority
        }

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
//------------------------- Files functions ----------------------------------------//
//----------------------------------------------------------------------------------//
bool ReadNonEmptyLine(ifstream& inputFile, string& currentLine, int& lineNumber, string& errorReason, const string& expectedLineName)
{
    while (getline(inputFile, currentLine))
    {
        lineNumber++;
        if (currentLine.empty())        continue;//Ignore this empty line and continue
        return true;
    }
    // So the expected line is missing.we reach the end of file 
    errorReason = "Missing line: " + expectedLineName;
    return false;
}
bool HasExtraValues(istringstream& lineReader)//Check if there are extra unexpected values in the current line.
{
    string extraValue;
    if (lineReader >> extraValue)        return true;
    return false;
}
bool QueueId(LinkedQueue<int>& idQueue, int targetId)
{
    int numberOfIds = idQueue.GetCount();
    bool found = false;
    for (int i = 0; i < numberOfIds; i++)
    {
        int currentId = 0;
        idQueue.dequeue(currentId);
        if (currentId == targetId)        found = true;
        idQueue.enqueue(currentId);
    }
    return found;
}
bool Restaurant::ValidateInputFile(const string& filename, string& errorReason) const
{
    ifstream inputFile(filename);
    if (!inputFile.is_open())
    {
        errorReason = "File cannot be opened.";
        return false;
    }
    string currentLine;
    int lineNumber = 0;
    int normalChefCount = 0, specialChefCount = 0;

    // 1) Read CN CS
    if (!ReadNonEmptyLine(inputFile, currentLine, lineNumber, errorReason, "CN CS"))        return false;

    istringstream chefCountLine(currentLine);
    if (!(chefCountLine >> normalChefCount >> specialChefCount) || HasExtraValues(chefCountLine))
    {
        errorReason = "Line " + to_string(lineNumber) + ": expected exactly 2 values: CN CS.";
        return false;
    }
    if (normalChefCount < 0 || specialChefCount < 0)
    {
        errorReason = "Line " + to_string(lineNumber) + ": chef counts cannot be negative.";
        return false;
    }
    if (normalChefCount + specialChefCount == 0)
    {
        errorReason = "Line " + to_string(lineNumber) + ": restaurant must have at least one chef.";
        return false;
    }
    float normalChefSpeed = 0, specialChefSpeed = 0;
    // 2) Read CN_speed CS_speed
    if (!ReadNonEmptyLine(inputFile, currentLine, lineNumber, errorReason, "CN_speed CS_speed"))     return false;
    istringstream chefSpeedLine(currentLine);
    if (!(chefSpeedLine >> normalChefSpeed >> specialChefSpeed) || HasExtraValues(chefSpeedLine))
    {
        errorReason = "Line " + to_string(lineNumber) + ": expected exactly 2 values: CN_speed CS_speed.";
        return false;
    }
    if (normalChefSpeed <= 0 || specialChefSpeed <= 0)
    {
        errorReason = "Line " + to_string(lineNumber) + ": chef speeds must be greater than zero.";
        return false;
    }
    int scooterCount = 0, scooterSpeed = 0;
    // 3) Read S_count S_speed
    if (!ReadNonEmptyLine(inputFile, currentLine, lineNumber, errorReason, "S_count S_speed"))  return false;
    istringstream scooterLine(currentLine);
    if (!(scooterLine >> scooterCount >> scooterSpeed) || HasExtraValues(scooterLine))
    {
        errorReason = "Line " + to_string(lineNumber) + ": expected exactly 2 values: S_count S_speed.";
        return false;
    }
    if (scooterCount < 0)
    {
        errorReason = "Line " + to_string(lineNumber) + ": scooter count cannot be negative.";
        return false;
    }
    if (scooterSpeed <= 0)
    {
        errorReason = "Line " + to_string(lineNumber) + ": scooter speed must be greater than zero.";
        return false;
    }
    int rescueScooterCount = 0;
    // 4) Read Rescue_Count
    if (!ReadNonEmptyLine(inputFile, currentLine, lineNumber, errorReason, "Rescue_Count"))  return false;
    istringstream rescueScooterLine(currentLine);
    if (!(rescueScooterLine >> rescueScooterCount) || HasExtraValues(rescueScooterLine))
    {
        errorReason = "Line " + to_string(lineNumber) + ": expected exactly 1 value: Rescue_Count.";
        return false;
    }
    if (rescueScooterCount < 0)
    {
        errorReason = "Line " + to_string(lineNumber) + ": Rescue_Count cannot be negative.";
        return false;
    }
    int maintenanceOrder = 0, maintenanceDuration = 0;

    // 5) Read Main_Ords Main_Dur
    if (!ReadNonEmptyLine(inputFile, currentLine, lineNumber, errorReason, "Main_Ords Main_Dur")) return false;
    istringstream maintenanceLine(currentLine);
    if (!(maintenanceLine >> maintenanceOrder >> maintenanceDuration) || HasExtraValues(maintenanceLine))
    {
        errorReason = "Line " + to_string(lineNumber) + ": expected exactly 2 values: Main_Ords Main_Dur.";
        return false;
    }
    if (maintenanceOrder <= 0)
    {
        errorReason = "Line " + to_string(lineNumber) + ": Main_Ords must be greater than zero.";
        return false;
    }
    if (maintenanceDuration < 0)
    {
        errorReason = "Line " + to_string(lineNumber) + ": Main_Dur cannot be negative.";
        return false;
    }
    int totalTableCount = 0;
    // 6) Read Total_no_of_Tables
    if (!ReadNonEmptyLine(inputFile, currentLine, lineNumber, errorReason, "Total_no_of_Tables"))return false;

    istringstream totalTablesLine(currentLine);
    if (!(totalTablesLine >> totalTableCount) || HasExtraValues(totalTablesLine))
    {
        errorReason = "Line " + to_string(lineNumber) + ": expected exactly 1 value: Total_no_of_Tables.";
        return false;
    }
    if (totalTableCount <= 0)
    {
        errorReason = "Line " + to_string(lineNumber) + ": total number of tables must be greater than zero.";
        return false;
    }
    int createdTableCount = 0, maximumTableCapacity = 0;
    // 7) Read Table_count Capacity pairs
    if (!ReadNonEmptyLine(inputFile, currentLine, lineNumber, errorReason, "Table_count Capacity pairs"))  return false;
    istringstream tablePairsLine(currentLine);
    while (createdTableCount < totalTableCount)
    {
        int tableCount = 0, tableCapacity = 0;
        if (!(tablePairsLine >> tableCount >> tableCapacity))
        {
            errorReason = "Line " + to_string(lineNumber) + ": missing or incomplete table count/capacity pair.";
            return false;
        }
        if (tableCount <= 0)
        {
            errorReason = "Line " + to_string(lineNumber) + ": table count must be greater than zero.";
            return false;
        }

        if (tableCapacity <= 0)
        {
            errorReason = "Line " + to_string(lineNumber) + ": table capacity must be greater than zero.";
            return false;
        }
        createdTableCount += tableCount;
        if (tableCapacity > maximumTableCapacity)            maximumTableCapacity = tableCapacity;
        if (createdTableCount > totalTableCount)
        {
            errorReason = "Line " + to_string(lineNumber) + ": table counts exceed Total_no_of_Tables.";
            return false;
        }
    }
    if (HasExtraValues(tablePairsLine))
    {
        errorReason = "Line " + to_string(lineNumber) + ": too many table count/capacity values.";
        return false;
    }
    int overwaitThreshold = 0;
    // 8) Read TH
    if (!ReadNonEmptyLine(inputFile, currentLine, lineNumber, errorReason, "TH"))        return false;
    istringstream overwaitThresholdLine(currentLine);
    if (!(overwaitThresholdLine >> overwaitThreshold) || HasExtraValues(overwaitThresholdLine))
    {
        errorReason = "Line " + to_string(lineNumber) + ": expected exactly 1 value: TH.";
        return false;
    }
    if (overwaitThreshold < 0)
    {
        errorReason = "Line " + to_string(lineNumber) + ": TH cannot be negative.";
        return false;
    }
    int numberOfActions = 0;
    // 9) Read M
    if (!ReadNonEmptyLine(inputFile, currentLine, lineNumber, errorReason, "M"))        return false;
    istringstream numberOfActionsLine(currentLine);
    if (!(numberOfActionsLine >> numberOfActions) || HasExtraValues(numberOfActionsLine))
    {
        errorReason = "Line " + to_string(lineNumber) + ": expected exactly 1 value: M.";
        return false;
    }
    if (numberOfActions < 0)
    {
        errorReason = "Line " + to_string(lineNumber) + ": number of actions cannot be negative.";
        return false;
    }
    LinkedQueue<int> allOrderIds;
    LinkedQueue<int> OVCIds;
    LinkedQueue<int> cancelledOVCIds;
    int lastActionTime = -1;
    // 10) Read action lines
    for (int i = 1; i <= numberOfActions; i++)
    {
        if (!ReadNonEmptyLine(inputFile, currentLine, lineNumber, errorReason, "Action line"))
        {
            errorReason = "Missing action line number " + to_string(i) + ".";
            return false;
        }
        istringstream actionLine(currentLine);
        char actionLetter = '\0';
        if (!(actionLine >> actionLetter))
        {
            errorReason = "Line " + to_string(lineNumber) + ": missing action letter.";
            return false;
        }
        if (actionLetter != 'Q' && actionLetter != 'X')
        {
            errorReason = "Line " + to_string(lineNumber) + ": action letter must be Q or X.";
            return false;
        }
        if (actionLetter == 'Q')
        {
            string orderType; int requestTime = 0, orderId = 0, orderSize = 0; float orderPrice = 0;
            if (!(actionLine >> orderType >> requestTime >> orderId >> orderSize >> orderPrice))
            {
                errorReason = "Line " + to_string(lineNumber) + ": incomplete request action. Expected: Q TYP TQ ID SIZE Price.";
                return false;
            }
            if (requestTime < 0)
            {
                errorReason = "Line " + to_string(lineNumber) + ": request time TQ cannot be negative.";
                return false;
            }
            if (requestTime < lastActionTime)
            {
                errorReason = "Line " + to_string(lineNumber) + ": actions must be sorted by time.";
                return false;
            }
            lastActionTime = requestTime;
            if (orderId <= 0)
            {
                errorReason = "Line " + to_string(lineNumber) + ": order ID must be greater than zero.";
                return false;
            }
            if (QueueId(allOrderIds, orderId))
            {
                errorReason = "Line " + to_string(lineNumber) + ": duplicate order ID.";
                return false;
            }
            if (orderSize <= 0)
            {
                errorReason = "Line " + to_string(lineNumber) + ": order size must be greater than zero.";
                return false;
            }
            if (orderPrice <= 0)
            {
                errorReason = "Line " + to_string(lineNumber) + ": order price must be greater than zero.";
                return false;
            }
            if (orderType == "ODG" || orderType == "ODN")
            {
                int requiredSeats = 0, dineInDuration = 0; char canShareLetter = '\0';
                if (!(actionLine >> requiredSeats >> dineInDuration >> canShareLetter) || HasExtraValues(actionLine))
                {
                    errorReason = "Line " + to_string(lineNumber) + ": OD format must be: Q TYP TQ ID SIZE Price Seats Duration Y/N.";
                    return false;
                }
                if (requiredSeats <= 0)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": required seats must be greater than zero.";
                    return false;
                }
                if (requiredSeats > maximumTableCapacity)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": required seats cannot fit in any single table.";
                    return false;
                }
                if (dineInDuration <= 0)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": dine-in duration must be greater than zero.";
                    return false;
                }
                if (canShareLetter != 'Y' && canShareLetter != 'N')
                {
                    errorReason = "Line " + to_string(lineNumber) + ": CanShare must be Y or N.";
                    return false;
                }
                if (orderType == "ODG" && specialChefCount == 0)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": ODG orders need at least one CS chef.";
                    return false;
                }
                allOrderIds.enqueue(orderId);
            }
            else if (orderType == "OT")
            {
                if (HasExtraValues(actionLine))
                {
                    errorReason = "Line " + to_string(lineNumber) + ": OT format must be: Q OT TQ ID SIZE Price.";
                    return false;
                }
                if (normalChefCount == 0)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": OT orders need at least one CN chef.";
                    return false;
                }
                allOrderIds.enqueue(orderId);
            }
            else if (orderType == "OVC" || orderType == "OVG" || orderType == "OVN")
            {
                float deliveryDistance = 0;
                if (!(actionLine >> deliveryDistance) || HasExtraValues(actionLine))
                {
                    errorReason = "Line " + to_string(lineNumber) + ": OV format must be: Q TYP TQ ID SIZE Price Distance.";
                    return false;
                }
                if (deliveryDistance <= 0)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": delivery distance must be greater than zero.";
                    return false;
                }
                if (scooterCount == 0)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": delivery orders need at least one normal scooter.";
                    return false;
                }
                if (orderType == "OVG" && specialChefCount == 0)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": OVG orders need at least one CS chef.";
                    return false;
                }
                if (orderType == "OVN" && normalChefCount == 0)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": OVN orders need at least one CN chef.";
                    return false;
                }
                allOrderIds.enqueue(orderId);
                if (orderType == "OVC")     OVCIds.enqueue(orderId);
            }
            else if (orderType == "Combo")
            {
                float deliveryDistance = 0; int neededChefCount = 0, neededScooterCount = 0;
                if (!(actionLine >> deliveryDistance >> neededChefCount >> neededScooterCount) || HasExtraValues(actionLine))
                {
                    errorReason = "Line " + to_string(lineNumber) + ": Combo format must be: Q Combo TQ ID SIZE Price Distance ChefNeeded ScooterNeeded.";
                    return false;
                }
                if (deliveryDistance <= 0)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": Combo distance must be greater than zero.";
                    return false;
                }
                if (neededChefCount < 1 || neededChefCount > 4)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": Combo chef count must be from 1 to 4.";
                    return false;
                }
                if (neededChefCount > normalChefCount + specialChefCount)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": Combo needs more chefs than available.";
                    return false;
                }
                if (specialChefCount == 0)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": Combo orders need at least one CS chef.";
                    return false;
                }
                if (neededScooterCount < 2 || neededScooterCount > 4)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": Combo scooter count must be from 2 to 4.";
                    return false;
                }
                if (neededScooterCount > scooterCount)
                {
                    errorReason = "Line " + to_string(lineNumber) + ": Combo needs more normal scooters than teh available.";
                    return false;
                }
                allOrderIds.enqueue(orderId);
            }
            else
            {
                errorReason = "Line " + to_string(lineNumber) + ": unknown order type: " + orderType + ".";
                return false;
            }
        }
        else if (actionLetter == 'X')
        {
            int cancelTime = 0, orderId = 0;
            if (!(actionLine >> cancelTime >> orderId) || HasExtraValues(actionLine))
            {
                errorReason = "Line " + to_string(lineNumber) + ": X format must be: X Tcancel ID.";
                return false;
            }
            if (cancelTime < 0)
            {
                errorReason = "Line " + to_string(lineNumber) + ": cancellation time cannot be negative.";
                return false;
            }
            if (cancelTime < lastActionTime)
            {
                errorReason = "Line " + to_string(lineNumber) + ": actions must be sorted by time.";
                return false;
            }
            lastActionTime = cancelTime;
            if (orderId <= 0)
            {
                errorReason = "Line " + to_string(lineNumber) + ": cancelled order ID must be greater than zero.";
                return false;
            }
            if (!QueueId(OVCIds, orderId))
            {
                errorReason = "Line " + to_string(lineNumber) + ": cancellation is allowed only for an requested OVC order.";
                return false;
            }
            if (QueueId(cancelledOVCIds, orderId))
            {
                errorReason = "Line " + to_string(lineNumber) + ": this OVC order is already cancelled before.";
                return false;
            }
            cancelledOVCIds.enqueue(orderId);
        }
    }
    while (getline(inputFile, currentLine))
    {
        lineNumber++;
        if (currentLine.empty())         continue;
        errorReason = "Line " + to_string(lineNumber) + ": extra data found after the expected number of actions.";
        return false;
    }
    return true;
}
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
    inputFile >> Rescue_Count;
    inputFile >> Main_Ords >> Main_Dur;
    for (int i = 0; i < Scooter_Count; i++)
    {
        Scooter* sco = new Scooter(Scooter_Speed, Main_Dur);
        freeScooters.enqueue(sco,-sco->GetDistance());
    }
    int rescueSpeed = Scooter_Speed * 3;// rescue scooters are faster
    for (int i = 0; i < Rescue_Count; i++)// create rescue scooters
    {
        Scooter* rescue = new Scooter(rescueSpeed, Main_Dur);
        freeRescueScooters.enqueue(rescue, -rescue->GetDistance());
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
            else if (TYP == "Combo")
            {
                float Distance; int ChefNeeded, ScooterNeeded;
                inputFile >> Distance >> ChefNeeded >> ScooterNeeded;
                action = new RequestAction(this, TYP, TQ, ID, SIZE, Price, Distance, ChefNeeded, ScooterNeeded);
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
    if (!outputFile.is_open()) return false;

    double sumTi = 0, sumTc = 0, sumTw = 0, sumTserv = 0;

    // ── helper: pad a number into a fixed-width string ─────────────
    // No iomanip needed — just use to_string + spaces
    auto pad = [](int v, int w) -> string {
        string s = to_string(v);
        while ((int)s.size() < w) s += " ";
        return s;
        };
    auto padd = [](double v, int w) -> string {
        // two decimal places manually
        int whole = (int)v;
        int frac = (int)((v - whole) * 100 + 0.5);
        string s = to_string(whole) + "." + (frac < 10 ? "0" : "") + to_string(frac);
        while ((int)s.size() < w) s += " ";
        return s;
        };

    // ================================================================
    //  ORDER LOG
    // ================================================================
    outputFile << "================================================================\n";
    outputFile << "                    RESTAURANT SIMULATOR                       \n";
    outputFile << "                        ORDER LOG                              \n";
    outputFile << "================================================================\n\n";
    outputFile << "TF    ID    TQ    TA    TR    TS    Ti    TC    Tw    Tserv\n";
    outputFile << "----------------------------------------------------------------\n";

    ArrayStack<Order*> tempFinish;
    Order* o = nullptr;
    while (finishedOrders.pop(o))
    {
        outputFile << pad(o->getTF(), 6) << pad(o->getID(), 6) << pad(o->getTQ(), 6)
            << pad(o->getTA(), 6) << pad(o->getTR(), 6) << pad(o->getTS(), 6)
            << pad(o->getTi(), 6) << pad(o->getTc(), 6) << pad(o->getTw(), 6)
            << pad(o->getTserv(), 6) << "\n";

        if (o->getTi() != -1) sumTi += o->getTi();
        if (o->getTc() != -1) sumTc += o->getTc();
        if (o->getTw() != -1) sumTw += o->getTw();
        if (o->getTserv() != -1) sumTserv += o->getTserv();

        tempFinish.push(o);
    }
    while (tempFinish.pop(o)) finishedOrders.push(o);
    outputFile << "----------------------------------------------------------------\n\n";

    // ── derived values ─────────────────────────────────────────────
    int    fi = finishedOrders.GetCount();
    double avgTi = fi ? sumTi / fi : 0.0;
    double avgTc = fi ? sumTc / fi : 0.0;
    double avgTw = fi ? sumTw / fi : 0.0;
    double avgTserv = fi ? sumTserv / fi : 0.0;

    double finishedPercent = orderCount ? (fi * 100.0) / orderCount : 0.0;
    double cancelledPercent = orderCount ? (cancelledOrders.GetCount() * 100.0) / orderCount : 0.0;
    double overwaitPercent = orderCount ? (overwaitCount * 100.0) / orderCount : 0.0;

    int    simTime = currentTime - 1;
    int    numChefs = num_CN + num_CS;
    double chefUtil = (simTime > 0 && numChefs > 0) ? (100.0 * totalChefBusyTime) / (simTime * numChefs) : 0.0;
    double scooterUtil = (simTime > 0 && Scooter_Count > 0) ? (100.0 * totalScooterBusyTime) / (simTime * Scooter_Count) : 0.0;

    // ================================================================
    //  STATISTICS
    // ================================================================
    outputFile << "================================================================\n";
    outputFile << "                        STATISTICS                             \n";
    outputFile << "================================================================\n\n";

    outputFile << "  [1] Order Counts\n";
    outputFile << "      +-----------------------+-------+\n";
    outputFile << "      | Total Orders          | " << pad(orderCount, 5) << " |\n";
    outputFile << "      +-----------------------+-------+\n";
    outputFile << "      | Dine-in Grilled (ODG) | " << pad(numODG, 5) << " |\n";
    outputFile << "      | Dine-in Normal  (ODN) | " << pad(numODN, 5) << " |\n";
    outputFile << "      | Takeaway        (OT)  | " << pad(numOT, 5) << " |\n";
    outputFile << "      | Delivery Cold   (OVC) | " << pad(numOVC, 5) << " |\n";
    outputFile << "      | Delivery Grilled(OVG) | " << pad(numOVG, 5) << " |\n";
    outputFile << "      | Delivery Normal (OVN) | " << pad(numOVN, 5) << " |\n";
    outputFile << "      | Combo                 | " << pad(numCombo, 5) << " |\n";
    outputFile << "      +-----------------------+-------+\n\n";

    outputFile << "  [2] Chef Counts\n";
    outputFile << "      +-----------------------+-------+\n";
    outputFile << "      | Total Chefs           | " << pad(numChefs, 5) << " |\n";
    outputFile << "      +-----------------------+-------+\n";
    outputFile << "      | Normal Chefs    (CN)  | " << pad(num_CN, 5) << " |\n";
    outputFile << "      | Special Chefs   (CS)  | " << pad(num_CS, 5) << " |\n";
    outputFile << "      +-----------------------+-------+\n\n";

    outputFile << "  [3] Scooter Counts\n";
    outputFile << "      +-----------------------+-------+\n";
    outputFile << "      | Total Scooters        | " << pad(Scooter_Count + Rescue_Count, 5) << " |\n";
    outputFile << "      +-----------------------+-------+\n";
    outputFile << "      | Normal Scooters       | " << pad(Scooter_Count, 5) << " |\n";
    outputFile << "      | Rescue Scooters       | " << pad(Rescue_Count, 5) << " |\n";
    outputFile << "      | Rescue Missions       | " << pad(rescueMissionCount, 5) << " |\n";
    outputFile << "      +-----------------------+-------+\n\n";

    outputFile << "  [4] Order Outcomes\n";
    outputFile << "      +-----------------------+--------+\n";
    outputFile << "      | Finished Orders %     | " << padd(finishedPercent, 5) << "% |\n";
    outputFile << "      | Cancelled Orders %    | " << padd(cancelledPercent, 5) << "% |\n";
    outputFile << "      | Overwait Orders %     | " << padd(overwaitPercent, 5) << "% |\n";
    outputFile << "      +-----------------------+--------+\n\n";

    outputFile << "  [5] Averages  (finished orders)\n";
    outputFile << "      +-----------------------+-------+\n";
    outputFile << "      | Avg Ti                | " << padd(avgTi, 5) << " |\n";
    outputFile << "      | Avg TC                | " << padd(avgTc, 5) << " |\n";
    outputFile << "      | Avg Tw                | " << padd(avgTw, 5) << " |\n";
    outputFile << "      | Avg Tserv             | " << padd(avgTserv, 5) << " |\n";
    outputFile << "      +-----------------------+-------+\n\n";

    outputFile << "  [6] Utilization\n";
    outputFile << "      +-----------------------+--------+\n";
    outputFile << "      | Chefs Util %          | " << padd(chefUtil, 5) << "% |\n";
    outputFile << "      | Scooters Util %       | " << padd(scooterUtil, 5) << "% |\n";
    outputFile << "      | (rescue scooters excluded)     |\n";
    outputFile << "      +-----------------------+--------+\n\n";

    outputFile << "================================================================\n";

    outputFile.close();
    return true;
}
//----------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------//

//----------------------------------------------------------------------------------//
//------------------------- Combo Functions ----------------------------------------//
//----------------------------------------------------------------------------------//
bool Restaurant::assignComboToChefs(Order* od)
{
    ComboOrder* combo = dynamic_cast<ComboOrder*>(od);
    if (!combo) return false;
    int need = combo->getChefNeeded();// number of chefs needed from input file
    if (need < 1 || need > 4) return false;// up to 4 chefs(this is impossible unless written wrong in the input file)
    if (availCS.GetCount() < 1) return false;// at least one CS is required
    if (availCN.GetCount() + availCS.GetCount() < need) return false; // not enough chefs
    Chef* chef = nullptr; 
    float totalSpeed = 0; // total speed of all assigned chefs
    availCS.dequeue(chef);// first chef must be CS
    chef->assignOrder();// mark chef busy
    combo->addComboChef(chef);// store exact chef in combo
    totalSpeed += chef->getSpeed();// add speed
    while (combo->getComboChefCount() < need)// assign remaining chefs
    {
        chef = nullptr;
        if (!availCN.isEmpty())      availCN.dequeue(chef);   // prefer CN for remaining chefs to save CS
        else availCS.dequeue(chef);// use CS if CN not enough
        chef->assignOrder();// mark chef busy
        combo->addComboChef(chef);// store exact chef in ComboChefs queue
        totalSpeed += chef->getSpeed();// add speed of the chefs
    }
    combo->setTA(currentTime);// time assigned to chefs
    int cookingTime = (int)ceil((float)combo->getSize() / totalSpeed); // combined cooking time
    int finishTime = currentTime + cookingTime;// when cooking finishes
    cooking.enqueue(combo, -finishTime);
    return true;
}
bool Restaurant::freeComboChefs(ComboOrder* combo)
{
    if (!combo) return false;
    int usedChefs = combo->getComboChefCount();
    if (combo->getTA() != -1 && combo->getTR() != -1)
        totalChefBusyTime += (combo->getTR() - combo->getTA()) * usedChefs;

    Chef* chef = nullptr;
    while (combo->removeComboChef(chef))// remove all combo chefs
    {
        if (chef)
        {
            chef->releaseOrder();// chef becomes free
            if (chef->getType() == "CS")    availCS.enqueue(chef);// return to correct chef list
            else    availCN.enqueue(chef);
        }
    }
    return true; 
}
bool Restaurant::assignComboToScooters(Order* od)
{
    ComboOrder* combo = dynamic_cast<ComboOrder*>(od);
    if (!combo) return false;
    int need = combo->getScooterNeeded();// number of scooters required
    if (need < 2) return false; //2 or more scooters(this is impossible unless written wrong in the input file)
    if (freeScooters.GetCount() < need) return false;// not enough scooters
    Scooter* scooter = nullptr;
    int pri = 0;
    int tripTime = 0;
    for (int i = 0; i < need; i++)
    {
        freeScooters.dequeue(scooter, pri);// take best available scooter
        if (i == 0)// all scooters have same speed
            tripTime = (int)ceil(combo->getDistance() / scooter->getSpeed());
        combo->addComboScooter(scooter); // remember exact scooter
    }
    combo->setTS(currentTime); // service time setup (service starts now)
    inServOrders.enqueue(combo, -(currentTime + tripTime));     // priority according to finishTime(order finishes after trip time)
    return true;
}
bool Restaurant::freeComboScooters(ComboOrder* combo)
{
    if (!combo) return false;
    Scooter* scooter = nullptr;
    while (combo->removeComboScooter(scooter))// remove all combo scooters
    {
        if (scooter)
        {
            int tripTime = (int)ceil(combo->getDistance() / scooter->getSpeed());
            totalScooterBusyTime += 2 * tripTime; // go + return busy time
            scooter->incDist((int)ceil(combo->getDistance() * 2)); // go + return distance(to update the distance of the scooter)
            scooter->incrementDeliOreders();// count delivered order (to Main_Ord)
            backScooters.enqueue(scooter, -(currentTime + tripTime));
        }
    }
    return true;
}
//----------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------//