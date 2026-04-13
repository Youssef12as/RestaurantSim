#pragma once
#include"entities/Order.h"
#include "entities/Scooter.h"
#include"entites/Table.h"

struct OVGPriorityItem
{
    double priorityValue;
    Order* orderPtr;

    OVGPriorityItem(double priority = 0, Order* order = nullptr) : priorityValue(priority), orderPtr(order)
    {}
    bool operator>(const OVGPriorityItem& other) const
    {
        return priorityValue > other.priorityValue;
    }

    bool operator==(const OVGPriorityItem& other) const
    {
        return priorityValue == other.priorityValue && orderPtr == other.orderPtr;
    }
};

struct CookingPriorityItem
{
    int finishTime; // time of finish cooking 
    Order* orderPtr;

    CookingPriorityItem(int time = 0, Order* o = nullptr) : finishTime(time), orderPtr(o) {}

    bool operator>(const CookingPriorityItem& rhs) const
    {
        return finishTime < rhs.finishTime;
    }
    bool operator==(const CookingPriorityItem& rhs) const
    {
        return finishTime == rhs.finishTime && orderPtr == rhs.orderPtr;
    }
};

struct TablePriorityItem
{
    int priorityValue;
    Table* tablePtr;

    TablePriorityItem(int p = 0, Table* t = nullptr) : priorityValue(p), tablePtr(t)
    {}

    bool operator>(const TablePriorityItem& other) const
    {
        return priorityValue > other.priorityValue;
    }

    bool operator==(const TablePriorityItem& other) const
    {
        return priorityValue == other.priorityValue && tablePtr == other.tablePtr;
    }
};

struct ScooterPriorityItem
{
    int priorityValue;
    Scooter* scooterPtr;

    ScooterPriorityItem(int p = 0, Scooter* s = nullptr) : priorityValue(p), scooterPtr(s)
    {}

    bool operator>(const ScooterPriorityItem& rhs) const
    {
        return priorityValue > rhs.priorityValue;
    }

    bool operator==(const ScooterPriorityItem& rhs) const
    {
        return priorityValue == rhs.priorityValue && scooterPtr == rhs.scooterPtr;
    }
};

struct InServicePriorityItem
{
    int priorityValue;
    Order* orderPtr;
    InServicePriorityItem(int p = 0, Order* o = nullptr) : priorityValue(p), orderPtr(o)
    {}

    bool operator>(const InServicePriorityItem& rhs) const
    {
        return priorityValue < rhs.priorityValue;
    }
    bool operator==(const InServicePriorityItem& rhs) const
    {
        return priorityValue == rhs.priorityValue && orderPtr == rhs.orderPtr;
    }
};