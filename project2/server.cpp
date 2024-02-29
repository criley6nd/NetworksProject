#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <csignal>

void signalHandler(int signum) {
    std::cout << "Shutting down" << std::endl;
    
    //tear down

    std::exit(0);
}


int
main(int argc, char* argv[])
{
  if(argc != 3){
    std::cerr << "Invalid arguments" << std::endl;
    return -1;
  }

  int portnum = std::stoi(argv[1]);
  char* filename = argv[2];

  std::signal(SIGINT, signalHandler);
  

  return 0;
}
