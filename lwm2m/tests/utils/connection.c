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
#include "driver/include/m2m_wifi.h"
#include "socket/include/socket.h"
#include "../../src/main.h"
//#include "../../src/asf.h"



int create_socket(const char * portStr)
{
	/* Zebra change: Reddy

	*/
	/** Socket for Tx */
	
	SOCKET tx_socket = -1;
	struct sockaddr_in addr;
	// Initialize socket address structure and bind service.
	addr.sin_family = AF_INET;
	addr.sin_port = _htons(MAIN_WIFI_M2M_SERVER_PORT);
	addr.sin_addr.s_addr = _htonl(MAIN_WIFI_M2M_SERVER_IP);	
			if (tx_socket < 0) {
				if ((tx_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
					printf("main : failed to create RX UDP Client socket error!\r\n");
					//continue;
				}
				printf("main: socket while tx_socket = (%d)\r\n",tx_socket);

				// Socket bind
				bind(tx_socket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
				
			}
	printf("connection: create_socket tx_socket = (%d)\r\n",tx_socket);

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
	*/
	/** Socket for Tx */
	SOCKET tx_socket = -1;
	struct sockaddr_in addr_in;
	int s;
    connection_t * connP = NULL;
	/** IP address of host. */
	uint32_t gu32HostIp = 0;
	
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = _htons(MAIN_WIFI_M2M_SERVER_PORT);
	addr_in.sin_addr.s_addr = _htonl(MAIN_WIFI_M2M_SERVER_IP);

	printf("connection: _create and connect sock = (%d)\r\n",sock);
	if (connect(sock, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in)) != SOCK_ERR_NO_ERROR) {
			printf("connect error.\r\n");
			return SOCK_ERR_INVALID;
	}
	/* Handle pending events from network controller. */
	m2m_wifi_handle_events(NULL);

    connP = connection_new_incoming(connList, sock, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in));
    close(tx_socket);
	
	//printf("connection: _create and connect tx_socket = (%d)\r\n",rx_socket);

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
*/
	printf("connection: _send socket = (%d), buffer = (%s), length(%d), addrlen (%d)\r\n",connP->sock, buffer, length, connP->addrLen);
	nbSent = sendto(connP->sock, buffer , length , 0, (struct sockaddr *)&(connP->addr), connP->addrLen);	
	if (nbSent == M2M_SUCCESS) {
		printf("connection: message sent\r\n");
		} else {
		printf("connection: failed to send message error!\r\n");
		}	
		
	/* Handle pending events from network controller. */
	m2m_wifi_handle_events(NULL);	
		
    return 0;
}


