/** @file timer.hpp
 *  @brief Timer function library, see Atmega640 documentation chapter 17.
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#ifndef TIMER_HPP
#define TIMER_HPP

#include <common.hpp>

namespace Timer1 {
	/** Prescaler values **/
	enum EClk {
		CLK_STOP   = 0x00, //!< Timer Stopped
		CLK_1      = 0x01, //!< Timer clocked at F_CPU
		CLK_8      = 0x02, //!< Timer clocked at F_CPU/8
		CLK_64     = 0x03, //!< Timer clocked at F_CPU/64
		CLK_256    = 0x04, //!< Timer clocked at F_CPU/256
		CLK_1024   = 0x05, //!< Timer clocked at F_CPU/1024
		CLK_T_FALL = 0x06, //!< Timer clocked at T falling edge
		CLK_T_RISE = 0x07, //!< Timer clocked at T rising edge
	};

	/** Enables the timer
	 * @param prescaler The timer prescaler, @see EClk
	 */
	void enable(EClk prescaler);

	/** Disables the timer */
	void disable();

	/** Restarts the timer (the timer is expected to be enabled) */
	void restart();

	/** Return the elapsed time
	 * @return The elapsed time in milliseconds
	 */
	uint32_t elapsed();
}

#endif
