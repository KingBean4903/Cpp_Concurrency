// Passing arguments to a thread function

void f(int i, std::string const& s);
std::thread t(f, 3, "Thread me");

// 2
void func(int param)
{
	char buffer[1024];
	sprintf(buffer, "%i", param);
	std::thread t(f, 3, buffer); //error: pointer is passed
	std::thread t(f, 3, std::string(buffer));
	t.detach();
}
// Scenario: passing references
void update(widget_id w, widget_data& data);
void oops_again(widget_id, w)
{
	widget_data data;
	std::thread t(update, w, data); // error: expects data to be passed by reference
	std::thread t(update, w, std::ref(data)); // Soln wrap data in  std::ref<F6>
	display_status();
	t.join();
	process_some_data(data);
}

// Parameter passing semantics std::bind
// You can pass a member function pointer as
// the function, provided you supplya suitable
// object pointer as the first argument.
class X
{
	public:
		void do_work();
};
X my_x;
std::thread t(&X::do_work, &my_work);
/*
	This code will invoke my_x.do
*/

// Scenario : move only arguments
// std::unique_ptr 
void process_big_object(std::unique_ptr<big_object>);
std::unique_ptr<big_object> p(new big_object);
p->prepare_data(42);
std::thread t(process_big_object, std::move(p));






























