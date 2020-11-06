#
# SPDX-License-Identifier: Apache-2.0
#

MAJOR ?= 1
MINOR ?= 0
PATCH ?= 0
MIBVER ?= 2.0

CC=gcc
DEST_DIR ?= /
DATA_DIR ?= /usr/share
LIB_DIR ?= /usr/lib64
BIN=./bin
SOURCE=./src

SRCS =  $(SOURCE)/snmpSubAgent.c $(SOURCE)/snmpActiveAlarms.c $(SOURCE)/snmpAgentPlugin.c $(SOURCE)/snmpEventLogs.c
OBJS = $(SRCS:.c=.o)
LDLIBS = -L/lib/x86_64-linux-gnu/ -L/usr/lib64  -lfmcommon -lnetsnmp
LDLIBS += -lnetsnmpagent -luuid -lnetsnmpmibs  -lnetsnmphelpers

INCLUDES += -I./ -I/include/net-snmp -I$(SOURCE) -I/usr/include/
CCFLAGS = -fPIC -g -O2 -Wall -Werror -Wformat -Wformat-security

TARGET =  snmpSubAgent

.c.o:
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(SRCS)
	$(CC)  $(CCFLAGS) $(INCLUDES) -o $@ $^ $(LDLIBS)

clean:
	@rm -f $(OBJ) *.o $(TARGET)

install:
	echo "install stage"
