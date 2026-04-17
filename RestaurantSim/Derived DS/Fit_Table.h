#pragma once
#include "../ds/PriQueue.h"
#include "../ds/LinkedQueue.h"
//#include "../PriorityParameters.h"
#include "../entities/Table.h"

class Fit_Tables : public priQueue<Table*>
{
public:
    Table* GetBest(int requiredSeats)
    {
        if (isEmpty()) return nullptr;

        Table* bestTable = nullptr;
        LinkedQueue<Table*> tempQueue; // temporaty Queue for not enough seats

        while (!isEmpty())
        {
            Table* currentItem = nullptr;
            int pri = 0;
            dequeue(currentItem, pri);

            if (currentItem && currentItem->GetFreeSeats() >= requiredSeats)
            {
                bestTable = currentItem;
                break;
            }
            tempQueue.enqueue(currentItem); //  if not found value > required seats
        }
        while (!tempQueue.isEmpty()) // for restoring back onto priQueue
        {
            Table* itemTable=nullptr;
            tempQueue.dequeue(itemTable);

            if (itemTable)
                enqueue(itemTable, -itemTable->GetFreeSeats());//for the best fit table(highest priority)
        }
        return bestTable;
    }

    void print() const override {
        priNode<Table*>* ptr = head;
        while (ptr) {
            cout << "[" << ptr->getItem() << ", " << ptr->getItem()->getCapacity() << ptr->getItem()->GetFreeSeats() <<"], ";
            ptr = ptr->getNext();
        }
    }
};