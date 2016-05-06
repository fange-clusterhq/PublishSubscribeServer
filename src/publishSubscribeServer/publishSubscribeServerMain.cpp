/*
 * @file main.c
 * @brief The main function for the Publish Subscribe Server
 * @author Yihua Eric Fang (yihuaf)
 */

#include "publishSubscribeServer.h"
#include <iostream>

/*
 * @brief Print out the usage.
 *
 * @return None.
 */
void
Usage()
{
   std::cout << "server [-p port]" << std::endl;
   exit(EXIT_FAILURE);
}


int
main(int argc,
     char **argv)
{
   int port = 0;
   int option = 0;

   while ((option = getopt(argc,argv,"p:")) != -1) {
      switch (option) {
         case 'p':
            port = atoi(optarg);
            break;
         default:
            Usage();
      }
   }

   if (port == 0) {
      Usage();
   }

   /* Start the server. */
   PublishSubscribeServer server = PublishSubscribeServer(port);
   server.Init();
   try {
      server.Start();
   } catch (const char *error) {
      perror(error);
      exit(EXIT_FAILURE);
   }

   return 0;
}
