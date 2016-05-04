
#include "publishSubscribeRequest.h"
#include <cstring>
#include <external/requestParser.hpp>

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

  //auto headerEndIter = get<1>(ret);
  if (parsedRequest.method.compare(GET) == 0) {
    return;
  } else if (parsedRequest.method.compare(DELETE) == 0) {
    return;
  } else if (parsedRequest.method.compare(POST) == 0) {
    return;
  } else {
    return;
  }

  return;

  // good so continue to parse.

}
