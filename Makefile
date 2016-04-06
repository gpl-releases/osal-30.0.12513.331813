# 
#
#  This file is provided under a dual BSD/GPLv2 license.  When using or 
#  redistributing this file, you may do so under either license.
#
#  GPL LICENSE SUMMARY
#
#  Copyright(c) 2008-2012 Intel Corporation. All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify 
#  it under the terms of version 2 of the GNU General Public License as
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful, but 
#  WITHOUT ANY WARRANTY; without even the implied warranty of 
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
#  General Public License for more details.
#
#  You should have received a copy of the GNU General Public License 
#  along with this program; if not, write to the Free Software 
#  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
#  The full GNU General Public License is included in this distribution 
#  in the file called LICENSE.GPL.
#
#  Contact Information:
#  intel.com
#  Intel Corporation
#  2200 Mission College Blvd.
#  Santa Clara, CA  95052
#  USA
#  (408) 765-8080
#
#
#  BSD LICENSE 
#
#  Copyright(c) 2008-2012 Intel Corporation. All rights reserved.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without 
#  modification, are permitted provided that the following conditions 
#  are met:
#
#    * Redistributions of source code must retain the above copyright 
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright 
#      notice, this list of conditions and the following disclaimer in 
#      the documentation and/or other materials provided with the 
#      distribution.
#    * Neither the name of Intel Corporation nor the names of its 
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
#

export COMPONENT := osal_linux
export PWD := $(shell pwd)
ifndef OSAL_BASE
export OSAL_BASE := $(PWD)
endif


.PHONY: all  install clean $(COMPONENT) 

all:$(COMPONENT) install

include Makefile.inc


SUB_DIRS := linux_kernel linux_user

$(COMPONENT):
	@echo ">>>Building  Components $(COMPONENT)"
	@$(foreach SUBDIR, $(SUB_DIRS), $(MAKE) -C $(SUBDIR) -f Makefile all && ) exit 0;

.PHONY: clean	
clean:
	@echo ">>>Cleaninng up  Components $(COMPONENT)"
	@$(foreach SUBDIR, $(SUB_DIRS), $(MAKE) -C $(SUBDIR) -f Makefile clean && ) exit 0;
	make uninstall

.PHONY: test	
test:
	@echo ">>>Buildinng up test unit for $(COMPONENT)"


.PHONY: install	
install: install_dev install_target	
	@echo ">>>install done for  Components $(COMPONENT)"

.PHONY: install_dev	
install_dev: install_dev_dirs
	@echo ">>>install dev for  Components $(COMPONENT)"
	install -c -m 554 $(PWD)/linux_kernel/src/osal_linux.ko $(BUILD_DEST)/lib/modules/osal_linux.ko
	install -c -m 554 $(PWD)/linux_user/src/libosal.so  $(BUILD_DEST)/lib/libosal.so
	install -c -m 554 $(PWD)/linux_user/src/libosal.a  $(BUILD_DEST)/lib/libosal.a
	install -c -m 664 $(PWD)/include/*.h $(BUILD_DEST)/include/
	install -c -m 664 $(PWD)/linux_kernel/include/stdint.h $(BUILD_DEST)/include/linux/stdint.h
	install -c -m 664 $(PWD)/linux_kernel/include/os/*.h $(BUILD_DEST)/include/linux/os/
	install -c -m 664 $(PWD)/linux_user/include/os/*.h $(BUILD_DEST)/include/linux_user/os/


.PHONY: install_dev_dirs	
install_dev_dirs:
	@if [ ! -d $(BUILD_DEST)/include ] ;  then\
		 mkdir -p $(BUILD_DEST)/include; \
	fi
	@if [ ! -d $(BUILD_DEST)/include/linux ] ;  then\
		 mkdir -p $(BUILD_DEST)/include/linux; \
	fi
	@if [ ! -d $(BUILD_DEST)/include/linux/os ] ;  then\
		 mkdir -p $(BUILD_DEST)/include/linux/os; \
	fi
	@if [ ! -d $(BUILD_DEST)/include/linux_user ] ;  then\
		 mkdir -p $(BUILD_DEST)/include/linux_user; \
	fi
	@if [ ! -d $(BUILD_DEST)/include/linux_user/os ] ;  then\
		 mkdir -p $(BUILD_DEST)/include/linux_user/os; \
	fi
	@if [ ! -d $(BUILD_DEST)/lib/modules ] ;  then\
		 mkdir -p $(BUILD_DEST)/lib/modules; \
	fi
	@if [ ! -d $(BUILD_DEST)/etc/init.d ] ; then\
		mkdir -p $(BUILD_DEST)/etc/init.d; \
	fi

.PHONY: install_target	
install_target:	install_target_dirs
	@echo ">>>install target for  Components $(COMPONENT)"
	install -c -m 554 $(PWD)/linux_kernel/src/osal_linux.ko $(FSROOT)/lib/modules/osal_linux.ko
	install -c -m 554 $(PWD)/linux_user/src/libosal.so  $(FSROOT)/lib/libosal.so
	install -c -m 554 $(PWD)/linux_user/src/libosal.a  $(FSROOT)/lib/libosal.a
	mkdir -p $(FSROOT)/lib/udev/
	install -m 0755 $(PWD)/scripts/osal_fw_hotplug.sh $(FSROOT)/lib/udev/
	mkdir -p $(FSROOT)/etc/udev/rules.d/
	install -m 0600 $(PWD)/scripts/51-osal-udev.rules $(FSROOT)/etc/udev/rules.d/


.PHONY: install_target_dirs	
install_target_dirs:
	@if [ ! -d $(FSROOT)/lib/modules ] ; \
	then mkdir -p $(FSROOT)/lib/modules; \
	fi
	@if [ ! -d $(FSROOT)/etc/init.d ] ; then\
		mkdir -p $(FSROOT)/etc/init.d ; \
	fi

.PHONY: uninstall
uninstall:
	@echo ">>>uninstall all dev and target files of  Component: $(COMPONENT)"
	rm -f $(BUILD_DEST)/lib/modules/osal_linux.ko
	rm -f $(BUILD_DEST)/lib/libosal.so
	rm -f $(BUILD_DEST)/etc/init.d/osal
	#rm -f $(BUILD_DEST)/include/osal*.h
	#rm -f $(BUILD_DEST)/include/linux/stdint.h
	#rm -f $(BUILD_DEST)/include/linux/os/*.h
	#rm -f $(BUILD_DEST)/include/linux_user/os/*.h
	rm -f $(FSROOT)/lib/modules/osal_linux.ko
	rm -f $(FSROOT)/lib/libosal.so
	rm -f $(FSROOT)/lib/libosal.a
	rm -f $(FSROOT)/etc/init.d/osal


