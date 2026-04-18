#include <iostream>
#include <cmath>
#include <string>
#include "../RestaurantSim/ds/LinkedQueue.h"
#include "../RestaurantSim/ds/ArrayStack.h"
#include "../RestaurantSim/ds/PriQueue.h"
#include "core/UI.h"
#include "core/Restaurant.h"

int Chef::ChefCount = 1;
int Scooter::ScooterCount = 1;
int Table::TableCount = 1;

int main() {

	Restaurant myres;
	myres.randomSimulate();

	
	return 0;
}