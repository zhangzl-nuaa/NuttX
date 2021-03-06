/************************************************************************************
 * arch/arm/src/lm/lm_gpio.h
 *
 *   Copyright (C) 2009-2010, 2013 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ************************************************************************************/

#ifndef __ARCH_ARM_SRC_LM_LM_GPIO_H
#define __ARCH_ARM_SRC_LM_LM_GPIO_H

/************************************************************************************
 * Included Files
 ************************************************************************************/

#include <nuttx/config.h>
#include <nuttx/compiler.h>

#include <stdint.h>
#include <stdbool.h>

#include "up_internal.h"
#include "chip.h"

/************************************************************************************
 * Pre-processor Definitions
 ************************************************************************************/

/* Bit-encoded input to lm_configgpio() *********************************************/

/* Encoding:
 *
 * LM3S: FFFS SPPP III. .... .... .... VPPP PBBB
 * LM4F: FFFS SPPP III. AAAA .... .... VPPP PBBB
 *
 * TODO: The LM4F also supports configuration of pins to trigger ADC and/or uDMA.
 * That configuratin is not addressed in this this encoding.
 */

/* These bits set the primary function of the pin:
 * FFFn .... .... .... .... .... .... ....
 */

#define GPIO_FUNC_SHIFT               29                         /* Bit 31-29: GPIO function */
#define GPIO_FUNC_MASK                (7 << GPIO_FUNC_SHIFT)     /* (See table 9-1 in data sheet) */
#  define GPIO_FUNC_INPUT             (0 << GPIO_FUNC_SHIFT)     /*   Digital GPIO input */
#  define GPIO_FUNC_OUTPUT            (1 << GPIO_FUNC_SHIFT)     /*   Digital GPIO output */
#  define GPIO_FUNC_ODINPUT           (2 << GPIO_FUNC_SHIFT)     /*   Open-drain GPIO input */
#  define GPIO_FUNC_ODOUTPUT          (3 << GPIO_FUNC_SHIFT)     /*   Open-drain GPIO output */
#  define GPIO_FUNC_PFODIO            (4 << GPIO_FUNC_SHIFT)     /*   Open-drain input/output (I2C) */
#  define GPIO_FUNC_PFINPUT           (5 << GPIO_FUNC_SHIFT)     /*   Digital input (Timer, CCP) */
#  define GPIO_FUNC_PFOUTPUT          (5 << GPIO_FUNC_SHIFT)     /*   Digital output (Timer, PWM, Comparator) */
#  define GPIO_FUNC_PFIO              (5 << GPIO_FUNC_SHIFT)     /*   Digital input/output (SSI, UART) */
#  define GPIO_FUNC_ANINPUT           (6 << GPIO_FUNC_SHIFT)     /*   Analog input (ADC, Comparator) */
#  define GPIO_FUNC_ANIO              (6 << GPIO_FUNC_SHIFT)     /*   REVISIT: Analog input/output (USB) */
#  define GPIO_FUNC_INTERRUPT         (7 << GPIO_FUNC_SHIFT)     /*   Interrupt function */
#  define GPIO_FUNC_MAX               GPIO_FUNC_INTERRUPT

/* That primary may be modified by the following options
 * ...S SPPP .... .... .... .... .... ....
 */

#define GPIO_STRENGTH_SHIFT           27                         /* Bits 28-27: Pad drive strength */
#define GPIO_STRENGTH_MASK            (3 << GPIO_STRENGTH_SHIFT)
#  define GPIO_STRENGTH_2MA           (0 << GPIO_STRENGTH_SHIFT) /*   2mA pad drive strength */
#  define GPIO_STRENGTH_4MA           (1 << GPIO_STRENGTH_SHIFT) /*   4mA pad drive strength */
#  define GPIO_STRENGTH_8MA           (2 << GPIO_STRENGTH_SHIFT) /*   8mA pad drive strength */
#  define GPIO_STRENGTH_8MASC         (3 << GPIO_STRENGTH_SHIFT) /*   8mA Pad drive with slew rate control */
#  define GPIO_STRENGTH_MAX           GPIO_STRENGTH_8MASC

#define GPIO_PADTYPE_SHIFT            24                         /* Bits 26-24: Pad type */
#define GPIO_PADTYPE_MASK             (7 << GPIO_PADTYPE_SHIFT)
#  define GPIO_PADTYPE_STD            (0 << GPIO_PADTYPE_SHIFT)  /*   Push-pull */
#  define GPIO_PADTYPE_STDWPU         (1 << GPIO_PADTYPE_SHIFT)  /*   Push-pull with weak pull-up */
#  define GPIO_PADTYPE_STDWPD         (2 << GPIO_PADTYPE_SHIFT)  /*   Push-pull with weak pull-down */
#  define GPIO_PADTYPE_OD             (3 << GPIO_PADTYPE_SHIFT)  /*   Open-drain */
#  define GPIO_PADTYPE_ODWPU          (4 << GPIO_PADTYPE_SHIFT)  /*   Open-drain with weak pull-up */
#  define GPIO_PADTYPE_ODWPD          (5 << GPIO_PADTYPE_SHIFT)  /*   Open-drain with weak pull-down */
#  define GPIO_PADTYPE_ANALOG         (6 << GPIO_PADTYPE_SHIFT)  /*   Analog comparator */

/* If the pin is an interrupt, then the following options apply
 * .... .... III. .... .... .... .... ....
 */

#define GPIO_INT_SHIFT                21                         /* Bits 23-21: Interrupt type */
#define GPIO_INT_MASK                 (7 << GPIO_INT_SHIFT)
#  define GPIO_INT_FALLINGEDGE        (0 << GPIO_INT_SHIFT)      /*   Interrupt on falling edge */
#  define GPIO_INT_RISINGEDGE         (1 << GPIO_INT_SHIFT)      /*   Interrupt on rising edge */
#  define GPIO_INT_BOTHEDGES          (2 << GPIO_INT_SHIFT)      /*   Interrupt on both edges */
#  define GPIO_INT_LOWLEVEL           (3 << GPIO_INT_SHIFT)      /*   Interrupt on low level */
#  define GPIO_INT_HIGHLEVEL          (4 << GPIO_INT_SHIFT)      /*   Interrupt on high level */

/* The LM4F120 supports up to 15 alternate functions per pin:
 * LM4F: .... .... .... AAAA .... .... .... ....
 */

#ifdef LM4F
#  define GPIO_ALT_SHIFT              16                         /* Bits 16-19: Alternate function */
#  define GPIO_ALT_MASK               (15 << GPIO_ALT_SHIFT)
#    define GPIO_ALT(n)               ((n) << GPIO_ALT_SHIFT)
#    define GPIO_ALT_NONE             (0  << GPIO_ALT_SHIFT)
#    define GPIO_ALT_1                (1  << GPIO_ALT_SHIFT)
#    define GPIO_ALT_2                (2  << GPIO_ALT_SHIFT)
#    define GPIO_ALT_3                (3  << GPIO_ALT_SHIFT)
#    define GPIO_ALT_4                (4  << GPIO_ALT_SHIFT)
#    define GPIO_ALT_5                (5  << GPIO_ALT_SHIFT)
#    define GPIO_ALT_6                (6  << GPIO_ALT_SHIFT)
#    define GPIO_ALT_7                (7  << GPIO_ALT_SHIFT)
#    define GPIO_ALT_8                (8  << GPIO_ALT_SHIFT)
#    define GPIO_ALT_9                (9  << GPIO_ALT_SHIFT)
#    define GPIO_ALT_10               (10 << GPIO_ALT_SHIFT)
#    define GPIO_ALT_11               (11 << GPIO_ALT_SHIFT)
#    define GPIO_ALT_12               (12 << GPIO_ALT_SHIFT)
#    define GPIO_ALT_13               (13 << GPIO_ALT_SHIFT)
#    define GPIO_ALT_14               (14 << GPIO_ALT_SHIFT)
#    define GPIO_ALT_15               (15 << GPIO_ALT_SHIFT)
#endif

/* If the pin is an GPIO digital output, then this identifies the initial output value:
 * .... .... .... .... .... .... V... ....
 */

#define GPIO_VALUE_SHIFT              7                          /* Bit 7: If output, inital value of output */
#define GPIO_VALUE_MASK               (1 << GPIO_VALUE_SHIFT)
#  define GPIO_VALUE_ZERO             (0 << GPIO_VALUE_SHIFT)    /*   Initial value is zero */
#  define GPIO_VALUE_ONE              (1 << GPIO_VALUE_SHIFT)    /*   Initial value is one */

/* This identifies the GPIO port
 * .... .... .... .... .... .... .PPP P...
 */

#define GPIO_PORT_SHIFT               3                          /* Bit 3-6:  Port number */
#define GPIO_PORT_MASK                (15 << GPIO_PORT_SHIFT)
#  define GPIO_PORTA                  (0 << GPIO_PORT_SHIFT)     /*   GPIOA */
#  define GPIO_PORTB                  (1 << GPIO_PORT_SHIFT)     /*   GPIOB */
#  define GPIO_PORTC                  (2 << GPIO_PORT_SHIFT)     /*   GPIOC */
#  define GPIO_PORTD                  (3 << GPIO_PORT_SHIFT)     /*   GPIOD */
#  define GPIO_PORTE                  (4 << GPIO_PORT_SHIFT)     /*   GPIOE */
#  define GPIO_PORTF                  (5 << GPIO_PORT_SHIFT)     /*   GPIOF */
#  define GPIO_PORTG                  (6 << GPIO_PORT_SHIFT)     /*   GPIOG */
#  define GPIO_PORTH                  (7 << GPIO_PORT_SHIFT)     /*   GPIOH */
#  define GPIO_PORTJ                  (8 << GPIO_PORT_SHIFT)     /*   GPIOJ */

/* This identifies the pin number in the port:
 * .... .... .... .... .... .... .... .BBB
 */

#define GPIO_PIN_SHIFT                 0                           /* Bits 0-2: GPIO pin: 0-7 */
#define GPIO_PIN_MASK                 (7 << GPIO_PIN_SHIFT)
#  define GPIO_PIN_0                  (0 << GPIO_PIN_SHIFT)
#  define GPIO_PIN_1                  (1 << GPIO_PIN_SHIFT)
#  define GPIO_PIN_2                  (2 << GPIO_PIN_SHIFT)
#  define GPIO_PIN_3                  (3 << GPIO_PIN_SHIFT)
#  define GPIO_PIN_4                  (4 << GPIO_PIN_SHIFT)
#  define GPIO_PIN_5                  (5 << GPIO_PIN_SHIFT)
#  define GPIO_PIN_6                  (6 << GPIO_PIN_SHIFT)
#  define GPIO_PIN_7                  (7 << GPIO_PIN_SHIFT)

/************************************************************************************
 * Public Types
 ************************************************************************************/

/************************************************************************************
 * Inline Functions
 ************************************************************************************/

#ifndef __ASSEMBLY__

/************************************************************************************
 * Public Data
 ************************************************************************************/

#if defined(__cplusplus)
extern "C"
{
#endif

/************************************************************************************
 * Public Function Prototypes
 ************************************************************************************/

/************************************************************************************
 * Name: lm_configgpio
 *
 * Description:
 *   Configure a GPIO pin based on bit-encoded description of the pin.
 *
 ************************************************************************************/

int lm_configgpio(uint32_t cfgset);

/************************************************************************************
 * Name: lm_gpiowrite
 *
 * Description:
 *   Write one or zero to the selected GPIO pin
 *
 ************************************************************************************/

void lm_gpiowrite(uint32_t pinset, bool value);

/************************************************************************************
 * Name: lm_gpioread
 *
 * Description:
 *   Read one or zero from the selected GPIO pin
 *
 ************************************************************************************/

bool lm_gpioread(uint32_t pinset, bool value);

/************************************************************************************
 * Function:  lm_dumpgpio
 *
 * Description:
 *   Dump all GPIO registers associated with the provided base address
 *
 ************************************************************************************/

int lm_dumpgpio(uint32_t pinset, const char *msg);

/************************************************************************************
 * Name: gpio_irqinitialize
 *
 * Description:
 *   Initialize all vectors to the unexpected interrupt handler
 *
 ************************************************************************************/

int weak_function gpio_irqinitialize(void);

#if defined(__cplusplus)
}
#endif

#endif /* __ASSEMBLY__ */
#endif /* __ARCH_ARM_SRC_LM_LM_GPIO_H */
