/* Concurrecy - a single system 
   performing multiple independent
   activities in parallel, rather
   than sequentially, or one 
   after the other.

   Approch to concurrency is to run
   multiple threads in a single process.

   Threads are much like lightweight
   processes: each thread runs independently
   of others and each may run a different
   sequence of instructions.

   But all threads in a process share the
   same address space, and most of the
   data can be accessed directly from 
   all threads-global variables remain
   global and pointets or references to
   objects or dat can be passed among threads.

   The shared address space and lack of
   protection of data between threads makes
   the overhead associated with using multiple
   threads much smaller from what than that from
   using multiple processes.

*/

#include <iostream>
#include <thread>

void hello()
{
	std::cout << "Hello concurrent World\n";
}

int main()
{
	std:thread t(hello);
	t.join(); // wait for thread to finish
}

// Launching threads
void do_some_work();
std::thread my_thread(do_some_work);

/* std::thread works with any 
   callable type, so you can pass an instance
   of a class with a function call operator
   to the std::thread constructor
*/

class background_task
{
	public:
		void operator() () const
		{
			do_something();
			do_something_else();
		}
};

background_task f;
std::thread my_thread(f);

/*
	The supplied function object is copied
	into the strorage belonging to the
	newly created thread of execution
	and invoked from there.
*/

/*Passed as a pointer to a function taking no
params and returns background_task-object, returns
a std::thread object rather than launching a new
thread.
*/
std::thread my_thread(background_task()) 
//Soln
std::thread my_thread((background_task()));
std::thread my_thread{background_task()};
// Or using a lambda expresssion
std::thread my_thread([]{
	do_something();
	do_something_else();
})
/*
	Once you have started your thread
	you, need to explicitly decide whether
	to wait for it to finish (by joining) or
	leave it on its own (by detaching it).

	Make this decision before the std::thread
	object is destroyed-the thread itself may
	well habe finished before you join with it
	or detach it, and if you detach it, then if 
	the thread is still running, it will continue
	to do so, and mya continue running long after
	the std::thread object is destroyed; it will
	only stop running when it finally returns from
	the thread function.
*/

/*
	If you don't wait for the thread to finish
	you need to ensure that the data accessed
	by the thread is valid until the thread
	has finished with it.
*/

















