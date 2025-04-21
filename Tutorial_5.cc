// Protecting a list with a mutex
#include <list>
#include <mutex>
#include <algorithm>
std::list<int> some_list;
std::mutex some_mutex;

void push_to_list(int val)
{
	std::lock_guard<std::mutex> guard(some_mutex);
	some_list.push_back(val);
}

bool search_list(int val)
{
	std::lock_guard<std::mutex> guard(some_mutex);
	return std::find(some_list.begin(), some_list.end(), val)
	!= some_list.end();
}






























