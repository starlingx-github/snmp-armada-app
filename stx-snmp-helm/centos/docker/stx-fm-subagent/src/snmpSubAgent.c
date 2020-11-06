/*
* Copyright (c) 2020 Wind River Systems, Inc.
*
* SPDX-License-Identifier: Apache-2.0
*
*/

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

/* include our parent header */
#include "snmpAgentPlugin.h"

#include <signal.h>

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
stop_server(int a) {
    keep_running = 0;
}

static void usage(void)
{
   printf("usage: wrsAlarmActiveTable [-D<tokens>] [-f] [-L] [-M] [-H] [LISTENING ADDRESSES]\n"
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

int
main (int argc, char **argv)
{
  int agentx_subagent=1; /* change this if you want to be a SNMP master agent */
  /* Defs for arg-handling code: handles setting of policy-related variables */
  int          ch;
  extern char *optarg;
  int dont_fork = 0, use_syslog = 0;
  char *agentx_socket = NULL;

  while ((ch = getopt(argc, argv, "D:fHLM-:x:")) != EOF)
    switch(ch)
    {
    case '-':
        if (strcasecmp(optarg, "help") == 0)
        {
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
      init_agent("wrsAlarmActiveTable");        /* register our .conf handlers */
      init_snmpAgentPlugin();
      init_snmp("wrsAlarmActiveTable");
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
      fprintf(stderr,"unknown option %c\n", ch);
      usage();
  }

  if (optind < argc)
  {
      int i;
      /*
       * There are optional transport addresses on the command line.
       */
      DEBUGMSGTL(("snmpd/main", "optind %d, argc %d\n", optind, argc));
      for (i = optind; i < argc; i++)
      {
          char *c, *astring;
          if ((c = netsnmp_ds_get_string(NETSNMP_DS_APPLICATION_ID,
                                         NETSNMP_DS_AGENT_PORTS)))
          {
              astring = (char*) malloc(strlen(c) + 2 + strlen(argv[i]));
              if (astring == NULL)
              {
                  fprintf(stderr, "malloc failure processing argv[%d]\n", i);
                  exit(1);
              }
              sprintf(astring, "%s,%s", c, argv[i]);
              netsnmp_ds_set_string(NETSNMP_DS_APPLICATION_ID,
                                    NETSNMP_DS_AGENT_PORTS, astring);
              SNMP_FREE(astring);
          }
          else
          {
              netsnmp_ds_set_string(NETSNMP_DS_APPLICATION_ID,
                                    NETSNMP_DS_AGENT_PORTS, argv[i]);
          }
      }
      DEBUGMSGTL(("snmpd/main", "port spec: %s\n",
                  netsnmp_ds_get_string(NETSNMP_DS_APPLICATION_ID,
                                        NETSNMP_DS_AGENT_PORTS)));
  }

  /* we're an agentx subagent? */
  if (agentx_subagent)
  {
    snmp_log(LOG_INFO,"Make us a agentx client.  \r\n");
    /* make us a agentx client. */
    netsnmp_enable_subagent();
    if (NULL != agentx_socket)
    {
        netsnmp_ds_set_string(NETSNMP_DS_APPLICATION_ID,
                              NETSNMP_DS_AGENT_X_SOCKET, agentx_socket);
    }
  }

  snmp_disable_log();
  if (use_syslog)
      snmp_enable_calllog();
  else
      snmp_enable_stderrlog();

  /* daemonize */
  if(!dont_fork)
  {
    int rc = netsnmp_daemonize(1,!use_syslog);
    if(rc)
       exit(-1);
  }

  /* initialize tcp/ip if necessary */
  SOCK_STARTUP;

  snmp_log(LOG_INFO,"initialize the agent library  \r\n");
  /* initialize the agent library */
  init_agent("wrsAlarmActiveTable");

  snmp_log(LOG_INFO,"init wrsAlarmActiveTable mib code  \r\n");
  /* init wrsAlarmActiveTable mib code */
  init_snmpAgentPlugin();

  snmp_log(LOG_INFO,"Read wrsAlarmActiveTable.conf files\r\n");
  /* read wrsAlarmActiveTable.conf files. */
  init_snmp("wrsAlarmActiveTable");

  /* If we're going to be a snmp master agent, initial the ports */
  if (!agentx_subagent)
    init_master_agent();  /* open the port to listen on (defaults to udp:161) */

  /* In case we recevie a request to stop (kill -TERM or kill -INT) */
  keep_running = 1;
  signal(SIGTERM, stop_server);
  signal(SIGINT, stop_server);
  snmp_log(LOG_INFO,"Sub Agent Initialized\r\n");

  /* you're main loop here... */
  while(keep_running)
  {
    /* if you use select(), see snmp_select_info() in snmp_api(3) */
    /*     --- OR ---  */
    agent_check_and_process(1); /* 0 == don't block */
  }

  /* at shutdown time */
  snmp_shutdown("wrsAlarmActiveTable");
  deinit_snmpAgentPlugin();
  SOCK_CLEANUP;

  exit(0);
}

