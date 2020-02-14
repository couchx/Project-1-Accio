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
using namespace std;
int main(int argc, char** argv)
{
  int portno;
  string filename;
  if(argc < 3)
  {
    cout << "You have not entered a port number or a file directory to save the file to\n";
    exit(0);
  }
  else
  {
    portno = atoi(argv[1]);
    if(portno < 1024)
    {
      cout << "The portno you have entered is not valid please try again." << endl;
      exit(0);
    }
    filename = argv[2];
  }
  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // allow others to reuse the address
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt");
    return 1;
  }

  // bind address to socket
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(portno);     // short, network byte order
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind");
    return 2;
  }

  // set socket to listen status
  if (listen(sockfd, 1) == -1) {
    perror("listen");
    return 3;
  }

  // accept a new connection
  struct sockaddr_in clientAddr;
  socklen_t clientAddrSize = sizeof(clientAddr);
  int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);

  if (clientSockfd == -1) {
    perror("accept");
    return 4;
  }

  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  std::cout << "Accept a connection from: " << ipstr << ":" <<
    ntohs(clientAddr.sin_port) << std::endl;

  // read/write data from/into the connection
  bool isEnd = false;
  char buf[20] = {0};
  stringstream ss;
  string path = filename;
  cout << path << endl;
  ofstream file(path);
  while (!isEnd) {
    memset(buf, '\0', sizeof(buf));

    if(file.is_open())
    {
      if (recv(clientSockfd, buf, 20, 0) == -1) {
        perror("recv");
        return 5;
      }
      file << buf;
      cout << "here";
      if (send(clientSockfd, buf, 20, 0) == -1) {
        perror("send");
        return 6;
      }
    }
    

    if (ss.str() == "close\n")
      break;

    ss.str("");
  }

  close(clientSockfd);

  return 0;
}
