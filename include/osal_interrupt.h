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

#ifndef _OSAL_INTERRUPT_H
#define _OSAL_INTERRUPT_H

/**
@defgroup interrupt OSAL interrupts

Provides OS independent interface for registering bottom half interrupt handlers
@{
*/


/**
 * This is an opaque data type that serves as a handle for an allocated
 * interrupt. It should not be modified or used for any purpose other than to
 * free the interrupt at a later time.
 */
typedef void * os_interrupt_t;

/**
 * This function pointer is used as an interrupt handler. When the interrupt is
 * triggered the function is called. The IRQ parameter is the integer number of
 * the irq that has been triggered, and the data pointer is the data provided
 * during the allocation of the interrupt.  
 *
 * These handlers run in the context of user level thread (pthreads) in Linux
 * user mode.
 *
 * The implementation for Linux kernel mode runs this code as an interrupt
 * handler (ISR) in kernel mode
 *
 * The following documentation exists in the orignal EID code:
 *
 * Interrupt handlers MUST meet these criteria
 * - They MUST NOT sleep.
 * - Interrupts are called with interrupts disabled or at least interrupts of
 *   equal or lower priority disabled.
 * - Any memory allocation, timers etc must not sleep.
 * - They MUST allow for interrupt sharing with other devices.
 * - They MUST return in a very short amount of time.
 *
 * @param data
 *      A void pointer to what ever data is needed by the handler.
 */
typedef void (os_interrupt_handler_t)(void *data);


/**
 * Used to Register a hard interrupt handler.
 *
 * Allows configuring a hard interrupt handler (a.k.a. a top half handler),
 * with all of the good and bad that comes with it.  Advantages include
 * generally lower latencies and less overhead.  The cost, is greatly reduced
 * access to OS functionality.  The handler may not do anything that would
 * sleep or schedule.  No calling sleep functions, memory mapping or allocation
 * functions, etc.  Assume anything other than register access and event wake
 * up functions are not safe unless their safety has been verified.
 *
 * There may also be shared interrupts.  An interrupt handler cannot assume
 * that because it was called, its device raised an interrupt.
 *
 * @param[in]  irqnum
 *      Number of the interrupt to register for.
 * @param[out] irqhandle
 *      If registration is successful, returns with a value used for
 *      unregistration later.
 * @param[in]  irqhandler
 *      Function to invoke when an interrupt occurs.
 * @param[in]  data
 *      User supplied pointer that will be passed to the irqhandler when when
 *      an interrupt occurs.
 * @param[in]  name
 *      String that may be used by the OS for information display purposes.
 */
 /* Legacy API is preserved to make the SDK buildable and will be removed soon */
osal_result os_register_top_half(
        int                     irqnum,
        os_interrupt_t *        irqhandle,
        os_interrupt_handler_t *irqhandler,
        void *                  data,
        const char *            name);

/**
 * Unregister a hard interrupt handler
 *
 * Removes an interrupt handler.  After this function returns, the interrupt
 * handler will not be called again.
 *
 * @param[in]  irqhandle
 *      Handle returned by registration to determine what handler to remove.
*/
/* Legacy API is preserved to make the SDK buildable and will be removed soon */
osal_result os_unregister_top_half(os_interrupt_t *irqhandle);

typedef enum {
	OS_IRQ_NONE = 0,
	OS_IRQ_HANDLED,
	OS_IRQ_WAKE_THREAD,
} os_irqreturn_t;

typedef os_irqreturn_t os_irq_handler_t(void *);
/**
 * @brief Register to receivers' shared interrrupts
 *
 * This function does the operating system specific calls for registering the 
 * top half and bottom half interrupt handler for the driver calling this function
 *
 * @param[in] irq           Indicates the device signaling the interrupt No.
 * @param[in] top_handler   Pointer to the top half interrupt handler which runs in hard IRQ (atomic) context(cannot sleep, cannot schedule,etc).
 * 						    It should clear the device interrupt when returning OS_IRQ_HANDLED, or it should disable device interrupt when returning OS_IRQ_WAKE_THREAD(bottom half interrrupt handler will be called).
 * @param[in] driver_name   ASCII string indicating the driver that the interrupt
 *                      is intended for.
 *
 * @param[in] bottom_handler       Pointer to the bottom half interrupt handler which runs in interrtupt thread context, the device interrupt should be 								cleared and re-enabled in the handler..
 * @param[in] data          Pointer to the context data for the handler.
 *                      registering the interrupt
 *
 * @retval handle       Handle to interrupt.
 */
os_interrupt_t os_acquire_shared_interrupt(int irq,
                                        os_irq_handler_t *top_handler,
                                        char *driver_name,
                                        os_interrupt_handler_t *bottom_handler,
                                        void *data);

 /**
* @brief  Releases the shared interrupt
*
* @param interrupt_handle   Handle to interrupt
*/
void os_release_shared_interrupt(os_interrupt_t interrupt_handle);

#define OS_DISABLE_INTERRUPTS _OS_DISABLE_INTERRUPTS
#define OS_ENABLE_INTERRUPTS  _OS_ENABLE_INTERRUPTS

/**
 * @brief Register to receiver interrrupts
 *
 * This function does the operating system specific calls for registering the 
 * bottom half interrupt handler for the driver calling this function
 *
 * @param[in] irq           Indicates the device signaling the interrupt.
 * @param[in] devicekey     Deprecated  paramter, we reserve it to keep the API no change. Any integer value is valid.
 * @param[in] driver_name   ASCII string indicating the driver that the interrupt
 *                      is intended for.
 *
 * @param[in] handler       Pointer to the interrupt handler.
 * @param[in] data          Pointer to the data needed by operating system for
 *                      registering the interrupt
 *
 * @retval handle       Handle to interrupt.
 */
os_interrupt_t os_acquire_interrupt(   int irq,
                                        int devicekey,
                                        char *driver_name,
                                        os_interrupt_handler_t *handler,
                                        void *data);

/**
 * @brief  Releases the interrupt
 *
 * @param interrupt_handle   Handle to interrupt
 */
void os_release_interrupt(os_interrupt_t interrupt_handle);


#endif
