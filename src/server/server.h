/*
 * @file server/server.h
 * @brief Definitions for the server base class.
 *
 * This contains the definitions for a server base class that can be
 * used for all other types of server implementation.
 *
 * @author Yihua Eric Fang (yihuaf)
 */

#pragma once

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <queue>

using namespace std;

/* This is just a random choice. 64 seems to be reasonable. */
static const int MAX_NUM_CONNECTION = 64;
/* Ideally, a HTTP header should not exceeds this length. */
static const int MAX_MSG_BUFFER_SIZE = 4096;

class Request {
   public:
      /* This request is received from thsi client. */
      int clientFd;
      char buffer[MAX_MSG_BUFFER_SIZE];
      size_t numBytes;

      /* @brief Constructor */
      Request();
};

class ReadRequest : public Request {
   public:
      ReadRequest();

      /* @brief Return a pointer to the buffer for receiving new data.
       *
       * The pointer returned is the position of where the current buffer
       * is. There might be data stored from the last receive and is only
       * a particial valid request.
       *
       * @return None.
       */
      inline char *GetBufferForRecv();

      /* @brief Return the size available in the buffer.
       *
       * As mentioned before, the buffer may have stored some data
       * already, so we return the size currently available.
       *
       * @return None.
       */
      inline size_t GetSizeForRecv();

      /* @brief Consumed X number of bytes.
       *
       * One receive may receive multiple requests into 1 buffer since we
       * do not know the boundary of the request until we process it. This
       * function will remove X number of bytes from the buffer. It is used
       * after processing one request successfully and remove the X bytes
       * as the request processed. The leftover can be reinterpreted as a
       * new request.
       *
       * @params bytes The number of bytes consumed.
       * @return None.
       */
      void Consume(size_t bytes);
};

class WriteRequest : public Request {
   public:
      WriteRequest();
};

class Server {
   public:
      /* @brief Constructor.
       *
       * @param port The port this server will listen to.
       * @return None.
       */
      Server(int port);
      /* @brief Destructor.
       *
       * @return None.
       */
      ~Server();

      /* @brief Initialize the server.
       *
       * We choose not to initialize everything in the Constructor because
       * we want to avoid throwing exceptions in the case of an error inside
       * a constructor.
       *
       * @return None.
       */
      void Init();
      /* @brief Start the server and serving forever.
       *
       * @return None.
       */
      void Start();
   private:
      /* Listening to this port. */
      int port;
      /* Fd sets for the select(). */
      fd_set readfds;
      fd_set writefds;
      /* Socket used for the server to accept new connections. */
      int masterSocket;
      /*
       * Store the client information for each of the client. The clientFd
       * should be present in the map if the connection is not closed. The
       * request should not be NULL us previously this client sent a partial
       * request.
       */
      map<int, ReadRequest *> clientContext;
      /*
       * Store all the pending out going messages.
       */
      map<int, queue<WriteRequest *>> msgOutgoingQueue;

      /* @brief Accept a new connection.
       *
       * To accept a new connection, we store the fd into the clientContext.
       *
       * @return None.
       */
      void AcceptConnection();

      /* @brief Handle an incoming request.
       *
       * When a new fd is ready for read, this method is called to received the
       * data and process it. If the request cannot be processed, it will store
       * tha partial request into the clientContent.
       *
       * @return None.
       */
      void HandleRequest(int clientFd);

      /* @brief Prepare the fd sets for the select().
       *
       * The select() function require the fd sets to be reinitialized each
       * time it is being called. Therefore, we need to put all the fds into
       * the set.
       *
       * @return None.
       */
      int PrepareSelect();

      /* @brief Handle the request after the data is received.
       *
       * The function should do the heavy lifting to process the data received
       * and determine if the current request is full or partial.  The derived
       * class of this class should override this method.  For this class, this
       * method will echo the received back to sender.
       *
       * @params request The request containing the data received.
       * @return Is this request done or only a partial.
       */
      virtual bool HandleRequestInt(ReadRequest *request);

      /* @brief Dequeue a msg from the message queue.
       *
       * @params clientFd The clientFd we want to dequeue a msg from.
       * @return The Msg/Request we want to write out.
       */
      WriteRequest *dequeueMsg(int clientFd);

      /* @brief Handle the destination fd become writable.
       *
       * This function is called after the destination fd becomes writable.
       * It will dequeue a message from the queue and sent out.
       */
      void HandleOutgoingMsg(int clientFd);

   protected:
      /* @brief Queue the msg to the queue.
       *
       * We first queue the messages to the queue. Then, the destnation fd will
       * be set for the select(). When the dest fd becomes available to write
       * the server will sent them out.
       *
       * @params clientFd The dest client fd.
       * @params request Contains msg to be sent out.
       * @return None.
       */
      void queueMsg(int clientFd, WriteRequest *request);
};
