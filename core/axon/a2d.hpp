/** @file a2d.hpp
 *  @brief Analog-to-Digital converter, see Atmega640 documentation chapter 26.
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#ifndef A2D_HPP
#define A2D_HPP

#include <common.hpp>

namespace A2D {
/** A2D clock prescalers
 *  - freq(A2D) = freq(CPU)/prescaler
 *  - lower division ratios make conversion go faster
 *  - higher division ratios make conversions more accurate
 */
enum EPrescale {
	PRESCALE_DIV2 = 0x01,  //!< CPU clk/2
	PRESCALE_DIV4 = 0x02,  //!< CPU clk/4
	PRESCALE_DIV8 = 0x03,  //!< CPU clk/8
	PRESCALE_DIV16 = 0x04, //!< CPU clk/16
	PRESCALE_DIV32 = 0x05, //!< CPU clk/32
	PRESCALE_DIV64 = 0x06, //!< CPU clk/64
	PRESCALE_DIV128 = 0x07 //!< CPU clk/128
};

/** A2D reference voltage */
enum EReference {
	REFERENCE_AREF = 0x00, //!< AREF pin, internal VREF turned off
	REFERENCE_AVCC = 0x40, //!< AVCC pin, internal VREF turned off
	REFERENCE_110V = 0x80, //!< Internal 1.1V VREF
	REFERENCE_256V = 0xC0  //!< Internal 2.56V VREF
};

/** A2D single ended input channels */
enum EChannel {
	CH_0    = 0x00,
	CH_1    = 0x01,
	CH_2    = 0x02,
	CH_3    = 0x03,
	CH_4    = 0x04,
	CH_5    = 0x05,
	CH_6    = 0x06,
	CH_7    = 0x07,
	CH_8    = 0x20,
	CH_9    = 0x21,
	CH_10   = 0x22,
	CH_11   = 0x23,
	CH_12   = 0x24,
	CH_13   = 0x25,
	CH_14   = 0x26,
	CH_15   = 0x27,
	CH_110V = 0x1E, //!< 1.1V voltage reference
	CH_GRND = 0x1F  //!< 0V (ground)
};

/** A2D differential input channels
 *  - CH_{P}_{N}_DIFF{G}
 *  - P = Positive differential input
 *  - N = Negative differential input
 *  - G = Gain
 */
enum EDiffChannel {
	CH_0_0_DIFF10X    = 0x08,
	CH_1_0_DIFF10X    = 0x09,
	CH_0_0_DIFF200X   = 0x0A,
	CH_1_0_DIFF200X   = 0x0B,
	CH_2_2_DIFF10X    = 0x0C,
	CH_3_2_DIFF10X    = 0x0D,
	CH_2_2_DIFF200X   = 0x0E,
	CH_3_2_DIFF200X   = 0x0F,
	CH_0_1_DIFF1X     = 0x10,
	CH_1_1_DIFF1X     = 0x11,
	CH_2_1_DIFF1X     = 0x12,
	CH_3_1_DIFF1X     = 0x13,
	CH_4_1_DIFF1X     = 0x14,
	CH_5_1_DIFF1X     = 0x15,
	CH_6_1_DIFF1X     = 0x16,
	CH_7_1_DIFF1X     = 0x17,
	CH_0_2_DIFF1X     = 0x18,
	CH_1_2_DIFF1X     = 0x19,
	CH_2_2_DIFF1X     = 0x1A,
	CH_3_2_DIFF1X     = 0x1B,
	CH_4_2_DIFF1X     = 0x1C,
	CH_5_2_DIFF1X     = 0x1D,
	CH_8_8_DIFF10X    = 0x28,
	CH_9_8_DIFF10X    = 0x29,
	CH_8_8_DIFF200X   = 0x2A,
	CH_9_8_DIFF200X   = 0x2B,
	CH_10_10_DIFF10X  = 0x2C,
	CH_11_10_DIFF10X  = 0x2D,
	CH_10_10_DIFF200X = 0x2E,
	CH_11_10_DIFF200X = 0x2F,
	CH_8_9_DIFF1X     = 0x30,
	CH_9_9_DIFF1X     = 0x31,
	CH_10_9_DIFF1X    = 0x32,
	CH_11_9_DIFF1X    = 0x33,
	CH_12_9_DIFF1X    = 0x34,
	CH_13_9_DIFF1X    = 0x35,
	CH_14_9_DIFF1X    = 0x36,
	CH_15_9_DIFF1X    = 0x37,
	CH_8_10_DIFF1X    = 0x38,
	CH_9_10_DIFF1X    = 0x39,
	CH_10_10_DIFF1X   = 0x3A,
	CH_11_10_DIFF1X   = 0x3B,
	CH_12_10_DIFF1X   = 0x3C,
	CH_13_10_DIFF1X   = 0x3D
};

/** Initializes the A2D converter
 *  Turns A2D converter on and prepares it for use.
 * @param prescale The clock division ratio, @see setPrescaler, defaults to PRESCALE_DIV64
 * @param reference The reference voltage, @see setReference, defaults to REFERENCE_AVCC
 */
void init(EPrescale prescale = PRESCALE_DIV64, EReference reference = REFERENCE_AVCC);

/** Turn off A2D converter */
void off();

/** Performs conversion on specified single-ended channel
 * @param ch A single-ended A2D channel, @see EChannel
 * @return Converted value in 10-bit precision
 */
uint16_t convert10bit(EChannel ch);

/** Performs conversion on specified differential channel
 *  - The result of the conversion is
 *     1024*(V_IN/V_REF)
 * @param ch A differential A2D channel, @see EDiffChannel
 * @return Converted value in 10-bit precision
 */
int16_t convertDiff10bit(EDiffChannel ch);

/** Sets the division ratio of the A2D converter clock
 *  Automatically called from a2dInit() with a default value.
 *  - Has no effect in autotrigger mode.
 *  - If a 1x or 10x gain channel is used, 8-bit resolution can be expected
 *  - If a 200x gain channel is used, 7-bit resolution can be expected
 *  - The result of the conversion is
 *     512*(V_POS - V_NEG)/(V_REF/GAIN)
 *  @param prescale A prescale symbolic constant, @see EPrescale
 */
void setPrescaler(EPrescale prescale);

/** Sets reference voltage for the A2D converter
 *  Automatically called from a2dInit() with a default value.
 *  @param ref A reference voltage symbolic constant, @see EReference
 */
void setReference(EReference ref);

/** Disable digital input buffer on ADC pin to conserve power
 *  @param ch An A2D channel, @see EChannel
 */
void disableDigitalInput(EChannel ch);

/** Enable digital input buffer on ADC pin
 *  @param ch An A2D channel, @see EChannel
 */
void enableDigitalInput(EChannel ch);

}
#endif
