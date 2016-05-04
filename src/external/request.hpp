
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <vector>
#include <cstring>
#include "header.hpp"

/// A request received from a client.
struct request
{
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::vector<header> headers;
};

void HTTPRequestToString(struct request &httpRequest)
{
   printf("HTTP Request:\n");
   printf("Method: %s\n", httpRequest.method.c_str());
   printf("URI: %s\n", httpRequest.uri.c_str());
   printf("Version: %d/%d\n", httpRequest.http_version_major,
          httpRequest.http_version_minor);
   for (header &h : httpRequest.headers) {
      printf("%s: %s\n", h.name.c_str(), h.value.c_str());
   }
}

#endif // HTTP_REQUEST_HPP
