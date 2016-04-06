#
#  This file is provided under a dual BSD/LGPLv2.1 license.  When using 
#  or redistributing this file, you may do so under either license.
# 
#  LGPL LICENSE SUMMARY
# 
#  Copyright(c) <2007-2012>. Intel Corporation. All rights reserved.
# 
#  This program is free software; you can redistribute it and/or modify 
#  it under the terms of version 2.1 of the GNU Lesser General Public 
#  License as published by the Free Software Foundation.
# 
#  This library is distributed in the hope that it will be useful, but 
#  WITHOUT ANY WARRANTY; without even the implied warranty of 
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
#  Lesser General Public License for more details.
# 
#  You should have received a copy of the GNU Lesser General Public 
#  License along with this library; if not, write to the Free Software 
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 
#  USA. The full GNU Lesser General Public License is included in this 
#  distribution in the file called LICENSE.LGPL.
# 
#  Contact Information:
#      Intel Corporation
#      2200 Mission College Blvd.
#      Santa Clara, CA  97052
# 
#  BSD LICENSE
# 
#  Copyright (c) <2007-2012>. Intel Corporation. All rights reserved.
# 
#  Redistribution and use in source and binary forms, with or without 
#  modification, are permitted provided that the following conditions 
#  are met:
# 
#    - Redistributions of source code must retain the above copyright 
#      notice, this list of conditions and the following disclaimer.
#    - Redistributions in binary form must reproduce the above copyright 
#      notice, this list of conditions and the following disclaimer in 
#      the documentation and/or other materials provided with the 
#      distribution.
#    - Neither the name of Intel Corporation nor the names of its 
#      contributors may be used to endorse or promote products derived 
#      from this software without specific prior written permission.
# 
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
#  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

vpath %.c \
	memmap

LIB_NAME = osal

LIB_OBJ = \
	memmap.o \
	osal_irq.o \
	osal_irqlock.o \
	lock.o \
	osal_sema.o \
	osal_pci.o \
	osal_thread.o \
	osal_event.o \
	osal_trace.o \
	osal_clock.o  \
	osal_version.o \
	osal_firmware.o

LDFLAGS+= -lpthread

CFLAGS+= -DLINUX 

CFLAGS_PP = -E -C -P

INCLUDE_PATH += -I$(OSAL_BASE)/include
INCLUDE_PATH += -I$(OSAL_BASE)/linux_user/include
INCLUDE_PATH += -I$(OSAL_BASE)/linux_kernel/tophalf/include
INCLUDE_PATH += -I$(BUILD_DEST)/include

CFLAGS += $(INCLUDE_PATH)

ifeq ($(PRE_SI),true)
    CFLAGS_PP += -DPRE_SILICON
endif

COMP_LIB_NAME = libosal
SHARE_LIB_NAME=$(COMP_LIB_NAME).so
STATIC_LIB_NAME=$(COMP_LIB_NAME).a


STATIC_LIB_OBJ = $(LIB_OBJ)
SHARE_LIB_OBJ = $(patsubst %.o, %.pic.o, $(LIB_OBJ))

.PHONY: all clean

all: $(STATIC_LIB_NAME) $(SHARE_LIB_NAME) 

$(STATIC_LIB_NAME): $(STATIC_LIB_OBJ)
	$(AR) $(ARFLAGS) $(STATIC_LIB_NAME) $^  

$(SHARE_LIB_NAME): $(SHARE_LIB_OBJ)
	$(CC) $(LDFLAGS) -shared  $^ -o $@ 


memmap.c: raw/memmap.c
	@echo Creating: memmap.c from raw/memmap.c; 
	$(CC) $(CFLAGS_PP) ./raw/memmap.c | sed 's/__#/#/' > ./memmap.c; 

%.pic.o: %.c 
	$(CC) -c $(CFLAGS) -fPIC -fpic -o $@ $<

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	@rm -f $(STATIC_LIB_NAME) $(SHARE_LIB_NAME) $(SHARE_LIB_OBJ) $(STATIC_LIB_OBJ) memmap.c 




