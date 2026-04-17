#pragma once
#include "priNode.h"
#include <iostream>
#include "../entities/Order.h"
#include "../entities/Table.h"
#include "../entities/Scooter.h"
using namespace std;


//This class impelements the priority queue as a sorted list (Linked List)
//The item with highest priority is at the front of the queue
template <typename T>
class priQueue
{
protected:
    priNode<T>* head;
    static int count;
public:
    priQueue() : head(nullptr) {}

    ~priQueue() {
        T tmp;
        int p;
        while (dequeue(tmp, p));
    }

    //insert the new node in its correct position according to its priority
    void enqueue(const T& data, int priority) {
        priNode<T>* newNode = new priNode<T>(data, priority);

        if (head == nullptr || priority > head->getPri()) {

            newNode->setNext(head);
            head = newNode;
            count++;
            return;
        }

        priNode<T>* current = head;
        while (current->getNext() && priority <= current->getNext()->getPri()) {
            current = current->getNext();
        }
        newNode->setNext(current->getNext());
        current->setNext(newNode);
        count++;
    }

    bool dequeue(T& topEntry, int& pri) {
        if (isEmpty())
            return false;

        topEntry = head->getItem(pri);
        priNode<T>* temp = head;
        head = head->getNext();
        delete temp;
        count--;
        return true;
    }

    bool peek(T& topEntry, int& pri) {
        if (isEmpty())
            return false;

        topEntry = head->getItem();
        pri = head->getPri();
        return true;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    virtual void print() const {
        priNode<T>* ptr = head;
        while (ptr) {
            cout << ptr->getItem() << ", ";
            ptr = ptr->getNext();
        }
    }

    void printInservice() const {
        priNode<T>* ptr = head;
        while (ptr) {
            cout << "[" << ptr->getItem() << ", ";
            DineInOrder* dineIn = dynamic_cast<DineInOrder*>(ptr->getItem());
            if (dineIn != nullptr) {
                cout << "T" << dineIn->getAssignedTable()->GetId();
            }
            DeliveryOrder* deliv = dynamic_cast<DeliveryOrder*>(ptr->getItem());
            if (deliv != nullptr) {
                cout << "S" << deliv->getAssignedScooter()->getID();
            }
            cout << "]";
            ptr = ptr->getNext();
        }
    }

    int GetCount() const {
        return count;
    }
};

template <typename T>
int priQueue<T>::count = 0;
