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
    fileName = fileName + ".txt";
    return fileName;
}

string UI::ReadOutputFileName() const
{
    string fileName;
    cout << "Enter output file name: ";
    cin >> fileName;
    fileName = fileName + ".txt";
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

void UI::PrintCurrentState(int timestep, LinkedQueue<Action*>& actions, LinkedQueue<Order*>& pODG, LinkedQueue<Order*>& pODN, LinkedQueue<Order*>& pOT, LinkedQueue<Order*>& pOVN, derivedQueue& pOVC, priQueue<Order*>& pOVG, LinkedQueue<Order*>& pCombo, LinkedQueue<Chef*>& freeCS, LinkedQueue<Chef*>& freeCN, CookingOrders& cookingOrds, LinkedQueue<Order*>& rOD, LinkedQueue<Order*>& rOT, derivedQueue& rOV, priQueue<Order*>& overWait, LinkedQueue<Order*>& rCombo, priQueue<Scooter*>& availScooters, priQueue<Scooter*>& rescueScooters, LinkedQueue<Scooter*>& maintScooters, priQueue<Scooter*>& backScooters, priQueue<Scooter*>& rescueBackScooters, priQueue<Scooter*>& failedBackScooters, int rescueMissions, Fit_Tables& availTables, Fit_Tables& sharedTables, priQueue<Order*>& inService, LinkedQueue<Order*>& canceled, ArrayStack<Order*>& finished) const
{
    // Palette: navy + steel blue + cyan + white + one red accent
    const char* RST = "\033[0m";
    const char* BOLD = "\033[1m";

    const char* BLU = "\033[34m";    // navy blue  — borders
    const char* BBLU = "\033[94m";    // steel blue — section headers
    const char* CYN = "\033[36m";    // teal cyan  — labels
    const char* BCYN = "\033[96m";    // bright cyan — counts
    const char* BWHT = "\033[97m";    // bright white — values
    const char* DGRY = "\033[90m";    // dark gray  — decorative chars
    const char* BRED = "\033[91m";    // bright red — danger only

    const char* BG_BLU = "\033[44m";   // blue bg for banner

    // borders
    const char* BNR = "+==========================================================+";
    const char* SEC = "+----------------------------------------------------------+";

    auto divider = [&](const char* title) {
        cout << "\n"
            << BLU << "  " << SEC << RST << "\n"
            << BBLU << "  |  " << RST << BOLD << BBLU << title << RST << "\n"
            << BLU << "  " << SEC << RST << "\n";
        };

    auto row = [&](const char* label, int count, auto& queue) {
        cout << DGRY << "  |    " << RST
            << BOLD << BCYN << count << RST
            << DGRY << "  |  " << RST
            << CYN << label << RST
            << DGRY << "  " << RST
            << BWHT;
        queue.print();
        cout << RST << "\n";
        };

    // same as row but red count — for danger sections
    auto rowRed = [&](const char* label, int count, auto& queue) {
        cout << DGRY << "  |    " << RST
            << BOLD << BRED << count << RST
            << DGRY << "  |  " << RST
            << CYN << label << RST
            << DGRY << "  " << RST
            << BWHT;
        queue.print();
        cout << RST << "\n";
        };

    auto close = [&]() {
        cout << BLU << "  " << SEC << RST << "\n";
        };

    // ── Banner ─────────────────────────────────────────────────────
    cout << "\n";
    cout << BOLD << BBLU << "  " << BNR << RST << "\n";
    cout << BBLU << "  |" << RST
        << BG_BLU << BOLD << BWHT
        << "     RESTAURANT SIMULATOR"
        << "     TIMESTEP: " << timestep
        << "     "
        << RST << BBLU << "|" << RST << "\n";
    cout << BOLD << BBLU << "  " << BNR << RST << "\n";

    // 1. Actions
    divider("Actions List");
    row("Actions remaining", actions.GetCount(), actions);
    close();

    // 2. Pending Orders
    divider("Pending Orders");
    row("Combo  ", pCombo.GetCount(), pCombo);
    row("ODG    ", pODG.GetCount(), pODG);
    row("ODN    ", pODN.GetCount(), pODN);
    row("OT     ", pOT.GetCount(), pOT);
    row("OVN    ", pOVN.GetCount(), pOVN);
    row("OVC    ", pOVC.GetCount(), pOVC);
    row("OVG    ", pOVG.GetCount(), pOVG);
    close();

    // 3. Available Chefs
    divider("Available Chefs");
    row("CS  (senior)", freeCS.GetCount(), freeCS);
    row("CN  (normal)", freeCN.GetCount(), freeCN);
    close();

    // 4. Cooking Orders
    divider("Cooking Orders  [Order ID, Chef ID]");
    cout << DGRY << "  |    " << RST
        << BOLD << BCYN << cookingOrds.GetCount() << RST
        << DGRY << "  |  " << RST
        << CYN << "Cooking" << RST
        << DGRY << "  " << RST << BWHT;
    cookingOrds.print();
    cout << RST << "\n";
    close();

    // 5. Ready Orders
    divider("Ready Orders");
    row("Combo", rCombo.GetCount(), rCombo);
    row("OD   ", rOD.GetCount(), rOD);
    row("OT   ", rOT.GetCount(), rOT);
    row("OV   ", rOV.GetCount(), rOV);
    close();

    // 6. Overwait — danger accent
    divider("Ready Overwait Orders");
    rowRed("OV  (overwait)", overWait.GetCount(), overWait);
    close();

    // 7. Scooters
    divider("Scooters");
    row("Available       ", availScooters.GetCount(), availScooters);
    row("Rescue          ", rescueScooters.GetCount(), rescueScooters);
    row("Maintenance     ", maintScooters.GetCount(), maintScooters);
    row("Back to rest.   ", backScooters.GetCount(), backScooters);
    row("Rescue back     ", rescueBackScooters.GetCount(), rescueBackScooters);
    rowRed("Failed back     ", failedBackScooters.GetCount(), failedBackScooters);
    cout << DGRY << "  |    " << RST
        << BOLD << BCYN << rescueMissions << RST
        << DGRY << "  |  " << RST
        << CYN << "Rescue missions so far" << RST << "\n";
    close();

    // 8. Tables
    divider("Tables  [ID, capacity, free seats]");
    row("Available tables", availTables.GetCount(), availTables);
    row("Shared tables   ", sharedTables.GetCount(), sharedTables);
    close();

    // 9. In-Service
    divider("In-Service Orders  [Order ID, Scooter/Table ID]");
    cout << DGRY << "  |    " << RST
        << BOLD << BCYN << inService.GetCount() << RST
        << DGRY << "  |  " << RST
        << CYN << "In service" << RST
        << DGRY << "  " << RST << BWHT;
    inService.printInservice();
    cout << RST << "\n";
    close();

    // 10. Cancelled & Finished
    divider("Cancelled & Finished Orders");
    rowRed("Cancelled", canceled.GetCount(), canceled);
    row("Finished ", finished.GetCount(), finished);
    close();

    cout << "\n";
}