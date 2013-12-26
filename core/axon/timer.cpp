/** @file timer.cpp
 *  @brief Timer function library, see Atmega640 documentation chapter 17.
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#include "timer.hpp"

#include <avr/pgmspace.h>

#define TCCRxA_COMxA_MASK 0xC0 // 1100 0000
#define TCCRxA_COM0B_MASK 0x30 // 0011 0000
#define TCCRxB_CS_MASK    0x07 // 0000 0111

///////////////////////////////////////////////////////////////////////////////

const uint16_t prescalingFactors[] PROGMEM = {0, 1, 8, 64, 256, 1024};


// Returns the prescaling factor set in the specified timer/counter control register B
static inline uint16_t _getTimerPrescaleFactor(uint8_t TCCRxB){
	const uint16_t* addr = prescalingFactors + (TCCRxB & TCCRxB_CS_MASK);
	return pgm_read_word(addr);
}

///////////////////////////////////////////////////////////////////////////////

static volatile uint16_t timer1_overflow_count;

void Timer1::disable()
{
	// Clear all interrupts, reset timer control registers
	TIMSK1 = TCCR1A = TCCR1B = TCCR1C = 0x00;
}

void Timer1::enable(EClk prescaler)
{
	disable();
	/* Wave generation mode is normal after reset (see tables 16-8, 17-2) */
	TCCR1B = (TCCR1B & ~TCCRxB_CS_MASK) | prescaler; /* Set prescaler */
	sbi(TIMSK1, TOIE1);                             /* Enable overflow interrupt */
	sei();
	restart();
}

void Timer1::restart(){
	timer1_overflow_count = TCNT1 = 0;
}

uint32_t Timer1::elapsed(){ // 16 bit
	uint32_t ticks = TCNT1 + 65536*timer1_overflow_count;
	uint32_t ticksPerMs = F_CPU/(_getTimerPrescaleFactor(TCCR1B)*1000);
	return ticks/ticksPerMs;
}

ISR(TIMER1_OVF_vect)
{
	++timer1_overflow_count;
}
