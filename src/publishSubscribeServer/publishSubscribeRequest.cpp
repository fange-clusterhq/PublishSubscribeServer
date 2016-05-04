
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
   struct request parsedRequest;
   request_parser parser = request_parser();
   auto ret = parser.parse(parsedRequest, httpRequest.begin(),
                           httpRequest.end());
   if (get<0>(ret) == request_parser::bad) {
      this->opCode = PublishSubscribeServerOp::ERROR;
      return;
   } else if (get<0>(ret) == request_parser::indeterminate) {
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
   vector<string> tokenizedUri = this->ParseUriTokenize(uri);
   if (tokenizedUri.size() != 2) {
      return false;
   }

   this->topic = tokenizedUri[0];
   this->username = tokenizedUri[1];
   return false;
}


bool
PublishSubscribeRequest::CheckAndParseUnsubscribe(string &uri)
{
   vector<string> tokenizedUri = this->ParseUriTokenize(uri);
   if (tokenizedUri.size() != 2) {
      return false;
   }

   this->topic = tokenizedUri[0];
   this->username = tokenizedUri[1];
   return false;
}


bool
PublishSubscribeRequest::CheckAndParsePublish(struct request &parsedRequest,
                                              string &httpRequest,
                                              string::iterator &headerEnd)
{
   return false;
}


bool
PublishSubscribeRequest::CheckAndParseGetNext(string &uri)
{
   vector<string> tokenizedUri = this->ParseUriTokenize(uri);
   if (tokenizedUri.size() != 2) {
      return false;
   }

   this->topic = tokenizedUri[0];
   this->username = tokenizedUri[1];
   return false;
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
