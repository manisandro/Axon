/** @file pwm.cpp
 *  @brief PWM function library, see Atmega640 documentation chapter 17.
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#include "pwm.hpp"

void PWM::initTimer3PWM(uint8_t activePorts)
{
	// Make the ports output ports
	DDRE |= 0b00111000;

	// ------00  WGM3{0,1}
	// ----00--  COM3C{0,1}
	// --00----  COM3B{0,1}
	// 00------  COM3A{0,1}
	TCCR3A = 0b00000000 | activePorts;

	// -----010  CS (Clock select) = clk/8
	// ---10---  WGM3{2,3}
	// -0------  ICES: Input edge select does not matter
	// 0-------  ICNC: No input noise cancelling
	TCCR3B = 0b00010010;

	// 20 ms PWM cycle time
	ICR3 = 20000;

	// => WGM = 1000 = Phase and Frequency correct PWM, TOP = ICR3 (See table 17-2)

	// Clear timer counter
	TCNT3 = 0x0000;
}

void PWM::initTimer4PWM(uint8_t activePorts)
{
	// Make the ports output ports
	DDRH |= 0b00111000;

	// ------00  WGM4{0,1}
	// ----00--  COM4C{0,1}
	// --00----  COM4B{0,1}
	// 00------  COM4A{0,1}
	TCCR4A = 0b00000000 | activePorts;

	// -----010  CS (Clock select) = clk/8
	// ---10---  WGM4{2,3}
	// -0------  ICES: Input edge select does not matter
	// 0-------  ICNC: No input noise cancelling
	TCCR4B = 0b00010010;

	// 20 ms PWM cycle time
	ICR4 = 20000;

	// => WGM = 1000 = Phase and Frequency correct PWM, TOP = ICR4 (See table 17-2)

	// Clear timer counter
	TCNT4 = 0x0000;
}
