/*

	Multithreading-aware memory model.

	One of the goals of the Standards Committee
	is that there will be no need for a lower-level
	language than C++.

	Allow programmers to get as close the the
	machine.

	Atomic types and operations allow just that,
	providing faciliteis for low-level synchronization
	operations that will commonly reduce to one or
	more CPU instructions.

	Memory model basics
	-----------------------------------------------
	
	There are two aspects to the memory model:
		1. structural aspects - relate to how
		   things are laid out in memory
		2. concurrency aspects.

	Objects and memory locations
	----------------------------------------------
	All data in a C++ program is made up of objects.

	C++ standard defines an object as "a region of
	storage."

	Whatever, its type, an object is stored in one
	or more memory locations.

	Point to note:
	1. Every variable is an object.
	2. Every object occupies at least one memory location.
	3. Adjacent bit fields are part of the same
	   memory location.

	Objects, memory locations and concurrency
	-------------------------------------------------
	If two threads access separate memory locations,
	there's no problem.

	Using synchronization properties of atomic operations
	either on the same memory locations to enforce an 
	ordering between the accesses in the two threads.

	Modification orders
	--------------------------------------------------
	Every object in a C++ program has a modification
	order composed of all the writes to that object
	from all threads in the program, starting with
	the object's initialization.
	
	In any execution of the program all threads in the
	system must agree on the order.

	If the object in question isn't one of the atomic
	types, you're responsible for making certain that
	there's sufficient synchronization to ensure that
	threads agree on the modification order of each
	variable.

	If different threads see distinct sequences of values
	for a single variable, you have a data race and
	undefined behaviour.

	If you do use atomic operations, the compiler is 
	responsible for ensuring that the necessary
	synchronization is in place.

	Although all threads must agree on the  modification
	orders of each individual object in a program, they
	don't necessarily have to agree on the relative ofder
	of operations on separate objects.

	How can atomic operations enfore ordering ?

	Atomic operations and types in C++
	----------------------------------------------------

	An atomic operation is an indivisible operation.
	You can't observe such an operation half-done from any
	thread in the system; it's either done or not done.

	In C++, you need to use an atomic type to get an 
	atomic operation in most cases.

	The standard atomic types
	----------------------------------------------------
	Found in <atomi> header.
	All operations on such types are atomic and only 
	operations on these types are atomic in the sense
	of the language definition.

	Since C++17, all atomic types hava a static constexpr
	member variable, X::is_always_lock_free, which is true
	if and only if the atomic type X is lock-free for all
	supported hardware that the output of the current
	compilation might run on.

	The only type that doesn't provide an is_lock_free()
	member function is std::atomic_flag.

	This type is a simple Boolean flag and opeations
	on this type are required to be lock-free; once you
	have a simple lock-free Boolean flag, you cna use
	that to implement a simple lock and implement all
	other atomic types using that as a basis.

	Objects of std::atomic_flag are initialized to clear
	, and they can either be queried and set (with the
	test_and_set() member function) or cleared(with the
	clear() member function).

	That's it: no assignment, no copy construction, no
	test and clear, no other operations at all.

	The remaining atomic types are accessed through
	specializations of the std::atomic<> class template
	and are a bit more full-featured but may not be lock
	free.

	On most platform it's expectd that the atomic 
	variants of all the built-in types (such as
	std::atomic<int> and std::atomic<void*> are indeed 
	lock free, but it isn't required.

	bitwise operations such as &= aren't defined for 
	plain pointers, so they aren't defined for atomic
	pointers either.

	The standard atomic types are not copyable or 
	assignable in the conventional sense, in that
	they have no copy constructors ro copy assignment
	operators.

	They do however, support assignment from and 
	implicit conversion to the corresponding built-in
	types as well as direct 
			load() and store() member functions.
			exchange,
			compare_exchange_weak()
			compare_exchange_strong()

	They also support the compound assignment operators
	where appropriate +=, -= *=., \= and so on, and 
	the integral types and std::atomic<> specialiations
	for ++ and -- pointers support.

	The std::atomic<> class template isn't only a set 
	of specializations.
	It does have a primary template that can be used
	to create an atomic variant of a user defined type.
	Because it's a generic class template, the operations
	are limited to load(), store() exchange() and 
	compare_exchange_weak() and compare_exchange_strong().
	
	Each of the operations on the atomic types has an
	optional memory-ordering argument which is  one of the
	values of the std::memory_order enumeration.

	This argument is used to specify the required
	memory-ordering semantics, it has six possible values
		1. std::mmeory_order_relaxec
		2. std::memory_order_acquire
		3. std::memory_order_consume
		4. std::memory_order_acq_rel
		5. std::memory_order_release
		6. std::memory_order_seq_cst - default ordering

	Operations are divided into 3 categories:
		
		1. Store operations, which can have 
			memory_order_relaxed
			memory_order_release
			memory_order_seq_cst ordering
		2. Load operations, which can have 
			memory_order_relaxed
			memory_order_consume
			memory_order_acquire
			memory_order_seq_cst
		3. Read-modify-write ops
			memory_order_relaxed
			memory_order_consume
			memory_order_acquire
			memory_order_release
			memory_order_acq_rel
			memory_order_seq_cst
	
	Operations on std::atomic_flag
	-----------------------------------------------
	std::atomic_flag is the simplest standard atomic
	type, which represents a Boolean flag.

	Objects of this type can be in one of two states:
	set or clear.

	It's intended as a building block.

	Objects of std::atomic_flag must be initialized with
	ATOMIC_FLAG_INIT.This initailizes the flag to a clear
	state.
	The flag always starts clear.
	
	std::atomic_flag f = ATOMIC_FLAG_INIT;
	
	Its the only atomic guaranteed to be lock free.

	Once initialized you have three things you can do to
	it
		1. destroy it. destructor
		2. clear it. clear()
		3. set it. test_and_set()
		4. query the previous state.

	clear() is a store operation and so can't have
	memory_order_acquire or memory_order_acq_rel semantics
	but test_and_set() is a read-modify-write operation
	and so can have any of the memory-ordering tags applied.

*/

	std::atomic_flag f = ATOMIC_FLAG_INIT;
	f.clear(std::memory_order_release); /*
		explicit;u request that the flag is cleared
		with release semantics
	*/ 
	bool x = f.test_and_set(); /*
		uses default memory ordering or setting the
		flag and retrieving the old value
	*/
	
/*
	
	A single operation on two distinct objects
	can't be atomic.
	
	std::atomic_flag is  so limited that it can't 
	even be used as a general Boolean flag,
	because it doesn't have a simple nonmodifying
	query operation. For that we will use 
	std::atomic<bool>


	Operations on std::atomic<bool>
	----------------------------------------------
	The most basic of the atomic integral types is
	std::atomic<bool>.

	This is a more full-featured Boolean flag than
	std::atomic_flag.

	You can construct it from a non-atomic bool, so it
	can be initially true or false, and you can assign
	to instances of std::atomic<bool> from a non-atomic
	bool:
		std::atomic<bool> b(true);
		b = false;
	so it can initially be true or false, and you can
	assign to instances of std::atomic<boo> from a
	non-atomic bool.

	The assignment operators of atomic types return
	values of non-atomic types rather than references.

	By returning the result of the assignment as 
	non-atomic value, you can avoid this additional
	load, and you know that the value obtained is
	the value stored.

	Writes are done with store() and the memory order
	can be specified.

	exchange() - member function allows you to replace
	the stored value with a new one of your choosing
	and atomically retrieve the original value.

	load() - plain nonmodifying query of the value.

	store() - is a store operation.
	load() - is a load operation.
	exchange() - is a read-modify-write operation.
			     it also introduces an operation to store
				 a new value if the current value is equal
				 to an expected value.		
*/

std::atomic<bool> b;
bool x = b.load(std::memory_order_acquire);
b.store(x);
x = b.exchange(false, std::memory_order_acq_rel);

/*
	Storing a New Value (Or Not) Depending on the 
	current value
	------------------------------------------------------
	compare_exchage_weak()
	compare_exchange_strong();

	The compare-exchange operation is the cornerstone
	of programming with atomic types.

	It compares the value of the atomic varialbe with
	a supplied expected value and stores the supplied
	desired value if they're equal.

	The return of compare-exchange  functions is a bool
	, which is true if the store was performed and false
	otherwise.

	The operation is said to succeed if the store was done
	(because the values were equal). and fail otherwise;

	Because compare_exchange_weak() can fail spuriously,
	it must typically be used in a loop:

	compare_exchange_strong() - use for time-consuming
	calcultions.

	std::atomic<bool> may not be lock-free; the implementation
	may have to acquire a  mutex internally in order to ensure
	the atomicity of operations.

	is_lock_free() - member function to check whether operations
	on std::atomic<boo> are lock free. This is common to all
	atomic types other than std::atomic_flag.

	Operations on std::atomic<T*> pointer arithmetic
	---------------------------------------------------------

	The atomic form of a pointer of some type T is std::atomic<T*>,
	just as the atomic form of boolo is std::atomic<bool>.
	The interface is the same, although it operates on values
	of the pointer type.

	its neither copy-constructible not copy-assignable, although 
	it can be both constructed from  the suitable pointer values.
 	
	As well as the obligatory is_lock_free() member function, 
	std::atomic<T*> also has 
		1.load(),
		2.store(), 
		3.exchange(), 
		4.compare_exchange_weak(), and 
		5.compare_exchange_strong() member functions with similar semantics
	to those of std::atomic<boo>.

		6 pointer arithmetic operations
		7.fetch_add() and fetch_sub() - do atomic addition
			and subtraction on the stored address.
		8.+= , -= , ++, --
	The function forms alsow allow the memory-ordering
	semantics to be specified as an additional function
	call argument.

	Operations on standard atomic integral types
	-----------------------------------------------------
	load()
	store() exchange() 
	compare_exchange_weak()
	compare_exchange_strong()
	fetch_add()/ fetch_sub()/ fetch_and()/ fetch_or()
	fetch_xor()
	operations (+= , -=, &=, |= and ^=, ++x, x++, --x, x--)
	Division and multiplication and shift ops are missing.

	The std::atomic<> primary class template
	-----------------------------------------------------
	The primary tmplate allows a user to create an
	atomic variant of a user-defined type, in addition to
	the standard atomic types.

	In order for some UDT to to qualify it must have
	a trivial copy assignment operator, the type must not
	have any virtual functions or virtual bases amd must
	use the compiler generated copy-assignment operator.
	This permits the compiler to use memcpy() or an 
	equivalent operation for assignment operations because
	there is no user-written code to run.

	Free functions for atomic operations
	----------------------------------------------------------
	But there are also equivalent nonmember functions for
	all the operations on the various atomic types.
	
	The nonmember functions are named after corresponding
	member functions but with the atomic_ prefix

	i.e std::atomic_load().

	These functions are then overloaded for each
	of the atomic types.

	They come in two varieties
		1. One without the tag 
		2. one with an _explicit suffix
		   and additional parameter or parameters for
		   the memory-ordering tag or tags.
		   std::atomic_store(&atomic_var, new_value);
		   std::atomic_store_explicit(&atomic_var, new_value,
		   	std::memory_order_release).

		Whereas the atomic object being referenced by the member 
		functions is implicit, all the free functions take
		a pointer to the atomic object as the first parameter.
	
		The free functions are designed to be C compatible,
		so they use pointers rather than references in all 
		cases.

		The C++ STL also provides free functions for accessing
		instances of std::shared_ptr<> in an atomic fashion.
		Atomic operations available are 
			load,
			store, exchange, compare_exchange
		They take a std::shared_ptr<>* as the first argument.
*/
std::shared_ptr<my_data> p;
void f()
{
	std::shared_ptr<my_data> local = std::atomic_load(&p);
	process_data(local);
}

void process_more()
{
	std::shared_ptr<my_data>  ptr(new my_data);
	std::atomic_store(&p, ptr);
}

/*
	As with the atomic operations on other types,
	the _explicit variants are also provided to allow
	you to specify the desired memory orderings,
	and the std::atomic_is_lock_free() function can
	be used to check whether the implementation uses
	locks to ensure atomicity.

	The standard atomic types allow the user to 
	enforce an ordering of operations between threads.

	
		The standard atomic types allow the user to 
		enforce an ordering of operations between threads.

		
			The standard atomic types allow the user to 
			enforce an ordering of operations between threads.

     The synchronizes-with relationship
	 -------------------------------------------------------
	
	The happens-before relationship
	------------------------------------

	Release sequences and synchronizes-with
	-----------------------------------------------------------

	Memory ordering for atomic operations
	---------------------------------------------------------
	There are 6 memory ordering operations
		1. memory_order_relaxed
		2. memory_order_consume
		3. memory_order_acquire
		4. memory_order_release
		5. memory_order_acq_rel
		6. memory_order_seq_cst - default mem ordering

	They represent 3 models
		1. sequentially consistent
			memory_order_seq_cst
		2. acquire release
			memory_order_consume
			memory_order_acquire
			memory_order_release
			memory_order_acq_rel
		3. relaxed
			memory_order_relaxed

	They can have varying costs on different CPU
	architectures.

	Sequentially Consistent Ordering
	--------------------------------------------
	This is the default ordering.

	Assumption is that all operations in a multithreaded
	program is as if all these operations were
	performed in some particular sequence by a single 
	thread.

	All threads must see the same order of operations.

	It also means that operations can't be reordered;
	if your code has one operation before another
	thread, that ordering must be seen by all other threads.

	A sequentially consistent store synchronizes with
	a sequentially consisitent load of the same varialbe
	that reads the value stored.

	Sequential consistency is the most straightforward
	and intuitive ordering, but it's also the most
	expensive memory ordering because it requires
	global synchronization between all threads.
	On a mulitprocessor system this may require extensive
	and time-consuming comm between processors.

	On a weakly-ordered machine with many processors, it
	can impose a noticeably performance penalty.
	x86 and x86-64 processors offer sequential consistency
	relatively cheaply.


	Non-sequentially Consistent Memory Orderings
	----------------------------------------------------------
	threads don't have to agree on the ordering 
	of events.

	In the absence of other ordering constraints, the only
	requirement is that all threads agree on the modification
	order of each individual variable.
	
	Operations on distinct variables can appear in different
	orders on different threads, provided the values seen
	are consistent with any additional ordering constraints
	imposed.


	Relaxed Ordering
	--------------------------------------------------------
	Operationss on atomic types performed with relaxed ordering
	don't participate in synchronize-with relationships.

	Without any additional synchronization, the modification order
	of each variable is the only thing shared between threads
	that are using memory_order_relaxed.

	Relaxed operations on different variables can be freely
	reordered provided they obey happens-befor relationships
	they're bound by.


	Understanding Relaxed Ordering
	---------------------------------------------------

	Acquire-Release Ordering
	---------------------------------------------------
	This is step up from relaxed ordering; there's still
	no total order of operations, but it does introduce
	some synchronizations.

	Under this ordering model, atomic loads are acquire
	operations(memory_order_acquire), atomic stores
	are release operations (memory_order_release), and
	atomic read-modify-write operations (such as fetch_add()
	or exchange()) are either acquire, release or both
	(memory_order_acq_rel).

	Synchronization is pairwise between the threads that
	does the release and the thread that does the acquire.

	A release operation synchronizes with an acquire operation
	that reads the value written.



	

	







*/






























