# Designing a Thread Pool in C++17

# Introduction
Concurrency is a necessity when building responsive
and scalable systems in modern software.

However, managing raw threads quickly becomes unsustainable
in non-trivial systems.

Thread pools are a mechanism to reuse a limited set 
of worker threads to execute many tasks efficiently.

In this article, we'll walk through the design and implementation
of a thread pool in modern C++17, and also hightlighting the
challenges that popup.

# Why not Spawn a Thread Per Task?
std::thread can manage our threads in simple contexts, but
in real-world workloads this will crumble.

Some of the common challenges in real-world systems include:
1. Resource Overhead: Threads are expensive to create and destroy.
2. Context Switching: Managing thousands of threads can overwhelm
					  the OS scheduler.

Thread pools help us address these problems.

# Core Concepts and Design Goals

Before looking at the code, let's go through
the prerequisites a thread pool needs to provide:

1. Task Submission: The pool should accept callable tasks(lambdas, functions, functors).
2. Worker Management: A set of threads to handle task execution.
3. Synchronization: Access to the shared task queue should be thread-safe without
					unnecessary locking overhead.
4. Graceful shutdown: The pool should finish pending tasks without leaks.
5. Exception Safety: If a task throws an exception, the pool should remain
                      functional and propagate errors appropriately.

# Language Tools from C++17
Thankfully,modern C++ gives us primitives to build poweful concurrent
systems:


# Building the Thread Pool
### Threads-Safe Queue
At the heart of the thread pool is the threadsafe-queue, from
where worker threads access shared tasks.

Here is the interface to our threadsafe-queue,
![Thread safe!](/assets/thread_safe_queue.png"San Juan Mountains")

### Submitting Tasks
A thread pool should provide a simple API for submitting tasks.
The caller, should not worry about how the tasks are managed 
internally - they should only care about.

1. Submitting any callable (function, lambda, etc).
1. Optionally getting the result of the task execution

A submit function might look this:

![Thread safe!](/assets/thread_pool_submit.png"queue")

Where:

1. Func is any callable
1. Args - variadic argument to pass zero or more args to the
		  callable
1. ReturnType: is automatically deduced based on the callable's
		  signature.
	
This design gives users maximum flexibility
![Thread safe!](/assets/thread_pool_submit.func.png"queue")

# Task Storage
Internally, we don't know the exact types of the task
submitted.

We just need to be able to store and call it later.
When a user submits a callable with arguments, the 
pool will:

1. Bind the function and its arguments together.
2. Wrap it into a std::packaged_task.
3. Store it as a std::function<void()> in the task queue.

# Class Skeleton
![Thread safe!](/assets/thread_pool_class.png"queue")

We use std::thread::hardware_concurrency() in our constructor to tell us
how many concurrent threads the hardware can support.
![Thread safe!](/assets/thread_pool_count.png"queue")

# Usage
Here is what using the thread pool would feel like
for a client:

![Thread safe!](/assets/thread_pool_sample.png"queue")


















