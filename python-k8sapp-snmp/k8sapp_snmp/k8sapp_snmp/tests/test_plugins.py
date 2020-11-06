#
# Copyright (c) 2020 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#

from sysinv.common import constants
from sysinv.tests.db import base as dbbase
from sysinv.tests.helm.test_helm import HelmOperatorTestSuiteMixin


class K8SAppSnmpAppMixin(object):
    app_name = constants.HELM_APP_SNMP
    path_name = app_name + '.tgz'

    def setUp(self):
        super(K8SAppSnmpAppMixin, self).setUp()


# Test Configuration:
# - Controller
# - IPv6
# - Ceph Storage
# - snmp app
class K8sAppSnmpControllerTestCase(K8SAppSnmpAppMixin,
                                      dbbase.BaseIPv6Mixin,
                                      dbbase.BaseCephStorageBackendMixin,
                                      HelmOperatorTestSuiteMixin,
                                      dbbase.ControllerHostTestCase):
    pass


# Test Configuration:
# - AIO
# - IPv4
# - Ceph Storage
# - snmp app
class K8SAppSnmpAIOTestCase(K8SAppSnmpAppMixin,
                               dbbase.BaseCephStorageBackendMixin,
                               HelmOperatorTestSuiteMixin,
                               dbbase.AIOSimplexHostTestCase):
    pass
