#pragma once
#include "Action.h"
#include <string>
#include "../entities/Order.h"
using namespace std;


// cancelation action line: 
/*Cancellation Action line has the following info 
❑ X(Letter X) means an order cancellation action 
❑ Tcancel is the action timestep. (Cancellation timestep) 
❑ ID is the id of the order to be canceled. This ID must be for an OV order. 
*/

class CancelAction : public Action
{
private:
	int Tcancel;
	int ID;

public:
	// after knowing what is the type of the action we will call the proper constructor

	CancelAction(Restaurant* rest, int cancelt, int id) :
		Action(rest), Tcancel(cancelt), ID(id)
	{
	} // all the parameters needed are got in the construction
	void Act() {
		//search for the order in the pending ov, cooking and ready ov lists
		if (pRest->cancelOrderFromPending(ID)) {
			return;
		}
		else if(pRest->cancelOrderFromCooking(ID)) {
			return;
		}
		else if (pRest->cancelOrderFromReady(ID)) {
			return;
		}
	}

	int getTcancel() const {
		return Tcancel;
	}


	void print(ostream& out) const override{
		out << "(X, " << Tcancel << ", " << ID << ")";
	}

};