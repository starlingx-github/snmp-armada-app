ARG BASE
FROM ${BASE} AS stx
RUN yum -y update
RUN yum -y install\
           fm-common \
           fm-common-dev

FROM centos:7 AS base
RUN yum -y install\
           net-snmp \
           net-snmp-utils \
           python3-setuptools \
           libpython2.7 \
           python3-devel \
           postgresql

WORKDIR /home/sub-agent
COPY --from=stx /usr/lib64/libfmcommon.so.1.0 /usr/lib64/

RUN ln -s  /usr/lib64/libfmcommon.so.1.0 /usr/lib64/libfmcommon.so
RUN ln -s  /usr/lib64/libfmcommon.so.1.0 /usr/lib64/libfmcommon.so.1

FROM base AS builder
RUN yum -y install\
           net-snmp-libs \
           net-snmp-devel\
           gcc \
           make \
           uuid

RUN ln -s /usr/lib64/libuuid.so.1.3.0 /usr/lib64/libuuid.so
COPY Makefile .
COPY ./src/* ./src/
COPY --from=stx /usr/include/fmDbAPI.h /usr/include/
COPY --from=stx /usr/include/fmAPI.h /usr/include/
RUN make


FROM base AS deployment-env
COPY --from=builder /home/sub-agent/snmpSubAgent .
COPY bootstrap.sh .
RUN chmod 755 bootstrap.sh
ENTRYPOINT ["./bootstrap.sh"]
