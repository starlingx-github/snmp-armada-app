FROM centos:8
RUN yum -y update
RUN yum -y install\
  net-snmp \
  net-snmp-utils \
  net-snmp-libs

COPY ./mibs/*  /usr/share/snmp/mibs/
EXPOSE 161/udp
EXPOSE 162/udp
EXPOSE 705/tcp
CMD [ "/usr/sbin/snmpd", "-f"]
