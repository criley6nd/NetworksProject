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

  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt");
    return 1;
  }

  // bind address to socket
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(portnum); // the server will listen on port 40000
  addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // open socket on localhost IP address for server
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind");
    return 2;
  }

  while(true){
    // set socket to listen status
    if (listen(sockfd, 1) == -1) {
      perror("listen");
      return 3;
    }
   

    // accept a new connection from a client
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);

    std::cout << "here" << std::endl;

    if (clientSockfd == -1) {
      perror("accept");
      return 4;
    }

    char ipstr[INET_ADDRSTRLEN] = {'\0'};
    inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
    std::cout << "Accept a connection from: " << ipstr << ":" <<
      ntohs(clientAddr.sin_port) << std::endl;

    int buffsize = 1024;
    char buf[buffsize] = {0};
    bool isEnd = false;

    while(!isEnd){
      memset(buf, '\0', sizeof(buf));

      int msglen = recv(clientSockfd, buf, buffsize, 0);

      if (msglen == -1) { //only worry about first three arguments
        perror("Client closed Connection");
        break;
      }

      std::cout << buf << " " << msglen << std::endl;

      if (send(clientSockfd, buf, buffsize, 0) == -1) { //only worry about first three arguments
        perror("send");
        return 6;
      }

    }

    close(clientSockfd);


  }
  
  return 0;
}
