#pragma once

#include <string>

using namespace std;

static const string GET = "GET";
static const string POST = "POST";
static const string DELETE = "DELETE";

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
    void Translate(string &httpRequest);

    PublishSubscribeServerOp opCode;
    string username;
    string topic;
    string msg;
};