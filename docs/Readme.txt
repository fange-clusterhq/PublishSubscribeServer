Q: What steps did I take to ensure this is production quality?
A:

To me, a project of production quality means:
* Correctness: The product should behave correctly based on the
  requirements and usercases.

* Maintable: The project is designed and implemented in such way that is easy
  to maintain. Readbility of the implementation, documentations and style are
  all falling into this area as well as the quality of the architacture and
  design.

* Well tested: Ideally, we want bug free code, but that is near impossible. So
  we strive for the next best thing which is well tested. The product should
  be well tested with positive, negative and stress tests to ensure the quality
  of the product.

* Plans and Execution: The execution of the project should be well planned
  into different milestones that are managable. My philosophy is always been
  start with something small and incrimentally change small bits at a time and
  always be testing along the way. In this way, it is not only easy to test the
  product but also easier to make changes along the way.

The first step I took was to make sure I fully understand the requirements
from the write up, namely the relationship between the topic, username, and
published content as well as how each of the four operations should behave.

The next step involves architecting the project and it begins with identifying
the potential components and items/objects in the product. The purpose is for
me to have a high level picture of what the product looks like and how
different components should interact with each other. There needs to be a
server component to maintain the network connections as well as managing the
incoming and outgoing IOs. There needs to be a http component that parse and
construct http requests and responses.  There also needs to be a parser to
parse the subscribe, unsubscribe, publish and getNext based on the
requirements. Finally, there needs to be an component to maintain the state of
the published contents and subscription.

At this step, I also needs to make a decision on the choice of languages and
taking into accounts what each programming languages offers in terms of
libraries and functionalities. Ideally, this type of project should be done in
python or some higher level language who offers an abundance of libraries
related to network, http and other data structures. However, given the fact
that I have spent most of my time in the past years using C and the fact that
I have little experience writing production level python code in the past, I
choose C++ as it offers enough STL libraries and enough resemblance to C.

At this point, I also want to define a coding style to follow and a
documentation guidelines. This will ensure the readibility of the final
implementation. Since there is no right or wrong way for a coding style, I
choose to follow what we are using at VMware. For documentations, I choose to
use a Doxygen style comments.

The next step is to flush out the designs of each of the components identified
previously.

First, http parser should be an existing solution from open
source since c++ does not offer standard http related lib. Writing and testing
a good http parser fron scratch is a good exercise but not the right way to
go. I found one using only the standard c++ lib.

Then, there should be a base server class that deal with the network aspect of
the project. There are two choices: thread-based async model or event driven
model. I choose to the event driven model using select() with a single thread
event loop. Both model has it's advantages and disadvantages in terms of
performance, scalibility, easy of implementation and etc, which I will not go
into too much details here. The reason I choose the event driven model is
because it is single threaded which is far easier to implement.  Dealing with
concurrency, racing conditions and locking can be a huge effort, which should
be avoid unless the requirements calls for them. In a small project like this
one, ease of implementation can be a huge reason to choose one over another.
An additional benefits of organizing the network aspect into a base class is
that if we decided to choose another model, we can make the change easily.
Also, TCP is standard choice over UDP for a server like this.

Then, there should be a class that derive from the server base class and
perform the publish and subscribe logic. This is the PublishSubscribeServer
class. Ideally, I could have another intermindiate class called HTTP server
but given the size of the project, I think it will be OK to skip it. The
PublishSubscribeServer class will be responsible for maintaining the state
information. Since the state information need not persist across server
restart, we can maintain the state purely in memory with STL data structure.

In addition to the PublishSubscribeServer, we need a components to parse the
incoming http request to match the API as well as form a http response based
on the API.  These are just a set of string manupulation functions for the
PublishSubscribeServer to consume.

The last is to consider some of the edge cases and error conditions into the
design. The first is the fact that http messages may not be received from one
receive call. So potentially for our parsing function, we have to make sure we
can handle this case. In addition, the case like this, we need to store the
data received until the whole request is received. The other edge cases are
Bad Request. To handle this, we need to correctly parse the Bad Requests and I
decided to, in addition to the API, provide a 400 response to Bad Request to
signal this case. Another edge case is that if we sent multiple request at the
same time, the receive will receive all of them. The server needs to be able
to process these multiple requests correctly. There are other minor error/edge
cases where the server should be able to behave correctly such as broken PIPE,
SIGPIPE, and Connection Reset By Peer errors, though these are more of an
implementation detail.

Then, given the design thoughts above, I need to come up with a plan to
execute the design. I have broken the project into four mile stones and each
with a small set of unit testing plans. I will explain my testing plans as a
whole later.

The first milestone is to have the network code or the server base class
implemented. The basic functionalities should be something like an echo server
where the server should be able to receive and echo back the same data
received. We should also write a small set of tests to make sure it is
working, although these tests will not be valid tests for the final project.

The second mile stone is to have http parsing integrated into the server and
working. The http parser is an open source solution, so we should make sure it
can compile with the existing code. The deliverable for this milestone is that
a server should be able to receive a message, parse the message using the http
parser and see the results being parsed correctly. The testing efforts mainly
involves sending http requests to see if the server can parse it correctly.
Again, these tests are throw away for the final product.

The third mile stone will be to implement a parser that parse the publish
subscribe requests on top of the http parser. Again, based on the requirement,
each op should parse the correct things from the http requests and identify
the request as ill formed when possible. This time is a good time to write a
the actual tests that will be used to test the final product. Although we will
not get a return code yet, we can observe from the logging to see if the
server is parsing the request correctly.

The fourth and the final mile stone will be implement the logic to handle the
publish subscribe server's requests. At this stage, there isn't many design
choice but to take care of all the edge cases mentioned before. There are two
main maps used to keep track of the state of the server. One map to keep track
of the topic to username mapping where each topic have a list of the
subscribers. The second map is a mapping from topic, user to a queue of
messages. This is indicating that the user on a topic has a message pending to
be sent. Since a message is published to all the subscriber at the time, only
the pointers to the message are queued and there will be only one copy of the
actual message. This is an optimization to avoid needless duplication. The
message itself are ref counted with the number of subscribers. Each time, a
user called GetNext on the message and the message is sent out, the ref count
is decreased. When the ref count reaches 0, the messages can be safely freed.

After the implementation, the next step to consider is testing. First, we
would build a simple command line interface (CLI) so we can manually send requests
to the server. Along with the CLI, we build a core lib that can also be used
for testing scripts. For simplicity, we will use python since it
already has many http and network libs avaliable. For testing scripts, we
mostly need to cover positive tests, negative tests, and edge cases. The
postive test case are testing the basic behavior based on the requirements.
The negative test is most to test for cases where response 204 and 404 is
returned. The edge cases are testing for the edges cases mentioned above such
as ill formed request, particial requests, and illegal values. We should
observe 400 Bad Request returned. There are more tests that I wish I would
have done and I covered them in the later sections.


Q: What I choose to left out due to the limitation of time.
I will explain what I choose to left out due to time limitation in a
enumerated fashion in no particular order.

1. Logging
I used printf in the implementation instead of a proper logging framework.
While there are an abundance of logging framework available in C++, there is
none as standard lib. I didn't want to consume too much time in the
integration of an open source framework. In real production code, logging is
extremely important for both documentation and debugging.

2. Per function Unit tests
Besides the tests that are run through http requests, per function unit tests
are also important. It helps to reason about the
requirement/pre-condition/postcondition/invariant of each of the functions,
and can easily test for edge cases where it is harder to trigger from the
server APIs. This can also allow us to write tests along with implementation
and catch bugs early.

3. Cross platform building.
As I am coding with C++, I have to consider cross platform capatibility for
the product. Given the time, I did not take too much consideration when I
implemented the server. One example where I took this into consideration is
SIGPIPE.  I mentioned in the design section about how SIGPIPE was handled
differently on different platform. For example, on linux, MSG_NOSIGNAL is
passed in as flag to send() to avoid sending SIGPIPE, but it is not
implemented outside linux. Ignore the SIGPIPE is a quick and dirty way to get
rid of the problem, but may have unintended consequences in the future.

4. More testing.
There are mainly three catagory of tests I wanted to do. First, I want to
automate the feature tests. I have already written some feature testing under
test/ directory, but I have to manually run them. It would be best that they
can be automated. In addition, I would like to add more positive and negative
tests to simulate real use cases of the servers. Second, I would like to
implement some form of stress tests, launching a large number of connections
and requests and see how the server would hold up. Third, I would like a long
running tests where the server gets requests for longer than 24 hours or even
longer. These tests can tremendously increase the accountability of the
product.

5. Scalibility, performance, and investigate other designs.
The scalibility and performance is two fold. One is considerations during the
architecting and the design phase. The other aspect would be in testing for
both performance and scalibility and profiling to improve as a result.

As I explained before, many design choices I made was due to the simplicity
and I concerned less about the scalibility and performance. I choose the
signle threaded event driven loop for it's simplicity, but since all
operations are small in memory updating involving no I/O, the performance
should be acceptable even if we scale the server to a large number of
concurrent requests. However, one serious draw back is that it will not take
advantages of multi core CPU concurrency as a thread base model can offer.
what is missing from this picture is the performance requirement. For example,
how many users and topics we want to support? How many concurrent
connection/requests we want to support? What is the user behavior or workload
for a publish subscirbe type of services? With all these questions in mind, we
can potentially choose a more suitable architecture than a event driven loop.

On the testing front, ideally we want to measure the throughput in terms of
number of requests handled per second and latency in terms of seconds taken
for each request. We also want to profile the server to see where the bottle
nects are as well as profiling the memory usages to see if they are
acceptable.


6. Data persistant on disk, caching, logging for consistency, crash recovery.
I know these are not part of the requirement as server state need not persist
through reboot, but in real production, these are the things a server should
think about.

7. Debugging asserts and debug build
For now, I used the simple assert from <cassert>. Ideally, we should have a
debug build option where only in the debug build will all asserts executed.
Otherwise, these asserts will slow down the system during production.

