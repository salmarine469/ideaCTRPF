#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>

int main(int arg, char **argv)
{
  std::string ip;
  if (2 <= arg)
    ip = std::string(argv[1]);
  else
  {
    std::cout << "Enter IP: ";
    std::cin >> ip;
  }

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    std::cout << "socket error\n";
    return 1;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(5000);
  addr.sin_addr.s_addr = inet_addr(ip.c_str());

  if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    std::cout << "connect error" << std::endl;
    return 1;
  }

  u_int32_t size = 0;
  std::string path;
  if (3 <= arg)
    path = std::string(argv[2]);
  else
  {
    std::cout << "Enter path: ";
    std::cin >> path;
  }

  std::ifstream ifs(path, std::ios::binary);
  if (!ifs)
  {
    std::cout << "File not found\n";
    return 1;
  }

  ifs.seekg(0, std::ios::end);
  size = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  send(sockfd, &size, sizeof(size), 0);

  char *data = (char *)malloc(size);
  ifs.read(data, size);
  ifs.close();

  send(sockfd, data, size, 0);

  free(data);
  close(sockfd);
}
