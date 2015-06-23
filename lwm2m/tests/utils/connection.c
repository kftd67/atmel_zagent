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
 *    Pascal Rieux - Please refer to git log
 *    
 *******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "connection.h"
#include "socket/include/socket.h"
#include "../../src/main.h"


int create_socket(const char * portStr)
{
	/* Zebra change: Reddy
    int s = -1;
    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    //if (0 != getaddrinfo(NULL, portStr, &hints, &res))
    //{
    //    return -1;
    //}

    for(p = res ; p != NULL && s == -1 ; p = p->ai_next)
    {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s >= 0)
        {
            if (-1 == bind(s, p->ai_addr, p->ai_addrlen))
            {
                close(s);
                s = -1;
            }
        }
    }

    //freeaddrinfo(res);

    return s;
	*/
	/** Socket for Tx */
	SOCKET tx_socket = -1;
	struct sockaddr_in addr;

	/* Create socket bind LWM2M Client to port .*/
	// Create socket for Tx UDP
	if ((tx_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Connection: failed to create TX UDP client socket!\r\n");
	}

	/* Initialize socket address structure and bind service. */
	addr.sin_family = AF_INET;
	addr.sin_port = _htons(MAIN_WIFI_M2M_SERVER_PORT);
	addr.sin_addr.s_addr = _htonl(MAIN_WIFI_M2M_SERVER_IP);
	bind(tx_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	
	return tx_socket;
	
}

connection_t * connection_find(connection_t * connList,
                               struct sockaddr * addr,
                               size_t addrLen)
{
    connection_t * connP;

    connP = connList;
    while (connP != NULL)
    {
        if ((connP->addrLen == addrLen)
         && (memcmp(&(connP->addr), addr, addrLen) == 0))
        {
            return connP;
        }
        connP = connP->next;
    }

    return connP;
}

connection_t * connection_new_incoming(connection_t * connList,
                                       int sock,
                                       struct sockaddr * addr,
                                       size_t addrLen)
{
    connection_t * connP;

    connP = (connection_t *)malloc(sizeof(connection_t));
    if (connP != NULL)
    {
        connP->sock = sock;
        memcpy(&(connP->addr), addr, addrLen);
        connP->addrLen = addrLen;
        connP->next = connList;
    }

    return connP;
}

connection_t * connection_create(connection_t * connList,
                                 int sock,
                                 char * host,
                                 uint16_t port)
{
	/* Zebra change: Reddy
    char portStr[6];
    struct addrinfo hints;
    struct addrinfo *servinfo = NULL;
    struct addrinfo *p;
    int s;
    struct sockaddr *sa;
    //socklen_t sl;
	uint16 sl;
    connection_t * connP = NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if (0 >= sprintf(portStr, "%hu", port)) return NULL;
    //if (0 != getaddrinfo(host, portStr, &hints, &servinfo) || servinfo == NULL) return NULL;

    // we test the various addresses
    s = -1;
    for(p = servinfo ; p != NULL && s == -1 ; p = p->ai_next)
    {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s >= 0)
        {
            sa = p->ai_addr;
            sl = p->ai_addrlen;
            if (-1 == connect(s, p->ai_addr, p->ai_addrlen))
            {
                close(s);
                s = -1;
            }
        }
    }
    if (s >= 0)
    {
        connP = connection_new_incoming(connList, sock, sa, sl);
        close(s);
    }
    if (NULL != servinfo) {
        free(servinfo);
    }

    return connP;
	*/
	/** Socket for Tx */
	SOCKET tx_socket = -1;
	struct sockaddr_in addr_in;
	int s;
    connection_t * connP = NULL;
	
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = _htons(MAIN_WIFI_M2M_SERVER_PORT);
	addr_in.sin_addr.s_addr = _htonl(MAIN_WIFI_M2M_SERVER_IP);

	/* Create secure socket */
	if (tx_socket < 0) {
		tx_socket = socket(AF_INET, SOCK_DGRAM, 0);
	}

	/* Check if socket was created successfully */
	if (tx_socket == -1) {
		printf("socket error.\r\n");
		close(tx_socket);
		return -1;
	}

	/* If success, connect to socket */
	if (connect(tx_socket, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in)) != SOCK_ERR_NO_ERROR) {
		printf("connect error.\r\n");
		return SOCK_ERR_INVALID;
	}

    connP = connection_new_incoming(connList, sock, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in));
    close(tx_socket);

    return connP;	
	
}



void connection_free(connection_t * connList)
{
    while (connList != NULL)
    {
        connection_t * nextP;

        nextP = connList->next;
        free(connList);

        connList = nextP;
    }
}

int connection_send(connection_t *connP,
                    uint8_t * buffer,
                    size_t length)
{
    int nbSent;
    size_t offset;
/* Zebra change: Reddy
    offset = 0;
    while (offset != length)
    {
        nbSent = sendto(connP->sock, buffer + offset, length - offset, 0, (struct sockaddr *)&(connP->addr), connP->addrLen);
        if (nbSent == -1) return -1;
        offset += nbSent;
    }
*/
	nbSent = sendto(connP->sock, buffer , length , 0, (struct sockaddr *)&(connP->addr), connP->addrLen);	
	if (nbSent == M2M_SUCCESS) {
		printf("connection: message sent\r\n");
		} else {
		printf("connection: failed to send message error!\r\n");
		}	
    return 0;
}


