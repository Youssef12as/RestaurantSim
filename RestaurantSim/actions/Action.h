#pragma once

class Restaurant;
#include <iostream>
using namespace std;

class Action
{
protected:
	Restaurant* pRest; // pointer to the restaurant to do the needed functions

public:
	Action(Restaurant* rest) { // constructor
		pRest = rest;
	}

	virtual void Act() = 0; // overwrite for each dervide classs

	virtual void print(ostream& out) const {  // override this for each class
	}

	friend ostream& operator<<(ostream& out, const Action* act) {	// polymorphism using reference
		act->print(out);
		return out;
	}

	// more functions will be added if needed
};