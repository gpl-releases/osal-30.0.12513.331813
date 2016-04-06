/*==========================================================================
 This file is provided under a BSD license.  When using or 
 redistributing this file, you may do so under this license.
 
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

#ifndef _OSAL_FIRMWARE_H
#define _OSAL_FIRMWARE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "osal_type.h"
#include "os/osal_firmware.h"

/**
@defgroup firmware OSAL Firmware

Provides OS independent interface for requesting firmware images
@{
*/

/*
   Interface structure between request_firmware() and its wrapper function.
   fw_entry is an opaque handle used internally by os_firmware_request() 
   and os_firmware_release().
   fw_address points to fw data buffer.
   fw_size is the number of bytes of data in the buffer.
*/
typedef struct {
   os_fw_entry_t *fw_entry;
   void *fw_address;
   size_t fw_size;
} os_firmware_t;

/**
Wrapper function to request_firmware() api

@param[in] fw_image_fs_path: fw image file name and path.
@param[out] fw_ctxt: firmware context.

@retval OSAL_SUCCESS : got access to requested fw image.
@retval OSAL_ERROR   : failed to get access to fw image.
@retval OSAL_INVALID_PARAM : output param is NULL.
*/

osal_result os_firmware_request(const char *fw_image_fs_path,
                                os_firmware_t *fw_ctxt);
/**
Wrapper function to release_firmware() api

@param[in] fw_ctxt   : firmware context.

@retval OSAL_SUCCESS : released fw image buffer.
@retval OSAL_ERROR   : failed to release fw image buffer.
@retval OSAL_INVALID_PARAM : input param is NULL.
*/

osal_result os_firmware_release(os_firmware_t *fw_ctxt);

#ifdef __cplusplus
}
#endif

#endif /* _OSAL_FIRMWARE_H */
