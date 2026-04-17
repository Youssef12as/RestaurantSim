#pragma once
#include "../ds/PriQueue.h"
#include "../entities/Order.h"
#include "../ds/LinkedQueue.h"
#include"../entities/Chef.h"
class CookingOrders : public priQueue<Order*>
{
public:
    bool CancelOrder(int id, Order*& cancelledOrder)
    {
        cancelledOrder = nullptr;
        if (isEmpty()) return false;

        bool found = false;
        LinkedQueue<Order*> tempQueue;
        while(!isEmpty())
        {
            Order* currentOrder=nullptr;
            int pri = 0;
            dequeue(currentOrder, pri);

            if (!found && currentOrder && currentOrder->getID() == id)
            {
                cancelledOrder = currentOrder;
                found = true;

                //Released its Chef
                Chef* chefPtr = currentOrder->getAssignedChef();
                if (chefPtr) chefPtr->releaseOrder();
            }
            else
            {
                tempQueue.enqueue(currentOrder); // if not found ID
            }
        }
        while (!tempQueue.isEmpty()) // for restoring back into priQueue
        {
            Order* orderPtr = nullptr;
            tempQueue.dequeue(orderPtr);
            if (orderPtr)
            {
                Chef* chefPtr = orderPtr->getAssignedChef();
                if (chefPtr)
                    enqueue(orderPtr, -orderPtr->getExpectedFinishTime(chefPtr->getSpeed()));
            }
        }
        return found;
    }

};