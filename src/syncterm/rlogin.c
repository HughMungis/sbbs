/* Copyright (C), 2007 by Stephen Hurd */

/* $Id$ */

#include <stdlib.h>

#include "sockwrap.h"

#include "bbslist.h"
#include "conn.h"
#include "uifcinit.h"

static SOCKET sock=INVALID_SOCKET;

#ifdef __BORLANDC__
#pragma argsused
#endif
void rlogin_input_thread(void *args)
{
	fd_set	rds;
	int		rd;
	int	buffered;
	size_t	buffer;

	SetThreadName("RLogin Input");
	conn_api.input_thread_running=1;
	while(sock != INVALID_SOCKET && !conn_api.terminate) {
		FD_ZERO(&rds);
		FD_SET(sock, &rds);
#ifdef __linux__
		{
			struct timeval tv;
			tv.tv_sec=0;
			tv.tv_usec=500000;
			rd=select(sock+1, &rds, NULL, NULL, &tv);
		}
#else
		rd=select(sock+1, &rds, NULL, NULL, NULL);
#endif
		if(rd==-1) {
			if(errno==EBADF)
				break;
			rd=0;
		}
		if(rd==1) {
			rd=recv(sock, conn_api.rd_buf, conn_api.rd_buf_size, 0);
			if(rd <= 0)
				break;
		}
		buffered=0;
		while(buffered < rd) {
			pthread_mutex_lock(&(conn_inbuf.mutex));
			buffer=conn_buf_wait_free(&conn_inbuf, rd-buffered, 1000);
			buffered+=conn_buf_put(&conn_inbuf, conn_api.rd_buf+buffered, buffer);
			pthread_mutex_unlock(&(conn_inbuf.mutex));
		}
	}
	conn_api.input_thread_running=0;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
void rlogin_output_thread(void *args)
{
	fd_set	wds;
	int		wr;
	int		ret;
	int	sent;

	SetThreadName("RLogin Output");
	conn_api.output_thread_running=1;
	while(sock != INVALID_SOCKET && !conn_api.terminate) {
		pthread_mutex_lock(&(conn_outbuf.mutex));
		ret=0;
		wr=conn_buf_wait_bytes(&conn_outbuf, 1, 100);
		if(wr) {
			wr=conn_buf_get(&conn_outbuf, conn_api.wr_buf, conn_api.wr_buf_size);
			pthread_mutex_unlock(&(conn_outbuf.mutex));
			sent=0;
			while(sent < wr) {
				FD_ZERO(&wds);
				FD_SET(sock, &wds);
#ifdef __linux__
				{
					struct timeval tv;
					tv.tv_sec=0;
					tv.tv_usec=500000;
					ret=select(sock+1, NULL, &wds, NULL, &tv);
				}
#else
				ret=select(sock+1, NULL, &wds, NULL, NULL);
#endif
				if(ret==-1) {
					if(errno==EBADF)
						break;
					ret=0;
				}
				if(ret==1) {
					ret=sendsocket(sock, conn_api.wr_buf+sent, wr-sent);
					if(ret==-1)
						break;
					sent+=ret;
				}
			}
		}
		else
			pthread_mutex_unlock(&(conn_outbuf.mutex));
		if(ret==-1)
			break;
	}
	conn_api.output_thread_running=0;
}

int rlogin_connect(struct bbslist *bbs)
{
	char	*ruser;
	char	*passwd;

	if (!bbs->hidepopups)
		init_uifc(TRUE, TRUE);

	ruser=bbs->user;
	passwd=bbs->password;
	if(bbs->conn_type==CONN_TYPE_RLOGIN_REVERSED) {
		passwd=bbs->user;
		ruser=bbs->password;
	}

	sock=conn_socket_connect(bbs);
	if(sock==INVALID_SOCKET)
		return(-1);

	if(!create_conn_buf(&conn_inbuf, BUFFER_SIZE))
		return(-1);
	if(!create_conn_buf(&conn_outbuf, BUFFER_SIZE)) {
		destroy_conn_buf(&conn_inbuf);
		return(-1);
	}
	if(!(conn_api.rd_buf=(unsigned char *)malloc(BUFFER_SIZE))) {
		destroy_conn_buf(&conn_inbuf);
		destroy_conn_buf(&conn_outbuf);
		return(-1);
	}
	conn_api.rd_buf_size=BUFFER_SIZE;
	if(!(conn_api.wr_buf=(unsigned char *)malloc(BUFFER_SIZE))) {
		FREE_AND_NULL(conn_api.rd_buf);
		destroy_conn_buf(&conn_inbuf);
		destroy_conn_buf(&conn_outbuf);
		return(-1);
	}
	conn_api.wr_buf_size=BUFFER_SIZE;

	if(bbs->conn_type == CONN_TYPE_RLOGIN || bbs->conn_type == CONN_TYPE_RLOGIN_REVERSED) {
		conn_send("",1,1000);
		conn_send(passwd,strlen(passwd)+1,1000);
		conn_send(ruser,strlen(ruser)+1,1000);
		if(bbs->bpsrate) {
			char	sbuf[30];
			sprintf(sbuf, "%s/%d", get_emulation_str(get_emulation(bbs)), bbs->bpsrate);

			conn_send(sbuf, strlen(sbuf)+1,1000);
		}
		else {
			char	sbuf[30];
			sprintf(sbuf, "%s/115200", get_emulation_str(get_emulation(bbs)));

			conn_send(sbuf, strlen(sbuf)+1,1000);
		}
	}

	/* Negotiate with GHost and bail if there's apparently no GHost listening. */
	if(bbs->conn_type == CONN_TYPE_MBBS_GHOST) {
		char	sbuf[80];
		char	rbuf[10];
		struct timeval tv;
		int		idx, ret;
		fd_set	rds;

		FD_ZERO(&rds);
		FD_SET(sock, &rds);

		tv.tv_sec=1;
		tv.tv_usec=0;

		/* Check to make sure GHost is actually listening */
		sendsocket(sock, "\r\nMBBS: PING\r\n", 14);

		idx = 0;
		while ((ret = select(sock+1, &rds, NULL, NULL, &tv))==1) {
			recv(sock, rbuf+idx, 1, 0);
			rbuf[++idx] = 0;

			/* It says ERROR, but this is a good response to PING. */
			if (strstr(rbuf,"ERROR\r\n")) {
				break;
			}

			/* We didn't receive the desired response in time, so bail. */
			if (idx >= sizeof(rbuf)) {
				return(-1);
			}
		}

		if (ret < 1) {
			return(-1);
		}

		sprintf(sbuf, "MBBS: %s %d '%s' %d %s\r\n",
			bbs->ghost_program, /* Program name */
			2, /* GHost protocol version */
			bbs->user, /* User's full name */
			999, /* Time remaining */
			"GR" /* GR = ANSI, NG = ASCII */
		);
		sendsocket(sock, sbuf, strlen(sbuf));

		idx = 0;
		while ((ret = select(sock+1, &rds, NULL, NULL, &tv))==1) {
			recv(sock, rbuf+idx, 1, 0);
			rbuf[++idx] = 0;

			/* GHost says it's launching the program, so pass terminal to user. */
			if (strstr(rbuf,"OK\r\n")) {
				break;
			}

			/* We didn't receive the desired response in time, so bail. */
			if (idx >= sizeof(rbuf)) {
				return(-1);
			}
		}

		if (ret < 1) {
			return(-1);
		}

	}

	_beginthread(rlogin_output_thread, 0, NULL);
	_beginthread(rlogin_input_thread, 0, NULL);

	if (!bbs->hidepopups) {
		uifc.pop(NULL);
	}

	return(0);
}

int rlogin_close(void)
{
	char garbage[1024];

	conn_api.terminate=1;
	closesocket(sock);
	while(conn_api.input_thread_running || conn_api.output_thread_running) {
		conn_recv_upto(garbage, sizeof(garbage), 0);
		SLEEP(1);
	}
	destroy_conn_buf(&conn_inbuf);
	destroy_conn_buf(&conn_outbuf);
	FREE_AND_NULL(conn_api.rd_buf);
	FREE_AND_NULL(conn_api.wr_buf);
	return(0);
}
