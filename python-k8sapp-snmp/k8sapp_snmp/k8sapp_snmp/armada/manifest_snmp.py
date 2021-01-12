#
# Copyright (c) 2020 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#
# All Rights Reserved.
#

""" System inventory Armada manifest operator."""

from sysinv.common import constants
from sysinv.helm import manifest_base as base

class SnmpArmadaManifestOperator(base.ArmadaManifestOperator):

  APP = constants.HELM_APP_SNMP
  ARMADA_MANIFEST = 'armada-manifest'

  # TODO: remove this once Ic83fbd25d23ae34889cb288330ec448f920bda39 merges
  def app_lifecycle_actions(self, context, conductor_obj, dbapi, operation, relative_timing):
    if operation == constants.APP_APPLY_OP or operation == constants.APP_REMOVE_OP:
      if relative_timing == constants.APP_LIFECYCLE_POST:
        config_dict = {
          "personalities": [constants.CONTROLLER],
          "classes": ['platform::fm::runtime']
        }
        config_uuid = conductor_obj._config_update_hosts(context, config_dict['personalities'])
        conductor_obj._config_apply_runtime_manifest(context, config_uuid, config_dict)

  def platform_mode_manifest_updates(self, dbapi, mode):
    """ Update the application manifest based on the platform

      :param dbapi: DB api object
      :param mode: mode to control how to apply the application manifest
    """
    pass
