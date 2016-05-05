#pragma once

#include <string>
#include <vector>

using namespace std;

/* Keys required for parsing the http request. */
static const string GET = "GET";
static const string POST = "POST";
static const string DELETE = "DELETE";
static const string CONTENT_LENGTH = "Content-Length";

enum class PublishSubscribeServerOp {
   FIRST,
   SUBSCRIBE,
   UNSUBSCRIBE,
   PUBLISH,
   GET_NEXT_MSG,
   ERROR,
   CONTINUE,
   SENTINEL
};

class PublishSubscribeRequest {
   public:
      PublishSubscribeRequest();
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
   private:
      bool CheckAndParseSubscribe(string &uri);
      bool CheckAndParseUnsubscribe(string &uri);
      bool CheckAndParsePublish(struct request &parsedRequest,
                                string &httpRequest,
                                string::iterator &headerEnd);
      bool CheckAndParseGetNext(string &uri);
      bool ParseMsg();
      bool ParseTopicUsername(string &uri);
      vector<string> ParseUriTokenize(string &uri);
};
