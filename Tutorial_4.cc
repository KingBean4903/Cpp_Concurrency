// Transferring Ownership of a thread
/*
	Resource owning types in C++ STL
	such as std::ifstream and std::unique_ptr
	are moveable but not copyable and
	std::thread is one of them.

*/
void func_1();
void func_2();
std::thread t1(func_1);
std::thread t2 = std::move(t1);
t1 = std::thread(func_2); // temporary object 
std::thread t3;
t3 = std::move(t2); // default constructed
t1 = std::move(t3);

// Returning a std::thread from a function
std::thread f()
{
	void some_func();
	return std::thread(some_func);
}

// std::thread moveable helps us move
// the ownership to an object and take full
// advantage of RAII
class scoped_thread {
	std::thread t;

	pubilc:
		explicit scoped_thread(std::thread _t) : t(std::move(_t)) {

			if (!t.isjoinable())
					throw std::logic_error("Not joinable");
		}
		scoped_thread(scoped_thread const&) = delete;
		scoped_thread& operator=(scoped_thread const&) = delete

		~scoped_thread()
		{	
			t.join();
		}
};

struct func;

void f()
{
	int local_state;
	scoped_thread g {std::thread(func(local_state))};
	some_processing()

}

































