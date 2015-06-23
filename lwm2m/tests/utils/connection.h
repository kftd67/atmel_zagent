/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    
 *******************************************************************************/

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <stdio.h>
#include <unistd.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <netdb.h>
//#include <sys/socket.h>
//#include "../client/socket.h"
#include <sys/stat.h>
#include "socket/include/socket.h"

#define LWM2M_STANDARD_PORT_STR "5683"
#define LWM2M_STANDARD_PORT      5683


typedef struct _connection_t
{
	struct _connection_t *  next;
	int                     sock;
	struct sockaddr_in     addr;
	size_t                  addrLen;
} connection_t;

struct addrinfo {
	int ai_flags;
	int ai_family;
	int ai_socktype;
	int ai_protocol;
	size_t ai_addrlen;
	char * ai_canonname;
	struct sockaddr * ai_addr;
	struct addrinfo * ai_next
};

/* Indicates that the socket is intended for bind()+listen(). */
#ifndef AI_PASSIVE
# define AI_PASSIVE     1
#endif /* AI_PASSIVE */

int create_socket(const char * portStr);

connection_t * connection_find(connection_t * connList, struct sockaddr * addr, size_t addrLen);
connection_t * connection_new_incoming(connection_t * connList, int sock, struct sockaddr * addr, size_t addrLen);
connection_t * connection_create(connection_t * connList, int sock, char * host, uint16_t port);

void connection_free(connection_t * connList);

int connection_send(connection_t *connP, uint8_t * buffer, size_t length);

#endif
