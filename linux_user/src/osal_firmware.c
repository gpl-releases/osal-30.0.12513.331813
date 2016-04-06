/*========================================================================
 This file is provided under a dual BSD/LGPLv2.1 license.  When using 
 or redistributing this file, you may do so under either license.
 
 LGPL LICENSE SUMMARY
 
 Copyright(c) <2005-2011>. Intel Corporation. All rights reserved.
 
 This program is free software; you can redistribute it and/or modify 
 it under the terms of version 2.1 of the GNU Lesser General Public 
 License as published by the Free Software Foundation.
 
 This library is distributed in the hope that it will be useful, but 
 WITHOUT ANY WARRANTY; without even the implied warranty of 
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public 
 License along with this library; if not, write to the Free Software 
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 
 USA. The full GNU Lesser General Public License is included in this 
 distribution in the file called LICENSE.LGPL.
 
 Contact Information:
 Intel Corporation
 2200 Mission College Blvd.
 Santa Clara, CA  97052
 
 BSD LICENSE
 
 Copyright (c) <2005-2011>. Intel Corporation. All rights reserved.
 
 Redistribution and use in source and binary forms, with or without 
 modification, are permitted provided that the following conditions 
 are met:
 
 - Redistributions of source code must retain the above copyright 
 notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright 
 notice, this list of conditions and the following disclaimer in 
 the documentation and/or other materials provided with the 
 distribution.
 - Neither the name of Intel Corporation nor the names of its 
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
 * NOT IMPLEMENTED FOR LINUX USER
 */

#include "osal.h"

/* This is user space implementation of osal_firmware_request()
   It copies fw image file to a malloced buffer and update the
   size and buffer pointer in fw_ctxt.
*/
osal_result os_firmware_request(const char *fw_image_fs_path,
                                os_firmware_t *fw_ctxt)
{
   FILE *fp = NULL;
   osal_result os_ret = OSAL_ERROR;

   if ( NULL != fw_ctxt) {
      fp = fopen(fw_image_fs_path, "rb");
      if (NULL != fp) {
         fseek(fp, 0L, SEEK_END);
         fw_ctxt->fw_size = ftell(fp); /* get image size */
         fseek(fp, 0L, SEEK_SET );
     
         /*Allocate buffer for fw image */ 
         fw_ctxt->fw_address = OS_ALLOC(fw_ctxt->fw_size);
         if (NULL != fw_ctxt->fw_address) {
            size_t data_read;

            data_read = fread(fw_ctxt->fw_address, 1, fw_ctxt->fw_size, fp);

            if (data_read == fw_ctxt->fw_size) {
               os_ret = OSAL_SUCCESS;       
            }
         }

         fclose(fp);
      } else {
         OS_PRINT("ERR: %s file not found \n", fw_image_fs_path);
      }
   } else {
      os_ret = OSAL_INVALID_PARAM;
      OS_PRINT("ERR: fw_ctxt is NULL\n");
   }

   return os_ret;
}

/* This is user space implementation of osal_firmware_release()
   It frees malloced buffer and update the size and buffer pointer 
   in fw_ctxt.
*/
osal_result os_firmware_release(os_firmware_t *fw_ctxt)
{
   osal_result os_ret = OSAL_ERROR;
   
   if (NULL != fw_ctxt) {
      if (NULL != fw_ctxt->fw_address) {
         OS_FREE(fw_ctxt->fw_address); /*Release fw buffer's memory */
         fw_ctxt->fw_address =  NULL;
         fw_ctxt->fw_size = 0;
         os_ret = OSAL_SUCCESS;
      }
   }
   else
   {
      os_ret = OSAL_INVALID_PARAM;
   }

   return os_ret;
}
