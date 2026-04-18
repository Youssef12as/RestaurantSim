#include "UI.h"
#include<limits>
UI::UI() {}
UI::~UI() {}
ProgramMode UI::ReadMode()
{
    int choice;
    cout << "Select Program Mode:\n";
    cout << "1. Interactive Mode\n";
    cout << "2. Silent Mode\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 2) mode= ProgramMode::Silent;
    else mode= ProgramMode::Interactive;
    return mode;
}

ProgramMode UI::GetMode() const
{
    return mode;
}

void UI::SetMode(ProgramMode m)
{
    mode = m;
}

string UI::ReadInputFileName() const
{
    string fileName;
    cout << "Enter input file name: ";
    cin >> fileName;
    return fileName;
}

string UI::ReadOutputFileName() const
{
    string fileName;
    cout << "Enter output file name: ";
    cin >> fileName;
    return fileName;
}

void UI::WaitForNextStep() const
{
    cout << "PRESS ANY KEY TO MOVE TO NEXT STEP !" << endl;
    cin.get();
}

void UI::PrintStartSilent() const
{
    cout << "Simulation Starts in Silent mode ..." << endl;
}

void UI::PrintEndSilent()const
{
    cout << "Simulation ends, Output file created" << endl;
}

void UI::PrintCurrentState(int timestep, LinkedQueue<Action*>& actions, LinkedQueue<Order*>& pODG, LinkedQueue<Order*>& pODN, LinkedQueue<Order*>& pOT, LinkedQueue<Order*>& pOVN, derivedQueue& pOVC, priQueue<Order*>& pOVG,
    LinkedQueue<Chef*>& freeCS, LinkedQueue<Chef*>& freeCN, CookingOrders& cookingOrds,
    LinkedQueue<Order*>& rOD, LinkedQueue<Order*>& rOT, derivedQueue& rOV,
    priQueue<Scooter*>& availScooters, LinkedQueue<Scooter*>& maintScooters, priQueue<Scooter*>& backScooters
    , Fit_Tables& availTables, priQueue<Order*>& inService, LinkedQueue<Order*>& canceled, ArrayStack<Order*>& finished) const
{
    cout << "\nCurrent Timestep: " << timestep << "\n";

    cout << "===============   Actions List   ================ \n";
    cout << actions.GetCount() << " actions remaining: "; actions.print(); cout << "\n";    //done

    cout << "--------------     Pending Orders IDs ---------------------- \n";
    cout << pODG.GetCount() << " ODG: "; pODG.print(); cout << "\n\n";   //done
    cout << pODN.GetCount() << " ODN: "; pODN.print(); cout << "\n\n";   //done
    cout << pOT.GetCount() << " OT: ";  pOT.print();  cout << "\n\n";    //done
    cout << pOVN.GetCount() << " OVN: "; pOVN.print(); cout << "\n\n";    //done
    cout << pOVC.GetCount() << " OVC: "; pOVC.print(); cout << "\n\n";    //done
    cout << pOVG.GetCount() << " OVG: "; pOVG.print(); cout << "\n\n";    //done

    cout << "--------------   Available chefs IDs ----------------------- \n";
    cout << freeCS.GetCount() << " CS: "; freeCS.print(); cout << "\n\n"; //done
    cout << freeCN.GetCount() << " CN: "; freeCN.print(); cout << "\n\n"; //done

    cout << "--------------     Cooking orders [Orders ID, chef ID] ---------------------- \n";
    cout << cookingOrds.GetCount() << " cooking orders: "; cookingOrds.print(); cout << "\n";   //done

    cout << "--------------     Ready Orders IDs ---------------------- \n";
    cout << rOD.GetCount() << " OD: ";  rOD.print();  cout << "\n\n";     //done
    cout << rOT.GetCount() << " OT: ";  rOT.print();  cout << "\n\n";     //done
    cout << rOV.GetCount() << " OV: "; rOV.print(); cout << "\n\n";    //done
           
    cout << "--------------   Available scooters IDs ----------------------- \n";
    cout << availScooters.GetCount() << " Scooters: "; availScooters.print(); cout << "\n\n";     //done

    cout << "--------------   Available tables [ID, capacity, free seats] ----------------------- \n";
    cout << availTables.GetCount() << " tables: "; availTables.print(); cout << "\n\n";   //done

    cout << "--------------   In-Service orders [order ID, scooter/Table ID]  ------------------------\n";
    cout << inService.GetCount() << " Orders: "; inService.printInservice(); cout << "\n\n";   // done this has a specific print function in the priqueue class

    cout << "--------------   In-maintainance scooters IDs   ---------------------- \n";
    cout << maintScooters.GetCount() << " scooters: "; maintScooters.print(); cout << "\n\n"; //done

    cout << "--------------    Scooters Back to Restaurant  IDs   ---------------------- \n";
    cout << backScooters.GetCount() << " scooters: "; backScooters.print(); cout << "\n\n";   //done

    cout << "--------------   Cancelled Orders IDs   ---------------------- \n";
    cout << canceled.GetCount() << " cancelled: "; canceled.print(); cout << "\n\n";  //done

    cout << "--------------   Finished orders IDs---------------------------- \n";
    cout << finished.GetCount() << " Orders: "; finished.print(); cout << "\n\n";     //done
}