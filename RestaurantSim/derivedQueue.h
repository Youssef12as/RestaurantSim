#pragma once
#include "./ds/LinkedQueue.h"
#include "./entities/Order.h"

class derivedQueue : public LinkedQueue<Order*>
{
public:
    bool CancelOrder(int id, Order*& cancelledOrder)
    {
        cancelledOrder = nullptr;
        if (isEmpty()) return false;

        int n = GetCount();

        for (int i = 0; i < n; i++)
        {
            Order* currentOrder = nullptr;
            dequeue(currentOrder);     // remove front and store it in currentOrder

            if (currentOrder && currentOrder->getID() == id)
            {
                cancelledOrder = currentOrder;
                return true;           // found and removed
            }
            enqueue(currentOrder);     // put it back if not the one we want
        }
        return false;
    }
};