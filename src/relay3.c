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
#include "pgp_crypto.h"

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
#define char_len 5

/**
 * A struct for client specific data, also includes pointer to create
 * a list of clients.
 */
struct client {
	/* The clients socket. */
	int 		fd;
	int 		fd_ui;
	FILE 		*fp;
	//int ui;
	char 		file_name[char_len];
	/* The bufferedevent for this client. */
	struct 	bufferevent *buf_ev;
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
buffered_on_read_ui(struct bufferevent *bev, void *arg)
{
	printf("buffered_on_read_ui\n");
}

void
buffered_on_read_hj(struct bufferevent *bev, void *arg)
{
	struct client *client = (struct client *)arg;
	int n = -1;
	char tmp[1024];
	printf("bufferd_on_read_hj\n");
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
  free(client);
}

void
buffered_on_error_relay(struct bufferevent *bev, short what, void *arg)
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
  fclose(client->fp);
  free(client);
}

/**
 * Called by libevent when there is an error on the underlying socket
 * descriptor.
 */
void
buffered_on_error(struct bufferevent *bev, short what, void *arg)
{
	struct client *client = (struct client *)arg;
	int ret = -1; // decrypt return 값
	char temp_file_name[char_len];
	int len = -1; //fread return 값
	char header_buff[128] = {0,}; // 헤더 버퍼
	int header_len_check = 2; //받은 파일의 시작이 0xdead(2바이트) 인지 확인
	int total_header_len = 4; // ####임시####, 헤더 총 길이
	
	// aslkfjasdlkfjasldf
	FILE *fp = NULL;
	int server_sockfd;
	struct sockaddr_in serveraddr;
	int client_len;
	char buffer[1024] = {0, };

	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
		perror("socket");

	serveraddr.sin_family = AF_INET;
	if (client->fd == 9) {
		printf("fd is 9\n");
		serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	}
	if (client->fd == 10) {
		printf("fd is 10\n");
		serveraddr.sin_addr.s_addr = inet_addr("143.248.231.97");
	}
	serveraddr.sin_port = htons(5555);
	client_len = sizeof(serveraddr);

	if (connect(server_sockfd, (struct sockaddr *)&serveraddr, client_len) < 0)
	{
			perror("connect error :");
	}	
	//aslkdjflaskdjfalskdf

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

	/*
	int sock_fd;
	sock_fd = conn
	*/
	/*
	//다른 노드로부터 받은 파일 파싱 [시작]
	ret = pgp_dec(client->file_name,"is521ghkdlxld",temp_file_name);
	printf("ret: %d, client_fd: %d\n",ret,client->fd);
	if (ret == 0) {
		printf("decryption success\n");
		printf("client->fd_ui: %d\n",client->fd_ui);
		client->fp = fopen(temp_file_name,"rb");
		if (!(client->fp))
			perror("fopen");
		len = fread(header_buff, sizeof(char), header_len_check, client->fp);
		if (len == 2) {
			printf("read %dbyte from decrypted file\n",header_len_check);
			if (header_buff[0] == 'a' && header_buff[1] == 'b') {
				printf("it is our protocol message\n");
				//write(client->fd_ui)           asdfasdfasdfasdfafds
				
				fread..(이전에 fread했으므로 rewind 고려
				if( destination is mine)
					write(port 5556, 데이터 도착) 포맷: "홍길동"파일 도착. 파일 이름 Temp_file_name->"홍길동"으로 변경
				if( destination is not mine)
					connect(from에 쓰여진 노드와 커넥트)
					write(from에 쓰여진 노드에게 전송)					
				
			}
		}
		if (len != 2) {
			printf(" ret != %d in fread, after decrypt\n",header_len_check);
			//어니언 프로토콜 데이터 아님. 파일 삭제
		}
	}
	//다른노드로부터 파일 파싱..
	//2.내꺼다.
	//3.릴레이 해줘야한다.
	
	client->buf_ev = bufferevent_new(client_fd, buffered_on_read_hj,
			buffered_on_write, buffered_on_error, client);

	bufferevent_enable(client->buf_ev, EV_READ);
	
	//4. 내꺼 아니다 or 에러다. -> 버림(모두 프리)

	if (ret == -1) {
		printf(" decryption failed\n");
		//디크립션 실패. 파일 삭제
	}
	//다른 노드로부터 받은 파일 파싱 [끝]
	*/
	free(client);
}

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
	/*
	struct client *arg_client = (struct client *)arg;
	client->ui = arg_client->ui; //수정 필요
	*/

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
	/*	
	if (arg) {
		printf("free, client_relay\n");
		free(arg);
	}
	*/
/*
	do {
		rand_string(client->file_name, char_len);
		printf("f_name: %s\n",client->file_name);
	} while(access(client->file_name, F_OK) != -1);

	client->fp = fopen(client->file_name,"wb");
	if (!(client->fp))
		perror("fopen");
*/
	/* Create the buffered event.
	 *
	 * The first argument is the file descriptor that will trigger
	 * the events, in this case the clients socket.
	 *
	 * The second argument is the callback that will be called
	 * when data has been read from the socket and is available to
	 * the application.
	 *
	 * The third argument is a callback to a function that will be
	 * called when the write buffer has reached a low watermark.
	 * That usually means that when the write buffer is 0 length,
	 * this callback will be called.  It must be defined, but you
	 * don't actually have to do anything in this callback.
	 *
	 * The fourth argument is a callback that will be called when
	 * there is a socket error.  This is where you will detect
	 * that the client disconnected or other socket errors.
	 *
	 * The fifth and final argument is to store an argument in
	 * that will be passed to the callbacks.  We store the client
	 * object here.
	 */
	client->buf_ev = bufferevent_new(client_fd, buffered_on_read_hj,
			buffered_on_write, buffered_on_error_relay, client);

	/* We have to enable it before our callbacks will be
	 * called. */
	bufferevent_enable(client->buf_ev, EV_READ|EV_WRITE);

	printf("[Relay] Accepted connection from %s\n", 
			inet_ntoa(client_addr.sin_addr));
}

void relay()
{
	int listen_fd, listen_fd2;
	struct sockaddr_in listen_addr, listen_addr2;
	struct event ev_accept;
	int reuseaddr_on, reuseaddr_on2;
	socklen_t client_len2;
	struct client *client;
	struct client *client_relay;

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

	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	listen_addr.sin_port = htons(SERVER_PORT);
	
	listen_addr2.sin_family = AF_INET;
	listen_addr2.sin_addr.s_addr = INADDR_ANY;
	listen_addr2.sin_port = htons(SERVER_PORT2);
	client_len2 = sizeof(listen_addr2);

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
	
	//2
	//TODO : free calloc
	client = (struct client *)calloc(1, sizeof(*client));
	if (client == NULL)
		err(1, "malloc failed");
	
	client_relay = (struct client *)calloc(1, sizeof(*client_relay));
	if (client_relay == NULL)
		err(1, "malloc failed");

	if (bind(listen_fd2, (struct sockaddr *)&listen_addr2,
		sizeof(listen_addr2)) < 0)
		err(1, "bind2 failed");
	if (listen(listen_fd2, 5) < 0)
		err(1, "listen2 failed");
	reuseaddr_on2 = 1;
	setsockopt(listen_fd2, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on2, 
	    sizeof(reuseaddr_on2));
	
	client->fd_ui = accept(listen_fd2, (struct sockaddr *)&listen_addr2, &client_len2);
	client_relay->fd_ui = client->fd_ui;

	/* Set the socket to non-blocking, this is essential in event
	 * based programming with libevent. */
	if (setnonblock(client->fd_ui) < 0)
		err(1, "failed to set server socket to non-blocking22222222222222");

	client->buf_ev = bufferevent_new(client->fd_ui, buffered_on_read,
			buffered_on_write_ui, buffered_on_error_ui, client);

	/* We have to enable it before our callbacks will be
	 * called. */
	bufferevent_enable(client->buf_ev, EV_READ|EV_WRITE);

	printf("[UI] Accepted connection from %s\n", inet_ntoa(listen_addr2.sin_addr));

	/* We now have a listening socket, we create a read event to
	 * be notified when a client connects. */
	event_set(&ev_accept, listen_fd, EV_READ|EV_PERSIST, on_accept, client_relay);
	event_add(&ev_accept, NULL);

	/* Start the event loop. */
	event_dispatch();
}
