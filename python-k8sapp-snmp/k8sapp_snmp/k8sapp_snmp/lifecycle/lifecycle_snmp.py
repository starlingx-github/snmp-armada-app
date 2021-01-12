#
# Copyright (c) 2021 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#
# All Rights Reserved.
#

""" System inventory App lifecycle operator."""

from oslo_log import log as logging
from sysinv.common import constants
from sysinv.helm import lifecycle_base as base
from sysinv.helm import lifecycle_utils as lifecycle_utils

LOG = logging.getLogger(__name__)


class SnmpAppLifecycleOperator(base.AppLifecycleOperator):
    def app_lifecycle_actions(self, context, conductor_obj, app_op, app, hook_info):
        """ Perform lifecycle actions for an operation

        :param context: request context
        :param conductor_obj: conductor object
        :param app_op: AppOperator object
        :param app: AppOperator.Application object
        :param hook_info: LifecycleHookInfo object

        """
        # Operation
        if hook_info.lifecycle_type == constants.APP_LIFECYCLE_TYPE_OPERATION:
            if hook_info.operation == constants.APP_APPLY_OP:
                if hook_info.relative_timing == constants.APP_LIFECYCLE_TIMING_POST:
                    return self.post_apply(context, conductor_obj, hook_info)
            elif hook_info.operation == constants.APP_REMOVE_OP:
                if hook_info.relative_timing == constants.APP_LIFECYCLE_TIMING_POST:
                    return self.post_remove(context, conductor_obj, hook_info)

        # Rbd
        elif hook_info.lifecycle_type == constants.APP_LIFECYCLE_TYPE_RBD:
            if hook_info.operation == constants.APP_APPLY_OP and \
                    hook_info.relative_timing == constants.APP_LIFECYCLE_TIMING_PRE:
                return lifecycle_utils.create_rbd_provisioner_secrets(app_op, app, hook_info)
            elif hook_info.operation == constants.APP_REMOVE_OP and \
                    hook_info.relative_timing == constants.APP_LIFECYCLE_TIMING_POST:
                return lifecycle_utils.delete_rbd_provisioner_secrets(app_op, app, hook_info)

        # Resources
        elif hook_info.lifecycle_type == constants.APP_LIFECYCLE_TYPE_RESOURCE:
            if hook_info.operation == constants.APP_APPLY_OP and \
                    hook_info.relative_timing == constants.APP_LIFECYCLE_TIMING_PRE:
                return lifecycle_utils.create_local_registry_secrets(app_op, app, hook_info)
            elif hook_info.operation == constants.APP_REMOVE_OP and \
                    hook_info.relative_timing == constants.APP_LIFECYCLE_TIMING_POST:
                return lifecycle_utils.delete_local_registry_secrets(app_op, app, hook_info)

        # Use the default behaviour for other hooks
        super(SnmpAppLifecycleOperator, self).app_lifecycle_actions(context, conductor_obj, app_op, app, hook_info)

    def post_apply(self, context, conductor_obj, hook_info):
        """Post apply actions

        :param context: request context
        :param conductor_obj: conductor object
        :param hook_info: LifecycleHookInfo object

        """
        config_dict = {
            "personalities": [constants.CONTROLLER],
            "classes": ['platform::fm::runtime']
        }
        config_uuid = conductor_obj._config_update_hosts(context, config_dict['personalities'])
        conductor_obj._config_apply_runtime_manifest(context, config_uuid, config_dict)

    def post_remove(self, context, conductor_obj, hook_info):
        """Post remove actions

        :param context: request context
        :param conductor_obj: conductor object
        :param hook_info: LifecycleHookInfo object

        """
        config_dict = {
            "personalities": [constants.CONTROLLER],
            "classes": ['platform::fm::runtime']
        }
        config_uuid = conductor_obj._config_update_hosts(context, config_dict['personalities'])
        conductor_obj._config_apply_runtime_manifest(context, config_uuid, config_dict)
