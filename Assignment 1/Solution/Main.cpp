/*--------------------------------------------------------------------------------------------------
Comments about this program
--------------------------------------------------------------------------------------------------*/
#include "Main.h"

int main()
{


	Queue queue;


	std::cout << "first print:" << std::endl;
	queue.Print();

	queue.Pop();
	std::cout << "second print:" << std::endl;
	queue.Print();

	std::cout << "third print:" << std::endl;
	queue.Push(4, queue);
	queue.Push(5, queue);
	queue.Push(6, queue);

	queue.Print();


	getchar();
	return 0;
}
