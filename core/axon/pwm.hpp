/** @file pwm.hpp
 *  @brief PWM function library, see Atmega640 documentation chapter 17.
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#ifndef PWM_HPP
#define PWM_HPP

#include <common.hpp>

namespace PWM {
	enum PortE {
		E3 = 0b10000000,
		E4 = 0b00100000,
		E5 = 0b00001000 };

	enum PortH {
		H3 = 0b10000000,
		H4 = 0b00100000,
		H5 = 0b00001000
	};

	/** Initializes Timer 3 for PWM on ports E3, E4 and E5
	 * @param activePorts The ports to activate, a bit-ored combination of E3, E4 and E5
	 */
	void initTimer3PWM(uint8_t activePorts);

	/** Initializes Timer 4 for PWM on ports H3, H4 and H5
	 * @param activePorts The ports to activate, a bit-ored combination of H3, H4 and H5
	 */
	void initTimer4PWM(uint8_t activePorts);

	/** Sets the value of PWM E3
	 * @param val The pulse time in milliseconds, between 1000 and 2000
	 */
	inline void setE3(uint16_t val){ OCR3A = val; }

	/** Sets the value of PWM E4
	 * @param val The pulse time in milliseconds, between 1000 and 2000
	 */
	inline void setE4(uint16_t val){ OCR3B = val; }

	/** Sets the value of PWM E5
	 * @param val The pulse time in milliseconds, between 1000 and 2000
	 */
	inline void setE5(uint16_t val){ OCR3C = val; }

	/** Sets the value of PWM H3
	 * @param val The pulse time in milliseconds, between 1000 and 2000
	 */
	inline void setH3(uint16_t val){ OCR4A = val; }

	/** Sets the value of PWM H4
	 * @param val The pulse time in milliseconds, between 1000 and 2000
	 */
	inline void setH4(uint16_t val){ OCR4B = val; }

	/** Sets the value of PWM H5
	 * @param val The pulse time in milliseconds, between 1000 and 2000
	 */
	inline void setH5(uint16_t val){ OCR4C = val; }
}

#endif
