# ocppws / ocpp C++ / ocpp cpp
OCPPws is a C++ implementation of the Open Charge Point Protocol.

It uses libwebsocket, a lighweight websocket C library for websocket management. **It is currently under development only supporting server mode** with Boot, Heartbeat, status, metervalue, start-stop transactions, authorize.

Any advice or comment is accepted with a smile :), I'm currently learning.

# Build

You need Make to install system dependencies (libwebsockets-dev (dynamic library) and nlohmannjson-dev (header-only)).

Cmake is used to build, run:
```
mkdir build
cmake ..
make
```
You will have a executable and the ocpp library in the build/src directory (it can be changed to be a static library)
