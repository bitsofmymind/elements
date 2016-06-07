Elements - A web programming framework for the embedded world
========

Elements was born out of a desire to bring the working principles of the web and HTTP to the embedded world:
* simple and clear protocols;
* human readability;
* portability;
* scalability;
* durability.

In Elements, architecture defines function. Web applications are assembled together using resources,
the basic building block of the framework. Resources are in essence very small webservers that once connected to one
another can pass messages around to build complex applications.

Elements requires no driver, no apps, no programs to install, no packages, it just speaks HTTP and can be explored using any browser (Chrome, Firefox, etc.) . That website that hasn't been updated since 1996 still works on your new IPhone because it's been built on durable and time proven standards. Will that bluetooth 4.1 gizmo that requires an app still function in 10 years? It probably won't, partly because Apple likes programmed obsolescence but mostly because maintaining apps is extremely demanding.

EXAMPLES
------------
### C++
```c++
Authority *root = new Authority(); // The root of the server.
SocketInterface *net = new SocketInterface(80); // A socket listener on port 80.
HelloWorld *hw = new HelloWorld(); // A resource that returns "Hello World!".
Processing *proc = new Processing(); // An abstraction of a processor.
Resource *res = new Resource(); // A vanilla resource that does nothing.

root->add_child("net", net);
root->add_child("resource", res);
res->add_child("hello_world", hw);
root->add_child("processing", proc);

proc->start(); // Starts the framework.
```
### Python
```python
root = Authority(); // The root of the server.
net = SocketInterface(80); // A socket listener on port 80.
hw = HelloWorld(); // A resource that returns "Hello World!".
proc = Processing(); // An abstraction of a processor.
res = Resource(); // A vanilla resource that does nothing.

root.add_child("net", net);
root.add_child("resource", res);
res.add_child("hello_world", hw);
root.add_child("processing", proc);

proc.start(); // Starts the framework.
```

Now all you need to do is to point you favorite browser (Chrome, Firefox, etc.) to http://localhost:80/hello_world.

REQUIREMENTS
------------
Elements does not have any requirements in particular but the g++ compiler. It has been tested on POSIX systems (Linux and Mac OS) and
on the Atmel AVR 8-bit architecture.

STRUCTURE
----------- 
    bindings/           bindings to scripting languages (python, php etc.)
    core/               core classes of the framework
    demos/              demos
    pal/                platform abstraction functions
    platforms/          platform specific libraries
    utils/              framework data structures
    configuration.h     configuration file

DEMOS
-----------
* [The framework running an oscilloscope on an Atmel ATMega328p](http://bitsofmymind.com/2011/05/19/a-milestone/).
