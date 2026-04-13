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

class Action;
class Order;
class Chef;
class Scooter;
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

	void PrintCurrentState(
		int timestep,
		LinkedQueue<Action*>& actions, LinkedQueue<Order*>& pODG, LinkedQueue<Order*>& pODN, LinkedQueue<Order*>& pOT, LinkedQueue<Order*>& pOVN, derivedQueue& pOVC, priQueue<OVGPriorityItem>& pOVG,
		LinkedQueue<Chef*>& freeCS, LinkedQueue<Chef*>& freeCN,
		CookingOrders& cookingOrds, LinkedQueue<Order*>& rOD, LinkedQueue<Order*>& rOT, LinkedQueue<Order*>& rOVG,
		LinkedQueue<Order*>& rOVN, derivedQueue& rOVC, priQueue<ScooterPriorityItem>& availScooters,
		LinkedQueue<Scooter*>& maintScooters, LinkedQueue<Scooter*>& backScooters,
		Fit_Tables& availTables, priQueue<InServicePriorityItem>& inService,
		LinkedQueue<Order*>& canceled, ArrayStack<Order*>& finished) const;
};

