/*
 * @file publishSubscribeServer/publishSubscribeRequest.h
 * @brief Contain the definitions related to Publish Subscribe requests.
 *
 * The functions contained in this file are pertinate to parsing out the
 * request based on the Publish Subscribe APIs. The validity of the http
 * request is checked by functions defined in the external directory. For
 * these commands, we parse the uri out for username and topic, and if present
 * parse out the content based on the Content-Length field in the header.
 *
 * @author Yihua Eric Fang (yihuaf)
 */

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

/* Operations used to handle different types of request. */
enum class PublishSubscribeServerOp {
   FIRST,
   SUBSCRIBE,
   UNSUBSCRIBE,
   PUBLISH,
   GET_NEXT_MSG,
   /*
    * Ill-formed http request. We simply return 400 Bad Request.
    */
   ERROR,
   /*
    * This indicates that the request on hand is a partial one, so we need
    * to received more bytes to determine.
    */
   CONTINUE,
   SENTINEL
};

class PublishSubscribeRequest {
   public:
      PublishSubscribeRequest();
      /*
       * @brief Translate from a httpRequest to a publish subscibe request.
       *
       * This function will try to translate and return the number of bytes
       * that currently being translated.
       *
       * @params httpRequest The HTTP request to be parsed and translated.
       * @return number of bytes consumed.
       */
      size_t Translate(string &httpRequest);

      PublishSubscribeServerOp opCode;
      string username;
      string topic;
      string msg;
   private:
      /*
       * @brief Check and parse the uri for different request.
       *
       * For Subscribe, Unsubscribe, and GetNextMessage, we parse the topic and
       * username out. For Publish, we will need to parse out the content
       * and the topic.
       *
       * @params uri Thr uri received.
       * @return Was parsing successfull?
       */
      bool CheckAndParseSubscribe(string &uri);
      bool CheckAndParseUnsubscribe(string &uri);
      bool CheckAndParseGetNext(string &uri);
      bool CheckAndParsePublish(string &uri);

      /*
       * @brief Parse out the username and topic from the uri.
       *
       * The function is a helper method for the above Check and Parse methods.
       *
       * @params uri The uri to be parsed.
       * @return Was the parsing successful.
       */
      bool ParseTopicUsername(string &uri);

      /*
       * @brief Tokenize the string uri using '/' delimiter.
       *
       * The Request, in the form of uri in the http header, are delimited by '/'.
       *
       * @params uri The uri to be tokenized.
       * @return A vector of substring seperate by delimiter '/'.
       */
      vector<string> ParseUriTokenize(string &uri);
};

/*
 * Some constant used in forming an http response.
 */
const static int OK = 200;
const static int NO_CONTENT = 204;
const static int BAD_REQUEST = 400;
const static int NOT_FOUND = 404;

/*
 * Since these are the only response possible for this server, we choose to
 * hardcode them in advance.
 */
static map<int, string> responseTable =
   {
      {OK, "HTTP/1.1 200 OK"},
      {NO_CONTENT, "HTTP/1.1 204 No Content"},
      {BAD_REQUEST, "HTTP/1.1 400 Bad Request"},
      {NOT_FOUND, "HTTP/1.1 404 Not Found"}
   };

class PublishSubscribeResponse {
   public:
      /*
       * @brief Form a http response based on the status code and body.
       *
       * @params statusCode The status of the http response.
       * @params body If not empty, attached the body to the http response.
       * @return A string form of http response.
       */
      static string FormResponse(int statusCode, string &body);
};
