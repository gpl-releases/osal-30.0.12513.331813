/*========================================================================
  This file is provided under a dual BSD/LGPLv2.1 license.  When using 
  or redistributing this file, you may do so under either license.
 
  LGPL LICENSE SUMMARY
 
  Copyright(c) 2005-2011. Intel Corporation. All rights reserved.
 
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
 
  Copyright (c) 2005-2011. Intel Corporation. All rights reserved.
 
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
#include <pthread.h>

#include "osal_memory.h"
#include "osal_lock.h"

os_lock_t os_create_lock()
{
    pthread_mutex_t *mutex;

    mutex = (pthread_mutex_t *)OS_ALLOC(sizeof(pthread_mutex_t));
    if (NULL == mutex)
       return NULL;
    pthread_mutex_init(mutex, NULL);

    return mutex;
}

int os_lock(os_lock_t lock)
{
    return pthread_mutex_lock((pthread_mutex_t *)lock);
}

int os_try_lock(os_lock_t lock)
{
    int ret_val=-1;  // Non-zero Not acquired, 0 acquired

    if(NULL != lock) {
        ret_val = pthread_mutex_trylock((pthread_mutex_t *)lock);
    }
    return (ret_val);
}


int os_unlock(os_lock_t lock)
{
    return pthread_mutex_unlock((pthread_mutex_t *)lock);
}

int os_destroy_lock(os_lock_t lock)
{
    int ret;

    ret = pthread_mutex_destroy((pthread_mutex_t *)lock);
    if (ret == 0) {
      free(lock);
    }

    return ret;
}


osal_result os_mutex_init(os_mutex_t *mtx)
{
    osal_result ret = OSAL_ERROR;

    if ( NULL == mtx ) {
        ret = OSAL_INVALID_PARAM;
    }
    else if ( 0 == pthread_mutex_init(&mtx->lock, NULL) ) {
        ret = OSAL_SUCCESS;
    }

    return ret;
}


osal_result os_mutex_destroy(os_mutex_t *mtx)
{
    osal_result ret = OSAL_ERROR;

    if ( NULL == mtx ) {
        ret = OSAL_INVALID_PARAM;
    }
    else if ( 0 == pthread_mutex_destroy(&mtx->lock) ) {
        ret = OSAL_SUCCESS;
    }

    return ret;
}


osal_result os_mutex_lock( os_mutex_t *mtx )
{ 
    osal_result ret = OSAL_ERROR;

    if ( NULL == mtx ) {
        ret = OSAL_INVALID_PARAM;
    }
    else if ( 0 == pthread_mutex_lock(&mtx->lock) ) {
        ret = OSAL_SUCCESS;
    }

    return ret;
}


osal_result os_mutex_trylock( os_mutex_t *mtx )
{ 
    osal_result ret = OSAL_ERROR;

    if ( NULL == mtx ) {
        ret = OSAL_INVALID_PARAM;
    }
    else if ( 0 == pthread_mutex_trylock(&mtx->lock) ) {
        ret = OSAL_SUCCESS;
    }

    return ret;
}


osal_result os_mutex_unlock( os_mutex_t *mtx )
{
    osal_result ret = OSAL_ERROR;

    if ( NULL == mtx ) {
        ret = OSAL_INVALID_PARAM;
    }
    else if ( 0 == pthread_mutex_unlock(&mtx->lock) ) {
        ret = OSAL_SUCCESS;
    }

    return ret;
}
