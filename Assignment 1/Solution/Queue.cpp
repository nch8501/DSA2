#include "Queue.h"



Queue::Queue()
{


	//set all pointers to default values
	pArray[0] = v1;
	pArray[1] = v2;
	pArray[2] = v3;
}


Queue::Queue(Queue const & other)
{
	//double the size of the queue
	pArraysize = other.pArraysize*2;

	//create the queue
	pArray = new int[pArraysize];

	//copy the number of elements and the index
	elements = other.elements;
	lastElementIndex = other.lastElementIndex;

	//copy over all elements
	for (int i = 0; i < elements; i++)
	{
		pArray[i] = other.pArray[i];
	}
}


Queue & Queue::operator=(Queue const & other)
{
	if (this != &other)
	{
		//double the size of the queue
		pArraysize = other.pArraysize;

		//create the queue
		pArray = new int[pArraysize];

		//copy the number of elements and the index
		elements = other.elements;
		lastElementIndex = other.lastElementIndex;

		//copy over all elements
		for (int i = 0; i < elements; i++)
		{
			pArray[i] = other.pArray[i];
		}
	}

	return *this;


}




Queue::~Queue()
{
}


void Queue::Push(int element, Queue original)
{
	//check if the queue is full
	if (isFull())
	{
		//make a new Queue
		Queue temp(original);
		original = temp;
	}

	pArray[lastElementIndex + 1] = element;
	lastElementIndex++;
	elements++;
}


void Queue::Pop()
{
	//check if queue is empty
	if (isEmpty())
	{
		std::cout << "Nothing to Pop" << std::endl;
		return;
	}

	//go through each element
	for (int i = 0; i < lastElementIndex; i++)
	{
		//set their index back one
		pArray[i] = pArray[i + 1];
	}

	//update the index of the last element
	lastElementIndex--;

	//update the amount of elements
	elements--;
}


int Queue::GetSize()
{
	return elements;
}



bool Queue::isEmpty()
{
	//check if empty
	if (elements == 0)
	{
		return true;
	}

	return false;
}


bool Queue::isFull()
{
	//check if last element is at the end of the queue
	if (pArraysize == elements)
	{
		return true;
	}

	return false;
}


void Queue::Print()
{
	//print each element
	for (int i = 0; i < elements; i++)
	{
		std::cout << pArray[i] << std::endl;
	}
}
