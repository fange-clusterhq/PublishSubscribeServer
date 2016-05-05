#pragma once

#include <string>
#include <vector>
#include <map>

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

const static int OK = 200;
const static int NO_CONTENT = 204;
const static int NOT_FOUND = 404;
static map<int, string> responseTable =
   {{OK, "HTTP/1.0 200 OK"},
    {NO_CONTENT, "HTTP/1.0 204 No Content"},
    {NOT_FOUND, "HTTP/1.0 404 Not Found"}};

class PublishSubscribeResponse {
   public:
      static string FormResponse(int statusCode, string &body);
};
