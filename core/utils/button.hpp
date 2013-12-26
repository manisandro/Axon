/** @file button.hpp
 *  @brief Utility class for controlling buttons
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <common.hpp>

/** Utility class for controlling buttons */
class Button {
public:
	/**
	 * @param ddr The data direction register of the pin to which the button is attached
	 * @param pin The pin to which the button is attached
	 * @param num The bit of the pin to which the button is attached
	 */
	Button(const sfr8_t ddr, const sfr8_t pin, const uint8_t num)
		: m_pin(pin), m_num(num), m_wasPressed(false)
		{
			// Make the bit an input pin
			cbi(ddr, num);
		}
	/**
	 * @return Whether the button was pressed
	 */
	bool pressed(){
		if(bit_is_clear(m_pin, m_num)){
			if(!m_wasPressed){
				m_wasPressed = true;
				return true;
			}
		}else{
			m_wasPressed = false;
		}
		return false;
	}
	
private:
	const sfr8_t m_pin;
	const uint8_t m_num;
	bool m_wasPressed;
};

#endif // BUTTON_HPP
