#pragma once
#include "ds/PriQueue.h"
#include "entities/Order.h"
#include "PriorityParameters.h"
#include "ds/LinkedQueue.h"

class CookingOrders : public priQueue<CookingPriorityItem>
{
public:
    bool CancelOrder(int id, Order*& cancelledOrder)
    {
        cancelledOrder = nullptr;
        if (isEmpty()) return false;

        bool found = false;
        LinkedQueue<CookingPriorityItem> tempQueue; // for holding items while searching for the id
        while (!isEmpty())
        {
            CookingPriorityItem currentItem;
            int pri = 0;
            dequeue(currentItem, pri);

            if (!found && currentItem.orderPtr && currentItem.orderPtr->getID() == id)
            {
                cancelledOrder = currentItem.orderPtr;
                found = true;
            }
            else
            {
                tempQueue.enqueue(currentItem); // if not found ID
            }
        }
        while (!tempQueue.isEmpty()) // for restoring back into priQueue
        {
            CookingPriorityItem item;
            tempQueue.dequeue(item);
            enqueue(item, -item.finishTime);
        }
        return found;
    }
};