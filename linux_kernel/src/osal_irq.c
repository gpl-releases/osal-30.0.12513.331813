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

#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include "osal.h"

#include <linux/sched.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 39)
#include <linux/irq.h>
#include <linux/irqdesc.h>
#endif

typedef struct {
    int                     irqnum;
    os_irq_handler_t       *irq_handler;
    os_interrupt_handler_t *interrupt_handler;
    void *                  data;
	bool 					init;
} os_irq_t;

typedef struct {
    int             irqnum;
    os_event_t      event;
    os_interrupt_t  irqhandle;
    int             disabled;
    int             received;
} os_irqproxy_t;

typedef struct _linux_int {
    int                     irq;
    void *                  data;
    wait_queue_head_t       wait_queue_head;
    struct task_struct *    task;
    bool                    pending;
    bool                    stop;
    unsigned int            devicekey;
    os_interrupt_t          irqhandle;
    os_interrupt_handler_t *handler;
} linux_int_t;


static struct proc_dir_entry *procfile;

static irqreturn_t os_irq_wrapper(int irqnum, void *data)
{
	os_irqreturn_t result;
	irqreturn_t ret;

    os_irq_t *irq = data;
 	result = irq->irq_handler(irq->data);
	switch (result) {
		case OS_IRQ_HANDLED:
			ret = IRQ_HANDLED;
			break;
		case OS_IRQ_WAKE_THREAD:
			ret = IRQ_WAKE_THREAD;
			break;
		case OS_IRQ_NONE:
		default:
			ret = IRQ_NONE;
			break;
	}
	return ret;
}

static irqreturn_t os_interrupt_wrapper(int irqnum, void *data)
{
    os_irq_t *irq = data;
    irq->interrupt_handler(irq->data);
    return IRQ_HANDLED;
}

static irqreturn_t os_thread_wrapper(int irqnum, void *data)
{
    os_irq_t *irq = data;
	if (!irq->init) {
    	struct          sched_param prio = {MAX_RT_PRIO - 1};

    		// set the priority of the kernel thread
    		if(sched_setscheduler(current, SCHED_FIFO, &prio)){
    	    		OS_PRINT("Couldn't set scheduler priority to %d!\n", prio.sched_priority);
		} else {
			 irq->init = true;
		}
	}
    irq->interrupt_handler(irq->data);
    return IRQ_HANDLED;
}

osal_result os_register_top_half(   int irqnum,
                                    os_interrupt_t *irqhandle,
                                    os_interrupt_handler_t *irqhandler,
                                    void *data,
                                    const char *name)
{
    os_irq_t *irq;

    if(!irqhandle || !irqhandler || !name) {
        return OSAL_INVALID_PARAM;
    }

    irq = OS_ALLOC(sizeof(os_irq_t));
    if(!irq) {
        return OSAL_ERROR;
    }

    irq->irqnum = irqnum;
    irq->interrupt_handler = irqhandler;
    irq->data = data;

    if(request_irq(irqnum, os_interrupt_wrapper, IRQF_SHARED, name, irq)){
        OS_FREE(irq);
        return OSAL_ERROR;
    }
    *irqhandle = (void *)irq;
    return OSAL_SUCCESS;
}

osal_result os_unregister_top_half(os_interrupt_t *irqhandle)
{
    os_irq_t *irq = *irqhandle;

    free_irq(irq->irqnum, irq);
    OS_FREE(irq);
    *irqhandle = NULL;
    return OSAL_SUCCESS;
}

// disable the interrupts from this device
void check_and_disable_device_irq(int irq)
{
    unsigned long flags;
    struct irq_desc *desc = irq_to_desc(irq);
    spin_lock_irqsave(&desc->lock, flags);
/*
       The irq_data and irq_chip struct varies at each version after 2.6.35
       Below code only support 2.6.39 after 2.6.35.  
*/
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 35)
    desc->chip->mask(irq);
#else
	// 2.6.39 style irq mask opeartion
    desc->irq_data.chip->irq_mask(irq_get_irq_data(irq));
#endif
    spin_unlock_irqrestore(&desc->lock, flags);
}


// re-enable interrupts from this device
void enable_device_irq(int irq)
{
    unsigned long   flags;

    struct irq_desc *desc = irq_to_desc(irq);
    spin_lock_irqsave(&desc->lock, flags);
/*
       The irq_data and irq_chip struct varies at each version after 2.6.35
       Below code only support 2.6.39 after 2.6.35.  
*/
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 35)
    desc->chip->unmask(irq);
#else
	// 2.6.39 style irq unmask opeartion
    desc->irq_data.chip->irq_unmask(irq_get_irq_data(irq));
#endif
    spin_unlock_irqrestore(&desc->lock, flags);
}


int kthread_wrapper(void *data)
{
    DEFINE_WAIT(wait);
    linux_int_t *   linux_int = (linux_int_t *)data;
    struct          sched_param prio = {MAX_RT_PRIO - 1};

    // set the priority of the kernel thread
    if(sched_setscheduler(current, SCHED_FIFO, &prio)){
        OS_PRINT("Couldn't set scheduler priority to %d!\n", prio.sched_priority);
    }

    while(0 == kthread_should_stop()) {
        // Return type is void
        prepare_to_wait(&linux_int->wait_queue_head, &wait, TASK_INTERRUPTIBLE);
        if(false == linux_int->pending) {
            schedule();
        }
        // Return type is void
        finish_wait(&linux_int->wait_queue_head, &wait);

        OS_DEBUG("Kernel thread woken up\n");
        // what do we do with signals
        // signal_pending(..) etc

        if(true == linux_int->pending) {
            // Call the handler and then re-enable the IRQ
            linux_int->handler(linux_int->data);

            // Reset the pending status
            linux_int->pending = false;
            enable_device_irq(linux_int->irq);
        }

        // check if we are being asked to stop
        // done in while loop
    }
	
    OS_DEBUG("thread wrapper returning\n");

    return 1;
}

void linux_interrupt_handler(void *data)
{
    linux_int_t *linux_int = (linux_int_t *)data;

    check_and_disable_device_irq(linux_int->irq);
    linux_int->pending = true;

    // Wake up kernel thread
    wake_up_interruptible(&linux_int->wait_queue_head);
}

#define MIN(a, b) ((a) < (b) ? (a):(b))

os_interrupt_t os_acquire_interrupt(   int                     irq,
                                        int                     device_key,
                                        char *                  driver_name,
                                        os_interrupt_handler_t *handler,
                                        void *                  data)
{
    int             ret;
    char            kthread_name[80];
    int             name_len = 0;
    linux_int_t *   linux_int = (linux_int_t *)OS_ALLOC(sizeof(linux_int_t));

    if(!linux_int) {
        OS_ERROR("No memory for interrupt.\n");
        return NULL;
    }

    OS_MEMSET(linux_int, 0, sizeof(linux_int_t));
    linux_int->irq      = irq;
    linux_int->data     = data;
    linux_int->handler  = handler;
    linux_int->pending  = false;
    linux_int->devicekey= device_key;

    if(driver_name != NULL) {
        name_len = MIN((strlen(driver_name)+1), sizeof(kthread_name));
        strncpy(kthread_name, driver_name, name_len);
        kthread_name[name_len - 1]='\0';
    } else {
        sprintf(kthread_name, "pal irq %d thread\n", irq);
    }

    init_waitqueue_head(&linux_int->wait_queue_head);
    linux_int->task = kthread_run(  kthread_wrapper,
                                    (void *)linux_int,
                                    "%s",
                                    kthread_name);

    if (IS_ERR(linux_int->task)) {
        OS_ERROR("kthread_run failed\n");
        OS_FREE(linux_int);
        linux_int = NULL;
    } else {
        OS_DEBUG("kthread_run succeeded\n");
    }

    if(linux_int){
        ret = os_register_top_half( irq,
                                    &linux_int->irqhandle,
                                    &linux_interrupt_handler,
                                    (void *)linux_int,
                                    driver_name);

        if(ret) {
            OS_ERROR("Request IRQ returned %d for irq %d\n", ret, irq);
            kthread_stop(linux_int->task);
            OS_FREE(linux_int);
            return NULL;
        }
    }

    OS_DEBUG("Installed handler: %p for interrupt %d\n", (void*)handler, irq);

    return (os_interrupt_t)linux_int;
}


void os_release_interrupt(os_interrupt_t interrupt)
{
    linux_int_t *linux_int = (linux_int_t *)interrupt;

    if(!linux_int) {
        OS_ERROR("parameter interrupt is NULL\n");
        return;
    }

    // Free the IRQ, this will ensure that the interrupt is disabled.
    // Return type is void
    OS_DEBUG("Freeing IRQ..");
    os_unregister_top_half(&linux_int->irqhandle);

    // this is a hack until the priority of the interrupt thread is fixed
    linux_int->pending = false;
    OS_DEBUG("IRQ freed\n");

    // wakes up the thread, sets kthread_should_stop() to true, and waits for thread to exit
    if( -EINTR == kthread_stop(linux_int->task)) {
        OS_ERROR("Error in kthread_stop\n");
    }
	
    OS_FREE(linux_int);
}


os_interrupt_t os_acquire_shared_interrupt( int irqnum,
                                            os_irq_handler_t *top_handler,
                                            char *driver_name,
                                            os_interrupt_handler_t *bottom_handler,
                                            void *data)
{
    os_irq_t *irq;

    if(!top_handler || !driver_name || !data) {
        return NULL;
    }

    irq = OS_ALLOC(sizeof(os_irq_t));
    if(!irq) {
        return NULL;
    }

    irq->irqnum = irqnum;
    irq->irq_handler = top_handler;
    irq->interrupt_handler = bottom_handler;
    irq->data = data;
	irq->init = false;
    if(request_threaded_irq(irqnum, os_irq_wrapper, os_thread_wrapper, IRQF_SHARED, driver_name, irq)){
        OS_FREE(irq);
        return NULL;
    }
    return (os_interrupt_t)irq;
}

void os_release_shared_interrupt(os_interrupt_t interrupt_handle)
{
    os_irq_t *irq = (os_irq_t *)interrupt_handle;
	
	if (!irq)
		return;
	free_irq(irq->irqnum, irq);
	OS_FREE(irq);
}

void osal_irqproxy_handler(void *data)
{
    os_irqproxy_t *irqproxy = data;
    os_event_set(&irqproxy->event);
    disable_irq_nosync(irqproxy->irqnum);
    irqproxy->disabled = 1;
}

static int osal_irqproxy_open(struct inode *inode, struct file *file)
{
    file->private_data = NULL;
    return 0;
}

static ssize_t osal_irqproxy_read(  struct file *file,
                                    char __user *buffer,
                                    size_t size,
                                    loff_t *offset)
{
    os_irqproxy_t *irqproxy = file->private_data;

    if(!irqproxy) {
        return 0;
    }
    if(irqproxy->disabled){
        if(irqproxy->received == 1){
            irqproxy->disabled = 0;
            enable_irq(irqproxy->irqnum);
        }
        else{
            irqproxy->received = 1;
            return 0;        
        }
    }
    if(os_event_wait(&irqproxy->event, 1000) != OSAL_SUCCESS){
	irqproxy->received = 0;
        return -EINTR;
    }
    irqproxy->received = 1;
    return 0;
}

static ssize_t osal_irqproxy_write( struct file *file,
                                    const char __user *buffer,
                                    size_t size,
                                    loff_t *offset)
{
    os_irqproxy_t *irqproxy;
    unsigned char irqnum;

    // Check that we haven't been called before
    // Only a write of one byte is valid
    if(file->private_data || size != 1) {
        return -EINVAL;
    }

    //get the irq number
    if(copy_from_user(&irqnum, buffer, 1)) {
        return -EFAULT;
    }

    // zero is not valid
    if(!irqnum) {
        return -EINVAL;
    }

    irqproxy = OS_ALLOC(sizeof(os_irqproxy_t));
    if(!irqproxy) {
        return -ENOMEM;
    }

    irqproxy->irqnum = irqnum;
    irqproxy->disabled = 0;
    irqproxy->received = 1;
    os_event_create(&irqproxy->event, 0);

    if(os_register_top_half(irqnum, &irqproxy->irqhandle, osal_irqproxy_handler, irqproxy, "irqproxy") != OSAL_SUCCESS){
        os_event_destroy(&irqproxy->event);
        OS_FREE(irqproxy);
        return -EINVAL;
    }

    file->private_data = irqproxy;
    return 0;
}

static int osal_irqproxy_release(struct inode *inode, struct file *file)
{
    os_irqproxy_t *irqproxy = file->private_data;

    if(file->private_data){
        os_unregister_top_half(&irqproxy->irqhandle);
        if(irqproxy->disabled || os_event_wait(&irqproxy->event, 0) == OSAL_SUCCESS){
            OS_INFO("cleaning up irq %d proxy while an IRQ was pending!\n",
                    irqproxy->irqnum);
            enable_irq(irqproxy->irqnum);
        }
        os_event_destroy(&irqproxy->event);
        OS_FREE(file->private_data);
    }
    return 0;
}


static struct file_operations irqproxy_fops = {
    .open    = osal_irqproxy_open,
    .read    = osal_irqproxy_read,
    .write   = osal_irqproxy_write,
    .release = osal_irqproxy_release,
};

osal_result osal_irqproxy_init(void)
{

    procfile = create_proc_entry("irqproxy", S_IRUSR | S_IWUSR, NULL);
    if(!procfile){
        OS_INFO("Cannot create osal irqproxy access file!\n");
        return OSAL_ERROR;
    }
    procfile->proc_fops = &irqproxy_fops;

    return OSAL_SUCCESS;
}

osal_result osal_irqproxy_exit(void)
{
    if(procfile){
        remove_proc_entry("irqproxy", NULL);
        procfile = NULL;
    }
    return OSAL_SUCCESS;
}
