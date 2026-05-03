#pragma once
#include "../DEFS.h"
#include"../ds/LinkedQueue.h"
#include"../ds/PriQueue.h"
#include"../ds/ArrayStack.h"
#include"../Derived DS/derivedQueue.h"
#include"../Derived DS/CookingOrders.h"
#include"../Derived DS/Fit_Table.h"
#include "../PriorityParameters.h"
using namespace std;

class UI
{
	ProgramMode mode;
public:
	UI();
	~UI();
	ProgramMode ReadMode();
	ProgramMode GetMode() const;
	void SetMode(ProgramMode m);

	string ReadInputFileName() const;
	string ReadOutputFileName() const;
	void WaitForNextStep() const;
	//for silent mode
	void PrintStartSilent() const;
	void PrintEndSilent() const;

	void PrintCurrentState(int timestep, LinkedQueue<Action*>& actions, LinkedQueue<Order*>& pODG, LinkedQueue<Order*>& pODN, LinkedQueue<Order*>& pOT, LinkedQueue<Order*>& pOVN, derivedQueue& pOVC, priQueue<Order*>& pOVG, LinkedQueue<Order*>& pCombo,
		LinkedQueue<Chef*>& freeCS, LinkedQueue<Chef*>& freeCN, CookingOrders& cookingOrds,
		LinkedQueue<Order*>& rOD, LinkedQueue<Order*>& rOT, derivedQueue& rOV, priQueue<Order*>& overWait, LinkedQueue<Order*>& rCombo,
		priQueue<Scooter*>& availScooters, priQueue<Scooter*>& rescueScooters, LinkedQueue<Scooter*>& maintScooters, priQueue<Scooter*>& backScooters, priQueue<Scooter*>& rescueBackScooters, priQueue<Scooter*>& failedBackScooters,int rescueMissions
		, Fit_Tables& availTables, Fit_Tables& sharedTables, priQueue<Order*>& inService, LinkedQueue<Order*>& canceled, ArrayStack<Order*>& finished) const;
};

