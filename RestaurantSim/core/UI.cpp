#include "UI.h"
#include<limits>
UI::UI() {}
UI::~UI() {}
ProgramMode UI::ReadMode()
{
    int mode;
    cout << "Select Program Mode:\n";
    cout << "1. Interactive Mode\n";
    cout << "2. Silent Mode\n";
    cout << "Enter choice: ";
    cin >> mode;

    if (mode == 2) return ProgramMode::Silent;
    return ProgramMode::Interactive;
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
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
    LinkedQueue<Order*>& rOD, LinkedQueue<Order*>& rOT, LinkedQueue<Order*>& rOVG, LinkedQueue<Order*>& rOVN, derivedQueue& rOVC,
    priQueue<Scooter*>& availScooters, LinkedQueue<Scooter*>& maintScooters, LinkedQueue<Scooter*>& backScooters
    , Fit_Tables& availTables, priQueue<Order*>& inService, LinkedQueue<Order*>& canceled, ArrayStack<Order*>& finished) const
{
    cout << "\nCurrent Timestep: " << timestep << "\n";

    cout << "===============   Actions List   ================ \n";
    cout << actions.GetCount() << " actions remaining: "; actions.print(); cout << "\n";    //done

    cout << "—-------------     Pending Orders IDs —--------------------- \n";
    cout << pODG.GetCount() << " ODG: "; pODG.print(); cout << "\n";   //done
    cout << pODN.GetCount() << " ODN: "; pODN.print(); cout << "\n";   //done
    cout << pOT.GetCount() << " OT: ";  pOT.print();  cout << "\n";    //done
    cout << pOVN.GetCount() << " OVN: "; pOVN.print(); cout << "\n";    //done
    cout << pOVC.GetCount() << " OVC: "; pOVC.print(); cout << "\n";    //done
    cout << pOVG.GetCount() << " OVG: "; pOVG.print(); cout << "\n";    //done

    cout << "—-------------   Available chefs IDs —---------------------- \n";
    cout << freeCS.GetCount() << " CS: "; freeCS.print(); cout << "\n"; //done
    cout << freeCN.GetCount() << " CN: "; freeCN.print(); cout << "\n"; //done

    cout << "—-------------     Cooking orders [Orders ID, chef ID] —--------------------- \n";
    cout << cookingOrds.GetCount() << " cooking orders: "; cookingOrds.print(); cout << "\n";   //done

    cout << "—-------------     Ready Orders IDs —--------------------- \n";
    cout << rOD.GetCount() << " OD: ";  rOD.print();  cout << "\n"; //done
    cout << rOT.GetCount() << " OT: ";  rOT.print();  cout << "\n"; //done
    cout << rOVG.GetCount() << " OVG: "; rOVG.print(); cout << "\n";    //done
    cout << rOVN.GetCount() << " OVN: "; rOVN.print(); cout << "\n";    //done
    cout << rOVC.GetCount() << " OVC: "; rOVC.print(); cout << "\n";    //done        

    cout << "—-------------   Available scooters IDs —---------------------- \n";
    cout << availScooters.GetCount() << " Scooters: "; availScooters.print(); cout << "\n";     //done

    cout << "—-------------   Available tables [ID, capacity, free seats] —---------------------- \n";
    cout << availTables.GetCount() << " tables: "; availTables.print(); cout << "\n";   //done

    cout << "—-------------   In-Service orders [order ID, scooter/Table ID]  --—---------------------\n";
    cout << inService.GetCount() << " Orders: "; inService.printInservice(); cout << "\n";   // done this has a specific print function in the priqueue class

    cout << "—-------------   In-maintainance scooters IDs   ---------------------- \n";
    cout << maintScooters.GetCount() << " scooters: "; maintScooters.print(); cout << "\n"; //done

    cout << "—-------------    Scooters Back to Restaurant  IDs   ---------------------- \n";
    cout << backScooters.GetCount() << " scooters: "; backScooters.print(); cout << "\n";   //done

    cout << "—-------------   Cancelled Orders IDs   ---------------------- \n";
    cout << canceled.GetCount() << " cancelled: "; canceled.print(); cout << "\n";  //done

    cout << "—-------------   Finished orders IDs---------------------------- \n";
    cout << finished.GetCount() << " Orders: "; finished.print(); cout << "\n";     //done
}