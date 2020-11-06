/*
* Copyright (c) 2020 Wind River Systems, Inc.
*
* SPDX-License-Identifier: Apache-2.0
*
*/

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <json-c/json.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>

#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "wrsAlarmMIBTrap.h"

#define MSG_SIZE 4
#define TCP_SOCKET_PORT 162
#define ALARM_CRITICAL "wrsAlarmCritical"
#define ALARM_MAJOR "wrsAlarmMajor"
#define ALARM_MINOR "wrsAlarmMinor"
#define ALARM_WARNING "wrsAlarmWarning"
#define ALARM_MSG "wrsAlarmMessage"
#define ALARM_CLEAR "wrsAlarmClear"
#define ALARM_HIERARCHICAL_CLEAR "wrsAlarmHierarchicalClear"
#define WARM_START "warmStart"

#define SA struct sockaddr

/*
 * If compiling within the net-snmp source code, this will trigger the feature
 * detection mechansim to ensure the agent_check_and_process() function
 * is left available even if --enable-minimialist is turned on.  If you
 * have configured net-snmp using --enable-minimialist and want to compile
 * this code externally to the Net-SNMP code base, then please add
 * --with-features="agent_check_and_process enable_stderrlog" to your
 * configure line.
 */
netsnmp_feature_require(agent_check_and_process)
    netsnmp_feature_require(enable_stderrlog)

        static int keep_running;

static RETSIGTYPE
stop_server(int a)
{
  keep_running = 0;
}

static void usage(void)
{
  printf("usage: wrsAlarmMIB [-D<tokens>] [-f] [-L] [-M] [-H] [LISTENING ADDRESSES]\n"
         "\t-f      Do not fork() from the calling shell.\n"
         "\t-DTOKEN[,TOKEN,...]\n"
         "\t\tTurn on debugging output for the given TOKEN(s).\n"
         "\t\tWithout any tokens specified, it defaults to printing\n"
         "\t\tall the tokens (which is equivalent to the keyword 'ALL').\n"
         "\t\tYou might want to try ALL for extremely verbose output.\n"
         "\t\tNote: You can't put a space between the -D and the TOKENs.\n"
         "\t-H\tDisplay a list of configuration file directives\n"
         "\t\tunderstood by the agent and then exit.\n"
         "\t-M\tRun as a normal SNMP Agent instead of an AgentX sub-agent.\n"
         "\t-x ADDRESS\tconnect to master agent at ADDRESS (default /var/agentx/master).\n"
         "\t-L\tDo not open a log file; print all messages to stderr.\n");
  exit(0);
}
void initSocketServer(void)
{
  int sockfd, connfd, read, bytes_read;
  int enable = 1;
  int socket_keep_running = 1;
  socklen_t len;
  struct sockaddr_in servaddr, cli;
  const int backlog = 1;

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd == -1) {
    snmp_log(LOG_ERR,"Socket creation failed\n");
    exit(-1);
  }
  else
    snmp_log(LOG_INFO,"Socket successfully created\n");

  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(TCP_SOCKET_PORT);

  // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0) {
    close(sockfd);
    snmp_log(LOG_ERR,"Socket bind failed\n");
    exit(-1);
  }
  else
    snmp_log(LOG_INFO,"Socket successfully binded\n");

  if ((listen(sockfd, backlog)) != 0) {
    snmp_log(LOG_ERR,"Failed to start server socket listen.\n");
    close(sockfd);
    exit(-1);
  }
  else {
    snmp_log(LOG_INFO,"Server listening\n");
  }

  len = sizeof(cli);

  while (socket_keep_running) {
  connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd < 0) {
      snmp_log(LOG_ERR,"Server acccept failed\n");
      close(sockfd);
      exit(-1);
    }
    else
      snmp_log(LOG_INFO,"Client socket accepted\n");

    int length_bytes = 0;
    uint32_t length, nlength;

    while (length_bytes < MSG_SIZE) {
      read = recv(connfd, ((char *)&nlength) + length_bytes, MSG_SIZE - length_bytes, 0);
      if (read == -1) {
        snmp_log(LOG_ERR,"Failed to receive message.\n");
      }
      length_bytes += read;
    }

    length = ntohl(nlength);
    char *msg = malloc(length);
    bytes_read = recv(connfd, msg, length, 0);

    if (bytes_read == -1) {
      snmp_log(LOG_ERR,"Failed to receive message.\n");
    }

    if (bytes_read != length) {
      snmp_log(LOG_ERR,"recv: Premature EOF.\n");
    }

    struct json_object *parsed_json = json_tokener_parse(msg);
    struct json_object *operation_type;
    json_object_object_get_ex(parsed_json, "operation_type", &operation_type);
    const char *opt_type = json_object_get_string(operation_type);

    if (strcmp(ALARM_CLEAR, opt_type) == 0) {

      send_wrsAlarmClear_trap(parsed_json);
    }
    else if (strcmp(ALARM_HIERARCHICAL_CLEAR, opt_type) == 0) {

      send_wrsAlarmHierarchicalClear_trap(parsed_json);
    }
    else if (strcmp(ALARM_MSG, opt_type) == 0) {

      send_wrsEventMessage_trap(parsed_json);
    }
    else if (strcmp(ALARM_WARNING, opt_type) == 0) {

      send_wrsAlarmWarning_trap(parsed_json);
    }
    else if (strcmp(ALARM_MINOR, opt_type) == 0) {

      send_wrsAlarmMinor_trap(parsed_json);
    }
    else if (strcmp(ALARM_MAJOR, opt_type) == 0) {

      send_wrsAlarmMajor_trap(parsed_json);
    }
    else if (strcmp(ALARM_CRITICAL, opt_type) == 0) {

      send_wrsAlarmCritical_trap(parsed_json);
    }
    else if (strcmp(WARM_START, opt_type) == 0) {

      send_wrsWarmStart_trap();
    }
    else {
      send_wrsAlarmMessage_trap(parsed_json);
    }
    json_object_put(parsed_json);
    free(msg);
  }
  close(sockfd);
}

int main(int argc, char **argv)
{
  int agentx_subagent = 1; /* change this if you want to be a SNMP master agent */
  /* Defs for arg-handling code: handles setting of policy-related variables */
  int ch;
  extern char *optarg;
  int dont_fork = 0, use_syslog = 0;
  char *agentx_socket = NULL;

  while ((ch = getopt(argc, argv, "D:fHLM-:x:")) != EOF)
    switch (ch) {
    case '-':
      if (strcasecmp(optarg, "help") == 0) {
        usage();
      }
      handle_long_opt(optarg);
      break;
    case 'D':
      debug_register_tokens(optarg);
      snmp_set_do_debugging(1);
      break;
    case 'f':
      dont_fork = 1;
      break;
    case 'H':
      netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
                             NETSNMP_DS_AGENT_NO_ROOT_ACCESS, 1);
      init_agent("wrsAlarmMIB");
      init_snmp("wrsAlarmMIB");
      fprintf(stderr, "Configuration directives understood:\n");
      read_config_print_usage("  ");
      exit(0);
    case 'M':
      agentx_subagent = 0;
      break;
    case 'L':
      use_syslog = 0; /* use stderr */
      break;
    case 'x':
      agentx_socket = optarg;
      break;
    default:
      fprintf(stderr, "unknown option %c\n", ch);
      usage();
    }

  if (optind < argc) {
    int i;
    /*
       * There are optional transport addresses on the command line.
       */
    DEBUGMSGTL(("snmpd/main", "optind %d, argc %d\n", optind, argc));
    for (i = optind; i < argc; i++) {
      char *c, *astring;
      if ((c = netsnmp_ds_get_string(NETSNMP_DS_APPLICATION_ID,
                                     NETSNMP_DS_AGENT_PORTS))) {
        astring = malloc(strlen(c) + 2 + strlen(argv[i]));
        if (astring == NULL) {
          fprintf(stderr, "malloc failure processing argv[%d]\n", i);
          exit(1);
        }
        sprintf(astring, "%s,%s", c, argv[i]);
        netsnmp_ds_set_string(NETSNMP_DS_APPLICATION_ID,
                              NETSNMP_DS_AGENT_PORTS, astring);
        SNMP_FREE(astring);
      }
      else {
        netsnmp_ds_set_string(NETSNMP_DS_APPLICATION_ID,
                              NETSNMP_DS_AGENT_PORTS, argv[i]);
      }
    }
    DEBUGMSGTL(("snmpd/main", "port spec: %s\n",
                netsnmp_ds_get_string(NETSNMP_DS_APPLICATION_ID,
                                      NETSNMP_DS_AGENT_PORTS)));
  }

  /* we're an agentx subagent? */
  if (agentx_subagent) {
    /* make us a agentx client. */
    netsnmp_enable_subagent();
    if (NULL != agentx_socket)
      netsnmp_ds_set_string(NETSNMP_DS_APPLICATION_ID,
                            NETSNMP_DS_AGENT_X_SOCKET, agentx_socket);
  }

  snmp_disable_log();
  if (use_syslog)
    snmp_enable_calllog();
  else
    snmp_enable_stderrlog();

  /* daemonize */
  if (!dont_fork) {
    int rc = netsnmp_daemonize(1, !use_syslog);
    if (rc)
      exit(-1);
  }

  /* initialize tcp/ip if necessary */
  SOCK_STARTUP;

  /* initialize the agent library */
  init_agent("wrsAlarmMIB");

  /* read wrsAlarmMIB.conf files. */
  init_snmp("wrsAlarmMIB");

  /* If we're going to be a snmp master agent, initial the ports */
  if (!agentx_subagent)
    init_master_agent(); /* open the port to listen on (defaults to udp:161) */

  /* In case we recevie a request to stop (kill -TERM or kill -INT) */
  keep_running = 1;
  signal(SIGTERM, stop_server);
  signal(SIGINT, stop_server);
  initSocketServer();
  while (keep_running) {
    /* if you use select(), see snmp_select_info() in snmp_api(3) */
    /*     --- OR ---  */
    agent_check_and_process(1); /* 0 == don't block */
  }

  /* at shutdown time */
  snmp_shutdown("wrsAlarmMIB");
  SOCK_CLEANUP;
  exit(0);
}
