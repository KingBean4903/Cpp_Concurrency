#include <iostream>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>

std::deque<std::packaged_task<int()>> taskQueue;
std::mutex queueMutex;
std::condition_variable cv;
bool done = false;

// Simulated task
int heavyTask(int value) {
    // Simulate a delay in processing
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "[WORKER] processing" << value << "\n";
    return value * value;
}

void workerThread() {
    while(true)
    {
        std::packaged_task<int()> task;
        {
            std::unique_lock lock(queueMutex);
            cv.wait(lock,[]{ return !taskQueue.empty() || done ;});
            if (done && taskQueue.empty()) break;
            task = taskQueue.front();
            taskQueue.pop_front();
        }
        task();
    }
}

int main() {
    
    std::thread worker(workerThread);
    
    std::vector<std::future<int>> futures;

    for (int i = 1; i < 10; i++){   
        std::packaged_task<int()> task([i](){ return heavyTask(i); });
        futures.push_back(task.get_future());
        {
            std::lock_guard lock(queueMutex);
            taskQueue.push_back(std::move(task));
        }
        cv.notify_one();
    }
    
    for (auto& fut : future) {
        std::cout << "[MAIN] Result " << fut.get() << "\n";
    }
    {
        std::lock_guard lock(queueMutex);
        done = true;
    }

    cv.notify_one();

    workerThread.join();
    std::cout << "[MAIN] All tasks computed. \n";
    return 0

}