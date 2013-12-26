/** @file ILI9325.hpp
 *  @brief Driver for the ILI9325 LCD display
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#ifndef ILI9325_HPP
#define ILI9325_HPP

#include <common.hpp>
#include <avr/pgmspace.h>
#include <stdio.h>

/** Driver for the ILI9325 LCD display */
class ILI9325 {
public:
	/** Display rotation:
	* ROT_0: Portrait, normal
	* ROT_90: Landscape, normal
	* ROT_180: Portrait, inverse
	* ROT_270: Landscape, inverse
	*/
	enum ERot { ROT_0 = 0, ROT_90 = 1, ROT_180 = 2, ROT_270 = 3 };

	/** Line directions: */
	enum EDir { DIR_H = 0, DIR_V = 1 };

	/** Text alignment */
	enum EAlign { ALIGN_LEFT = 0, ALIGN_CENTER = 1, ALIGN_RIGHT = 2 };

	/** The type for the internal color format */
	typedef uint16_t rgb565_t;

	/** Stream state, can be used with printf and similar */
	struct Stream {
		typedef struct __file fdev_t;
		FILE fdev;
		ILI9325* instance;
		uint16_t x, x0, y, y0;
		uint8_t scale;
		rgb565_t color;
		operator fdev_t*(){ return &fdev; }
	};

private:
	static int printChar(char c, FILE*);

	uint16_t m_HorEntryMode, m_VerEntryMode;
	ERot m_rotation;
	sfr8_t CTRL_PORT;
	sfr8_t DATA_PORT;

	inline void writeCmd(uint16_t cmd);
	inline void writeData(uint16_t data);
	inline void writeReg(uint16_t cmd, uint16_t data);
	inline void writeRegRep(uint16_t cmd, uint16_t data, uint32_t count);
	inline void rotatePixel(uint16_t x0, uint16_t y0, uint16_t& x, uint16_t& y);
	void writeChar(uint16_t x0, uint16_t y0, uint16_t& x, uint16_t& y, rgb565_t color, uint8_t scale, char c);

public:
	/** Initializes the LCD display
	 * @param _CTRL_DDR The data direction register for the control pins
	 * @param _DATA_DDR The data direction register for the data pins
	 * @param _CTRL_PORT The port to which the control pins are connected
	 *                   The pins are the following:
	 *                    0 : CS (Chip Select)
	 *                    1 : C/D (Command/Data)
	 *                    2 : WR (Write)
	 *                    3 : RD (Read)
	 *                    4 : RST (Reset)
	 *                   Pins 5-7 are not used for the LCD and may be used for
	 *                   other purposes.
	 * @param _DATA_PORT The port to which the data pins are connected.
	 *                   The pins 0-7 must be connected to (in order) D0-D7
	 * @param rot The rotation
	 */
	ILI9325(sfr8_t _CTRL_DDR, sfr8_t _DATA_DDR, sfr8_t _CTRL_PORT, sfr8_t _DATA_PORT, ERot rot = ROT_0);

	/** Resets the LCD display */
	void reset();

	/** Sets the LCD rotation
	 * @param rot A rotation, @see ERot
	 */
	void setRotation(ERot rot);

	/** Draw a pixel
	 * @param x0 The x coordinate of the pixel
	 * @param y0 The y coordinate of the pixel
	 * @param color The pixel color, @see rgbTo565
	 */
	void drawPixel(uint16_t x0, uint16_t y0, rgb565_t color);

	/** Draw a line
	 * @param x0 The x coordinate of the starting point
	 * @param y0 The y coordinate of the starting point
	 * @param l The line length
	 * @param color The line color, @see rgbTo565
	 * @param dir The line direction, @see EDir
	 */
	void drawLine(uint16_t x0, uint16_t y0, uint16_t l, rgb565_t color, EDir dir);

	/** Fill a rectangle
	 * @param x0 The x coordinate of the top-left corner
	 * @param y0 The y coordinate of the top-left corner
	 * @param w The rectangle width
	 * @param h The rectangle height
	 * @param color The fill color, @see rgbTo565
	 */
	void fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, rgb565_t color);

	/** Fills the entire screen
	 * @param color The fill color, @see rgbTo565
	 */
	void fillScreen(rgb565_t color);

	/** Fills the area which the specified number of rows and cols of test occupy
	 * @param x0 The x-coordinate of the top-left corner of the bounding box
	 * @param y0 The y-coordinate of the top-left corner of the bounding box
	 * @param cols The maximum number of characters per line (columns)
	 * @param rows The number of text rows (lines)
	 * @param scale The text scale
	 * @param color The fill color
	 */
	void fillTextExtents(uint16_t x0, uint16_t y0, uint16_t cols, uint16_t rows, uint16_t scale, rgb565_t color);

	/** Fills the specified bit-pattern
	 * @param pgm_pat The pattern, specified as a row-major bit-array, stored in Program Memory
	 * @param w The pattern width
	 * @param h The pattern height
	 * @param x The x coordinate of the top-left corner
	 * @param y The y coordinate of the top-left corner
	 * @param fg The foreground color, see rgbTo565
	 * @param bg The background color, see rgbTo565
	 */
	void fillPattern(const uint8_t* pgm_pat, uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, rgb565_t fg, rgb565_t bg);

	/** Write a string
	 * @param x0 The x-coordinate of the top-left corner of the bounding box
	 * @param y0 The y-coordinate of the top-left corner of the bounding box
	 * @param pgm_string The string, must be null-terminated, stored in Program Memory
	 * @param color The color, @see rgbTo565
	 * @param scale The string scale, defaults to 1
	 * @param align The text alignment, defaults to ALIGN_LEFT, @see EAlign
	 */
	void writeString(uint16_t x0, uint16_t y0, const char* pgm_string, rgb565_t color, uint8_t scale = 1, EAlign align = ALIGN_LEFT);

	/** Creates a stream structure to use with fprintf and similar
	 * @param x0 The x-coordinate of the top-left corner of the bounding box
	 * @param y0 The y-coordinate of the top-left corner of the bounding box
	 * @param color The color, @see rgbTo565
	 * @param scale The string scale, defaults to 1
	 * @return A new LCD::Stream instance
	 */
	ILI9325::Stream createStream(uint16_t x0, uint16_t y0, rgb565_t color, uint8_t scale);

	/** Modifies an existing stream structure
	 * @param x0 The x-coordinate of the top-left corner of the bounding box
	 * @param y0 The y-coordinate of the top-left corner of the bounding box
	 * @param color The color, @see rgbTo565
	 * @param scale The string scale, defaults to 1
	 */
	void modifyStream(ILI9325::Stream& stream, uint16_t x0, uint16_t y0, rgb565_t color, uint8_t scale) const;

	/** Converts a RGB color definition to the internal color format
	 * @param r The red component
	 * @param g The green component
	 * @param b The blue component
	 * @return The corresponding "565" color
	 */
	static rgb565_t rgbTo565(uint8_t r, uint8_t g, uint8_t b);
};

#endif // ILI9325_HPP
