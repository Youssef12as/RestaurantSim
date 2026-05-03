#pragma once
#include"Order.h"
#include"../ds/LinkedQueue.h"
class ComboOrder :public DeliveryOrder
{
private:
	int chefNeeded;//number of chefs needed maximum 4(at least 1 CS)
	int scooterNeeded;// number of scoooter needed minimum 2 
	LinkedQueue<Chef*> comboChefs;//chefs assigned to this combo
	LinkedQueue<Scooter*> comboScooters;//scooters assigned to this combo
public:
	ComboOrder(int id,string type, int TQ, int size,float price,float distance, int chefs, int scooters)
	: DeliveryOrder(id,type,TQ,size,price,distance), chefNeeded(chefs), scooterNeeded(scooters) {}
	int getChefNeeded()const { return chefNeeded;}
	int getScooterNeeded()const { return scooterNeeded; }
	int getComboChefCount() const { return comboChefs.GetCount(); }//number of chefs currently assigned
	int getComboScooterCount() const { return comboScooters.GetCount(); }//number of scooters currently assigned
	bool addComboChef(Chef* chef)
	{
		if (!chef) return false;
		comboChefs.enqueue(chef);// store the assigned chef
		if (getAssignedChef() == nullptr)
			setAssignedChef(chef);

		return true;
	}
	bool removeComboChef(Chef*& chef)
	{
		bool removed = comboChefs.dequeue(chef); // Remove one chef from combo order
		if (comboChefs.GetCount() == 0)// If no chefs left inside combo
			setAssignedChef(nullptr);// clear the assignedChef 
		return removed;
	}
	bool addComboScooter(Scooter* scooter)
	{
		if (!scooter) return false;
		comboScooters.enqueue(scooter);
		if (getAssignedScooter() == nullptr)
			setAssignedScooter(scooter);

		return true;
	}
	bool removeComboScooter(Scooter*& scooter)
	{
		bool removed = comboScooters.dequeue(scooter);  // Remove one scooter from combo order
		if (comboScooters.GetCount() == 0)// If no scooters left inside combo
			setAssignedScooter(nullptr);// Clear  assignedScooter

		return removed;
	}
};