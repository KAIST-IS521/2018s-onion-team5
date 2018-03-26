#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>


#include <psocksxx/udpnsockstream.h>
#include "../common/config.h"

#include <string>
#include <cstdlib>
#include <cstring>
#include <cstdio>

void advertise(std::string github_id) {
  int sock;
  struct sockaddr_in bcastAddr;

  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    perror("socket");
    exit(1);
  }

  int flag = 1;
  if (setsockopt (sock, SOL_SOCKET, SO_BROADCAST, &flag, sizeof flag) == -1) {
    perror("setsockopt");
    exit(1);
  }

  memset(&bcastAddr, 0, sizeof(bcastAddr));
	bcastAddr.sin_family = AF_INET;
	bcastAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
	bcastAddr.sin_port = htons(BROADCAST_PORT);

  std::string buf;

  buf += "\xBA\xAA";
  buf += (char) github_id.size();
  buf += github_id;
  buf += "\xAA\xAD";

  int result;
  if ((result = sendto(sock, buf.c_str(), buf.size(), 0,
    (struct sockaddr *)&bcastAddr, sizeof(bcastAddr))) == -1) {
    perror("socket");
    exit(1);
  }

  close(sock);
}
