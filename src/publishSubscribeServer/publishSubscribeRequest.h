#pragma once

#include <string>

using namespace std;

/* Keys required for parsing the http request. */
static const string GET = "GET";
static const string POST = "POST";
static const string DELETE = "DELETE";
static const string CONTENT_LENGTH = "Content-Length";

enum class PublishSubscribeServerOp {
   SUBSCRIBE,
   UNSUBSCRIBE,
   PUBLISH,
   GET_NEXT_MSG,
   ERROR,
   CONTINUE
};

class PublishSubscribeRequest {
   public:
      /*
       * @brief Translate from a httpRequest to a publish subscibe request.
       *
       * @params httpRequest The HTTP request to be parsed and translated.
       * @return None.
       */
      void Translate(string &httpRequest);

      PublishSubscribeServerOp opCode;
      string username;
      string topic;
      string msg;
};
