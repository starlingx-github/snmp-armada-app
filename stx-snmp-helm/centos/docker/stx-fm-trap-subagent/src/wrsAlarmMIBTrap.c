/*
* Copyright (c) 2020-2021 Wind River Systems, Inc.
*
* SPDX-License-Identifier: Apache-2.0
*
*/

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include "wrsAlarmMIBTrap.h"

#define ALARM_ID "wrsAlarmActiveAlarmId"
#define ALARM_INSTANCE_ID "wrsAlarmActiveEntityInstanceId"
#define ALARM_DATE_TIME "wrsAlarmActiveDateAndTime"
#define ALARM_SEVERITY "wrsAlarmActiveAlarmSeverity"
#define ALARM_REASON_TEXT "wrsAlarmActiveReasonText"
#define ALARM_EVENT_TYPE "wrsAlarmActiveEventType"
#define ALARM_CAUSE "wrsAlarmActiveProbableCause"
#define ALARM_REPAIR_ACTION "wrsAlarmActiveProposedRepairAction"
#define ALARM_SERVICE_AFFECTING "wrsAlarmActiveServiceAffecting"
#define ALARM_SUPPRESSION "wrsAlarmActiveSuppressionAllowed"

#define EVENT_ID "wrsEventEventId"
#define EVENT_INSTANCE_ID "wrsEventEntityInstanceId"
#define EVENT_DATE_TIME "wrsEventDateAndTime"
#define EVENT_SEVERITY "wrsEventSeverity"
#define EVENT_REASON_TEXT "wrsEventReasonText"
#define EVENT_EVENT_TYPE "wrsEventEventType"
#define EVENT_CAUSE "wrsEventProbableCause"
#define EVENT_SERVICE_AFFECTING "wrsEventServiceAffecting"

#define ALARM_CRITICAL "wrsAlarmCritical"
#define ALARM_MAJOR "wrsAlarmMajor"
#define ALARM_MINOR "wrsAlarmMinor"
#define ALARM_WARNING "wrsAlarmWarning"
#define ALARM_MSG "wrsAlarmMessage"
#define ALARM_CLEAR "wrsAlarmClear"
#define ALARM_HIERARCHICAL_CLEAR "wrsAlarmHierarchicalClear"
#define WARM_START "warmStart"

extern const oid snmptrap_oid[];
extern const size_t snmptrap_oid_len;

int
send_wrsAlarmCritical_trap( struct json_object *jobj )
{
    snmp_log(LOG_INFO,"Init send_wrsAlarmCritical_trap\n");
    int exists;
    struct json_object *alarmObj;
    netsnmp_variable_list  *var_list = NULL;
    const oid wrsAlarmCritical_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,0,1 };
    const oid wrsAlarmActiveAlarmId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,3,0};
    const oid wrsAlarmActiveEntityInstanceId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,4,0};
    const oid wrsAlarmActiveDateAndTime_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,5,0};
    const oid wrsAlarmActiveAlarmSeverity_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,6,0};
    const oid wrsAlarmActiveReasonText_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,7,0};
    const oid wrsAlarmActiveEventType_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,8,0};
    const oid wrsAlarmActiveProbableCause_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,9,0};
    const oid wrsAlarmActiveProposedRepairAction_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,10,0};
    const oid wrsAlarmActiveServiceAffecting_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,11,0};
    const oid wrsAlarmActiveSuppressionAllowed_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,12,0};

    exists=json_object_object_get_ex(jobj,"alarm",&alarmObj);
    if(exists==0) {
        snmp_log(LOG_ERR,"Key alarm not found in JSON");
        return SNMP_ERR_BADVALUE;
    }
    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        wrsAlarmCritical_oid, sizeof(wrsAlarmCritical_oid));


    struct json_object *alarmObjField;
    if(json_object_object_get_ex(alarmObj,ALARM_ID,&alarmObjField)) {
        const char *alarmId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveAlarmId_oid, OID_LENGTH(wrsAlarmActiveAlarmId_oid),
            ASN_OCTET_STR,
            alarmId,strlen(alarmId));

    }

    if(json_object_object_get_ex(alarmObj,ALARM_INSTANCE_ID,&alarmObjField)) {
        const char *alarmInstanceId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveEntityInstanceId_oid, OID_LENGTH(wrsAlarmActiveEntityInstanceId_oid),
            ASN_OCTET_STR,
            alarmInstanceId,
            strlen(alarmInstanceId));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_DATE_TIME,&alarmObjField)) {
        const char *alarmDateTime = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveDateAndTime_oid),
            ASN_OCTET_STR,
            alarmDateTime,strlen(alarmDateTime));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SEVERITY,&alarmObjField)) {
        int alarmSeverity = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveAlarmSeverity_oid, OID_LENGTH(wrsAlarmActiveAlarmSeverity_oid),
            ASN_INTEGER,
            &alarmSeverity,sizeof(alarmSeverity));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_REASON_TEXT,&alarmObjField)) {
        const char *alarmReasonText = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveReasonText_oid),
            ASN_OCTET_STR,
            alarmReasonText,strlen(alarmReasonText));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_EVENT_TYPE,&alarmObjField)) {
        int eventType = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveEventType_oid, OID_LENGTH(wrsAlarmActiveEventType_oid),
            ASN_INTEGER,
            &eventType,sizeof(eventType));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_CAUSE,&alarmObjField)) {
        int probableCause = json_object_get_int(json_object_object_get(alarmObj,ALARM_CAUSE));
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveProbableCause_oid, OID_LENGTH(wrsAlarmActiveProbableCause_oid),
            ASN_INTEGER,
            &probableCause,sizeof(probableCause));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_REPAIR_ACTION,&alarmObjField)) {
        const char *proposedRepairAction = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveProposedRepairAction_oid, OID_LENGTH(wrsAlarmActiveProposedRepairAction_oid),
            ASN_OCTET_STR,
            proposedRepairAction,strlen(proposedRepairAction));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SERVICE_AFFECTING,&alarmObjField)) {
        int serviceAffecting = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveServiceAffecting_oid, OID_LENGTH(wrsAlarmActiveServiceAffecting_oid),
            ASN_INTEGER,
            &serviceAffecting,sizeof(serviceAffecting));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SUPPRESSION,&alarmObjField)) {
        int activeSuppressionAllowed = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveSuppressionAllowed_oid, OID_LENGTH(wrsAlarmActiveSuppressionAllowed_oid),
            ASN_INTEGER,
            &activeSuppressionAllowed,sizeof(activeSuppressionAllowed));
    }

    snmp_log(LOG_INFO,"wrsAlarmCritical trap sent\n");
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_wrsAlarmMajor_trap( struct json_object *jobj )
{
    snmp_log(LOG_INFO,"Init send_wrsAlarmMajor_trap\n");
    int exists;
    struct json_object *alarmObj;
    netsnmp_variable_list  *var_list = NULL;
    const oid wrsAlarmMajor_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,0,2 };
    const oid wrsAlarmActiveAlarmId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,3,0};
    const oid wrsAlarmActiveEntityInstanceId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,4,0};
    const oid wrsAlarmActiveDateAndTime_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,5,0};
    const oid wrsAlarmActiveAlarmSeverity_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,6,0};
    const oid wrsAlarmActiveReasonText_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,7,0};
    const oid wrsAlarmActiveEventType_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,8,0};
    const oid wrsAlarmActiveProbableCause_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,9,0};
    const oid wrsAlarmActiveProposedRepairAction_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,10,0};
    const oid wrsAlarmActiveServiceAffecting_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,11,0};
    const oid wrsAlarmActiveSuppressionAllowed_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,12,0};

    exists=json_object_object_get_ex(jobj,"alarm",&alarmObj);
    if(exists==0) {
        snmp_log(LOG_ERR,"Key alarm not found in JSON");
        return SNMP_ERR_BADVALUE;
    }
    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        wrsAlarmMajor_oid, sizeof(wrsAlarmMajor_oid));


    struct json_object *alarmObjField;
    if(json_object_object_get_ex(alarmObj,ALARM_ID,&alarmObjField)) {
        const char *alarmId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveAlarmId_oid, OID_LENGTH(wrsAlarmActiveAlarmId_oid),
            ASN_OCTET_STR,
            alarmId,strlen(alarmId));

    }
    if(json_object_object_get_ex(alarmObj,ALARM_INSTANCE_ID,&alarmObjField)) {
        const char *alarmInstanceId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveEntityInstanceId_oid, OID_LENGTH(wrsAlarmActiveEntityInstanceId_oid),
            ASN_OCTET_STR,
            alarmInstanceId,
            strlen(alarmInstanceId));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_DATE_TIME,&alarmObjField)) {
        const char *alarmDateTime = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveDateAndTime_oid),
            ASN_OCTET_STR,
            alarmDateTime,strlen(alarmDateTime));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SEVERITY,&alarmObjField)) {
        int alarmSeverity = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveAlarmSeverity_oid, OID_LENGTH(wrsAlarmActiveAlarmSeverity_oid),
            ASN_INTEGER,
            &alarmSeverity,sizeof(alarmSeverity));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_REASON_TEXT,&alarmObjField)) {
        const char *alarmReasonText = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveReasonText_oid),
            ASN_OCTET_STR,
            alarmReasonText,strlen(alarmReasonText));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_EVENT_TYPE,&alarmObjField)) {
        int eventType = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveEventType_oid, OID_LENGTH(wrsAlarmActiveEventType_oid),
            ASN_INTEGER,
            &eventType,sizeof(eventType));
    }
    
    if(json_object_object_get_ex(alarmObj,ALARM_CAUSE,&alarmObjField)) {
        int probableCause = json_object_get_int(json_object_object_get(alarmObj,ALARM_CAUSE));
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveProbableCause_oid, OID_LENGTH(wrsAlarmActiveProbableCause_oid),
            ASN_INTEGER,
            &probableCause,sizeof(probableCause));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_REPAIR_ACTION,&alarmObjField)) {
        const char *proposedRepairAction = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveProposedRepairAction_oid, OID_LENGTH(wrsAlarmActiveProposedRepairAction_oid),
            ASN_OCTET_STR,
            proposedRepairAction,strlen(proposedRepairAction));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SERVICE_AFFECTING,&alarmObjField)) {
        int serviceAffecting = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveServiceAffecting_oid, OID_LENGTH(wrsAlarmActiveServiceAffecting_oid),
            ASN_INTEGER,
            &serviceAffecting,sizeof(serviceAffecting));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SUPPRESSION,&alarmObjField)) {
        int activeSuppressionAllowed = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveSuppressionAllowed_oid, OID_LENGTH(wrsAlarmActiveSuppressionAllowed_oid),
            ASN_INTEGER,
            &activeSuppressionAllowed,sizeof(activeSuppressionAllowed));
    }

    snmp_log(LOG_INFO,"wrsAlarmMajor trap sent\n");
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_wrsAlarmMinor_trap( struct json_object *jobj )
{
    snmp_log(LOG_INFO,"Init send_wrsAlarmMinor_trap\n");
    int exists;
    struct json_object *alarmObj;
    netsnmp_variable_list  *var_list = NULL;
    const oid wrsAlarmMinor_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,0,3 };
    const oid wrsAlarmActiveAlarmId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,3,0};
    const oid wrsAlarmActiveEntityInstanceId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,4,0};
    const oid wrsAlarmActiveDateAndTime_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,5,0};
    const oid wrsAlarmActiveAlarmSeverity_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,6,0};
    const oid wrsAlarmActiveReasonText_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,7,0};
    const oid wrsAlarmActiveEventType_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,8,0};
    const oid wrsAlarmActiveProbableCause_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,9,0};
    const oid wrsAlarmActiveProposedRepairAction_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,10,0};
    const oid wrsAlarmActiveServiceAffecting_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,11,0};
    const oid wrsAlarmActiveSuppressionAllowed_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,12,0};

    exists=json_object_object_get_ex(jobj,"alarm",&alarmObj);
    if(exists==0) {
        snmp_log(LOG_ERR,"Key alarm not found in JSON");
        return SNMP_ERR_BADVALUE;
    }
    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        wrsAlarmMinor_oid, sizeof(wrsAlarmMinor_oid));


    struct json_object *alarmObjField;
    if(json_object_object_get_ex(alarmObj,ALARM_ID,&alarmObjField)) {
        const char *alarmId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveAlarmId_oid, OID_LENGTH(wrsAlarmActiveAlarmId_oid),
            ASN_OCTET_STR,
            alarmId,strlen(alarmId));

    }

    if(json_object_object_get_ex(alarmObj,ALARM_INSTANCE_ID,&alarmObjField)) {
        const char *alarmInstanceId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveEntityInstanceId_oid, OID_LENGTH(wrsAlarmActiveEntityInstanceId_oid),
            ASN_OCTET_STR,
            alarmInstanceId,
            strlen(alarmInstanceId));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_DATE_TIME,&alarmObjField)) {
        const char *alarmDateTime = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveDateAndTime_oid),
            ASN_OCTET_STR,
            alarmDateTime,strlen(alarmDateTime));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SEVERITY,&alarmObjField)) {
        int alarmSeverity = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveAlarmSeverity_oid, OID_LENGTH(wrsAlarmActiveAlarmSeverity_oid),
            ASN_INTEGER,
            &alarmSeverity,sizeof(alarmSeverity));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_REASON_TEXT,&alarmObjField)) {
        const char *alarmReasonText = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveReasonText_oid),
            ASN_OCTET_STR,
            alarmReasonText,strlen(alarmReasonText));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_EVENT_TYPE,&alarmObjField)) {
        int eventType = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveEventType_oid, OID_LENGTH(wrsAlarmActiveEventType_oid),
            ASN_INTEGER,
            &eventType,sizeof(eventType));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_CAUSE,&alarmObjField)) {
        int probableCause = json_object_get_int(json_object_object_get(alarmObj,ALARM_CAUSE));
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveProbableCause_oid, OID_LENGTH(wrsAlarmActiveProbableCause_oid),
            ASN_INTEGER,
            &probableCause,sizeof(probableCause));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_REPAIR_ACTION,&alarmObjField)) {
        const char *proposedRepairAction = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveProposedRepairAction_oid, OID_LENGTH(wrsAlarmActiveProposedRepairAction_oid),
            ASN_OCTET_STR,
            proposedRepairAction,strlen(proposedRepairAction));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SERVICE_AFFECTING,&alarmObjField)) {
        int serviceAffecting = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveServiceAffecting_oid, OID_LENGTH(wrsAlarmActiveServiceAffecting_oid),
            ASN_INTEGER,
            &serviceAffecting,sizeof(serviceAffecting));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SUPPRESSION,&alarmObjField)) {
        int activeSuppressionAllowed = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveSuppressionAllowed_oid, OID_LENGTH(wrsAlarmActiveSuppressionAllowed_oid),
            ASN_INTEGER,
            &activeSuppressionAllowed,sizeof(activeSuppressionAllowed));
    }

    snmp_log(LOG_INFO,"wrsAlarmMinor trap sent\n");
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_wrsAlarmWarning_trap( struct json_object *jobj )
{
    snmp_log(LOG_INFO,"Init send_wrsAlarmWarning_trap\n");
    int exists;
    struct json_object *alarmObj;
    netsnmp_variable_list  *var_list = NULL;
    const oid wrsAlarmWarning_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,0,4 };
    const oid wrsAlarmActiveAlarmId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,3,0 };
    const oid wrsAlarmActiveEntityInstanceId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,4,0};
    const oid wrsAlarmActiveDateAndTime_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,5,0};
    const oid wrsAlarmActiveAlarmSeverity_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,6,0};
    const oid wrsAlarmActiveReasonText_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,7,0};
    const oid wrsAlarmActiveEventType_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,8,0};
    const oid wrsAlarmActiveProbableCause_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,9,0};
    const oid wrsAlarmActiveProposedRepairAction_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,10,0};
    const oid wrsAlarmActiveServiceAffecting_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,11,0};
    const oid wrsAlarmActiveSuppressionAllowed_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,12,0};

    exists=json_object_object_get_ex(jobj,"alarm",&alarmObj);
    if(exists==0) {
        snmp_log(LOG_ERR,"Key alarm not found in JSON");
        return SNMP_ERR_BADVALUE;
    }
    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        wrsAlarmWarning_oid, sizeof(wrsAlarmWarning_oid));


    struct json_object *alarmObjField;
    if(json_object_object_get_ex(alarmObj,ALARM_ID,&alarmObjField)) {
        const char *alarmId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveAlarmId_oid, OID_LENGTH(wrsAlarmActiveAlarmId_oid),
            ASN_OCTET_STR,
            alarmId,strlen(alarmId));

    }

    if(json_object_object_get_ex(alarmObj,ALARM_INSTANCE_ID,&alarmObjField)) {
        const char *alarmInstanceId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveEntityInstanceId_oid, OID_LENGTH(wrsAlarmActiveEntityInstanceId_oid),
            ASN_OCTET_STR,
            alarmInstanceId,
            strlen(alarmInstanceId));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_DATE_TIME,&alarmObjField)) {
        const char *alarmDateTime = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveDateAndTime_oid),
            ASN_OCTET_STR,
            alarmDateTime,strlen(alarmDateTime));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SEVERITY,&alarmObjField)) {
        int alarmSeverity = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveAlarmSeverity_oid, OID_LENGTH(wrsAlarmActiveAlarmSeverity_oid),
            ASN_INTEGER,
            &alarmSeverity,sizeof(alarmSeverity));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_REASON_TEXT,&alarmObjField)) {
        const char *alarmReasonText = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveReasonText_oid),
            ASN_OCTET_STR,
            alarmReasonText,strlen(alarmReasonText));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_EVENT_TYPE,&alarmObjField)) {
        int eventType = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveEventType_oid, OID_LENGTH(wrsAlarmActiveEventType_oid),
            ASN_INTEGER,
            &eventType,sizeof(eventType));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_CAUSE,&alarmObjField)) {
        int probableCause = json_object_get_int(json_object_object_get(alarmObj,ALARM_CAUSE));
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveProbableCause_oid, OID_LENGTH(wrsAlarmActiveProbableCause_oid),
            ASN_INTEGER,
            &probableCause,sizeof(probableCause));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_REPAIR_ACTION,&alarmObjField)) {
        const char *proposedRepairAction = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveProposedRepairAction_oid, OID_LENGTH(wrsAlarmActiveProposedRepairAction_oid),
            ASN_OCTET_STR,
            proposedRepairAction,strlen(proposedRepairAction));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SERVICE_AFFECTING,&alarmObjField)) {
        int serviceAffecting = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveServiceAffecting_oid, OID_LENGTH(wrsAlarmActiveServiceAffecting_oid),
            ASN_INTEGER,
            &serviceAffecting,sizeof(serviceAffecting));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SUPPRESSION,&alarmObjField)) {
        int activeSuppressionAllowed = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveSuppressionAllowed_oid, OID_LENGTH(wrsAlarmActiveSuppressionAllowed_oid),
            ASN_INTEGER,
            &activeSuppressionAllowed,sizeof(activeSuppressionAllowed));
    }

    snmp_log(LOG_INFO,"wrsAlarmWarning trap sent\n");
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_wrsEventMessage_trap( struct json_object *jobj )
{
    snmp_log(LOG_INFO,"Init send_wrsEventMessage_trap\n");
    int exists;
    struct json_object *alarmObj;
    netsnmp_variable_list  *var_list = NULL;

    const oid wrsAlarmMessage_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,0,5 };
    const oid wrsAlarmActiveAlarmId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,3,0};
    const oid wrsAlarmActiveEntityInstanceId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,4,0};
    const oid wrsAlarmActiveDateAndTime_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,5,0};
    const oid wrsAlarmActiveAlarmSeverity_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,6,0};
    const oid wrsAlarmActiveReasonText_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,7,0};
    const oid wrsAlarmActiveEventType_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,8,0};
    const oid wrsAlarmActiveProbableCause_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,9,0};
    const oid wrsAlarmActiveServiceAffecting_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,11,0};

    exists=json_object_object_get_ex(jobj,"alarm",&alarmObj);
    if(exists==0) {
        snmp_log(LOG_ERR,"Key alarm not found in JSON");
        return SNMP_ERR_BADVALUE;
    }
    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        wrsAlarmMessage_oid, sizeof(wrsAlarmMessage_oid));


    json_object *alarmObjField;
    if(json_object_object_get_ex(alarmObj,EVENT_ID,&alarmObjField)) {
        const char *alarmId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveAlarmId_oid, OID_LENGTH(wrsAlarmActiveAlarmId_oid),
            ASN_OCTET_STR,
            alarmId,strlen(alarmId));

    }

    if(json_object_object_get_ex(alarmObj,EVENT_INSTANCE_ID,&alarmObjField)) {
        const char *alarmInstanceId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveEntityInstanceId_oid, OID_LENGTH(wrsAlarmActiveEntityInstanceId_oid),
            ASN_OCTET_STR,
            alarmInstanceId,
            strlen(alarmInstanceId));
    }

    if(json_object_object_get_ex(alarmObj,EVENT_DATE_TIME,&alarmObjField)) {
        const char *alarmDateTime = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveDateAndTime_oid),
            ASN_OCTET_STR,
            alarmDateTime,strlen(alarmDateTime));
    }

    if(json_object_object_get_ex(alarmObj,EVENT_SEVERITY,&alarmObjField)) {
        int alarmSeverity = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveAlarmSeverity_oid, OID_LENGTH(wrsAlarmActiveAlarmSeverity_oid),
            ASN_INTEGER,
            &alarmSeverity,sizeof(alarmSeverity));
    }

    if(json_object_object_get_ex(alarmObj,EVENT_REASON_TEXT,&alarmObjField)) {
        const char *alarmReasonText = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveReasonText_oid),
            ASN_OCTET_STR,
            alarmReasonText,strlen(alarmReasonText));
    }

    if(json_object_object_get_ex(alarmObj,EVENT_EVENT_TYPE,&alarmObjField)) {
        int eventType = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveEventType_oid, OID_LENGTH(wrsAlarmActiveEventType_oid),
            ASN_INTEGER,
            &eventType,sizeof(eventType));
    }

    if(json_object_object_get_ex(alarmObj,EVENT_CAUSE,&alarmObjField)) {
        int probableCause = json_object_get_int(json_object_object_get(alarmObj,ALARM_CAUSE));
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveProbableCause_oid, OID_LENGTH(wrsAlarmActiveProbableCause_oid),
            ASN_INTEGER,
            &probableCause,sizeof(probableCause));
    }

    if(json_object_object_get_ex(alarmObj,EVENT_SERVICE_AFFECTING,&alarmObjField)) {
        int serviceAffecting = json_object_get_int(alarmObj);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveServiceAffecting_oid, OID_LENGTH(wrsAlarmActiveServiceAffecting_oid),
            ASN_INTEGER,
            &serviceAffecting,sizeof(serviceAffecting));
    }

    snmp_log(LOG_INFO,"wrsEvent Message trap sent\n");
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_wrsAlarmMessage_trap( struct json_object *jobj )
{
    snmp_log(LOG_INFO,"Init send_wrsAlarmMessage_trap\n");
    int exists;
    struct json_object *alarmObj;
    netsnmp_variable_list  *var_list = NULL;
    const oid wrsAlarmMessage_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,0,5 };
    const oid wrsAlarmActiveAlarmId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,3,0};
    const oid wrsAlarmActiveEntityInstanceId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,4,0};
    const oid wrsAlarmActiveDateAndTime_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,5,0};
    const oid wrsAlarmActiveAlarmSeverity_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,6,0};
    const oid wrsAlarmActiveReasonText_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,7,0};
    const oid wrsAlarmActiveEventType_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,8,0};
    const oid wrsAlarmActiveProbableCause_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,9,0};
    const oid wrsAlarmActiveProposedRepairAction_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,10,0};
    const oid wrsAlarmActiveServiceAffecting_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,11,0};
    const oid wrsAlarmActiveSuppressionAllowed_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,12,0};


    exists=json_object_object_get_ex(jobj,"alarm",&alarmObj);
    if(exists==0) {
        snmp_log(LOG_ERR,"Key alarm not found in JSON");
        return SNMP_ERR_BADVALUE;
    }
    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        wrsAlarmMessage_oid, sizeof(wrsAlarmMessage_oid));


    json_object *alarmObjField;
    if(json_object_object_get_ex(alarmObj,ALARM_ID,&alarmObjField)) {
        const char *alarmId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveAlarmId_oid, OID_LENGTH(wrsAlarmActiveAlarmId_oid),
            ASN_OCTET_STR,
            alarmId,strlen(alarmId));

    }

    if(json_object_object_get_ex(alarmObj,ALARM_INSTANCE_ID,&alarmObjField)) {
        const char *alarmInstanceId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveEntityInstanceId_oid, OID_LENGTH(wrsAlarmActiveEntityInstanceId_oid),
            ASN_OCTET_STR,
            alarmInstanceId,
            strlen(alarmInstanceId));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_DATE_TIME,&alarmObjField)) {
        const char *alarmDateTime = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveDateAndTime_oid),
            ASN_OCTET_STR,
            alarmDateTime,strlen(alarmDateTime));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SEVERITY,&alarmObjField)) {
        int alarmSeverity = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveAlarmSeverity_oid, OID_LENGTH(wrsAlarmActiveAlarmSeverity_oid),
            ASN_INTEGER,
            &alarmSeverity,sizeof(alarmSeverity));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_REASON_TEXT,&alarmObjField)) {
        const char *alarmReasonText = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveReasonText_oid),
            ASN_OCTET_STR,
            alarmReasonText,strlen(alarmReasonText));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_EVENT_TYPE,&alarmObjField)) {
        int eventType = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveEventType_oid, OID_LENGTH(wrsAlarmActiveEventType_oid),
            ASN_INTEGER,
            &eventType,sizeof(eventType));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_CAUSE,&alarmObjField)) {
        int probableCause = json_object_get_int(json_object_object_get(alarmObj,ALARM_CAUSE));
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveProbableCause_oid, OID_LENGTH(wrsAlarmActiveProbableCause_oid),
            ASN_INTEGER,
            &probableCause,sizeof(probableCause));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_REPAIR_ACTION,&alarmObjField)) {
        const char *proposedRepairAction = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveProposedRepairAction_oid, OID_LENGTH(wrsAlarmActiveProposedRepairAction_oid),
            ASN_OCTET_STR,
            proposedRepairAction,strlen(proposedRepairAction));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SERVICE_AFFECTING,&alarmObjField)) {
        int serviceAffecting = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveServiceAffecting_oid, OID_LENGTH(wrsAlarmActiveServiceAffecting_oid),
            ASN_INTEGER,
            &serviceAffecting,sizeof(serviceAffecting));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_SUPPRESSION,&alarmObjField)) {
        int activeSuppressionAllowed = json_object_get_int(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveSuppressionAllowed_oid, OID_LENGTH(wrsAlarmActiveSuppressionAllowed_oid),
            ASN_INTEGER,
            &activeSuppressionAllowed,sizeof(activeSuppressionAllowed));
    }

    snmp_log(LOG_INFO,"wrsAlarmMessage trap sent\n");
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_wrsAlarmClear_trap( struct json_object *jobj )
{
    snmp_log(LOG_INFO,"Init send_wrsAlarmClear_trap\n");
    int exists;
    struct json_object *alarmObj;
    netsnmp_variable_list  *var_list = NULL;
    const oid wrsAlarmClear_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,0,9 };
    const oid wrsAlarmActiveAlarmId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,3,0 };
    const oid wrsAlarmActiveEntityInstanceId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,4,0};
    const oid wrsAlarmActiveDateAndTime_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,5,0};
    const oid wrsAlarmActiveReasonText_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,7,0};

    exists=json_object_object_get_ex(jobj,"alarm",&alarmObj);
    if(exists==0) {
        snmp_log(LOG_ERR,"Key alarm not found in JSON");
        return SNMP_ERR_BADVALUE;
    }

    /*
     * Set the snmpTrapOid.0 value
    */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        wrsAlarmClear_oid, sizeof(wrsAlarmClear_oid));

    json_object *alarmObjField;
    if(json_object_object_get_ex(alarmObj,ALARM_ID,&alarmObjField)) {
        const char *alarmId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveAlarmId_oid, OID_LENGTH(wrsAlarmActiveAlarmId_oid),
            ASN_OCTET_STR,
            alarmId,strlen(alarmId));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_INSTANCE_ID,&alarmObjField)) {
        const char *alarmInstanceId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveEntityInstanceId_oid, OID_LENGTH(wrsAlarmActiveEntityInstanceId_oid),
            ASN_OCTET_STR,
            alarmInstanceId,
            strlen(alarmInstanceId));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_DATE_TIME,&alarmObjField)) {
        const char *alarmDateTime = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveDateAndTime_oid),
            ASN_OCTET_STR,
            alarmDateTime,strlen(alarmDateTime));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_REASON_TEXT,&alarmObjField)) {
        const char *alarmReasonText = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveReasonText_oid),
            ASN_OCTET_STR,
            alarmReasonText,strlen(alarmReasonText));
    }

    snmp_log(LOG_INFO,"wrsAlarmClear trap sent\n");
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}
int
send_wrsAlarmHierarchicalClear_trap(struct json_object *jobj )
{
    snmp_log(LOG_INFO,"Init send_wrsAlarmHierarchical_trap\n");
    struct json_object *alarmObj;
    int exists;
    netsnmp_variable_list  *var_list = NULL;
    const oid wrsAlarmHierarchicalClear_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,0,99 };
    const oid wrsAlarmActiveEntityInstanceId_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,4,0};
    const oid wrsAlarmActiveDateAndTime_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,5,0};
    const oid wrsAlarmActiveReasonText_oid[] = { 1,3,6,1,4,1,731,1,1,1,1,1,1,7,0};

    exists=json_object_object_get_ex(jobj,"alarm",&alarmObj);
    if(exists==0) {
        snmp_log(LOG_ERR,"Key alarm not found in JSON");
        return SNMP_ERR_BADVALUE;
    }

    /*
     * Set the snmpTrapOid.0 value
    */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        wrsAlarmHierarchicalClear_oid, sizeof(wrsAlarmHierarchicalClear_oid));

    json_object *alarmObjField;
    if(json_object_object_get_ex(alarmObj,ALARM_INSTANCE_ID,&alarmObjField)) {
        const char *alarmInstanceId = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveEntityInstanceId_oid, OID_LENGTH(wrsAlarmActiveEntityInstanceId_oid),
            ASN_OCTET_STR,
            alarmInstanceId,
            strlen(alarmInstanceId));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_DATE_TIME,&alarmObjField)) {
        const char *alarmDateTime = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveDateAndTime_oid),
            ASN_OCTET_STR,
            alarmDateTime,strlen(alarmDateTime));
    }

    if(json_object_object_get_ex(alarmObj,ALARM_REASON_TEXT,&alarmObjField)) {
        const char *alarmReasonText = json_object_get_string(alarmObjField);
        snmp_varlist_add_variable(&var_list,
            wrsAlarmActiveDateAndTime_oid, OID_LENGTH(wrsAlarmActiveReasonText_oid),
            ASN_OCTET_STR,
            alarmReasonText,strlen(alarmReasonText));
    }

    snmp_log(LOG_INFO,"wrsHierarchicalClear trap sent\n");
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}

int
send_wrsWarmStart_trap()
{
    snmp_log(LOG_INFO,"Init send_wrsWarmStart_trap\n");
    netsnmp_variable_list  *var_list = NULL;
    const oid wrsWarmStart_oid[] = {1,3,6,1,6,3,1,1,5,2};
    /*
     * Set the snmpTrapOid.0 value
     */
    snmp_varlist_add_variable(&var_list,
        snmptrap_oid, snmptrap_oid_len,
        ASN_OBJECT_ID,
        wrsWarmStart_oid, sizeof(wrsWarmStart_oid));

    snmp_log(LOG_INFO,"wrsWarmStart trap sent\n");
    send_v2trap( var_list );
    snmp_free_varbind( var_list );

    return SNMP_ERR_NOERROR;
}

int
process_json_alarm(const char* msg )
{

    if(msg == NULL) {
        snmp_log(LOG_ERR,"JSON message alarm to processs is null\n");
        return 1;
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

    return 0;
}