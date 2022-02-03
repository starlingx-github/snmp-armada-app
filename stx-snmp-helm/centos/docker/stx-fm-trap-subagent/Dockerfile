FROM centos:8 AS builder
RUN sed -i -e 's%^mirrorlist=%#mirrorlist=%' -e 's%^#baseurl=http://mirror%baseurl=https://vault%' /etc/yum.repos.d/*repo
RUN yum -y update && yum upgrade
RUN yum -y install\
           net-snmp-libs \
           net-snmp-devel \
           make

RUN dnf -y --enablerepo=powertools install json-c-devel
WORKDIR /home/trapagent
COPY src/* ./src/
COPY Makefile .
RUN make


FROM centos:8
RUN sed -i -e 's%^mirrorlist=%#mirrorlist=%' -e 's%^#baseurl=http://mirror%baseurl=https://vault%' /etc/yum.repos.d/*repo
RUN yum -y update && yum upgrade
RUN yum -y install\
           net-snmp \
           net-snmp-utils

WORKDIR /home/trapagent
COPY --from=builder /home/trapagent/wrsAlarmMIB .
COPY bootstrap.sh .
RUN chmod 755 ./bootstrap.sh
ENTRYPOINT ["./bootstrap.sh"]
