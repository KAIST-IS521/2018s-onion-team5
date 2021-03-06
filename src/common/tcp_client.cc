#include "tcp_client.h"
#include "util.h"
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fstream>

TCP_Client::TCP_Client(std::string host, int port) {
  this->client = true;
  this->host = host;
  this->port = port;

  this->client_sock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&this->client_sockaddr, 0, sizeof(struct sockaddr_in));
	this->client_sockaddr.sin_family = AF_INET;
	this->client_sockaddr.sin_addr.s_addr = ::inet_addr(this->host.c_str());
	this->client_sockaddr.sin_port = ::htons(this->port);
}

TCP_Client::TCP_Client(int sock, struct sockaddr_in sockaddr) {
  this->client = false;
  this->client_sock = sock;
  ::memcpy(&this->client_sockaddr, &sockaddr, sizeof(struct sockaddr_in));
}

TCP_Client::~TCP_Client() {
  this->close();
}

int TCP_Client::write(void *ptr, int len) {
  if (::write(this->client_sock, ptr, len) == len) {
    return len;
  } else {
    ::perror("write");
    return -1;
  }
}

int TCP_Client::read(void *ptr, int len) {
  int size;
  if ((size = ::read(this->client_sock, ptr, len)) >= 0) {
    return size;
  } else {
    ::perror("write");
    return -1;
  }
}

bool TCP_Client::connect() {
  if (this->client == false) return false;

  if (::connect(this->client_sock, (struct sockaddr *) &this->client_sockaddr, sizeof(struct sockaddr_in)) == -1) {
    //::perror("connect");
    return false;
  }
  return true;
}

int TCP_Client::send(std::string data) {
  size_t payload_size = data.size();
  size_t temp = BSWAP64(payload_size);
  unsigned char type = 0;
  if (this->write(&type, sizeof(unsigned char)) == -1) {
    return -1;
  }

  // send size
  if (this->write(&temp, sizeof(size_t)) == -1) {
    return -1;
  }

  const char *ptr = data.c_str();
  int offset = 0;
  while (offset < payload_size) {
    int length = BUFSIZ < (payload_size - offset)? BUFSIZ: (payload_size - offset);
    //printf("length %d \n", length);
    int written_bytes = this->write((void *) (ptr + offset), length);
    //printf("written_bytes %d \n", written_bytes);
    if (written_bytes == -1) {
      // Error occured
      break;
    }
    offset += written_bytes;
  }

  return offset;
}

int TCP_Client::send_file(std::string path) {
  unsigned char type = 1;
   // send type
  if (this->write(&type, sizeof(unsigned char)) == -1) {
    return -1;
  }

  size_t payload_size = get_file_size(path);
  size_t temp = BSWAP64(payload_size);

  // send size
  if (this->write(&temp, sizeof(size_t)) == -1) {
    return -1;
  }

  //const char *ptr = data.c_str();
  std::ifstream is;
  is.open(path, std::ifstream::binary);
  int offset = 0;
  while (offset < payload_size) {
    //int length = BUFSIZ < (payload_size - offset)? BUFSIZ: (payload_size - offset);

    char buff[BUFSIZ + 1];
    memset(buff, 0, BUFSIZ + 1);

    is.read(buff, BUFSIZ);

    int length = is.gcount();

    int written_bytes = this->write((void *) buff, length);
    if (written_bytes == -1) {
      // Error occured
      break;
    }
    offset += written_bytes;
  }

  return offset;
}


int TCP_Client::recv(std::string &data) {
  std::string retval;

  size_t temp = 0;
  unsigned char type = 0;

  if (this->read(&type, sizeof(unsigned char)) == -1) {

  }

  // recv size
  if (this->read(&temp, sizeof(size_t)) == -1) {

  }

  size_t payload_size = BSWAP64(temp);

  int offset = 0;
  char buff[BUFSIZ + 1];
  std::ofstream os;
  if (type == 1) {
    retval = get_temppath();
    os.open(retval, std::ofstream::binary);
  }

  while (offset < payload_size) {
    ::memset(buff, 0, BUFSIZ + 1);
    int read_bytes = this->read(buff, BUFSIZ);

    if (read_bytes == -1) {
      // Error occured
      break;
    }

    if (type == 0) {
      retval.append(buff, read_bytes);
    } else if (type == 1) {
       os.write(buff, read_bytes);
    }
    offset += read_bytes;
  }

  if (type == 1) {
     os.close();
  }

  data = retval;

  return offset;
}

void TCP_Client::close() {
  this->client = false;
  this->host.clear();
  this->port = 0;
  ::close(this->client_sock);
  memset(&this->client_sockaddr, 0, sizeof(struct sockaddr_in));
}


bool TCP_Client::s_timeout(long tv_sec, long tv_usec) {
  struct timeval timeout;
  timeout.tv_sec = tv_sec;
  timeout.tv_usec = tv_usec;

  return this->s_timeout(timeout);
}

bool TCP_Client::s_timeout(struct timeval timeout) {
  if (setsockopt (this->client_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
    perror("setsockopt");
    return false;
  }

  if (setsockopt (this->client_sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
    perror("setsockopt");
    return false;
  }
  return true;
}
