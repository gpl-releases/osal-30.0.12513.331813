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

__#include "osal.h"

#ifdef PRE_SILICON
__#ifndef FIX_BROKEN_GEN3_PRE_SI_ADDRESSING
__#define FIX_BROKEN_GEN3_PRE_SI_ADDRESSING 1
__#endif
#endif // PRE_SILICON

void * os_map_io_to_mem_nocache( unsigned long base_address, unsigned long size)
{
    void *mmio;
    unsigned pg_size = PAGE_SIZE;
    unsigned pg_aligned_base = (base_address/pg_size)*pg_size;

    // pg_aligned_base always <= base_address
    unsigned offset = base_address - pg_aligned_base;

    size += offset;

#ifdef PRE_SILICON
__#if FIX_BROKEN_GEN3_PRE_SI_ADDRESSING
    // for anything below 512MB, we do this fakeout to point to the
    // addresses the devices will see
    if(pg_aligned_base <= 0x20000000){
        os_pci_dev_t pcidev;
        unsigned pa_offset;
        unsigned barnum = -1;
        if (OSAL_SUCCESS == OS_PCI_FIND_FIRST_DEVICE(0x8086,0x2e63,&pcidev)){   
            barnum = 3;
        }else if (OSAL_SUCCESS==OS_PCI_FIND_FIRST_DEVICE(0x001,0x5406,&pcidev)){
            // This is the RAMBO card on SLE
            barnum = 2;
        }else if (OSAL_SUCCESS==OS_PCI_FIND_FIRST_DEVICE(0x002,0x5406,&pcidev)){
            barnum = 2; 
        }else if(OSAL_SUCCESS==OS_PCI_FIND_FIRST_DEVICE(0x8086,0x0001,&pcidev)){
            barnum = 0;
        } else {
            OS_PRINT("os_map_io_to_mem_nocache: Pre-silicon environment not detected\n");
        }

        if ( (barnum != -1)
        && ( OSAL_SUCCESS==OS_PCI_READ_CONFIG_32(pcidev,0x10+(barnum<<2),&pa_offset )) ){
            //OS_PRINT(" BAR = %x \n",pa_offset);
            pg_aligned_base += pa_offset;
        }
        if(NULL != pcidev) {
            OS_PCI_FREE_DEVICE(pcidev);
        }
    }

__#endif
#endif // PRE_SILICON

    mmio = ioremap_nocache(pg_aligned_base, size);
    return (mmio + offset);
}
