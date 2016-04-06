/*==========================================================================
 This file is provided under a dual BSD/GPLv2 license.  When using or 
 redistributing this file, you may do so under either license.
 
 GPL LICENSE SUMMARY
 
 Copyright(c) 2005-2012 Intel Corporation. All rights reserved.
 
 This program is free software; you can redistribute it and/or modify 
 it under the terms of version 2 of the GNU General Public License as
 published by the Free Software Foundation.
 
 This program is distributed in the hope that it will be useful, but 
 WITHOUT ANY WARRANTY; without even the implied warranty of 
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 General Public License for more details.
 
 You should have received a copy of the GNU General Public License 
 along with this program; if not, write to the Free Software 
 Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 The full GNU General Public License is included in this distribution 
 in the file called LICENSE.GPL.
 
 Contact Information:
 Intel Corporation
 
 2200 Mission College Blvd.
 Santa Clara, CA  97052
 
 BSD LICENSE 
 
 Copyright(c) 2005-2012 Intel Corporation. All rights reserved.
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without 
 modification, are permitted provided that the following conditions 
 are met:
 
 * Redistributions of source code must retain the above copyright 
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright 
 notice, this list of conditions and the following disclaimer in 
 the documentation and/or other materials provided with the 
 distribution.
 * Neither the name of Intel Corporation nor the names of its 
 contributors may be used to endorse or promote products derived 
 from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 =========================================================================*/

/*
 * This file contains OS abstracted interfaces for firmware requests.
 */

#include "osal.h"

static void fw_dev_release(struct device *dev)
{
   OS_PRINT("OSAL firmware load device released\n");
}

static struct device fw_device = {
   .init_name  = "fw_device",
   .release    = fw_dev_release
};

/* This is wrapper function to request_firmware() and called
   to get access to a fw image through a kernel buffer.
   This function calls request_firmware(), and saves the the pointer 
   to fw_entry, returned by it and on success it saves the pointer to
   fw buffer and size in fw context.
   request_firmware() returns fw_entry structure having a pointer to
   fw image kernel buffer, which is contiguous in virtual memory space. 
*/
osal_result os_firmware_request(const char *fw_image_fs_path,
                                os_firmware_t *fw_ctxt)
{
   osal_result os_ret = OSAL_INVALID_PARAM;
   int result = 0;

   if ( NULL != fw_ctxt ) {
  
      const struct firmware *fw_entry = (struct firmware *) fw_ctxt->fw_entry;

      /* Request firmware image through Linux API */
      result = request_firmware(&fw_entry, fw_image_fs_path, &fw_device);
      if (0 != result) {
         os_ret = OSAL_ERROR;
      } else {
         fw_ctxt->fw_entry    = (void *)fw_entry;
         fw_ctxt->fw_address  = (void *)fw_entry->data;
         fw_ctxt->fw_size     = fw_entry->size;
         os_ret = OSAL_SUCCESS;
      }
   }

   return os_ret;
}

/* This is wrapper function release_firmware() called to
   release kernel buffer of a fw image.
   This function calls release_firmware(), passing the pointer 
   to fw_entry which was saved in fw context.And initializes the
   fimrware context. 
*/
osal_result os_firmware_release(os_firmware_t *fw_ctxt)
{
   osal_result os_ret = OSAL_SUCCESS;
   const struct firmware *fw_entry;

   if (NULL != fw_ctxt) {
      fw_entry = (struct firmware *) fw_ctxt->fw_entry;
      if (fw_entry != NULL) {
         release_firmware(fw_entry);
         fw_ctxt->fw_entry = NULL;
         fw_ctxt->fw_address = NULL;
         fw_ctxt->fw_size = 0;
      } else {
         os_ret = OSAL_ERROR;
      }
   } else {
      os_ret = OSAL_INVALID_PARAM;
   }

   return os_ret;
}

/* This function is called during the initialization of osal_kernel module.
   It registers a dummy device fw_device. request_firmware() api requests
   firmware for this dummy device. 
*/
osal_result osal_firmware_init(void)
{
   osal_result os_ret = OSAL_SUCCESS;
   int result = 0;

   result = device_register(&fw_device);
   if (result) {
      OS_PRINT("Error registering firmware load device. \
               Cannot use request_firmware() to load FW images.\n");
      os_ret = OSAL_ERROR;
   }

   return os_ret;
}

/* This function is called during the rmmod of osal_kernel module.
   It unregisters a dummy device fw_device.
*/
osal_result osal_firmware_exit(void)
{
   device_unregister(&fw_device);
   return OSAL_SUCCESS;
}
