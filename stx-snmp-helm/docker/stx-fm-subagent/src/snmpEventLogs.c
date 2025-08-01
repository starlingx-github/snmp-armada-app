/*
* Copyright (c) 2020-2023 Wind River Systems, Inc.
*
* SPDX-License-Identifier: Apache-2.0
*
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <fmDbAPI.h>
#include "snmpAgentPlugin.h"

#define MINLOADFREQ 2     /* min reload frequency in seconds */

netsnmp_feature_require(date_n_time);


static long Event_Log_Count = 0;
static struct activealarm *alarm_list;
static struct activealarm *alarmaddr, savealarm, *savealarmaddr;
static int saveIndex = 0;
static size_t retryCount = 5;
static char saveUuid[36];
static long LastLoad = 0;     /* ET in secs at last table load */
extern long long_return;


int Event_Log_Get_Count(void);

static void
Event_Log_Scan_Init()
{
    struct timeval et;       /* elapsed time */
    struct activealarm  **activealarm_ptr;
    SFmAlarmQueryT aquery;
    size_t i = 0;

    saveIndex = 0;
    netsnmp_get_monotonic_clock(&et);
    if ( et.tv_sec < LastLoad + MINLOADFREQ ) {
        DEBUGMSG(("cgtsAgentPlugin", "Skip reload" ));
        alarmaddr = alarm_list;
        return;
    }
    LastLoad = et.tv_sec;

    /*
     * free old list:
     */
    while (alarm_list) {
        struct activealarm   *old = alarm_list;
        alarm_list = alarm_list->next;
        free(old);
    }
    alarmaddr = 0;
    activealarm_ptr = &alarm_list;

    /*
     * query event log list from DB
     */
    bool isEventObtained = false;
    for (i = 0; i < retryCount; i++){
        if (fm_snmp_util_get_all_event_logs(getAlarmSession(), &aquery)){
            DEBUGMSG(("cgtsAgentPlugin", "get_all_event_logs done\n"));
            isEventObtained = true;
            break;
        } else {
            DEBUGMSG(("cgtsAgentPlugin", 
            "get_all_event_logs returns false (%zu/%ld). Try again\n",
             (i+1), retryCount));
            renewAlarmSession();
        }
    }
    if (!isEventObtained) {
        DEBUGMSG(("cgtsAgentPlugin", "get_all_event_logs from db failed\n"));
        return;
    }
    DEBUGMSG(("cgtsAgentPlugin", "get_all_event_logs returns %lu logs\n", aquery.num));
    for (i = 0; i < aquery.num; ++i){
        struct activealarm   *almnew;
        /*populate alarm_list*/
        almnew = (struct activealarm *) calloc(1, sizeof(struct activealarm));
        if (almnew == NULL)
            break;              /* alloc error */
        *activealarm_ptr = almnew;
        activealarm_ptr = &almnew->next;
        memset(&almnew->alarmdata, 0 , sizeof(almnew->alarmdata));
        memcpy(&almnew->alarmdata, aquery.alarm + i, sizeof(almnew->alarmdata));
    }
    alarmaddr = alarm_list;
    free(aquery.alarm);
}

static int
Event_Log_Scan_NextLog(int *Index,
        char *Name,
        struct activealarm *Aalm)
{
    struct activealarm alm;
    while (alarmaddr) {
        alm = *alarmaddr;
        strlcpy(saveUuid, alm.alarmdata.uuid, sizeof(saveUuid));
        if (Index)
            *Index = ++saveIndex;
        if (Aalm)
            *Aalm = alm;
        if (Name)
            strcpy(Name, saveUuid);

        savealarm = alm;
        savealarmaddr = alarmaddr;
        alarmaddr = alm.next;
        return 1;
    }
    return 0;
}

static int
Event_Log_Scan_By_Index(int Index,
        char *Name,
        struct activealarm *Aalm)
{
    int i;

    DEBUGMSGTL(("cgtsAgentPlugin","Event_Log_Scan_By_Index"));
    Event_Log_Scan_Init();
    while (Event_Log_Scan_NextLog(&i, Name, Aalm)) {
        if (i == Index)
            break;
    }
    if (i != Index)
        return (-1);            /* Error, doesn't exist */
    return (0);                 /* DONE */
}

static int
header_eventLogEntry(struct variable *vp,
               oid * name,
               size_t * length,
               int exact, size_t * var_len,
               WriteMethod ** write_method)
{
#define ALM_ENTRY_NAME_LENGTH    14
    oid             newname[MAX_OID_LEN];
    register int    index;
    int             result, count;

    DEBUGMSGTL(("cgtsAgentPlugin", "header_eventLogEntry: "));
    DEBUGMSGOID(("cgtsAgentPlugin", name, *length));
    DEBUGMSG(("cgtsAgentPlugin", "exact %d\n", exact));

    memcpy((char *) newname, (char *) vp->name,
           (int) vp->namelen * sizeof(oid));
    /*
     * find "next" log
     */
    count = Event_Log_Get_Count();
    DEBUGMSG(("cgtsAgentPlugin", "count %d\n", count));
    for (index = 1; index <= count; index++) {
        newname[ALM_ENTRY_NAME_LENGTH] = (oid) index;
        result =
            snmp_oid_compare(name, *length, newname,
                             (int) vp->namelen + 1);
        if ((exact && (result == 0)) || (!exact && (result < 0)))
            break;
    }
    if (index > count) {
        DEBUGMSGTL(("cgtsAgentPlugin", "... index out of range\n"));
        return MATCH_FAILED;
    }

    memcpy((char *) name, (char *) newname,
           ((int) vp->namelen + 1) * sizeof(oid));
    *length = vp->namelen + 1;
    *write_method = 0;
    *var_len = sizeof(long);    /* default to 'long' results */

    DEBUGMSGTL(("cgtsAgentPlugin", "... get ALM data "));
    DEBUGMSGOID(("cgtsAgentPlugin", name, *length));
    DEBUGMSG(("cgtsAgentPlugin", "\n"));

    DEBUGMSG(("cgtsAgentPlugin","Return index: %d\n", index));
    return index;
}

int
Event_Log_Get_Count(void)
{
    static time_t   scan_time = 0;
    time_t          time_now = time(NULL);

    if (!Event_Log_Count || (time_now > scan_time + 60)) {
        scan_time = time_now;
        Event_Log_Scan_Init();
        Event_Log_Count = 0;
        while (Event_Log_Scan_NextLog(NULL, NULL, NULL) != 0) {
            Event_Log_Count++;
        }
    }
    return (Event_Log_Count);
}

u_char *
var_events(struct variable *vp,
            oid * name,
            size_t * length,
            int exact, size_t * var_len,
            WriteMethod ** write_method)
{
    static struct activealarm alrm;
    static char     Name[36];
    char           *cp;
    int index = 0;

    DEBUGMSGTL(("cgtsAgentPlugin", "var_events"));
    index = header_eventLogEntry(vp, name, length, exact, var_len, write_method);
    if (index == MATCH_FAILED)
        return NULL;

    Event_Log_Scan_By_Index(index, Name, &alrm);

    switch (vp->magic) {
    case EVENT_INDEX:
        long_return = index;
        return (u_char *) & long_return;
    case EVENT_UUID:
        cp = Name;
        *var_len = strlen(cp);
        return (u_char *) cp;
    case EVENT_EVENT_ID:
        cp = alrm.alarmdata.alarm_id;
        *var_len = strlen(cp);
        return (u_char *) cp;
    case EVENT_STATE:
        long_return = alrm.alarmdata.alarm_state;
        return (u_char *) & long_return;
    case EVENT_INSTANCE_ID:
        cp = alrm.alarmdata.entity_instance_id;
        *var_len = strlen(cp);
        return (u_char *) cp;
    case EVENT_TIME:{
        time_t when = alrm.alarmdata.timestamp/SECOND_PER_MICROSECOND;
        cp = (char *) date_n_time(&when, var_len );
        return (u_char *) cp;
    }
    case EVENT_SEVERITY:
        long_return = alrm.alarmdata.severity;
        return (u_char *) & long_return;
    case EVENT_REASONTEXT:
        cp = alrm.alarmdata.reason_text;
        *var_len = strlen(cp);
        return (u_char *) cp;
    case EVENT_EVENTTYPE:
        long_return = alrm.alarmdata.alarm_type;
        return (u_char *) & long_return;
    case EVENT_PROBABLECAUSE:
        long_return = alrm.alarmdata.probable_cause;
        return (u_char *) & long_return;
    case EVENT_REPAIRACTION:
        cp = alrm.alarmdata.proposed_repair_action;
        *var_len = strlen(cp);
        return (u_char *) cp;
    case EVENT_SERVICEAFFECTING:
        long_return = alrm.alarmdata.service_affecting;
        return (u_char *) & long_return;
    case EVENT_SUPPRESSION:
        long_return = alrm.alarmdata.suppression;
        return (u_char *) & long_return;
    default:
        DEBUGMSGTL(("snmpd", "unknown sub-id %d in var_events\n",
                vp->magic));
    }
    return NULL;
}

