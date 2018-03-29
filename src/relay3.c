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
	int fd;
	FILE *fp;
	int ui;
	char file_name[char_len];
	/* The bufferedevent for this client. */
	struct bufferevent *buf_ev;
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
buffered_on_read_temp(struct bufferevent *bev, void *arg)
{
	printf("buffered_on_read_temp\n");
}

void
buffered_on_read_hj(struct bufferevent *bev, void *arg)
{
	struct client *client = (struct client *)arg;
	int n = -1;
	char tmp[1024];
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
		printf("client->ui:%d\n",client->ui);
		fclose(client->fp);
	}

	if (client->ui == 0) {
		printf("f_name:%s\n",client->file_name);
		do {
			rand_string(temp_file_name, char_len);
			printf("temp_f_name: %s\n", temp_file_name);
		} while(access(temp_file_name, F_OK) != -1);

		ret = pgp_dec(client->file_name,"is521ghkdlxld",temp_file_name);
		printf("ret: %d, client_fd: %d\n",ret,client->fd);
		if (ret == 0) {
			printf("decryption success\n");
			client->fp = fopen(temp_file_name,"rb");
			if (!(client->fp))
				perror("fopen");
			len = fread(header_buff, sizeof(char), header_len_check, client->fp);
			if (len == 2) {
				printf("read %dbyte from decrypted file\n",header_len_check);
				if (header_buff[0] == 'a' && header_buff[1] == 'b') {
					printf("it is our protocol message\n");
					write(client->fd_ui)           asdfasdfasdfasdfafds
					/*
					fread..(이전에 fread했으므로 rewind 고려
					if( destination is mine)
						write(port 5556, 데이터 도착) 포맷: "홍길동"파일 도착. 파일 이름 Temp_file_name->"홍길동"으로 변경
					if( destination is not mine)
						connect(from에 쓰여진 노드와 커넥트)
						write(from에 쓰여진 노드에게 전송)					
					*/
				}
			}
			if (len != 2) {
				printf(" ret != %d in fread, after decrypt\n",header_len_check);
				//어니언 프로토콜 데이터 아님. 파일 삭제
			}
		}
		if (ret == -1) {
			printf(" decryption failed\n");
			//어니언 프로토콜 데이터 아님. 파일 삭제
		}
	}

	if (client->ui == 1) {
		printf("[UI] RECV complete\n");
	}

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
	int ui = *((int *)arg);

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
	client->ui = ui;
	client->fd = client_fd;
	if (ui == 0) {
		do {
			rand_string(client->file_name, char_len);
			printf("f_name: %s\n",client->file_name);
		} while(access(client->file_name, F_OK) != -1);

		client->fp = fopen(client->file_name,"wb");
		if (!(client->fp))
			perror("fopen");
		
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
				buffered_on_write, buffered_on_error, client);

		/* We have to enable it before our callbacks will be
		 * called. */
		bufferevent_enable(client->buf_ev, EV_READ);

		printf("Accepted connection from %s\n", 
				inet_ntoa(client_addr.sin_addr));
	}

	if (ui == 1) {		
		client->buf_ev = bufferevent_new(client_fd, buffered_on_read_temp,
				buffered_on_write, buffered_on_error, client);

		/* We have to enable it before our callbacks will be
		 * called. */
		bufferevent_enable(client->buf_ev, EV_READ|EV_WRITE);

		printf("Accepted connection from [UI] %s\n", 
				inet_ntoa(client_addr.sin_addr));
	}
}

void relay()
{
	int listen_fd, listen_fd2;
	struct sockaddr_in listen_addr, listen_addr2;
	struct event ev_accept, ev_accept2;
	int reuseaddr_on, reuseaddr_on2;
	int connect_from_UI = 1;
	int connect_from_others = 0;
	
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
	memset(&listen_addr, 0, sizeof(listen_addr));

	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	listen_addr.sin_port = htons(SERVER_PORT);
	
	listen_addr2.sin_family = AF_INET;
	listen_addr2.sin_addr.s_addr = INADDR_ANY;
	listen_addr2.sin_port = htons(SERVER_PORT2);

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
	if (bind(listen_fd2, (struct sockaddr *)&listen_addr2,
		sizeof(listen_addr2)) < 0)
		err(1, "bind2 failed");
	if (listen(listen_fd2, 5) < 0)
		err(1, "listen2 failed");
	reuseaddr_on2 = 1;
	setsockopt(listen_fd2, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on2, 
	    sizeof(reuseaddr_on2));

	/* Set the socket to non-blocking, this is essential in event
	 * based programming with libevent. */
	if (setnonblock(listen_fd2) < 0)
		err(1, "failed to set server socket to non-blocking22222");

	/* We now have a listening socket, we create a read event to
	 * be notified when a client connects. */
	event_set(&ev_accept, listen_fd, EV_READ|EV_PERSIST, on_accept, &connect_from_others);
	event_add(&ev_accept, NULL);

	event_set(&ev_accept2, listen_fd2, EV_READ, on_accept, &connect_from_UI);
	event_add(&ev_accept2, NULL);

	/* Start the event loop. */
	event_dispatch();
}
