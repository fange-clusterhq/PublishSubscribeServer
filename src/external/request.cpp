#include "request.hpp"

void
HTTPRequestToString(struct request &httpRequest)
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

