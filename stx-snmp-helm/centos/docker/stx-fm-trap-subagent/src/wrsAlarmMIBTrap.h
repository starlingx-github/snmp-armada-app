/*
* Copyright (c) 2020 Wind River Systems, Inc.
*
* SPDX-License-Identifier: Apache-2.0
*
*/

#ifndef WRSALARMMIBTRAP_H
#define WRSALARMMIBTRAP_H
#include<json-c/json.h>

/* function declarations */
int send_wrsAlarmCritical_trap(struct json_object *);
int send_wrsAlarmMajor_trap(struct json_object *);
int send_wrsAlarmMinor_trap(struct json_object *);
int send_wrsAlarmWarning_trap(struct json_object *);
int send_wrsAlarmMessage_trap(struct json_object *);
int send_wrsEventMessage_trap(struct json_object *);
int send_wrsAlarmClear_trap(struct json_object *);
int send_wrsAlarmHierarchicalClear_trap(struct json_object *);
int send_wrsWarmStart_trap(void);

#endif /* WRSALARMMIBTRAP_H */
