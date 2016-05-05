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

void
PublishSubscribeRequest::Translate(string &httpRequest)
{
   /* Parse the HTTP Header. */
   struct request parsedRequest;
   request_parser parser = request_parser();
   auto ret = parser.parse(parsedRequest, httpRequest.begin(),
                           httpRequest.end());
   if (get<0>(ret) == request_parser::bad) {
      this->opCode = PublishSubscribeServerOp::ERROR;
      printf("PARSING ERROR\n");
      return;
   } else if (get<0>(ret) == request_parser::indeterminate) {
      printf("INDETERMIN\n");
      this->opCode = PublishSubscribeServerOp::CONTINUE;
      return;
   }

   HTTPRequestToString(parsedRequest);

   if (parsedRequest.method.compare(GET) == 0) {
      this->CheckAndParseSubscribe(parsedRequest.uri);
   } else if (parsedRequest.method.compare(DELETE) == 0) {
      this->CheckAndParseUnsubscribe(parsedRequest.uri);
   } else if (parsedRequest.method.compare(POST) == 0) {
      /* The iterator to the end of the header. */
      auto headerEnd = get<1>(ret);
      /* There is two possibilities, so we need to check both. */
      if (!this->CheckAndParsePublish(parsedRequest, httpRequest, headerEnd)) {
         this->CheckAndParseSubscribe(parsedRequest.uri);
      }
   }

   /* Before returning, the op code should have been set. */
   assert(this->opCode != PublishSubscribeServerOp::FIRST);
   return;
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
PublishSubscribeRequest::CheckAndParsePublish(struct request &parsedRequest,
                                              string &httpRequest,
                                              string::iterator &headerEnd)
{
   /* Parsing out the content length first. */
   int contentLen = 0;
   for (auto &header : parsedRequest.headers) {
      if (header.name.compare(CONTENT_LENGTH) == 0) {
         contentLen = stoi(header.value, nullptr, 0);
      }
   }

   if (contentLen <= 0) {
      this->opCode = PublishSubscribeServerOp::ERROR;
      return false;
   }

   /* Not enough bytes to match content length. */
   if (distance(headerEnd, httpRequest.end()) != contentLen) {
      this->opCode = PublishSubscribeServerOp::CONTINUE;
      return false;
   }

   this->msg = string(headerEnd, httpRequest.end());
   /* Parsing topic out. */
   vector<string> tokenizedUri = this->ParseUriTokenize(parsedRequest.uri);
   if (tokenizedUri.size() != 2) {
      this->opCode = PublishSubscribeServerOp::ERROR;
      return false;
   }

   /* In the form of /topic */
   this->topic = tokenizedUri[1];
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
