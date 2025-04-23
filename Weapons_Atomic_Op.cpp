#include <iostream>
#include <chrono>
#include <atomic>

// atomic variable
std::atomic<int> cooldownCounter{0};

void fireWeapons() {
    if (cooldownCounter.load() == 0)
    {
        std::cout << "[Weapons] Plasma cannon fired. \n";
        cooldownCounter.store(5);
    } else {
        std::cout << "[Weapons] Still cooling down...";
    }
}

void cooldownHandler() {

    while (true)
    {
        int counter = cooldownCounter.load();
        if (counter > 0)
        {
            cooldownCounter.fetch_sub(1);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void operatorInterface()
{
    for (int i = 0; i < 10; ++i)
    {
        fireWeapons();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {

    std::cout << " == Optimus Prime Weapons Systems =="

    std::thread cooldownThread(cooldownHandler);
    std::thread operatorThread(operatorInterface);

    operatorThread.join();

    // Runs in the background
    cooldownThread.detach();

    std::cout <<" == Decepticons Eliminated == ";
    return 0;
}
