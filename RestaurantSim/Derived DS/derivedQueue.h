#pragma once
#include "../ds/LinkedQueue.h"
#include "../entities/Order.h"

class derivedQueue : public LinkedQueue<Order*>
{
public:
    bool CancelOrder(int id, Order*& cancelledOrder)
    {
        cancelledOrder = nullptr;
        if (isEmpty()) return false;

        bool found = false;
        int n = GetCount();
        // there is a logic problem: what if the order is in the middle the queue order is disturbed
        for (int i = 0; i < n; i++)
        {
            Order* currentOrder = nullptr;
            dequeue(currentOrder);     // remove front and store it in currentOrder

            if (currentOrder && currentOrder->getID() == id)
            {
                cancelledOrder = currentOrder;
                found = true;           // found and removed    mohamed: i removed the return and put a flag
                continue;
            }
            enqueue(currentOrder);     // put it back if not the one we want
        }
        return found;
    }
};