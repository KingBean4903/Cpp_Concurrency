/*
	Synchronizing Concurrent Operations
	-----------------------------------------
	-----------------------------------------

	Need to synchronize actions across separate
	threads.

	One thread might need to wait for another thread
	to complete a task before the first thread can 
	complete its own.

	In general, it's common to want a thread to wait
	for a specific event to happen or a condition
	to be true.

	The C++ Standard Library provides facilities to
	handle it, in the form of condition variables
	and futures.

	Synchronization facilities liek latches and 
	barriers.

	We will focus on waiting for events with:
		1. Conditional variables
		2. futures
		3. Latches
		4. barriers
	

	Waiting for an event or other condition
	-----------------------------------------------
	
	If one thread is waiting for a second therad
	to complete a task, it has several options:
		1. Keep checking a flag in shared data
		   (protected by a mutes) and have the
		   second thread set the flag when it
		   completes the task.
			- This is wasteful on resources.
	
	The most basic mechanism for waiting for an
	event to be triggered by another thread (such
	as the presence of additional work in the 
	pipeline mentioned previously) is the condition
	varialbe.
	
	A condition variable is associated with an event
	or other condition, and one or more threads can 
	wait for that condition to be satisfied.

	When a thread has determined that the condition
	is satisfied, it can then notify one or more
	of the threads waiting on the condition varialbe
	in order to wake them up and allow them to 
	continue processing.

	Waiting for a condition with a condition variable
	--------------------------------------------------

	C++ STL provides 2 implementations of a condition
	variable
		1. std::condition_variable
		2. std::condition_variable_any
	Both are declared in <condition_variable> lib header.

	In both cases, they need to work with a mutex in
	order to provide appropriate synchronization.

	std::condition_variable is limited to working with
	std::mutex, should be preferred unless the additional
	flexibility is required.

	std::condition_variable_any: can work with anything
	that meets the minimal criteria for being mutex-like,
	it has a potential for additional costs in terms
	of size and performance, or OS resources.

	How do you let the thread that's waiting for work
	sleep until there's data to process?	
*/
std::mutex mut;
// queue to pass data between the 2 threads
std::queue<data_chunk> data_queue;
std::condition_variable data_cond;
void data_preparation_thread()
{
	while(more_data_to_prepare())
	{
		data_chunk const data = prepare_data();
		{
			// lock mutex, protecting the queue
			std::lock_guard<std::mutex> lk(mut);
			data_queue.push(data);
		}
		// notifying the waiting thread
		// after unlocking the mutex.
		data_cond.notify_one();
	}	
}

void data_processing_thread() 
{
	while (true) {
		// lock the mutex
		std::unique_lock<std::mutex> lk(mut);
		/*
			Pass the lock object and the condition
		 	to wait on.
			Lambda used as a predicate. (a function that
			returns true or false).
			wait() check the condition and returns
			if it's true.
			
			If the condition isn't satisfied wait() unlocks
			the mutex and puts the thread in a blocked or 
			waiting state.
			
		*/
		data_cond.wait(
			lk,
			[]{ return != data_queue.empty(); });
		data_chunk data = data.queue.front();
		data_queue.pop();
		lk.unlock();
		process(data);
		if (is_last_chunk(data))
			break;
	}

}

/*
	When the condition variable is notified by a call
	to notify_one() from the data-preparation thread,
	the thread wakes from its slumber, reacquires the
	lock on the mutex and checks the condition again.


	std::unique_lock: enable the waiting thread to unlock
	the mutex while it's still waiting and lock it again
	afterward.o
	
	wait() can take a function or callable object.
		During a call to wait(), a condition variable may
		check the supplied condition any number of times;
		but it always does so with the mutex locked and will 
		return immediately if the function provided to test
		the mutex locked and will return immediately if the 
		function provided to test the condition returns true.

	When the waiting thread reacquires the mutex and checks
	the condition, if it isn't in direct response to a notification
	from another thread, its called a spurious wake.

	Building a thread-safe queue with condition variables
	-----------------------------------------------------------
	
	If the waiting thread is going to wait only once, so when the
	condition is true it will never wait on this condition
	variable again, a condition variable might nott be the
	best choice for synchronization mechanisms.

	This is especially true if the condition being waited for is
	the availability of a particular piece of data. In this 
	scenario, a future might be more appropriate.


	Waiting for one-off events with futures.
	----------------------------------------------------------------
	If a thread needs to wait for a specific one-off event, it
	somehow obtains a future representing that event.
	
	There are 2 sorts of futures in the C++ Standard Library, implemented
	as two class templates declared in the <future> lib header.

		1. unique futures (std::future<>).
		2. shared futures (std::shared_future<>).
	
	These are modeled after std::unique_ptr and std::shared_ptr.

	An instance of std::future is the one and only instance
	that refers to its associated event, whereas multiple
	instances of std::shared_future may refer to 
	the same event.

	This associated data is the reason these are templates;
	just like std::unique_ptr and std::shared_ptr, the
	template parameter is the type of the associated data.

	The std::future<void> and std::shared_future<void> 
	template specializations should be used where there's no
	associated data.

	Although futures are used to communicate between threads,
	the future objects don't provide synchronized access.

	The CTS provides extended versions of these class 
	templates in the std::experimental namespace: 
		std::experimental::future<> 
		std::experimental::shared_future<>.
	
	Returning values from background tasks
	----------------------------------------------
	
	std::thread does not provide a direct mechanism
	for transferring the result back.
	This is where std::async function template
	comes in.

	You use std::async to start an asynchronous task
	for whish you don't nee the result right away.
	
	Rather than giving you a std::thread object to wait
	on, std::async returns a std::future object, which
	will eventually hold the return value of the function.

	When you need the falue, you can just call get() on
	the future, and the thread blocks until the future
	is ready and then returns the value.
*/

#include <future>
#include <iostream>

int find();
void do_more();
int main() {
	std::future<int> tAns = std::async(find);
	do_some();
	std::cout << tAns.get() << std::endl;
}

/*
	std::async allows you to pass additional arguments
	to the function by adding extra arguments to the 
	call, in the same way that std::thread does.

	Just as with std::thread if the arguments are
	rvalues, the copies are created by moving the
	originals.
	This allows the use of move-only types as both the
	function object and the arguments.
*/
#include <string>
#include <future>
struct X 
{
	void foo(int, std::string const&);
	std::string bar(std::string const&);
};
X x;
auto f1 = std::async(&X::foo,&x,42,"hello");
// Calls p->foo(42,"hello") where p is &x
auto f2 = std::async(&X::bar, x "goodbye"); // Calls tmpx.bar("goodbye")
										   // where tmpx is a copy of x
struct Y
{
	double operator()(double);
};
Y y;
auto f3 = std::async(Y(), 3.142); // Calls tmpy(3.142) where
// tmpx is move-constructed from Y()
auto f4 = std::async(std::ref(y), 2.178); // Calls y(2.178)
X baz(X&);
std::async(baz, std::ref(x)); // Calls baz(x)
class move_only
{
	public:
		move_only();
		move_only(move_only&&);
		move_only(move_only const&) = delete;
		move_only& operator=(move_only&&);
		move_only& operator=(move_only const&) = delete;
		void operator()();
};

auto fs = std::asnyc(move_only()); // Calls tmp() where tmp
// is constructd from std::move(move_only())

/*

	By default, it's up to the implementation whether
	std::async starts a new thread, or whether the task
	runs synchronously when the future is waited for.
	
	You can specify which to use with an additional 
	parameter to std::async before the function to call.

	This parameter is of type std::launch, and can either
	be std::launch::deferred to indicate that the function
	call is to be deferred until wait() or get() is called
	on the future,
	
	std::launch::async to indicate the function
					   must be run on its own thread.
	std::launch::deferred - the function call is to be 
			deferred until either wait() or get() is called
		on the future.

	std::launc::async | std::launch::deferred - default choice

*/
auto f5 = std::async(std::launch::async, Y(), 1.2);
auto f6 = std::async(std::launch::deferred,baz,
			std::ref(x));
auto f7 = std::async(
		std::launch::deferred | std::launch::async,
		baz, std::ref(x));
auto f8 = std::async(baz, std::ref(z));
f6.wait();

/*
	std::async is not the only way to associate
	a std::future with a task; you can do it by
	wrapping the task in an instance of the
	std::packaged_task<> class template or
	by writing code to explicitly set the values
	using the std::promise<> class template.

	Associating a task with a future
	-----------------------------------------
	std::packaged_task<> ties a future to a function
	                      or callable object.
	
	When a std::package_task<> object is invoked, it
	calls the associated function or callable object
	and makes the future ready, with the return value
	as the associated data.

	If a large operation can be divided into 
	self-contained sub-tasks, each of these can be 
	wrapped in a std::packaged_task<> instance, and
	then that instane passed to a task scheduler or
	thread pool.

	This abstracts out the details of the tasks;
	the scheduler just deals with std::packaged_task<>
	instances rather than individual functions.

	When you construct an instance of std::packaged_task,
	you must pass in a function or callable object that
	can accept the specified parameters and that 
	returns a type that's convertible to the specified
	return type.

	The template parameter for the std::packaged_task<>
	class template is a function signature, like void<>
	for function taking no parameters with no return value.

	The return type of the specified function signature
	identifies the type of the std::future<> returned
	from the get_future() member function,whereas the
	argument list of the function signature is used
	is used to specify the signature of the packaged
	task's function call operator.

	Partial class definitio for a specialization of
	std::packaged_task<>
	------------------------------------------------
*/
template<>
class packaged_task<std::string(std::vector<char>*,int)>
{
	public:
		template<typename Callable>
		explicit packaged_task(Callable&& f);
		std::future<std::string> get_future();
		void operator()(std::vector<char>*. int);
};

/*
	The std::packaged_task object is a callable
	object, and it can be wrapped in a std::function
	object, passd to a std::thread as the thread function
	, passed to another function that requires a
	callable object, or even invoked directly.

	You can thus wrap a task in std::packaged_task and
	retrieve the future before passing the std::packaged_task
	objet elsewhere to be invoked in due course.
	When you need the result you can wait for the
	future to become ready.
*/

template<typename F>
std::future<void> post_task_for_gui_thread(Func f) {
	std::packaged_task<void> task(f);
	std::future<void> res = task.get_future();
	std::lock_guard<std::mutex> lk(m);
	tasks.push_back(std::move(task));
	return res;
}

/* 
	What about tasks that cannot be expressed
	as a simple function call or those tasks
	where the result may come form more than
	one place?

	This can be handled with std::promise,
	a way of creating a future.

	Making (std::) promises
	----------------------------------------

	Saving an Exception for the future
	---------------------------------------
	If the function call invoked as part of
	std::async throws an exception, the exception
	is stored in the future in place of a
	stored value, the future becomes read and a
	call to get() throws that stored exception.

	std::future has its limitations, one of them
	being that only one thread can wait on the
	result.

	Waiting from multiple threads
	---------------------------------------------
	Although std::future handles all the synchronization
	necessary to transfer the data from one thread
	to another, calls to the other member functions
	of a particular std::future instance are not synchronized
	with each other.

	If you access a single std::future object from 
	multiple threads without additional synchronization,
	you have a data race and undefined behaviour.

	Only one thread can access the value, because after
	the first call to get() there's no value left to 
	retrieve.

	std::shared_future - multiple threads can wait on
	for the same event.

	Whereas std::future is only moveable(so ownership
	can be transferred between instances, but only one
	instance refers to a particular asynchronous result
	at a time).
	std::shared_future instances are copyable(so you can
	have multiple objects referring to the same associated
	state).

	Now, with std::shared_future, member functions on
	an individual object are still unsynchronized, so as
	to avoid data reces when accessing a single object from
	multiple thread, you must protect access with a lock.

	Waiting with a time limit
	-------------------------------------------------------
	All the blocking calls introduced will block for an
	indefinite period of time suspending the thread until
	the event being waited for occurs.
	
	There are 2 sorts of timeouts
		1. Duration based timeout - where you wait for a
			specific amount of time i.e 30 milliseconds.
		2. Absolute timeout - where you wait until a 
		   specific point in time.

	Most of the waiting functions provide various handles
	that handle both forms of timeouts.

	The variants that handle the duration based timeouts
	have a _for suffix, and those that handle the absolute
	timeouts hava an _until suffix.

	How times are specified in C++
	----------------------------------------------------
	1. Clocks
	   A clock is a source of information in C++.
	   Specifically, a clock is a class that provides four
	   distinct pieces of info
	   	a. The time now.
		b. The type of he value used to represent the times
		   obtained from the clock.
		c. The tick period of the clock.
		d. Whether or not the clock ticks at a uniform rate
		   and is therefore considered to be a steady clock.

		Current time is obtained by calling now() static
		member function for that clock class.
			i.e std::chrono::system_clock::now() - return
				current time of the system clock.
		The type of the time for a particular clock is
		specified by the time_point member typedef, so
		the return type of some_clock::now is
		some_clock::time_point.

		Using synchronization of operations to simplify
		code
		--------------------------------------------------
		Focus on operations that need synchronizing rather
		than the mechanics.
		One way this can help simplify your code is that it
		accomodates a much more functional (functional 
		programming) approach to programming concurrency.

		Rather than sharing data directly between threads
		, each task can be provided with the data it needs
		, and the result can be disseminated to any other
		threads that need it through the use of futures.
		

		Functional programming with futures
		---------------------------------------------------
		Refers to a style of programming where the result
		of a function call depends soleley on the parameters
		to that function and doesn't depend on any external state

		A pure function is a function that doesn't modify
		any external state either; the effects of the function
		are entirely limited to the return value.

		This makes things easy to think about, especially when
		concurrency is involved, because many of the problems
		associated with shared memory disappear.

		If there are no  modifications to shared date, there
		can be no race conditions and thus no need to protect
		share data with mutexes either.

		Futures are the final piece to the puzzle that makes
		FP-style concurrency viable in C++; a future can
		be passed around between threads to allow the result
		of one computation to depend on the result of another,
		without any explicit access to shared data..


		Synchronizing Operations with message passing
		-------------------------------------------------------
		Finite state machine.
		Threads act as state machines.
		Actor model -there are several discrete actors in the
		system, which send messages to each other to perform
		the task at hand, and there's no shared state except
		that which is directly passed viam messages.

		It is an example of using multiple threads to separate
		concerns and as such requireds you to explicitly decide
		how to divide tasks between threads.

		Continuation-style concurrency with the Concurrency TS
		---------------------------------------------------------
		

		Latches and barriers in the Concurrecy TS
		----------------------------------------------------------
		A latch is a synchronization object that becomes ready
		when its counter is decremented to zero.

		Its name comes from the fact that it latches the output-
		once its ready, it stays ready until it is destroyed.

		A latch is thus a lightweight facility for waiting
		for a series of events to occur.

		A barrier is a reusable synchronization component used for
		internal synchronization between a set of threads.

		Whereas a latch does't care which threads decrement the
		counter-the same thread can decrement the counter multiple
		times or multiple threads can each decrement the counter once,
		or some combination of the two-with barriers, each thread can
		only arrive at the barrier once per cycle.

		When threads arrive at a barrier, they block until all of
		the threads involved have arrived at the barrier, at which
		point they are released.
		The barrier can the be reused-the threads can then arrive 
		at the barrier again to wait for all the threads fo the next
		cycle.

		Latches are simpler than barriers.

		A basic latch type: std::experimental::latch
		-----------------------------------------------------------
		std::experimental::latch <experimental/latch>
		When you construct a std::experimental::latch, you specify
		the initial counter value as the one and only argument to
		the constructor.

		Then, as the events that you are waiting for occur, you call
		count_down on your latch object, and the latch becomes ready
		when that count reaches zero.

		std::experimental::barrier
		------------------------------------------------------------
		The Concurrency TS provides two types of barriers in the
		<experimental/barrier> header
			1. std::experimental::barrier
			2. std::experimental::flex_barrier
		A group of threads operating on some data.
		Each thread can do its processing on the data independently
		of others, so no synchronization is needed during the
		processing, but al the threads must have completed their
		processing before the next data item can be processed, or before
		the subsequent processing can be done.


		You construct a barrier with a count specifying the number of 
		threads involved in the synchronization group.
		As each thread is done with its processing, it arrives at 
		the barrier and waits for the res of the group by
		calling arrive_and_wait on the barrier object.

		When the last thread in the group arrives, all the threads
		are released and the barrier is reset.

		The threads in the group can resume their processing and either
		process the next data item or proceed with the next stage of
		processing, as appropriate.

	

		

		
*/
































