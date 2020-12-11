FROM centos:8 AS builder
RUN yum -y update
RUN yum -y upgrade
RUN yum -y install \
           net-snmp-libs \
           net-snmp-devel \
           libuuid\
           gcc\
           make
RUN ln -s /usr/lib64/libuuid.so.1.3.0 /usr/lib64/libuuid.so
WORKDIR /home
COPY ./src/* /home/src/
WORKDIR /home/src/
RUN make -f Makefile

FROM centos:8
RUN yum -y update
RUN yum -y upgrade
RUN yum -y install\
           net-snmp \
           net-snmp-utils \
           net-snmp-libs

COPY ./mibs/*  /usr/share/snmp/mibs/
COPY --from=builder /home/src/libsnmpAuditPlugin.so /usr/lib64/
EXPOSE 161/udp
EXPOSE 162/udp
EXPOSE 705/tcp
CMD /usr/sbin/snmpd $OPTIONDEBUG -f
