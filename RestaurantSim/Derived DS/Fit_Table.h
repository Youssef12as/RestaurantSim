#pragma once
#include "../ds/PriQueue.h"
#include "../ds/LinkedQueue.h"
#include "../PriorityParameters.h"
#include "../entities/Table.h"

class Fit_Tables : public priQueue<TablePriorityItem>
{
public:
    Table* GetBest(int requiredSeats)
    {
        if (isEmpty()) return nullptr;

        Table* bestTable = nullptr;
        LinkedQueue<TablePriorityItem> tempQueue; // temporaty Queue for not enough seats

        while (!isEmpty())
        {
            TablePriorityItem currentItem;
            int pri = 0;
            dequeue(currentItem, pri);

            int freeSeats = currentItem.tablePtr->GetFreeSeats(); 

            if (freeSeats >= requiredSeats)
            {
                bestTable = currentItem.tablePtr;
                break;
            }
            tempQueue.enqueue(currentItem); //  if not found value > required seats
        }
        while (!tempQueue.isEmpty()) // for restoring back onto priQueue
        {
            TablePriorityItem itemTable;
            tempQueue.dequeue(itemTable);

            int freeSeats = itemTable.tablePtr->GetFreeSeats();   
            itemTable.priorityValue = -freeSeats; //for the best fit table(highest priority)
            enqueue(itemTable, itemTable.priorityValue);
        }
        return bestTable;
    }
};