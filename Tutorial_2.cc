/*
	A function that returns with
	a thread still has access to local 
	variables
*/

struct func 
{
	int& i;
	func (int& i_) : i(i_) {}
	void operator() ()
	{
		for (unsigned j=0; j<1000000;  j++)
		{
			do_something(i); // Potential access to dangling reference
		}
	}
};

void oops()
{
	int some_local_state = 0;
	func my_func(some_local_state);
	std::thread my_thread(my_func);
	my_thread.detach(); // Don't wait for the thread to finish
} // New thread might still be running

// -------- Waiting on a thread to complete ---------
/* call join() on the associated std::therad instance 
   calling join() also cleans up any storage associated
   with the now finished thread.
*/

// Using RAII to wait for a threaed to complete

class thread_guard
{
	std::thread& t;
	public:
		explicit thread_guard(std::thread& t_) : t(t_) {}

		thread_guard(thread_guard const&) = delete;
		thread_guard& operator=(thread_guard const&) = delete;

		~thread_guard() 
		{
			if (t.isjoinable())
			{
				t.join();
			}
		}
};

struct func;

void f()
{
	int x = 0;
	func my_func(x);
	std:thread t(my_func);
	thread_guard g(t);
	do_something_else();
}

// Running threads in the background
/*
	calling detach() on a std::thread object
	leaves the thread to run in the background
	, with no direct means of communicating
	with it.

	It's no longer possible to wait for 
	that thread to complete; if a thread
	becomes detached, it isn't possible
	to obtain a std::thread object that
	references it, so it can no longer
	be joined.

	Ownership and control are passed on
	to the C++ Runtime Library, which
	ensures resources associated with the
	thread are correctly reclaimed when the
	thread exits.


	deamon thread;
*/























