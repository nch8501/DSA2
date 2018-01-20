/*--------------------------------------------------------------------------------------------------
This project was generated in 2013
--------------------------------------------------------------------------------------------------*/
#ifndef _QUEUE_H
#define _QUEUE_H

#include <iostream>


class Queue
{
	//array pointer that contains the elements
	int *pArray = new int[3];

	//setup some base values
	int v1 = 1;
	int v2 = 2;
	int v3 = 3;


	//keep track of the total size of array
	int pArraysize = 3;

	//amount of elements in the queue
	int elements = 3;

	//index of the last element
	int lastElementIndex = 2;


public:

	//constructor
	Queue();

	//copy constructor
	Queue(Queue const& other);

	//copy assignment
	Queue& operator=(Queue const& other);

	//deconstructor
	~Queue();

	//adds an element to the queue
	void Push(int element, Queue original);

	//removes the first element of the queue
	void Pop();


	//gets the size of the queue
	int GetSize();



	//checks if the Queue is empty
	bool isEmpty();
	


	//checks if the Queue is full
	bool isFull();



	//prints the contents of the Queue
	void Print();
	
};


#endif //_QUEUE_H
