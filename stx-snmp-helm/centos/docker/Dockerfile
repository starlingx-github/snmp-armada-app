FROM centos:8
RUN yum -y update
RUN yum -y install net-snmp

EXPOSE 161/udp
CMD [ "/usr/sbin/snmpd", "-f"]
