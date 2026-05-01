#pragma once

#include "Action.h"
#include <string>
#include "../entities/Order.h"
using namespace std;


// request action line: 
/*Request Action Line has the following info
❑ Q(letter Q in the beginning of its line) means a reQuest action
❑ TYP is the order type(ODG, ODN, OT, OVC, ...etc)
❑ TQ is the request timestep
❑ ID is a unique sequence number that identifies each order.
❑ SIZE is the number of dishes of the order
❑ Price is the order price.
❑ No.of Seats : for OD orders only
❑ Order Duration : for OD orders only
❑ CanShare : for OD orders only
❑ Distance : for OV only(in meters)
*/

class RequestAction : public Action
{
private:
	string TYP;
	int TQ;
	int ID;
	int SIZE;
	float Price;
	int Seats;
	int Duration;
	bool CanShare;
	float Distance;
	int NumChefs;      // COMBO only
	int NumScooters;   // COMBO only

public:
	// after knowing what is the type of the action we will call the proper constructor

	RequestAction(Restaurant* rest, string typ, int tq, int id, int size, float price, int seats, int duration, bool share) :
		Action(rest), TYP(typ), TQ(tq), ID(id), SIZE(size), Price(price), Seats(seats), Duration(duration), CanShare(share), Distance(0), NumChefs(0), NumScooters(0)
	{ } // OD orders

	RequestAction(Restaurant* rest, string typ, int tq, int id, int size, float price, float dist) :
		Action(rest), TYP(typ), TQ(tq), ID(id), SIZE(size), Price(price), Seats(0), Duration(0), CanShare(false), Distance(dist), NumChefs(0), NumScooters(0)
	{ } // OV orders
	
	RequestAction(Restaurant* rest, string typ, int tq, int id, int size, float price) :
		Action(rest), TYP(typ), TQ(tq), ID(id), SIZE(size), Price(price), Seats(0), Duration(0), CanShare(false), Distance(0), NumChefs(0), NumScooters(0)
	{ } // OT orders

	RequestAction(Restaurant* rest, string typ, int tq, int id, int size, float price, float dist, int nChefs, int nScooters) :
		Action(rest), TYP(typ), TQ(tq), ID(id), SIZE(size), Price(price), Seats(0), Duration(0), CanShare(false), Distance(dist), NumChefs(nChefs), NumScooters(nScooters)
	{ } // COMBO orders

	void Act() {
		string mainType = TYP.substr(0, 2);
		Order* newOrder;
		// first create the new order
		if (TYP == "OC") {
			newOrder = new ComboOrder(ID, TQ, SIZE, Price, Distance, NumChefs, NumScooters);
		}
		else if (mainType == "OV") { 
			newOrder = new DeliveryOrder(ID, TYP, TQ, SIZE, Price, Distance);
		}
		else if (mainType == "OD") {
			newOrder = new DineInOrder(ID, TYP, TQ, SIZE, Price, Seats, Duration, CanShare);
		}
		else {
			newOrder = new TakeawayOrder(ID, TYP, TQ, SIZE, Price);
		}

		//then add it to the appropiate list in the retaurant

		pRest->AddOrderToPending(newOrder);
	}

	int getTQ() const {
		return TQ;
	}

	void print(ostream& out) const override {
		out << "[" << TYP << ", " << TQ << ", " << ID << "]";
	}
	
};

