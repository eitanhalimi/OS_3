# 🌐 Convex Hull Network Service

## 📋 Authors
- **Student Name**: Ido Cohen  
- **Student Name**: Eitan Halimi

---

## 📋 Project Overview

This project implements a sophisticated service for calculating the area of the **Convex Hull** of a given set of 2D points. The project evolves progressively through multiple stages, starting from a basic command-line tool and developing into a sophisticated, concurrent network server.
### 🎯 Core Features

- **🔺 Convex Hull Calculation**: Implementing Andrew's monotone chain algorithm to find the convex hull for a set of points
- **📐 Polygon Area Calculation**: Calculating the area of the resulting convex hull polygon  
- **🌐 Network Service**: Exposing functionality through a TCP server with different concurrency models
- **⚡ Asynchronous I/O**: Reactor pattern implementation for non-blocking operations
- **🧵 Multi-threading**: Proactor pattern and thread-per-client models
- **🔒 Synchronization**: Thread-safe access to shared data using modern C++ synchronization primitives

> 💻 **Technology Stack**: C++ with standard libraries for data structures, networking, and concurrency

---

## 🏗️ Project Structure
The project is organized into **10 sequential stages**, with each stage residing in a separate directory named `Q_*`. Each stage builds upon the functionality of the previous one, introducing new features or architectural patterns.

| Stage | Directory | Description |
|-------|-----------|-------------|
|  **Stage 1** | `Q_1` | Basic command-line Convex Hull area calculator |
|  **Stage 2** | `Q_2` | Profiling of different data structures for the algorithm |
|  **Stage 3** | `Q_3` | Interactive command-line interface for graph manipulation |
|  **Stage 4** | `Q_4` | Simple, single-threaded, iterative TCP server |
|  **Stage 5** | `Q_5` | Reusable Reactor pattern implementation |
|  **Stage 6** | `Q_6` | Server from Q4, rebuilt using the Reactor pattern |
|  **Stage 7** | `Q_7` | Multi-threaded server using thread-per-client model |
|  **Stage 8** | `Q_8` | Reusable Proactor pattern implementation |
|  **Stage 9** | `Q_9` | Server from Q7, rebuilt using the Proactor pattern |
|  **Stage 10** | `Q_10` | Proactor server with Producer-Consumer mechanism |

---

## 🛠️ Build and Run Instructions

It is assumed that a Makefile exists in the root directory to build each stage.

### General Build Command
To build a specific stage, you can use a command like:
```bash
make q<stage_number>
# For example, to build stage 3:
make q3
```

### Running Each Stage

**Stage 1 & 2 (CLI with file input):**
```bash
./Q_1/main < input.txt
./Q_2/main < input.txt
```

**Stage 3 (Interactive CLI):**
```bash
./Q_3/main
```

**Stages 4, 6, 7, 9, 10 (Server):**

Start the server:
```bash
./Q_<stage_number>/server
```

Connect with a client like telnet or netcat:
```bash
telnet localhost 9034
```
---

## 📚 Stages Breakdown

### Stage 1: Basic Convex Hull Calculator (Q_1)
**Functionality:** A command-line program that reads a set of 2D points from standard input, calculates their convex hull, and prints the area of the resulting polygon to standard output.

**Implementation:**
- `Point.hpp`: Defines a Point struct with x, y coordinates and comparison operators.
- `ConvexHull.cpp`: Implements the convex_hull function using Andrew's monotone chain algorithm. It takes a `std::vector<Point>` as input.
- `Polygon.cpp`: Implements polygon_area to calculate the area of a polygon using the Shoelace formula.
- `main.cpp`: Orchestrates the program flow: reads the number of points, parses each point from stdin, calls the convex_hull and polygon_area functions, and prints the result.

**Assumptions:** Input is well-formed. The first line is an integer n, followed by n lines each containing a point in x,y format.

### Stage 2: Data Structure Profiling (Q_2)
**Functionality:** This stage profiles the performance of the convex_hull algorithm when using different underlying containers for the input points.

**What's New:**
- **Templated Functions:** convex_hull and polygon_area are converted into template functions that can accept any standard container (e.g., `std::vector`, `std::list`, `std::deque`).
- **Profiling:** The main function now reads points into both a `std::list` and a `std::deque`. It uses `std::chrono` to measure and print the execution time for calculating the convex hull on each container type.
- **Point Comparison:** The `Point::operator==` is updated to use an epsilon for safer floating-point comparisons.

**Conclusion:** This stage is designed to demonstrate that the choice of data structure impacts performance, especially since the algorithm requires sorting, which is more efficient on random-access containers.

### Stage 3: Interactive Command-Line Interface (Q_3)
**Functionality:** Transforms the program into an interactive CLI that maintains a persistent set of points (a "graph").

**What's New:**
- **Command Loop:** The main function now contains a loop that reads and processes commands from stdin.
- **Graph State:** A global `std::list<Point>` is used to store the current set of points.
- **Commands Implemented:**
  - `newgraph n`: Clears the current graph and reads n new points.
  - `newpoint x,y`: Adds a new point to the graph.
  - `removepoint x,y`: Removes an existing point from the graph.
  - `ch`: Calculates and prints the convex hull area of the current graph.
  - `quit`: Exits the program.
- **Helper Functions:** A `graph.hpp/.cpp` module is introduced to encapsulate graph manipulation logic (newGraph, newPoint, removePoint, calcCH, and stringToPoint parser).

### Stage 4: Simple TCP Server (Q_4)
**Functionality:** Migrates the interactive CLI from Stage 3 to a network service.

**What's New:**
- **Networking:** Implements a basic TCP server using the standard socket API (socket, bind, listen, accept).
- **Architecture:** This is an iterative, single-threaded server. It handles one client at a time. The main loop calls accept(), and once a client connects, it enters a recv() loop to process commands from that single client. When the client disconnects, the server closes the connection and goes back to accept() to wait for a new client.
- **Shared State:** The graph (the `std::list<Point>`) is a global variable. Since the server is iterative, there are no concurrent modifications, so no synchronization is needed.

**Assumption:** Client interactions are sequential. A new client can only connect after the previous one has disconnected.

### Stage 5: Reactor Pattern Library (Q_5)
**Functionality:** This stage provides a reusable Reactor pattern implementation as a C++ class. It is a library component and not a runnable program.

**What's New:**
- **Reactor Class:** A Reactor class that uses select() for asynchronous I/O multiplexing.
- **Concurrency:** The reactor runs its event loop in a separate `std::thread`, making it non-blocking. It uses a `std::recursive_mutex` to protect its internal map of file descriptors and handlers, making its public methods thread-safe.

**API:**
```cpp
using ReactorFunc = std::function<void(int)>;
// The type of handler function to be called when an fd is ready.

void startReactor();
// Starts the reactor's event loop in a background thread.

void stopReactor();
// Stops the event loop and joins the background thread.

int addFd(int fd, ReactorFunc func);
// Registers a file descriptor fd with a corresponding handler function func.

int removeFd(int fd);
// Removes a file descriptor from the reactor.
```

### Stage 6: Reactor-Based Server (Q_6)
**Functionality:** Re-implements the server from Stage 4 using the Reactor library from Stage 5.

**What's New:**
- **Architecture:** The server is now an asynchronous, single-threaded, concurrent server.
- **Event-Driven:** The main function initializes the server socket and starts the global Reactor. The main loop only calls accept().
- When a new client is accepted, its file descriptor (client_fd) is added to the reactor with a handler function (handle_client_input).
- All client communication is now handled by handle_client_input, which is invoked by the reactor's thread whenever data is available to be read from a client socket.

### Stage 7: Multi-Threaded Server (Thread-per-Client) (Q_7)
**Functionality:** Implements a concurrent server using a thread-per-client model.

**What's New:**
- **Architecture:** Abandons asynchronous I/O in favor of multi-threading. The main thread's only job is to run an accept() loop.
- **Thread Creation:** For each incoming connection, a new `std::thread` is created to handle that client exclusively. The thread is detach()ed to run independently.
- **Synchronization:** Since multiple threads can now modify the global points list concurrently, a `std::mutex` (graph_mutex) is introduced. All operations that access or modify the graph (newGraph, newPoint, removePoint, calcCH) are protected by a `std::lock_guard`, ensuring thread safety.

### Stage 8: Proactor Pattern Library (Q_8)
**Functionality:** This stage provides a reusable Proactor pattern implementation. Like the Reactor, it is a library component.

**What's New:**
- **Proactor Class:** A Proactor class that simplifies the thread-per-client model. It encapsulates the accept() loop and thread creation logic.
- **Asynchronous Operations, Synchronous Handlers:** The Proactor asynchronously waits for new connections. When a connection is established, it synchronously invokes a user-provided handler in a new thread.

**API:**
```cpp
using proactorFunc = std::function<void(int)>;
// The type of handler function that will run in a new thread for each client.

Proactor(int listening_fd, proactorFunc threadFunc);
// Constructor that takes the listening socket and the client handler function.

void startProactor();
// Starts the proactor's accept loop in a background thread.

void stopProactor();
// Stops the accept loop.
```

### Stage 9: Proactor-Based Server (Q_9)
**Functionality:** Re-implements the multi-threaded server from Stage 7 using the Proactor library from Stage 8.

**What's New:**
- **Simplified Main Logic:** The main function is now much cleaner. It sets up the listening socket, creates a Proactor instance (passing it the socket and the client_thread handler from Stage 7), and calls startProactor().
- **Architecture:** The architecture is identical to Stage 7 (thread-per-client with a mutex for the shared graph), but the Proactor class abstracts away the accept loop and thread management boilerplate.

### Stage 10: Producer-Consumer Extension (Q_10)
**Functionality:** Extends the Proactor-based server from Stage 9 with a producer-consumer mechanism to monitor the Convex Hull's area.

**What's New:**
- **Watcher Thread (Consumer):** A new watcher_thread is launched at startup. This thread acts as a consumer, waiting on a `std::condition_variable`.
- **Client Threads (Producers):** The client handler threads now act as producers. After a client issues a ch command, the thread calculates the area.
- **Notification Logic:** If the calculated area crosses a threshold of 100 (either from below 100 to >=100, or vice versa), the client thread notifies the waiting watcher_thread using `cond.notify_one()`.
- **Server-Side Output:** The watcher_thread, upon being woken up, prints a status message to the server's standard output, such as "At Least 100 units belongs to CH" or "At Least 100 units no longer belongs to CH".
- **Synchronization:** A `std::condition_variable` and an associated `std::mutex` are added to coordinate between the producer (client) threads and the consumer (watcher) thread.