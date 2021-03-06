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
  string hostname;
  int portno;
  string filename;
  if(argc < 4)
  {
    cout << "You have not entered a port number or a file directory to save the file to\n";
    exit(0);
  }
  else
  {
    portno = atoi(argv[2]);
    if(portno < 1024)
    {
      cout << "The portno you have entered is not valid please try again." << endl;
      exit(0);
    }
    hostname = argv[1];
    filename = argv[3];
  }
  // create a socket using TCP IP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // struct sockaddr_in addr;
  // addr.sin_family = AF_INET;
  // addr.sin_port = htons(40001);     // short, network byte order
  // addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  // memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
  // if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
  //   perror("bind");
  //   return 1;
  // }

  struct sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(portno);     // short, network byte order
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  // connect to the server
  if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
    perror("connect");
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


  // send/receive data to/from connection
  bool isEnd = false;
  string input;
  char buf[2000000] = {0};
  stringstream ss;
  ifstream myfile (filename);
  while (!isEnd) {
    memset(buf, '\0', sizeof(buf));

    if(myfile.is_open())
    {
      cout << "ere" << endl;
      while(getline(myfile, input))
      {
        if (send(sockfd, input.c_str(), input.size(), 0) == -1) {
          perror("send");
          return 4;
        }
        if (recv(sockfd, buf, sizeof(buf), 0) == -1) {
          perror("recv");
          return 5;
        }
      }
      myfile.close();
    }
  //  cin >> input;
  /*  if (send(sockfd, input.c_str(), input.size(), 0) == -1) {
      perror("send");
      return 4;
    }*/


    if (recv(sockfd, buf, 20, 0) == -1) {
      perror("recv");
      return 5;
    }
  }

  close(sockfd);

  return 0;
}
