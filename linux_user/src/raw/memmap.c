/*========================================================================
  This file is provided under a dual BSD/LGPLv2.1 license.  When using 
  or redistributing this file, you may do so under either license.
 
  LGPL LICENSE SUMMARY
 
  Copyright(c) <2005-2010>. Intel Corporation. All rights reserved.
 
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
 
  Copyright (c) <2005-2010>. Intel Corporation. All rights reserved.
 
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
 * File Description:
 *  This file contains the XFree86 implementations of the OSAL
 *  memory.h abstractions.
 *
 *----------------------------------------------------------------------------
 * Authors:
 *  Alan Previn <alan.previn.teres.alexis@intel.com>
 *
 *----------------------------------------------------------------------------
 */

__#ifndef _OSAL_LINUX_USER_IO_MEMMAP_H
__#define _OSAL_LINUX_USER_IO_MEMMAP_H


__#include <stdio.h>
__#include <errno.h>
__#include <string.h>
__#include <sys/mman.h>
__#include <sys/types.h>
__#include <sys/stat.h>
__#include <fcntl.h>
__#include <unistd.h>
__#include "osal.h"

#ifdef PRE_SILICON
__#ifndef FIX_BROKEN_GEN3_PRE_SI_ADDRESSING
__#define FIX_BROKEN_GEN3_PRE_SI_ADDRESSING 1
__#endif
#endif // PRE_SILICON


os_pci_dev_t *pci_find_device(unsigned int vid, unsigned int did, os_pci_dev_t *pdev);


#ifdef PRE_SILICON
__#define ENABLE_EMUL_SUPPORT 1

__#if ENABLE_EMUL_SUPPORT
int g_use_rambo_backdoor =1 ;

void use_rambo_backdoor(int i)
{
    g_use_rambo_backdoor = i;
    OS_PRINT("use_rambo_backdoor=%x\n",i);
}

__#endif
#endif // PRE_SILICON

void * os_map_io_to_mem_cache(
        unsigned long base_address,
        unsigned long size
        )
{
    int fd;
    void *mmio;
    int pg_size = getpagesize();
    int pg_aligned_base = (base_address/pg_size)*pg_size;
    int errsv = 0;

    // pg_aligned_base always <= base_address
    int offset = base_address - pg_aligned_base;

    size += offset;
        
    fd = open("/dev/devmem",O_RDWR);
    if(fd == -1) {
        printf("Could not open /dev/devmem.\n");
        return NULL;
    }

    mmio = mmap(NULL,
                size,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                fd,
                pg_aligned_base);
    errsv = errno;
    close(fd);
    if(mmio == MAP_FAILED) {
        printf("Unable to mmap: %s (%d)\n", strerror(errsv), errsv);
        return NULL;
    }

    return (mmio + offset);
}

void * os_map_io_to_mem_nocache(
        unsigned long base_address,
        unsigned long size
        )
{
    int fd;
    void *mmio;
    unsigned pg_size = getpagesize();
    unsigned pg_aligned_base = (base_address/pg_size)*pg_size;

    // pg_aligned_base always <= base_address
    unsigned offset = base_address - pg_aligned_base;
    int errsv = 0;

    size += offset;
        
    fd = open("/dev/devmem",O_RDWR | O_SYNC);
    if(fd < 0) {
        printf("Could not open /dev/devmem.\n");
        return NULL;
    }
#if PRE_SILICON
__#if FIX_BROKEN_GEN3_PRE_SI_ADDRESSING
        // for anything below 512MB, we do this fakeout to point to the
    // addresses the devices will see
    if(pg_aligned_base <= 0x20000000){
        os_pci_dev_t *pcidev;
        unsigned pa_offset;
        int barnum = -1;
        // This is the HDMI Proto Card
        if ((pcidev = pci_find_device(0x8086,0x2e63,NULL)))
        { 
            barnum = 3;
        }

__#if ENABLE_EMUL_SUPPORT
        else if ((pcidev = pci_find_device(0x8086,0x5453,NULL)))
        {
            OS_PRINT("DINI Gateway Card\n");
            barnum = 3;
        } 
        // This is the RAMBO card on SLE
        else if ((pcidev = pci_find_device(0x001,0x5406,NULL)) && g_use_rambo_backdoor)
        {
            OS_PRINT("Rambo Back Door PCI\n");
            barnum = 2;
        } 
        else if ((pcidev = pci_find_device(0x002,0x5406,NULL)) && g_use_rambo_backdoor)
        {
            OS_PRINT("Rambo Back Door PCI\n");
            barnum = 2;    
        }
        // This is the Mav Front door
        else if ((pcidev = pci_find_device(0x8086,0x1127,NULL)))
        {
            OS_PRINT("Mav Front Door PCI\n");
            barnum = 3; 
        } 
        else if((pcidev = pci_find_device(0x8086,0x0001,NULL)))
        {
            barnum = 0;
        }
__#endif // ENABLE_EMUL_SUPPORT

        if ( barnum != -1 && OSAL_SUCCESS == OS_PCI_READ_CONFIG_32(pcidev, 0x10+ (barnum << 2),&pa_offset ) ){
          //OS_PRINT(" BAR = %x \n",pa_offset);
            pg_aligned_base += pa_offset;
        }

        if(pcidev) {
            os_pci_free_device(pcidev);
        }
    }

__#endif // FIX_BROKEN_GEN3_PRE_SI_ADDRESSING
#endif // PRE_SILICON

    mmio = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, pg_aligned_base);

    errsv = errno;
    close(fd);
    if(mmio == MAP_FAILED) {
        printf("Unable to mmap: %s (%d)\n", strerror(errsv), errsv);
        return NULL;
    }
    return (mmio + offset);
}

void os_unmap_io_from_mem(
    void * virt_addr,
    unsigned long size
    )
{
    unsigned long base_address = (unsigned long)virt_addr;
    int pg_size = getpagesize();
    int pg_aligned_base = (base_address/pg_size)*pg_size;

    // pg_aligned_base always <= base_address
    int offset = base_address - pg_aligned_base;
    size += offset;
    
    munmap((void *)pg_aligned_base, size);
}


__#endif // _OSAL_LINUX_USER_IO_MEMMAP_H
