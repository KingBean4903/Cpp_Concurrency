// Using std::lock() and std::lock_guard in a swap operation
class big_object;
void swap(big_object& lhs, big_object& rhs);
class X
{
	private:
		big_object some_detail;
		std::mutex m;
	public:
		X(big_object const& bg): big_object(bg) {}

	
	friend void swap(X& rhs, X& lhs)
	{	
		if (&lhs == &rhs)
			return;
		std::lock(lhs.m, rhs.m); // locks the two mutexes
		std::lock_guard lock_a(lhs.m, std::adopt_lock);
		std::lock_guard lock_b(rhs.m, std::adopt_lock);
		swap(lhs.some_detail, rhs.some_detail);
	}
};



























