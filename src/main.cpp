/*
 * @file main.c
 * @brief The main function for the Publish Subscribe Server
 */

#include "server.h"
#include <iostream>

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
            std::cout << "server [-p port]" << std::endl;
            exit(EXIT_FAILURE);
      }
   }

   Server server = Server(port);
   server.Init();
   server.Start();
   return 0;
}
