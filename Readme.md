This is a simple Publish Subscribe Server.

Directory Layout:
src/:
        Contain the source code of the publish subscribe server.

src/external:
        Contain the external open source code needed for the project The main
component included is an http parser written in c++ with only STL.

src/server:
        Contain the implementation of a base server class.

src/publishSubscribeServer:
        Contain the implementation of the publish subscribe server based on
the requirements in the exercise.

test/:
        Contain the tests and a command line interface to the server.
docs/:
        Contain the documentations and required write up.

Instruction of Use:
1. git clone the repo.
2. In the project root directory, run 'make'
3. The binary will be created under bin/.
4. To run: ./bin/publishSubscribeServer -p <port>

Tests:
To run tests, simply do: python test.py <port>
To run CLI, simply do: python cli.py <port>
Typing help in cli should print out the usage.

Note: I have only tested the implementation on Ubuntu 14.04 and Mac El
Capitan. C++ 11 is used in the project and therefore the right g++ is
required.

