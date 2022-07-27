/*
* Copyright (c) 2021 Wind River Systems, Inc.
*
* SPDX-License-Identifier: Apache-2.0
*
*/

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <json-c/json.h>
#include <netdb.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include "wrsAlarmMIBServer.h"
#include "wrsAlarmMIBTrap.h"

#define HOST_NAME "localhost"
#define MSG_SIZE 4
#define BACKLOG 1
#define ENABLE 1
#define RETRIES 5
#define RETRIES_MESSAGE_SIZE 20
#define DEFAULT_ERROR -1
#define DEFAULT_SUCCESS 0

struct sServerInfo {
        char address[INET6_ADDRSTRLEN];
        int  address_family;
        int  port;
        int  fd;
        int  retries;
        int  error;
};

/*
Returns a ServerInfo error attribute.
*/
int get_error(ServerInfo server_info){
    assert(server_info != NULL);
    return server_info->error;
}

/*
This procedure attempts to start to listen a Server.
In case of success returns DEFAULT_SUCCESS.
*/
int init_server(ServerInfo server_info){
    assert(server_info != NULL);
    int optval = ENABLE;
    int error = DEFAULT_ERROR;
    int fd = DEFAULT_ERROR;

    snmp_log(LOG_INFO, "server init_server init on address: %s family: %d port:"
        " %d\n", server_info->address, server_info->address_family,
        server_info->port);

    fd = socket(server_info->address_family, SOCK_STREAM, 0);
    if( fd == DEFAULT_ERROR){
        snmp_log(LOG_ERR, "server init_server socket errno: (%d) (%s)\n", errno,
            strerror(errno));
        return DEFAULT_ERROR;
    }
    server_info->fd = fd;

    error = setsockopt(server_info->fd, SOL_SOCKET, SO_REUSEADDR,
        &optval, sizeof optval);
    if( error != DEFAULT_SUCCESS){
        snmp_log(LOG_ERR, "server init_server setsockopt errno: (%d) (%s)\n",
            errno, strerror(errno));
        close(server_info->fd);
        snmp_log(LOG_ERR, "server init_server setsockopt errno: (%d) (%s)\n",
            errno, strerror(errno));
        return DEFAULT_ERROR;
    }

    switch (server_info -> address_family) {
            //When address is IPv4
            case AF_INET:
            {
                struct sockaddr_in addr_4;
                addr_4.sin_family = AF_INET;
                addr_4.sin_port = htons(server_info -> port);
                error = inet_pton(AF_INET, server_info->address,
                    &(addr_4.sin_addr.s_addr));
                if(error <= DEFAULT_SUCCESS){
                    snmp_log(LOG_ERR,
                        "server init_server inet_pton error ip address %s with "
                        "error: %d errno: (%d) (%s)\n",
                        server_info->address, error, errno, strerror(errno));
                    close(server_info->fd);
                    snmp_log(LOG_ERR,
                        "server init_server closing socket errno: (%d) (%s)\n",
                        errno, strerror(errno));
                    return DEFAULT_ERROR;
                }
                error = bind(server_info->fd, (const struct sockaddr *)&addr_4,
                    sizeof(addr_4));
                break;
            }
            //When address is IPv6
            case AF_INET6:
            {
                struct sockaddr_in6 addr_6;
                addr_6.sin6_family = AF_INET6;
                addr_6.sin6_port = htons(server_info->port);
                error = inet_pton(AF_INET6, server_info->address,
                    &(addr_6.sin6_addr));
                if(error <= DEFAULT_SUCCESS){
                    snmp_log(LOG_ERR, "server init_server inet_pton error ip"
                        "address %s with error: %d errno: (%d) (%s)\n",
                        server_info->address, error, errno, strerror(errno));
                    close(server_info->fd);
                    snmp_log(LOG_ERR, "server init_server closing socket errno:"
                        "(%d) (%s)\n", errno, strerror(errno));
                    return DEFAULT_ERROR;
                }
                error = bind(server_info->fd, (const struct sockaddr *)&addr_6,
                    sizeof(addr_6));
                break;
            }
            //Should never get here, needed for completeness
            default:
            {
                snmp_log(LOG_ERR, "server init_server run default state %d\n",
                    server_info -> address_family);
                close(server_info->fd);
                snmp_log(LOG_ERR, "server init_server closing socket errno:"
                    "(%d) (%s)\n", errno, strerror(errno));
                return DEFAULT_ERROR;
            }
        }

        if (error != DEFAULT_SUCCESS) {
            snmp_log(LOG_ERR, "server init_server binding errno: (%d) (%s)\n",
                errno, strerror(errno));
            close(server_info->fd);
            snmp_log(LOG_ERR, "server init_server closing socket errno:"
                "(%d) (%s)\n", errno, strerror(errno));
            return DEFAULT_ERROR;
        }

        snmp_log(LOG_INFO, "server init_server bind success address: %s with "
            "port: %d\n", server_info->address, server_info->port);
        error = listen(server_info->fd, BACKLOG);
        if (error != DEFAULT_SUCCESS) {
            snmp_log(LOG_ERR, "server init_server listen fd:(%d) error, errno:"
            " (%d) (%s)\n", server_info->fd, errno, strerror(errno));
            close(server_info->fd);
            snmp_log(LOG_ERR, "server init_server closing socket errno: "
                "(%d) (%s)\n", errno, strerror(errno));
            return DEFAULT_ERROR;
        }
        snmp_log(LOG_INFO, "server listening fd: %d \n", server_info->fd);
        snmp_log(LOG_INFO, "server init_server success\n");
        return DEFAULT_SUCCESS;
}

/*
This procedure returns a ServerInfo with host_name and port. Also with all
metadata needed to start listening. In case of error returns NULL.
*/
ServerInfo get_server_info(const char * host_name, int port){

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    struct in_addr in4addr_any;
    int error = -1;
    ServerInfo server_info;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    snmp_log(LOG_INFO, "server get_server_info for hostname: %s\n", host_name);
    error = getaddrinfo(host_name, NULL, &hints, &result);
    if (error != 0) {
        snmp_log(LOG_ERR, "server get_server_info getaddrinfo: %s\n",
            gai_strerror(error));
        return NULL;
    }

    server_info = malloc(sizeof(struct sServerInfo));
    if(server_info == NULL){
        snmp_log(LOG_ERR, "server get_server_info error allocation ServerInfo \n");
        freeaddrinfo(result);
        result =  NULL;
        return NULL;
    }

    server_info -> port = port;
    server_info -> error = DEFAULT_SUCCESS;
    server_info -> retries =  RETRIES;

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if(rp->ai_family == AF_INET) {
            in4addr_any.s_addr = INADDR_ANY;
            inet_ntop(AF_INET, &in4addr_any,
            server_info->address, sizeof(server_info->address));
            server_info->address_family = AF_INET;
            snmp_log(LOG_INFO, "server get_server_info found IPv4: %s\n",
                server_info->address);
            break;
        } else if (rp->ai_family == AF_INET6) {
            inet_ntop(AF_INET6, &in6addr_any,
                server_info->address, sizeof(server_info->address));
            server_info->address_family = AF_INET6;
            snmp_log(LOG_INFO, "server get_server_info found IPv6: %s \n",
                server_info->address);
            break;
        }
    }

    if(rp == NULL){
        snmp_log(LOG_ERR, "server get_server_info ip not found at %s\n",
            host_name);
        free(server_info);
        server_info = NULL;
    }

    freeaddrinfo(result);
    result = NULL;
    return server_info;
}

/*
This procedure intents to close the listener socket of the server and free the
memory of ServerInfo. In case of error returns an non-zero value.
*/
int stop_alarm_server(ServerInfo server_info){
    assert(server_info!=NULL);
    int error = close(server_info->fd);
    snmp_log(LOG_ERR, "server stop_alarm_server close errno: (%d) (%s)\n",
        errno, strerror(errno));
    free(server_info);
    return error;
}

/*
This procedures is responsible to listen for new connections.
On error stops listening and return a non-zero value.
*/
int handle_server_messages(ServerInfo server_info){

    int socket_keep_running = 1;
    struct sockaddr cli;
    int connfd = 0;
    int read = 0;
    int bytes_read = 0;
    socklen_t cli_len= sizeof(cli);
    int length_bytes = 0;
    uint32_t length = 0;
    uint32_t nlength = 0;
    int error_msg_len = DEFAULT_SUCCESS;
    int error_processing = DEFAULT_ERROR;
    int partial_retries = 0;
    int terminate = 0;

    // reduce recall tries and check if max retries reached
    server_info->retries = server_info->retries -1;
    if(server_info->retries < 0 ){
        snmp_log(LOG_ERR,"server handle_server_messages handling messages MAX"
            "retries reach\n");
        return DEFAULT_ERROR;
    }

    // start handling messages
    snmp_log(LOG_INFO,"server handle_server_messages start handling messages\n");
    snmp_log(LOG_INFO,"server handle_server_messages retries counter: %d\n",
        server_info->retries);

    while (socket_keep_running) {
        // accepts connections
        connfd = accept(server_info->fd, (struct sockaddr*) &cli, &cli_len);
        if (connfd < DEFAULT_SUCCESS) {
            snmp_log(LOG_ERR,"server handle_server_messages acccept error, errno:"
                "(%d) (%s)\n", errno, strerror(errno));
            continue;
        }
        else{
             snmp_log(LOG_INFO,"server handle_server_messages client socket"
                " accepted %d\n", connfd);
        }

        // message length read
        length_bytes = 0;
        nlength = 0;
        error_msg_len = DEFAULT_SUCCESS;
        partial_retries = 0;
        while (length_bytes < MSG_SIZE) {
            read = recv(connfd, ((char *)&nlength) + length_bytes, MSG_SIZE - length_bytes, 0);
            if (read < DEFAULT_SUCCESS) {
                snmp_log(LOG_ERR,
                    "\nserver handle_server_messages failed to receive message "
                    "length errno: (%d) (%s)\n",
                    errno, strerror(errno));
                error_msg_len = DEFAULT_ERROR;
                break;
            }

            if (partial_retries >  RETRIES_MESSAGE_SIZE) {
                snmp_log(LOG_ERR,"server handle_server_messages message length "
                    "max retries: (%d)\n", partial_retries);
                error_msg_len = DEFAULT_ERROR;
                break;
            }else{
                partial_retries ++;
            }

            length_bytes += read;
        }

        // if error in message len read then recall
        if(error_msg_len != DEFAULT_SUCCESS) {
            snmp_log(LOG_ERR,"server handle_server_messages recall because handle"
                " message len failed\n");
            close(connfd);
            snmp_log(LOG_ERR,"server handle_server_messages port close errno: "
                "(%d) (%s)\n", errno, strerror(errno));
            terminate = handle_server_messages(server_info);
            if(terminate == DEFAULT_ERROR){
                snmp_log(LOG_ERR,"server handle_server_messages recall failed"
                    " with state %d \n", terminate);
                break;
            }
        }

        // message allocation memory
        length = ntohl(nlength);
        char *msg = malloc(length);

        // memory allocation error
        if(msg == NULL) {
            snmp_log(LOG_ERR,"server handle_server_messages recall because "
                "message malloc failed\n");
            close(connfd);
            snmp_log(LOG_ERR,"server handle_server_messages port close errno:"
                " (%d) (%s)\n",
                errno, strerror(errno));
            terminate = handle_server_messages(server_info);
            if(terminate == DEFAULT_ERROR){
                snmp_log(LOG_ERR,"server handle_server_messages recall failed"
                    " with state %d \n", terminate);
                break;
            }
        }

        // message read
        bytes_read = recv(connfd, msg, length, 0);

        // no message arrived
        if (bytes_read < DEFAULT_SUCCESS) {
            free(msg);
            msg = NULL;
            snmp_log(LOG_ERR,"server handle_server_messages recall because"
                " message receive failed\n");
            close(connfd);
            snmp_log(LOG_ERR,"server handle_server_messages port close errno:"
                " (%d) (%s)\n", errno, strerror(errno));
            terminate = handle_server_messages(server_info);
            if(terminate == DEFAULT_ERROR){
                snmp_log(LOG_ERR,"server handle_server_messages recall failed"
                    " with state %d \n", terminate);
                break;
            }
        }

        // error message incomplete
        if (bytes_read != length) {
            free(msg);
            msg = NULL;
            snmp_log(LOG_ERR,
                "server handle_server_messages recall because message bytes_read"
                " != length failed\n");
            close(connfd);
            snmp_log(LOG_ERR,"server handle_server_messages port close errno: "
                "(%d) (%s)\n", errno, strerror(errno));
            terminate = handle_server_messages(server_info);
            if(terminate == DEFAULT_ERROR){
                snmp_log(LOG_ERR,"server handle_server_messages recall failed "
                    "with state %d \n", terminate);
                break;
            }
        }

        // process the message
        error_processing = process_json_alarm(msg);
        if(error_processing != DEFAULT_SUCCESS){
            snmp_log(LOG_ERR,"server handle_server_messages bad message "
                "processing.\n");
        }

        // free resources
        free(msg);
        msg = NULL;
        close(connfd);
        snmp_log(LOG_INFO,"server handle_server_messages port close fd: (%d),"
            " errno: (%d) (%s)\n", connfd, errno, strerror(errno));
        snmp_log(LOG_INFO,"server handle_server_messages message received ok.\n\n");
    }

    // if breaks return error
    snmp_log(LOG_ERR,"server handle_server_messages unespected error\n");
    return DEFAULT_ERROR;
}

/*
This procedure starts the server workflow.
In case of memory allocation failure returns NULL.
In case of communication errors returns a ServerInfo with an error
that can be retrieved by get_error() method.
*/
ServerInfo start_alarm_server (int port){
    ServerInfo server_info = get_server_info(HOST_NAME, port);
    int error = DEFAULT_ERROR;
    if( server_info != NULL){
        error = init_server(server_info);
        if(error == DEFAULT_SUCCESS){
            error = handle_server_messages(server_info);
            if (error != DEFAULT_SUCCESS){
                // unespected server error
                // retry until MAX tries
                server_info -> error = DEFAULT_ERROR;
                snmp_log(LOG_ERR,
                    "server start_alarm_server stopped by error in "
                    "handle_server_messages\n");
            }
        }else{
            // an error during starting up
            server_info -> error = DEFAULT_ERROR;
            snmp_log(LOG_ERR, "server start_alarm_server stopped by error "
                "in init_server\n");
        }
    }else{
        // cannot get host address info
        snmp_log(LOG_ERR, "server start_alarm_server stopped by error in "
            "get_server_info\n");
    }
    return server_info;
}
