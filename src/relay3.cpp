/*
 * The MIT License (MIT)
 * Copyright (c) 2011 Jason Ish
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*
 * libevent echo server example using buffered events.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "common.h"
//#include "pgp_crypto.h"
#include <iostream>
#include <vector>

/* Required by event.h. */
#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

/* Libevent. */
#include <event.h>

/* Port to listen on. */
#define SERVER_PORT 5555
#define SERVER_PORT2 5556
#define SERVER_PORT3 9099
#define char_len 5

std::string pack_total(std::string str1, std::string str2, int sel) {
	std::string pkt;
	if (sel == 0) {
		pkt += '0';
	}
	else if(sel == 1) {
		pkt += '1';
	}
	else if(sel == 3) {
		pkt += '3';
	}
	else if(sel == 4) {
		pkt += '4';
	}
	pkt += str1.size();
	pkt += str1;
	pkt += str2.size();
	pkt += str2;

	return pkt;
}

//TODO: nodelist
//Nodelist nodelist;

/**
 * A struct for client specific data, also includes pointer to create
 * a list of clients.
 */
struct client {
	/* The clients socket. */
	int 		fd;
	int 		fd_ui;
	FILE 		*fp;
	char 		file_name[char_len];
	/* The bufferedevent for this client. */
	struct 	bufferevent *buf_ev;
	std::string github_id;
	std::string passphrase;
};

/**
 * Set a socket to non-blocking mode.
 */
int
setnonblock(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0)
		return flags;
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0)
		return -1;

	return 0;
}

/**
 * Called by libevent when there is data to read.
 */
void
buffered_on_read(struct bufferevent *bev, void *arg)
{
	/* Write back the read buffer. It is important to note that
	 * bufferevent_write_buffer will drain the incoming data so it
	 * is effectively gone after we call it. */
	bufferevent_write_buffer(bev, bev->input);
}

void
buffered_on_read_server(struct bufferevent *bev, void *arg)
{
	struct client *client = (struct client *)arg;
	//TODO:
	//server가 nodelist msg보내주면 파싱해서 전역변수 nodelist에 저장.
	//그리고 msg를 UI한테도 넘겨줘야하므로, msg를 그대로 client->fd_ui에 write.
	//ex) write(client->fd_ui, buffer, sizeof(buffer));

	/* 참고예제.
	int n = -1;
	char tmp[1024] = {0, };
	while (1) {
		n = bufferevent_read(bev, tmp, sizeof(tmp));
		if (n <= 0)
			break;
		fwrite(tmp, sizeof(char), n, client->fp);
	}
	*/
	bufferevent_write_buffer(bev, bev->input);
}

void
buffered_on_write_server(struct bufferevent *bev, void *arg)
{
	//여기서는 구현하지 않음. 비워둘것.
}

void
buffered_on_read_empty(struct bufferevent *bev, void *arg)
{
	printf("read emtpy\n");
}

void
buffered_on_write_empty(struct bufferevent *bev, void *arg)
{
	printf("write_empty\n");
	//bufferevent_write_buffer(bev, client->ev_buff);
}

void
buffered_on_read_ui(struct bufferevent *bev, void *arg)
{
	//printf("buffered_on_read_ui\n");
	struct client *client = (struct client *)arg;
	int n = -1;
	char tmp[1024] = {0, };
	char write_buff[1024] = {0, };
	unsigned char check, len1, len2;
	std::string str1, str2, input_file_name;
	FILE *fp = NULL;

	while (1) {
		n = bufferevent_read(bev, tmp, sizeof(tmp));
		if (n <= 0)
			break;
		//recv = tmp;
		std::string recv(tmp);
		//TODO 메세지가 중간에 끊겨서 들어오면 버그.
		while(!recv.empty()) {
			char temp_file_name[char_len] = {0, };
			int fd = -1;
			struct sockaddr_in serveraddr;
			int client_len;

			//TODO 테스트 변수
			std::vector<std::string> vec_out;
			//std::string next_dest_ip = "143.248.231.97";
			std::string next_dest_ip = argv[1];
			std::string output_file = "zpzpzpzp";
//vector<string> = randomizer(Nodelist nodelist,std::string my_github_id, std::string final_github_id);

			check = recv[0];
			len1 = recv[1];
			str1 = recv.substr(2, len1);
			len2 = recv[2 + len1];
			str2 = recv.substr(len1 + 3, len2);

			// **************************test start
			if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
				perror("socket");

			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = inet_addr(next_dest_ip.c_str()); //TODO 깃허브 아이디 받아서 IP로 변환 전역변수 nodelist에서 ip 빼오기.
			serveraddr.sin_port = htons(SERVER_PORT);
			client_len = sizeof(serveraddr);

			if (connect(fd, (struct sockaddr *)&serveraddr, client_len) < 0)
				perror("connect error :");

			int recv_len = recv.size();
			write(fd, recv.c_str(), recv_len);
			close(fd);

			// ********************************test end
			if (check == '0') {
				/*// 테스트
				std::cout << "TXT" <<std::endl;
				std::cout << (int)len1 <<std::endl;
				std::cout << str1 <<std::endl;
				std::cout << (int)len2 <<std::endl;
				std::cout << str2 <<std::endl;
				*/
				/*
				// string msg
				do {
					rand_string(temp_file_name, char_len);
					printf("temp_f_name: %s\n", temp_file_name);
				} while(access(temp_file_name, F_OK) != -1);

				fp = fopen(temp_file_name,"wb");
				if (!(fp))
					perror("fopen");

				fwrite(write_buff, sizeof(char), str2.size(), fp);
				fclose(fp);
				memset(write_buff, 0, 1024);

				input_file_name = temp_file_name;
				//vec_out = randomizer(nodelist, client->github_id, str1);
				//output_file = enc(input_file_name, client->passphrase, vec_out);

				if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
					perror("socket");

				serveraddr.sin_family = AF_INET;
				serveraddr.sin_addr.s_addr = inet_addr(next_dest_ip.c_str()); //TODO 깃허브 아이디 받아서 IP로 변환 전역변수 nodelist에서 ip 빼오기.
				serveraddr.sin_port = htons(SERVER_PORT);
				client_len = sizeof(serveraddr);

				if (connect(fd, (struct sockaddr *)&serveraddr, client_len) < 0)
					perror("connect error :");

				fp = fopen(output_file.c_str(),"rb");
				if (!fp)
					perror("fopen");
				n = -1;
				while (feof(fp) == 0) {
					n = fread(write_buff, sizeof(char), 1024, fp);    // 1바이트씩 4번(4바이트) 읽기
					write(fd, write_buff, n);
					memset(write_buff, 0, 1024);                          // 버퍼를 0으로 초기화
				}
				fclose(fp);
				close(fd);
				//remove(input_file_name); //TODO 인크립트에 넣으려고 만든 템프파일 지우기
				//remove(output_file_name); //TODO 인크립트에 넣으려고 만든 템프파일 지우기

				//TODO:
				//파일 연다, str2 메세지 저장, 파일 닫는다, enc에 파일 넣는다. 암호 넣는다.type 입력한다.
				//str1이 마지막 데스티네이션, 내가 첫번째가 되도록 vector를 만든다.
				// 위 vector에서 두번째, github_id의 ip주소를 구해서 connect한다. wrtie한다.
				//input_file_name 삭제
				*/

				/*
				std::string output_file_name =
					enc(std::string input_file_name,
							std::string passphrase,
							Nodelist nodelist,
							int type);
				*/

			}
			else if (check == '1') {
				/*// 테스트
				std::cout << "FILE" <<std::endl;
				std::cout << (int)len1 <<std::endl;
				std::cout << str1 <<std::endl;
				std::cout << (int)len2 <<std::endl;
				std::cout << str2 <<std::endl;
				*/
				// FILE
				/*

				//vec_out = randomizer(nodelist, client->github_id, str1);
				//output_file = enc(str2, client->passphrase, vec_out);

				if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
					perror("socket");

				serveraddr.sin_family = AF_INET;
				serveraddr.sin_addr.s_addr = inet_addr(next_dest_ip.c_str()); //TODO 깃허브 아이디 받아서 IP로 변환 전역변수 nodelist에서 ip 빼오기.
				serveraddr.sin_port = htons(SERVER_PORT);
				client_len = sizeof(serveraddr);

				if (connect(fd, (struct sockaddr *)&serveraddr, client_len) < 0)
					perror("connect error :");

				fp = fopen(output_file.c_str(),"rb");
				if (!fp)
					perror("fopen");
				n = -1;
				while (feof(fp) == 0) {
					n = fread(write_buff, sizeof(char), 1024, fp);    // 1바이트씩 4번(4바이트) 읽기
					write(fd, write_buff, n);
					memset(write_buff, 0, 1024);                          // 버퍼를 0으로 초기화
				}
				fclose(fp);
				close(fd);
				*/

				//TODO:
				//str2 메세지 저장. enc에 str2 파일 경로 넣는다. 암호 넣는다. type 입력한다.
				//str1이 마지막 데스티네이션, 내가 첫번째가 되도록 vector를 만든다.
				// 위 vector에서 두번째, github_id의 ip주소를 구해서 connect한다. wrtie한다.
				//파일 삭제 하지 않음.

			}
			recv.erase(0, 3 + len1 + len2);
		}
	}
}

void
buffered_on_read_hj(struct bufferevent *bev, void *arg)
{
	struct client *client = (struct client *)arg;
	int n = -1;
	char tmp[1024] = {0, };
	while (1) {
		n = bufferevent_read(bev, tmp, sizeof(tmp));
		if (n <= 0)
			break;
		fwrite(tmp, sizeof(char), n, client->fp);
	}
}

/**
 * Called by libevent when the write buffer reaches 0.  We only
 * provide this because libevent expects it, but we don't use it.
 */
void
buffered_on_write(struct bufferevent *bev, void *arg)
{
	printf("buffered_on_write\n");
}

void
buffered_on_write_ui(struct bufferevent *bev, void *arg)
{
	printf("buffered_on_write_ui\n");
}

void
buffered_on_error_server(struct bufferevent *bev, short what, void *arg)
{
  struct client *client = (struct client *)arg;

  if (what & EVBUFFER_EOF) {
    /* Client disconnected, remove the read event and the
     * free the client structure. */
    printf("Client disconnected.\n");
  }
  else {
    warn("Client socket error, disconnecting.\n");
  }
  bufferevent_free(client->buf_ev);
  close(client->fd);
  //free(client);
}

void
buffered_on_error_ui(struct bufferevent *bev, short what, void *arg)
{
  struct client *client = (struct client *)arg;

  if (what & EVBUFFER_EOF) {
    /* Client disconnected, remove the read event and the
     * free the client structure. */
    printf("Client disconnected.\n");
  }
  else {
    warn("Client socket error, disconnecting.\n");
  }
  bufferevent_free(client->buf_ev);
  close(client->fd_ui);
  //free(client);
}

void
buffered_on_error_hj(struct bufferevent *bev, short what, void *arg)
{
  struct client *client = (struct client *)arg;
	struct sockaddr_in serveraddr;
	int client_len;
	char buffer[1024] = {0, };
	int n = -1;
	std::string pkt, str1, str2;
	int ret = 0;

	//테스트용 변수들. 원래는 다른곳에서 받아와야함.
	std::string input_file_name;
	std::string next_dest_ip = "143.248.231.97";
	std::string output_file_name = "zpzpzpzp";
	std::string next_github_id = "omnibusor";
	int type = 1;

  if (what & EVBUFFER_EOF) {
    /* Client disconnected, remove the read event and the
     * free the client structure. */
    printf("Client disconnected.\n");
		printf("fd_ui: %d, fd: %d\n",client->fd_ui, client->fd);
  	bufferevent_free(client->buf_ev);
  	fclose(client->fp);
  	close(client->fd);

		/*
			dec(string input_file_name,
					string my_github_id,
					string passphrase,
					int *type,
					string &output_file_name,
					string &next_github_id);
		*/
		//ret 1: 릴레이, ret 0: 내꺼, ret -1: 버림
		if (ret == 1) {

			if ((client->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
				perror("socket");
			serveraddr.sin_family = AF_INET;
			serveraddr.sin_addr.s_addr = inet_addr(next_dest_ip.c_str()); //TODO 깃허브 아이디 받아서 IP로 변환 전역변수 nodelist에서 ip 빼오기.
			serveraddr.sin_port = htons(SERVER_PORT);
			client_len = sizeof(serveraddr);
			/*
			if (setnonblock(client->fd) < 0)
				err(1, "[error_hj] failed to set server socket to non-blocking");
			*/
			if (connect(client->fd, (struct sockaddr *)&serveraddr, client_len) < 0)
				perror("connect error :");

			client->fp = fopen(client->file_name,"rb");
			if (!(client->fp))
				perror("fopen");

			while (feof(client->fp) == 0) {
				n = fread(buffer, sizeof(char), 1024, client->fp);    // 1바이트씩 4번(4바이트) 읽기
				write(client->fd, buffer, n);
				memset(buffer, 0, 1024);                          // 버퍼를 0으로 초기화
			}
			fclose(client->fp);
			close(client->fd);
			//remove(client->file_name); //TODO 릴레이해주고 지움

			/*
			len = get_file_size(client->fp);
			ret2 = evbuffer_add_file((client->buf_ev)->output, fileno(client->fp), 0, len);
			printf("ret2: %d\n",ret2);
			//ret2 = bufferevent_write_buffer(client->buf_ev, ev_buff);
			bufferevent_enable(client->buf_ev, EV_WRITE);
			*/
		}
		else if (ret == 0) {
			//TODO: TEST NEEDED when ret = 0
			//write to UI
			//문자
			if (type == 1) {
				int file_len = -1;
				//client->fp = fopen(output_file_name.c_str(),"rb");  // ******************test
				client->fp = fopen(client->file_name,"rb");
				if (!(client->fp))
					perror("fopen");

				file_len = get_file_size(client->fp);
				n = fread(buffer, sizeof(char), file_len, client->fp);
				fclose(client->fp);
				//remove(output_file_name); //TODO txt 파일 읽고 버퍼에 저장한다음 지움.
				str1 = next_github_id;
				str2 = buffer;
				pkt = pack_total(str1, str2, 0);
				std::cout << str1 <<std::endl;
				std::cout << str2 <<std::endl;
			}
			//파일
			else if (type == 2) {
				str1 = next_github_id;
				str2 = output_file_name;
				pkt = pack_total(str1, str2, 1);
				std::cout << str1 <<std::endl;
				std::cout << str2 <<std::endl;
			}
			size_t pkt_len = pkt.length();
			write(client->fd_ui, pkt.c_str(), pkt_len); //TODO write, read 실제 읽고 쓴값 보고 에러체킹
		}
		else if (ret == -1) {
			//remove? ->outputfile_name 널이므로 안지워도됨.
		}
		else {
			printf("You must not see this message\n");
		}
  }
  else {
    warn("Client socket error, disconnecting.\n");
  	bufferevent_free(client->buf_ev);
  	close(client->fd);
  	fclose(client->fp);
		if (remove(client->file_name) == 0) {
			printf("[Socket error] remove file name:%s\n",client->file_name);
		}
		else {
			printf("[Socket error] can't remove file name:%s\n",client->file_name);
		}
  	//free(client);
  }
	printf("error_hj end\n");
}

/*
void
buffered_on_error(struct bufferevent *bev, short what, void *arg)
{
	struct client *client = (struct client *)arg;
	char temp_file_name[char_len];

	// aslkfjasdlkfjasldf
	FILE *fp = NULL;
	int server_sockfd;
	struct sockaddr_in serveraddr;
	int client_len;
	char buffer[1024] = {0, };

	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
		perror("socket");

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("143.248.231.97");
	serveraddr.sin_port = htons(5555);
	client_len = sizeof(serveraddr);

	if (connect(server_sockfd, (struct sockaddr *)&serveraddr, client_len) < 0)
	{
			perror("connect error :");
			exit(-1);
	}
	//aslkdjflaskdjfalskdf

	if (what & EVBUFFER_EOF) {
		printf("Client disconnected.\n");
	}
	else {
		warn("Client socket error, disconnecting.\n");
	}
	bufferevent_free(client->buf_ev);
	close(client->fd);

	if (client->fp) {
		printf("client->fp close\n");
		fclose(client->fp);
	}

	printf("f_name:%s\n",client->file_name);
	// output 파일이름 지정: temp_file_name. 겹치면 안되니까 랜덤 스트링
	do {
		rand_string(temp_file_name, char_len);
		printf("temp_f_name: %s\n", temp_file_name);
	} while(access(temp_file_name, F_OK) != -1);

	fp = fopen(client->file_name,"rb");
	if (!fp)
		perror("fopen");


	while (feof(fp) == 0)    // 파일 포인터가 파일의 끝이 아닐 때 계속 반복
	{
			fread(buffer, sizeof(char), 1024, fp);    // 1바이트씩 4번(4바이트) 읽기
			write(server_sockfd, buffer, 1024);
			memset(buffer, 0, 1024);                          // 버퍼를 0으로 초기화
	}
	close(server_sockfd);
	fclose(fp);

	free(client);
}
*/

/**
 * This function will be called by libevent when there is a connection
 * ready to be accepted.
 */
void
on_accept(int fd, short ev, void *arg)
{
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	struct client *client;

	client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0) {
		warn("accept failed");
		return;
	}

	/* Set the client socket to non-blocking mode. */
	if (setnonblock(client_fd) < 0)
		warn("failed to set client socket non-blocking");

	/* We've accepted a new client, create a client object. */
	client = (struct client *)calloc(1, sizeof(*client));
	if (client == NULL)
		err(1, "malloc failed");
	client->fd_ui = ((struct client *)arg)->fd_ui;
	client->fd = client_fd;
	printf("[on_accept] fd_ui:%d, fd:%d\n", client->fd_ui, client->fd);

	do {
		rand_string(client->file_name, char_len);
		printf("f_name: %s\n",client->file_name);
	} while(access(client->file_name, F_OK) != -1);

	client->fp = fopen(client->file_name,"wb");
	if (!(client->fp))
		perror("fopen");

	client->buf_ev = bufferevent_new(client_fd, buffered_on_read_hj,
			buffered_on_write, buffered_on_error_hj, client);

	/* We have to enable it before our callbacks will be
	 * called. */
	bufferevent_enable(client->buf_ev, EV_READ|EV_WRITE);

	printf("[on_accept_Relay] Accepted connection from %s\n",
			inet_ntoa(client_addr.sin_addr));
}

void
on_accept2(int fd, short ev, void *arg)
{
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	struct client *client;

	client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
	if (client_fd < 0) {
		warn("accept failed");
		return;
	}

	/* Set the client socket to non-blocking mode. */
	if (setnonblock(client_fd) < 0)
		warn("failed to set client socket non-blocking");

	/* We've accepted a new client, create a client object. */
	client = (struct client *)calloc(1, sizeof(*client));
	if (client == NULL)
		err(1, "malloc failed");
	client->fd_ui = ((struct client *)arg)->fd_ui;
	client->fd = client_fd;
	printf("[on_accept_server] fd_ui:%d, fd:%d\n", client->fd_ui, client->fd);

	client->buf_ev = bufferevent_new(client_fd, buffered_on_read_server,
			buffered_on_write_server, buffered_on_error_server, client);

	/* We have to enable it before our callbacks will be
	 * called. */
	bufferevent_enable(client->buf_ev, EV_READ|EV_WRITE);

	printf("[on_accept_SERVER] Accepted connection from %s\n",
			inet_ntoa(client_addr.sin_addr));
}

void relay()
{
	int listen_fd, listen_fd2, listen_fd3;
	struct sockaddr_in listen_addr, listen_addr2, listen_addr3;
	struct event ev_accept, ev_accept2;
	int reuseaddr_on, reuseaddr_on2, reuseaddr_on3;
	socklen_t client_len2;
	struct client *client_ui;
	struct client *client_relay;
	struct client *client_server;
	char id_pw_buff[300] = {0, };

	srand(time(NULL));

	/* Initialize libevent. */
	event_init();

	/* Create our listening socket1. */
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
		err(1, "listen failed");
	memset(&listen_addr, 0, sizeof(listen_addr));

	/* Create our listening socket2. */
	listen_fd2 = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd2 < 0)
		err(1, "listen2 failed");
	memset(&listen_addr2, 0, sizeof(listen_addr2));

	/* Create our listening socket2. */
	listen_fd3 = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd3 < 0)
		err(1, "listen3 failed");
	memset(&listen_addr3, 0, sizeof(listen_addr3));

	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	listen_addr.sin_port = htons(SERVER_PORT);

	listen_addr2.sin_family = AF_INET;
	listen_addr2.sin_addr.s_addr = INADDR_ANY;
	listen_addr2.sin_port = htons(SERVER_PORT2);
	client_len2 = sizeof(listen_addr2);

	listen_addr3.sin_family = AF_INET;
	listen_addr3.sin_addr.s_addr = INADDR_ANY;
	listen_addr3.sin_port = htons(SERVER_PORT3);

	//1
	if (bind(listen_fd, (struct sockaddr *)&listen_addr,
		sizeof(listen_addr)) < 0)
		err(1, "bind failed");
	if (listen(listen_fd, 5) < 0)
		err(1, "listen failed");
	reuseaddr_on = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on,
	    sizeof(reuseaddr_on));

	/* Set the socket to non-blocking, this is essential in event
	 * based programming with libevent. */
	if (setnonblock(listen_fd) < 0)
		err(1, "failed to set server socket to non-blocking");

	//3
	if (bind(listen_fd3, (struct sockaddr *)&listen_addr3,
		sizeof(listen_addr3)) < 0)
		err(1, "bind3 failed");
	if (listen(listen_fd3, 5) < 0)
		err(1, "listen3 failed");
	reuseaddr_on3 = 1;
	setsockopt(listen_fd3, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on3,
	    sizeof(reuseaddr_on3));

	/* Set the socket to non-blocking, this is essential in event
	 * based programming with libevent. */
	if (setnonblock(listen_fd3) < 0)
		err(1, "failed to set server socket to non-blocking 333333333333333333333");

	//2
	client_ui = (struct client *)calloc(1, sizeof(*client_ui));
	if (client_ui == NULL)
		err(1, "malloc failed");

	client_relay = (struct client *)calloc(1, sizeof(*client_relay));
	if (client_relay == NULL)
		err(1, "malloc failed");

	client_server = (struct client *)calloc(1, sizeof(*client_server));
	if (client_server == NULL)
		err(1, "malloc failed");

	if (bind(listen_fd2, (struct sockaddr *)&listen_addr2,
		sizeof(listen_addr2)) < 0)
		err(1, "bind2 failed");
	if (listen(listen_fd2, 5) < 0)
		err(1, "listen2 failed");
	reuseaddr_on2 = 1;
	setsockopt(listen_fd2, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on2,
	    sizeof(reuseaddr_on2));

	client_ui->fd_ui = accept(listen_fd2, (struct sockaddr *)&listen_addr2, &client_len2);
	client_relay->fd_ui = client_ui->fd_ui;
	client_server->fd_ui = client_ui->fd_ui;

	//UI와 소켓 연결 맺고, UI에서 아디, 비번 치기전까지 블록.
	read(client_ui->fd_ui, id_pw_buff, 300);

	/* TODO: 잠시 주석.
	unsigned char check, len1, len2;
	std::string str1, str2;
	std::string recv(id_pw_buff);

	check = recv[0];
	len1 = recv[1];
	str1 = recv.substr(2, len1);
	len2 = recv[2 + len1];
	str2 = recv.substr(len1 + 3, len2);
	*/

	/* 테스트용
	if (check == '3') {
		std::cout << str1 <<std::endl;
		std::cout << str2 <<std::endl;
	}
	*/
	//TODO:
	//id_pw_buff 파싱해서, 아이디 비번 도출 후 gpg_verify에 집어넣고 맞는지 검증
	//맞으면 client_ui->github_id, client_ui->passphrase에 집어넣음. 틀리면 exit(-1);

	//TODO 쓰레드 사용해서 advise(str1) 날리기.

	//TODO 9090포트 열어서 accept하자. on_accept 똑같이 하면됨.
	//TODO[박정환]:
	//listner 구현??
	//listner(client_ui->github_id, ?, ?);
	//내 생각: 서버에게 connect 보내서 소켓 열고, 이벤트 등록해서 논블록킹으로 구동.
	// client_server->fd_ui = client_ui->fd_ui;


	/* Set the socket to non-blocking, this is essential in event
	 * based programming with libevent. */
	if (setnonblock(client_ui->fd_ui) < 0)
		err(1, "failed to set server socket to non-blocking22222222222222");

	client_ui->buf_ev = bufferevent_new(client_ui->fd_ui, buffered_on_read_ui,
			buffered_on_write_ui, buffered_on_error_ui, client_ui);

	/* We have to enable it before our callbacks will be
	 * called. */
	bufferevent_enable(client_ui->buf_ev, EV_READ|EV_WRITE);

	printf("[UI] Accepted connection from %s\n", inet_ntoa(listen_addr2.sin_addr));

	/* We now have a listening socket, we create a read event to
	 * be notified when a client connects. */
	event_set(&ev_accept, listen_fd, EV_READ|EV_PERSIST, on_accept, client_relay);
	event_add(&ev_accept, NULL);
	event_set(&ev_accept2, listen_fd3, EV_READ|EV_PERSIST, on_accept2, client_server);
	event_add(&ev_accept2, NULL);

	/* Start the event loop. */
	event_dispatch();
}
