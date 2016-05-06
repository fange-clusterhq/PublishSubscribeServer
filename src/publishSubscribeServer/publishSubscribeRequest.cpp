/*
 * @file publishSubscribeServer/publishSubscribeRequest.cpp
 * @brief Contains the methods to parse and handle different requests.
 *
 * @author Yihua Eric Fang (yihuaf)
 */

#include "publishSubscribeRequest.h"
#include <cstring>
#include <external/requestParser.hpp>
#include <cassert>
#include <vector>
#include <sstream>

PublishSubscribeRequest::PublishSubscribeRequest()
   :opCode(PublishSubscribeServerOp::FIRST)
{
}

size_t
PublishSubscribeRequest::Translate(string &httpRequest)
{
   size_t bytesConsumed = 0;
   /* Parse the HTTP Header. */
   struct request parsedRequest;
   request_parser parser = request_parser();
   auto ret = parser.parse(parsedRequest, httpRequest.begin(),
                           httpRequest.end());
   if (get<0>(ret) == request_parser::bad) {
      this->opCode = PublishSubscribeServerOp::ERROR;
      printf("PARSING ERROR\n");
      return 0;
   } else if (get<0>(ret) == request_parser::indeterminate) {
      printf("INDETERMIN\n");
      this->opCode = PublishSubscribeServerOp::CONTINUE;
      return 0;
   }

   // HTTPRequestToString(parsedRequest);
   /* The iterator to the end of the header. */
   auto headerEnd = get<1>(ret);
   bytesConsumed += distance(httpRequest.begin(), headerEnd);
   /* Parsing out the content length first. */
   int contentLen = 0;
   for (auto &header : parsedRequest.headers) {
      if (header.name.compare(CONTENT_LENGTH) == 0) {
         contentLen = stoi(header.value, nullptr, 0);
      }
   }

   if (contentLen > 0) {
      /* Not enough bytes to match content length. */
      if (distance(headerEnd, httpRequest.end()) < contentLen) {
         this->opCode = PublishSubscribeServerOp::CONTINUE;
         return 0;
      }

      this->msg = string(headerEnd, httpRequest.end());
      bytesConsumed += contentLen;
   }

   if (parsedRequest.method.compare(GET) == 0) {
      this->CheckAndParseGetNext(parsedRequest.uri);
   } else if (parsedRequest.method.compare(DELETE) == 0) {
      this->CheckAndParseUnsubscribe(parsedRequest.uri);
   } else if (parsedRequest.method.compare(POST) == 0) {
      /* There is two possibilities, so we need to check both. */
      if (!this->CheckAndParsePublish(parsedRequest.uri)) {
         this->CheckAndParseSubscribe(parsedRequest.uri);
      }
   } else {
      /*
       * The request is a valid http request, but not a request we recognize.
       * We choose to drop this http request. Note, we do not drop any other
       * request if there is more than one request in the bytes received. Thus,
       * we return the bytes consumed.
       */
      this->opCode = PublishSubscribeServerOp::ERROR;
   }

   /* Before returning, the op code should have been set. */
   assert(this->opCode != PublishSubscribeServerOp::FIRST);
   assert(bytesConsumed > 0);
   return bytesConsumed;
}

bool
PublishSubscribeRequest::CheckAndParseSubscribe(string &uri)
{
   if (this->ParseTopicUsername(uri)) {
      this->opCode = PublishSubscribeServerOp::SUBSCRIBE;
      return true;
   } else {
      this->opCode = PublishSubscribeServerOp::ERROR;
      return false;
   }
}


bool
PublishSubscribeRequest::CheckAndParseUnsubscribe(string &uri)
{
   if (this->ParseTopicUsername(uri)) {
      this->opCode = PublishSubscribeServerOp::UNSUBSCRIBE;
      return true;
   } else {
      this->opCode = PublishSubscribeServerOp::ERROR;
      return false;
   }
}


bool
PublishSubscribeRequest::CheckAndParsePublish(string &uri)
{
   /* Publish must have a content with it. */
   if (this->msg.empty()) {
      this->opCode = PublishSubscribeServerOp::ERROR;
      return false;
   }

   /* Parsing topic out. */
   vector<string> tokenizedUri = this->ParseUriTokenize(uri);
   if (tokenizedUri.size() != 2) {
      this->opCode = PublishSubscribeServerOp::ERROR;
      return false;
   }

   /* In the form of /topic */
   this->topic = tokenizedUri[1];
   if (this->topic.empty()) {
      this->opCode = PublishSubscribeServerOp::ERROR;
      return false;
   }

   this->opCode = PublishSubscribeServerOp::PUBLISH;
   return true;
}


bool
PublishSubscribeRequest::CheckAndParseGetNext(string &uri)
{
   if (this->ParseTopicUsername(uri)) {
      this->opCode = PublishSubscribeServerOp::GET_NEXT_MSG;
      return true;
   } else {
      this->opCode = PublishSubscribeServerOp::ERROR;
      return false;
   }
}

bool
PublishSubscribeRequest::ParseTopicUsername(string &uri)
{
   vector<string> tokenizedUri = this->ParseUriTokenize(uri);
   /* In the form of /topic/username. So 3 items required from token. */
   if (tokenizedUri.size() != 3) {
      return false;
   }

   this->topic = tokenizedUri[1];
   this->username = tokenizedUri[2];
   if (this->topic.empty() || this->username.empty()) {
      return false;
   }

   return true;
}

vector<string>
PublishSubscribeRequest::ParseUriTokenize(string &uri)
{
   char delimiter = '/';
   stringstream ss(uri);
   string item;
   vector<string> result;
   while(getline(ss, item, delimiter)) {
      result.push_back(item);
   }

   return result;
}

string
PublishSubscribeResponse::FormResponse(int statusCode,
                                       string &body)
{
   ostringstream oss;
   /* Should always exist. */
   oss << responseTable[statusCode];
   oss << "\r\n";
   /* Attach content length in the header. */
   if (!body.empty()) {
      oss << CONTENT_LENGTH;
      oss << ": ";
      oss << body.size();
      oss << "\r\n";
   }

   /* End of header. */
   oss << "\r\n";
   if (!body.empty()) {
      oss << body;
   }

   return oss.str();
}
