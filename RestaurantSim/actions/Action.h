#pragma once

class Restaurant;


class Action
{
protected:
	Restaurant* pRest; // pointer to the restaurant to do the needed functions

public:
	Action(Restaurant* rest) { // constructor
		pRest = rest;
	}

	virtual void Act() = 0; // overwrite for each dervide classs

	// more functions will be added if needed
};