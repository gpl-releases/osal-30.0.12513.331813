/*==========================================================================
  This file is provided under a BSD license.  When using or 
  redistributing this file, you may do so under either license.

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
 * This file contains common data types used in OS abstracted interfaces.
 */

#ifndef _OSAL_TYPE_H_
#define _OSAL_TYPE_H_
#include "os/osal_type.h"

/** OSAL return codes */
typedef enum _osal_result_t
{
    OSAL_ERROR              = -1,
    OSAL_SUCCESS            = 0,
    OSAL_INVALID_STATE,
    OSAL_INVALID_HANDLE,
    OSAL_INVALID_PARAM,
    OSAL_NOT_IMPLEMENTED,
    OSAL_INVALID_OPERATION,
    OSAL_INVALID_SETTING,
    OSAL_INSUFFICIENT_RESOURCES,
    OSAL_INSUFFICIENT_MEMORY,
    OSAL_INVALID_PERMISSION,
    OSAL_COMPLETED,
    OSAL_NOT_DONE,
    OSAL_PENDING,
    OSAL_TIMEOUT,
    OSAL_CANCELED,
    OSAL_REJECT,
    OSAL_OVERRUN,
    OSAL_NOT_FOUND,
    OSAL_UNAVAILABLE,
    OSAL_BUSY,
    OSAL_DISCONNECT,
    OSAL_DUPLICATE,
    OSAL_STATUS_COUNT    // Must be the last value
} osal_result;

#endif //OSAL_TYPE_H
