#
# Copyright (c) 2020 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#

from k8sapp_snmp.common import constants as app_constants

from sysinv.common import constants
from sysinv.common import exception

from sysinv.helm import base
from sysinv.helm import common
from sysinv.puppet import openstack

from oslo_log import log as logging


class SnmpHelm(base.BaseHelm, openstack.OpenstackBasePuppet):
    """Class to encapsulate helm operations for the SNMP chart"""

    SUPPORTED_NAMESPACES = base.BaseHelm.SUPPORTED_NAMESPACES + \
        [common.HELM_NS_KUBE_SYSTEM]
    SUPPORTED_APP_NAMESPACES = {
        constants.HELM_APP_SNMP:
            base.BaseHelm.SUPPORTED_NAMESPACES + [common.HELM_NS_KUBE_SYSTEM],
    }

    CHART = app_constants.HELM_CHART_SNMP

    SERVICE_NAME = 'snmp'
    SERVICE_FM_NAME = 'fm'
    SERVICE_FM_PORT = 18002

    def get_namespaces(self):
        return self.SUPPORTED_NAMESPACES

    def get_public_url(self):
        return self._format_public_endpoint(self.SERVICE_FM_PORT)

    def get_internal_url(self):
        return self._format_private_endpoint(self.SERVICE_FM_PORT)

    def get_admin_url(self):
        return self._format_admin_endpoint(self.SERVICE_FM_PORT)

    def get_secure_system_config(self):
        config = {
            'fm::database_connection':
                self._format_database_connection(self.SERVICE_FM_NAME),
        }

        return config

    def get_overrides(self, namespace=None):

        db_url = self.get_secure_system_config()['fm::database_connection']

        overrides = {
            common.HELM_NS_KUBE_SYSTEM: {
                'configmap': {
                    'connection': str(db_url)
                },
            }
        }

        if namespace in self.SUPPORTED_NAMESPACES:
            return overrides[namespace]
        elif namespace:
            raise exception.InvalidHelmNamespace(chart=self.CHART,
                                                 namespace=namespace)
        else:
            return overrides
