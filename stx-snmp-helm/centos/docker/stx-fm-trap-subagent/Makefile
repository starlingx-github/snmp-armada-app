CC=gcc
PREFIX=wrsAlarmMIB

NETSNMPCONFIG=net-snmp-config
SOURCE=./src

# uncomment this if you have GNU make
#NETSNMPCFLAGS := $(shell $(NETSNMPCONFIG) --base-cflags)
#NETSNMPLIBS := $(shell $(NETSNMPCONFIG) --agent-libs)
NETSNMPCFLAGS=`$(NETSNMPCONFIG) --base-cflags`
NETSNMPLIBS=`$(NETSNMPCONFIG) --agent-libs`

LIBS=$(NETSNMPLIBS) -lpthread -ljson-c

STRICT_FLAGS = -Wall -Wstrict-prototypes
CFLAGS=-I. $(NETSNMPCFLAGS) $(STRICT_FLAGS) -fPIC

SRCS = $(SOURCE)/$(PREFIX)Trap.c \
	$(SOURCE)/$(PREFIX)Subagent.c \
	$(SOURCE)/$(PREFIX)Server.c

OBJS = $(SRCS:.c=.o)

TARGETS=$(PREFIX)

.SUFFIXES:
.SUFFIXES: .c .o .deps


all: $(TARGETS)

user: $(USER_OBJS)

$(TARGETS): $(LIB_DEPS)

$(PREFIX): $(OBJS) Makefile
	$(CC) -o $(PREFIX) $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGETS)

