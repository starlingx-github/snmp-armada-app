/*
* Copyright (c) 2021 Wind River Systems, Inc.
*
* SPDX-License-Identifier: Apache-2.0
*
*/

#ifndef WRSALARMMIBSERVER_H
#define WRSALARMMIBSERVER_H

typedef struct sServerInfo * ServerInfo;


/*
Description: start listening in an specified port.
Parameters: an integer port to start listening
Return value: On error a NULL value if a memory allocation error occurs.
              On success a ServerInfo with the runtime information.
*/
ServerInfo start_alarm_server (int port);


/*
Description: stop listening for connections and free memory structures.
Parameters: a ServerInfo not null value.
Return value: a non-zero value on close port error.
*/
int stop_alarm_server(ServerInfo server_info);


/*
Description: retrieve the ServerInfo status.
Parameters: a ServerInfo not null value .
Return value: retrieves non-zero value indicating error status.
*/
int get_error(ServerInfo server_info);


#endif /* WRSALARMMIBSERVER_H */