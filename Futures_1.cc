/* using std::future to get the
   return value of an asynchronous
   task.
*/
#include <future>
#include <iostream>
int find_the_answer();
void do_other_stuff();
int main()
{
	std::future<int> the_answer = std::async(find_the_answer);
	do_other_stuff();
	std::cout <<  the_answer.get() << std::endl;
	return 0;
}
