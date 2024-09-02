SRCS = snmpAuditPlugin.c

OBJS = $(SRCS:.c=.o)
LDLIBS = -lnetsnmp -lnetsnmpagent -luuid
INCLUDES += -I./
CCFLAGS = -fPIC -g -O2 -Wall -Werror -Wformat -Wformat-security
LIBCGTSAGENT_SO := libsnmpAuditPlugin.so
MAJOR ?= 1
MINOR ?= 0
PATCH ?= 0
DESTDIR ?= /usr/lib

build: lib

.c.o:
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

lib: $(OBJS)
	$(CC) $(OBJS) $(LDLIBS) -o $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) -shared
	ln -sf $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(LIBCGTSAGENT_SO).$(MAJOR)
	ln -sf $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(LIBCGTSAGENT_SO)

clean:
	@rm -f $(OBJ) *.o *.so.*

install:
	install -m 755 -d $(DESTDIR)
	install -m 755 $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(DESTDIR)
	ln -s $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(DESTDIR)/$(LIBCGTSAGENT_SO).$(MAJOR)
	ln -s $(LIBCGTSAGENT_SO).$(MAJOR).$(MINOR).$(PATCH) $(DESTDIR)/$(LIBCGTSAGENT_SO)
