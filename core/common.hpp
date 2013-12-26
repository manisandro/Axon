/** @file common.hpp
 *  @brief Common definitions.
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Hack for kdevelop parsing
#ifndef COMPILE
#define __AVR_ATmega640__
#include <avr/iom640.h>
#endif

// Helper macros
#define set_bits(reg, bits, mask) reg = (reg & ~mask) | bits
#define disable_interrupts uint8_t _sreg = SREG; cli()
#define restore_interrupts SREG = _sreg

// GIO control macros.
// * Every GIO has a data direction register (DDRn)
// * If DDRn = 0, the GIO is an input pin
// * If DDRn = 1, the GIO is an output port
// The macros below allow to define GIOs by letter and number, i.e.
// #define SYSTEM_LED         B,6
// and to operate on them, i.e.
// DD_OUT(SYSTEM_LED)
#ifndef cbi
#  define cbi(port, bit) (port) &= ~(1 << (bit))
#endif
#ifndef sbi
#  define sbi(port, bit) (port) |= (1 << (bit))
#endif

#define _cbi(t, l, bit) sbi(t##l, bit)
#define _sbi(t, l, bit) cbi(t##l, bit)
#define _bit_is_clear(t, l, bit) bit_is_clear(t##l, bit)
#define _bit_is_set(t, l, bit) bit_is_set(t##l, bit)
#define _set_bit(t, l, bit, val) (t##l) = (t##l) & ~(1 << bit) | (val << bit)

#define DD_IN(name)         _cbi(DDR, name)
#define DD_OUT(name)        _sbi(DDR, name)
#define PORT_OFF(name)      _sbi(PORT, name)
#define PORT_ON(name)       _cbi(PORT, name)
#define PORT_HIGH(name)     _sbi(PORT, name)
#define PORT_LOW(name)      _cbi(PORT, name)
#define PORT_SET(name, val) _set_bit(PORT, name, val)
#define PIN_IS_LOW(name)    _bit_is_clear(PIN, name)
#define PIN_IS_HIGH(name)   _bit_is_set(PIN, name)

#define BUTTON_PRESSED(name) _bit_is_clear(PIN, name)

// Constants
#define F_CPU         16000000        // 16MHz processor
#define CYCLES_PER_US (F_CPU/1000000) // CPU cycles per microsecond (16 for 16 MHz)
#define PI            3.14159265359

// Typedefs
typedef volatile uint8_t& sfr8_t;
typedef volatile uint16_t& sfr16_t;

#endif
