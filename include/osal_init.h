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

/*
 * This file contains functions required to use the OSAL for some Operating Systems. Notable Win32.
 */

//
// Kelly Couch
// 05/21/2007
// 11/21/2008 - Updated documentation to reflect current osal style.
//
/** \defgroup init init
 *
 * <I> API definition of OSAL initialization functions
 * </I>
 * 
 *\{*/
 
#ifndef OSAL_INIT_H
#define OSAL_INIT_H

#include "osal_type.h"
#include "osal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OSAL_WIN32

/**
This function performs any required initialization prior to using the osal library.
This is currently only required for Win32.

@retval OSAL_SUCCESS : Successfully initalized the osal library.  Always returns OSAL_SUCCESS. 
Nothing to initialize for this platform
*/
static inline osal_result os_init(void) { return OSAL_SUCCESS; }

/**
This function performs any required un-initialization when done using the osal library.
This is currently only required for Win32.

@retval OSAL_SUCCESS : Successfully initalized the osal library.  Always returns OSAL_SUCCESS. 
Nothing to initialize for this platform
*/
static inline osal_result os_uninit(void) { return OSAL_SUCCESS; }

#else

/**
This function performs any required initialization prior to using the osal library.
This is currently only required for Win32.

@retval OSAL_SUCCESS : Successfully initalized the osal library.
@retval OSAL_ERROR :  Failded to initalize the library.
*/
osal_result os_init(void);

/**
This function performs any required un-initialization when done using the osal library.
This is currently only required for Win32.

@retval OSAL_SUCCESS : Successfully un-initalized the osal library.
@retval OSAL_ERROR :  Failded to un-initalize the library.
*/
osal_result os_uninit(void);

#endif

#ifdef __cplusplus
}
#endif

#endif //OSAL_INIT_H
