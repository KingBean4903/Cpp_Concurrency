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
