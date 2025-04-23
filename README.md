## C++ Concurrency 
Welcome back gang to a multipart tutorial on
C++ concurrency. No matter which language I work
one, I find concurrency to be a topic that I
want to dig into the belly of the beast.

Gaining exposure to memory layouts, deadlocks,
data races and everything in between. 

This will be a deep dive that will help expose
and understand the intricacies and quirks of
working with threads and other concurrency tools
that the C++ libraries provide.

## Motivation
As a programmer it's an interesting topic that
introduces you to the underbelly of how processors
work and run your code.

Another reason is that we would like to take
advantage of the processing power of the machine and
help scale our programs.

In the recent past, with processors getting cheaper
and more powerful, we find ourselves in an era where
we can explore the endless possibilities of concurrency.

With the advent of AI and cloud servers that can 
handle thousands of transactions/second, concurrency has
been at the core of this increase in scalability.

## What is concurrency?
In basic english, concurrency is the ability to handle
multiple tasks at the same time.

In computing, it is a single system performing multiple
independent activities in parallel rather than sequentially.

## Approaches to Concurrency
We will introduce the 2 common approaches to
concurrency.
1. Concurrency with multiple processes.
2. Concurrency with multiple threads.

In this article we will focus on the second
form which is not resource expensive as the first one.
I hope to cover that in another blog post, stay tuned.

## Concurrency vs Parallelism
These 2 terms have overlapping meanings in 
multithreaded code.
Parallelism is about taking advantage
of available hardware resources to increase performance.
Concurrency is about responsiveness or separation 
of concerns.

# When not to use concurrency
Every new feature or unexplored area is an attractive
shiny object to any developer/programmer. Everything
that glitters is not gold.
Points to consider when choosing to apply concurrency
to your code
1. It's hard to maintain and write mulithreaded
   code and the additional complexity can lead to more bugs.
2. Unless the performance gain is significant to justify
   the additional development time required.
3. Threads are a limited resource, too many threads hogging
   on your system could eatup your address space especially
   on 32bit systems.
4. Context switching is expensive on your operating system.

# C++ Concurrency and Multithreading
In this article, we will be exploring the quirks
of multithreading in C++ with the analogy of an Autobot (Optimus Prime) - sorry
deceptorcon fans, with the each subsystem running on it's on thread.
Join me on this learning journey as we put together and
breakdown this Prime's subsystems using concurrency techniques provided
in  C++;
Autobots rollout!!

### Hello, World

![Threading!](/assets/thread_prime_1.png "Hello thread")

We really don't need a thread to do this, the performance
benefits are negligible but this is Optimus he has a 
huge address space and creating threads is inexpensive for
his AllSpark, so why not.

#include <thread>, the declaration for the multithreading
support in the Standard C++ Library.
Every thread has to have an initial function,where the
new thread begins. For the initial thread in an application,
this is main(), but for every other thread it's specified
in the constructor of a std::thread object-- in this case
, the std::thread object named callOut has the new rolloutCall()
function as its initial function.

The join() causes the calling thread in main() to wait
for the thread associated with std::thread object, callout, to
complete and clean up any storage associated with the
now finished thread.

Without calling join() on the callOut thread main() would end
the program before the new thread has a chance to run.

If you don't need to wait on a thread replace join() with
a call to **detach()**;

### Waiting for a thread to finish
Your application could be terminated if an exception
is thrown after the thread has been startd but before
the call to join().

To make sure our simple, inexepensive AllSpark supported
threads manage their threads, we will apply C++ standard
Resource Acquisition Is Initializatin (RAII) idiom and
provide a class that does the join() in its destructor

![Threading!](/assets/fire_and_track.png "Allspark")

When the execution of the current thread reaches the 
end of fire_and_track, the local objects are destroyed
in reverse order of construction.

RAII acts as guide when it comes to object lifetimes.
The thread is the resource.
We delete the copy constructor and copy-assignment
to ensure that no copy exists that might outlive
the scope of the thread it was joining.

## Running threads in the background
Optimus Prime needs to scan for deceptorcons
in the background, how do we do this in the
background using C++ facilities ?

Calling **detach()**  on a std::thread object
leaves the thread to run in the background.

Detached threads are often called *daemon 
processes* without any explicit user interface.

Such threads are long-running; they run for
almost the entire lifetime of the application.

A detached thread is not a joinable thread.
![Megatron!](/assets/dcptcon_scan_detach.png "Scan")

Later on we will look at mechanisms for communicating
with detached threads and also returning values
from them.

## Problems with sharing data between threads

### Conceptual Scenario
#### Context:
Optimus prime is mid-battle. One thread is auto-firing
missiles, while another is reloading ammo at the same
time.
Both threads are reading and modifying ammoCount without
synchronization, leading to a data race.

##### Thread A: Auto-firing
1. Checks if ammoCount > 0
1. Fires a missile
1. Decrements ammoCount

##### Thread B: Reloading
1. Adds missiles to payload
1. Increments ammoCount

This is an example of a race condition.

In Concurrency, a race condition is anything where the
outcome depends on the relative ordering of execution
of operations on two or more threads.

### Avoiding race conditions
There are various ways to protect your data
from a race condition.
1. Wrap your data structure with a protection
   mechanism.
2. Use lock-free programming mechanisms.
3. Handle updates to the data structure as 
   transaction.

In this section we will a mutex, the most basic 
protection mechanism provided by the C++ standard
to protect our shared data.

### Protecting shared data with mutexes

![!Data race](/assets/data_race.png "data")
In C++ you create a mutex by constructing an instance
of **std::mutex**, lock it with std::lock_guard class template
which implements RAII idiom for a mutex; it locks the
supplied mutex on construction and unlocks it on
destruction.

__Don't pass pointers and references to protected data
outside the scope of the lock, whether returning from a
function, storing them in externally visible memory. or 
passing them as arguments to user-supplied functions__.

## Deadlock 
This is not a name of a Transformer but it a problem
that occurs in multithreaded programs.
A deadlock occurs when you have to lock two or more
mutexes in order to perform an operation.

### Deadlock Scenario

1. Thread A(Fire Control System)
   - Locks **weaponsMutex** to prepare missiles.
   - Then tries to lock **sensorMutex** to verify
     target heat signature.
   - Waits for sensorMutex...

2. Thread B (Sensor Diagnostic System)
    - Locks **sensorMutex** to recalibrate sensors.
    - Then tries to lock weaponsMutex to verify
      missle status.
    - Waits for **weaponsMutex**

3. Deadlock
    - Thread A is waiting for **sensorMutex**, held by
      Thread B.
    - Thread B is waiting for **weaponsMutex**, held by
      Thread A.
    - Neither can proceed - deadlock occurs.

![!Data race](/assets/deadlock.png "data")

C++17 provides the std::scoped_lock in the form
of a RAII template, it accepts a list of mutex types
as template parameters and a list of mutexes as 
constructor arguments.

## Synchronizing concurrent operations
In multithreaded applications protecting your data
is one of the fundamentals, in this sections we
will dive into the different mechansims for synchronizing
threads.

One thread might need to wait for another thread to complete
a task before the first thread can complete its own.

It is also common to want a thread to wait for a specific
event to happen or a condition to be true.

The C++ STL provides facilities to support/provide
synchronization in the form of
    1. Conditional Variables
    2. Futures

### Waiting for a condition with conditional variables.

#### Scenario: Decepticon Sightings Queue

    - Sensors detect Decepticons and push enemy data 
      (like location or type) into a shared queue.
    - A ThreatAnalysisSystems consumes teh queue to
      analyze and prioritze threats.
    - We will use this challenge to simulate a 
      producer-consumer synchronization using a queue.
        - Multiple sensor threads produce sightings.
        - A single consumer thread pulls and processes
          them.
        - If improperly sychronized, it leads to data races,
          or lost data     

![Threading!](/assets/cv_synch.png "Hello thread")

In our code above we use a Condition Variable to synchronize
between the threatAnalysis thread and sensorInput.

## Conclusion
I really wanted to squeeze in alot more into this blog post
but I will post a follow up on more mechanisms offered by
C++ STL.

In my next blog post I will focus on futures, the other
synchronization mechanism. 

***If you didn't grasp much from this blog post, just
remember decepticons are the evil ones and you should always
be rooting for the Autobots***

## Waiting for one-off events with futures
In the previous section we looked at using a condition
variable as a synchronization mechanism, but what
if we needed to wait on a one-off event.

Let's explore a scenario to help us understand
this.

### Scenario: Map Loading in the Background
#### Problem
The map takes time to load, but the navigation
can't start without it.
We offload map loading to a background task, 
and NavigationSystems waits using a std::future.

![Future !](/assets/future_event.png "Future")

In our code sample, startNavigation() will 

If a thread needs to wait for a specific one-off
event, it somehow obtains a future representing 
that event.

Using std::future<>, the thread can do another
task until it needs the event to have happened
before it can proceed and just wait for the future
to become ready.

## std::packaged_task<>

`std::async<>` isn’t the only way to hook a task up to 
a `std::future<>`. 
You can also use `std::packaged_task<>`, 
which basically wraps any function or callable and ties it to a future.

Once you call the `packaged_task`, it runs the function and fills
in the future with the result—ready for you to grab when you need it.

For example std::packaged_task<double(double)>, takes
a callable that returns a double and takes a double 
as a parameter.

This can be used as a building block for thread pools
or other task management schemes, such as running each
task on its own thread.

This abstracts out the details of the  tasks; the scheduler
just deals with std::packaged_task<> instances rather than
individual functions.

## Scenario
Imagine Optimus Prime has a mission control system that queues
up calculations (e.g, target trajectories, power diagnostics)
that need to be handled by a worker thread. We wrap these in
std::packaged_task<> and store them in a shared std::deque.

### How it works
1. Main thread creates tasks and pushes them to
   the deque.
2. Worker thread(s) pull them from the deque and
  run the packaged_task.
3. Futures are used to get results back once tasks
   complete.
4. Synchronization is handled with std::mutex + std::condition_variable.

![Future !](/assets/synching_pkg_2.png "Future")

In the workerThread() function we create an 
task using std::packaged_task<int>;

Then we obtain a lock on the mutex(queueMutex) using
std::unique_lock.
Using the condition variable cv we wait passing in
the lock and a predicate as a lambda function.

If the predicate returns true we contine with function
execution else we release the lock and the thread
waits to be notified.

In the main() function we create tasks in the for loop
and push them to our queue and we notify the waiting
thread once we push a task to our queue.

## C++17 Atomic operations and types in C++
An atomic operation is an indivisible opertion.
You can't observe such an operation half-done
from any thread in the system; it's either done
or not done.

The key use case for atomic operations is as
a replacement for an operation that would otherwise
use a mutex for synchronization.

If one operation that reads the value of an object
is atomic, and all modifications to that object are
also atomic, that load will retrieve the initial value
of the object or the value stored by one of the
modifications.

## The standard atomic types
They are supplied in the <atomic> header.

Here is a table listing atomic types available
in the C++17 Standard.
![Future !](/assets/atomic_types.jpg "Future")
The standard atomic types are not copyable or 
assignable in the conventional sense, they have
no copy constructors or copy assignment operators.

Here is a list of operations on atomic types
1. test_and_set
2. clear
3. load
4. store
5. exchange
6. compare_exchange_weak
7. compare_exchange_strong
8. fetch_add, +=
9. fetch_sub, -=
10. fetch_or, |=
11. fetch_and, &=
12. fetch_xor, ^=
13. ++, --

Each of the operations on atomic types has an
optional memory-ordering argument which is one
of the values of the st::memory_order enumeration.

Here is a list of the available memory-ordering
options:
1. memory_order_relaxed
2. memory_order_release
3. memory_order_seq_cst
4. memory_order_consume
5. memory_order_acquire
6. memory_order_acq_rel

## Weapons Cooldown Timer
### Scenario
Optimus Prime has a high powered plasma cannon that needs
to cool down after firing. The targeting system
must know whether the cannon is ready again.

### System
1. std::atomic<int> cooldownCounter.
1. Updated by the WeaponsFiringSystem.
1. Read by the TargetSystem.

![Atomic example !](/assets/atomic_ops.png "Weapons sytems")

#### Breakdown
1. std::atomic<int> protects the shared state (countdownCounter) 
   without mutexes.
1. Cooldown runs in the background thread ticking down every
   second.
1. fireWeapons() reads from the atomic and acts accordingly.

## Conclusion
In C++ atomics are used in designing lock-free data-structures.

I hope you found this educational as well as entertaining, the
series still continues with more concepts to illustrate and
breakdown.

In our next article we will explore what is happening in 
memory,we will look into caches, memory barriers, cache
lines and more.

Thank you!!

