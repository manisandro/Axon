/** @file a2d.cpp
 *  @brief Analog-to-Digital converter, see Atmega640 documentation chapter 26.
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#include "a2d.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define ADCSRA_PRESCALE_MASK   0b00000111
#define ADMUX_REFERENCE_MASK   0b11000000
#define ADMUX_MUX_MASK         0b00011111
#define ADCSRB_MUX5_MASK       0b00100000
#define ADCSRA_ADTS_MASK       0b00000111

// See Atmega640 documentation chapter 26.8.1 and 26.8.2 and table 26-4 
static inline void _a2dSetChannel(uint8_t ch)
{
	if((ch & 0x20) == 0x20){ // 0x20 = 0010 0000 => if 5th bit is set, need to set MUX5
		sbi(ADCSRB, MUX5);
	}else{
		cbi(ADCSRB, MUX5);
	}
	ADMUX = (ADMUX & ~ADMUX_MUX_MASK) | (ch & ADMUX_MUX_MASK);
}

// Initialize A2D converter
void A2D::init(EPrescale prescale, EReference reference)
{
	sbi(ADCSRA, ADEN);              // enable ADC (turn on ADC power)
	cbi(ADMUX, ADLAR);              // set to right-adjusted result
	cbi(ADCSRA, ADATE);             // default to single sample convert mode
	A2D::setPrescaler(prescale);
	A2D::setReference(reference);
}

// Turn off A2D converter
void A2D::off()
{
	cbi(ADCSRA, ADIE);                // disable ADC interrupts
	cbi(ADCSRA, ADEN);                // disable ADC (turn off ADC power)
}

// See Atmega640 documentation chapter 26.8.3
void A2D::setPrescaler(EPrescale prescale)
{
	ADCSRA = (ADCSRA & ~ADCSRA_PRESCALE_MASK) | prescale;
}

// See Atmega640 documentation chapter 26.8.1
void A2D::setReference(EReference ref){
	ADMUX = (ADMUX & ~ADMUX_REFERENCE_MASK) | ref;
}

uint16_t A2D::convert10bit(EChannel ch)
{
	_a2dSetChannel(ch);
	sbi(ADCSRA, ADSC);               // Start conversion
	while(bit_is_set(ADCSRA, ADSC)); // Wait until conversion done
 	return ADC;
}

int16_t A2D::convertDiff10bit(EDiffChannel ch)
{
	_a2dSetChannel(ch);
	_delay_us(125);                  // See Atmega640 documentation chapter 26.5
	sbi(ADCSRA, ADSC);               // Start conversion
	while(bit_is_set(ADCSRA, ADSC)); // Wait until conversion done
	// The result is a 10bit signed integer: need to sign-extend it to 16-bits
	int16_t i = *reinterpret_cast<volatile int16_t*>(&ADC);
	return (i & 0x200) ? (i | 0xFC00) : i;
}

void A2D::disableDigitalInput(EChannel ch)
{
	if((ch & 0x20) == 0x20){ // 0x20 = 0010 0000 => if 5th bit is set, need to set MUX5
		DIDR2 &= ~(ch-0x20);
	}else{
		DIDR1 &= ~ch;
	}
}

void A2D::enableDigitalInput(EChannel ch)
{
	if((ch & 0x20) == 0x20){ // 0x20 = 0010 0000 => if 5th bit is set, need to set MUX5
		DIDR2 |= (ch-0x20);
	}else{
		DIDR1 |= ch;
	}
}
