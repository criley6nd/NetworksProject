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
#include <fstream>


int
main(int argc, char* argv[])
{

  if(argc != 4){
    std::cerr << "Invalid arguments" << std::endl;
    return -1;
  }

    char* hostname = argv[1];
    int portnum = std::stoi(argv[2]);
    std::string filename = argv[3];

  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(portnum);  // open a socket on port 4000 of the server
  serverAddr.sin_addr.s_addr = inet_addr(hostname); // use localhost as the IP address of the server to set up the socket
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  // connect to the server
  if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
    perror("Incorrect hostname or port number");
    return 2;
  }

  struct sockaddr_in clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  if (getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1) {
    perror("getsockname");
    return 3;
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  std::cout << "Set up a connection from: " << ipstr << ":" <<
    ntohs(clientAddr.sin_port) << std::endl;


  // send/receive data (1 message) to/from the server
  bool isEnd = false;
  std::string input;

  int buffsize = 1024;
  char readbuf[buffsize] = {0};
  memset(readbuf, '\0', sizeof(readbuf));

  char recvbuf[buffsize] = {0};
  memset(recvbuf, '\0', sizeof(recvbuf));
  std::stringstream ss;

  std::ifstream inputFile(filename, std::ios::binary);

  while (inputFile.read(readbuf, buffsize)) {
    

    // std::cout << "send: ";
    // std::cin >> input;
    if (send(sockfd, readbuf, buffsize, 0) == -1) {
      perror("send");
      return 4;
    }


    if (recv(sockfd, recvbuf, buffsize, 0) == -1) {
      perror("recv");
      return 5;
    }
    // ss << buf << std::endl;
    // std::cout << "echo: ";
    // std::cout << buf << std::endl;

    // if (ss.str() == "close\n")
    //   break;

    // ss.str("");

    memset(readbuf, '\0', sizeof(readbuf));
    memset(recvbuf, '\0', sizeof(recvbuf));
  }

  close(sockfd);

  return 0;
}
