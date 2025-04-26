template<typename Func, typename... Args>
auto submit(Func&& f, Args... args) -> std::future<ReturnType>

auto bound_op = [](int x, int y) { return x + y };
auto future = tPool.submit(bound_op, 8, 20);

class thread_pool
{

    std::atomic_bool done;
    threadsafe_queue<std::function<void()>> task_queue;
    std::vector<std::thread> threads;

    join_threads joiner;

    void worker_thread() {}

    public:
        thread_pool(): done(false),  joiner(threads) {
            
            unsigned const thread_count = std::thread::hardware_concurrency()

        }
        ~thread_pool() {}
        
        template<class F, class... Args>
        auto submit (F&& f, Args&&... args)-> std::future<ReturnType>
};


thread_pool tPool();

auto future1 = tPool.submit([]{ 5 * 2});
auto future2 = tPool.submit([](int x) { return x * x}, 5);

std::cout << "Result1: " << future1.get() << "\n"; // prints 10
std::cout << "Result2: " << future2.get() << "\n"; // prints 10