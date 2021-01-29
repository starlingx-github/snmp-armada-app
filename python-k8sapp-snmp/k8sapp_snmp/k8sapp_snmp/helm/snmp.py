#
# Copyright (c) 2020-2021 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#


from k8sapp_snmp.common import constants as app_constants
from os import uname

from six import ensure_str
from six import ensure_text
from six import string_types
from sysinv.common import constants
from sysinv.common import exception 
from sysinv.db import api
from sysinv.helm import base
from sysinv.helm import common
from sysinv.puppet import openstack



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
    UNDEFINED_CONF_VALUE = '?'
    KERNEL_RELEASE_IDX = 2

    def _unicode_represent(self, data):
        if isinstance(data, string_types):
            try:
                result = ensure_str(data)
                #Try to encode to detect bad translation
                #for multi-bytes characters
                result.encode('utf-8')
                return  result
            except Exception as e:
                return ensure_text(data)
        else:
            #If data is NoneType
            return ensure_str(self.UNDEFINED_CONF_VALUE)

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

    def get_system_info(self):
        return uname()[self.KERNEL_RELEASE_IDX]

    def get_overrides(self, namespace=None):

        db_url = self.get_secure_system_config()['fm::database_connection']
        dbapi = api.get_instance()

        # Get the contact, location, name and desciption info
        system = dbapi.isystem_get_one()
        overrides = {
            common.HELM_NS_KUBE_SYSTEM: {
                'configmap': {
                    'connection': self._unicode_represent(db_url),
                    'system_contact' : self._unicode_represent(system.contact),
                    'system_location' : self._unicode_represent(
                        system.location),
                    'system_name' : self._unicode_represent(system.name),
                    'system_description' : self._unicode_represent(
                        system.software_version) + ' ' + self.get_system_info()
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
