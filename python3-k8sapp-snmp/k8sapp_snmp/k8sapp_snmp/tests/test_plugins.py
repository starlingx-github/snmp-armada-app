#
# Copyright (c) 2020 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#

import mock

from k8sapp_snmp.helm.snmp import SnmpHelm

from sysinv.common import constants
from sysinv.tests.db import base as dbbase


class K8SAppSnmpAppMixin(object):
    app_name = constants.HELM_APP_SNMP
    path_name = app_name + '.tgz'

    def setUp(self):
        super(K8SAppSnmpAppMixin, self).setUp()
        get_db_connection = mock.patch.object(SnmpHelm,
                '_get_database_connection')
        self.mock_save_overrides = get_db_connection.start()
        self.addCleanup(get_db_connection.stop)


# Test Configuration:
# - Controller
# - IPv6
# - Ceph Storage
# - snmp app
class K8sAppSnmpControllerTestCase(K8SAppSnmpAppMixin,
                                      dbbase.BaseIPv6Mixin,
                                      dbbase.BaseCephStorageBackendMixin,
                                      dbbase.ControllerHostTestCase):
    pass


# Test Configuration:
# - AIO
# - IPv4
# - Ceph Storage
# - snmp app
class K8SAppSnmpAIOTestCase(K8SAppSnmpAppMixin,
                               dbbase.BaseCephStorageBackendMixin,
                               dbbase.AIOSimplexHostTestCase):
    pass
