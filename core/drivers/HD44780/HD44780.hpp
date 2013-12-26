/** @file HD44780.hpp
 *  @brief Driver for HD44780 OLED LCD controllers
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#ifndef HD44780_HPP
#define HD44780_HPP

#include <common.hpp>

/** Driver for HD44780 OLED LCD controllers */
class HD44780 {
public:
	enum FontSize { Size5x8 = 0, Size5x10 = 1 };
	enum FontTable { EN_JP = 0x00, WE_EU = 0x01, EN_RU = 0x02 };
	enum Direction { Left = 0, Right = 1 };
	enum DisplayMode { Character = 0, Graphic = 1 };
	
	/** Initializes the OLED display
	 * The default configuration is as follows:
	 * - Two lines, 5x8 dot chars, WE_EU font table
	 * - Cursor increases and display shifts uppon write to DDRAM
	 * 
	 * Pin layout:
	 * - Pin 0 of the control port must be connected to RS
	 * - Pin 1 of the control port must be connected to RW
	 * - Pin 2 of the control port must be connected to E
	 * - Pins 4-7 of the control port are unused and may be used for other purposes
	 * - Pins 0-7 of the data port must be connected (in order) to D0-D7
	 * 
	 * @param _CTRL_DDR The data direction register for the control pins
	 * @param _DATA_DDR The data direction register for the data pins
	 * @param _CTRL_PORT The port for the control pins
	 * @param _DATA_PORT The port for the data pins.
	 * @param _DATA_PIN The pin for the data pins.
	 */
	HD44780(sfr8_t _CTRL_DDR, sfr8_t _DATA_DDR, sfr8_t _CTRL_PORT, sfr8_t _DATA_PORT, sfr8_t _DATA_PIN);

	/** Sets display lines, font size and font table
	 * The LCD can display one or two lines. If only one line is displayed,
	 * the font size may be 5x8 or 5x10 (@see FontSize). If two lines are
	 * displayed, the font size must be 5x8.
	 * The font-tables are illustrated in the spec, pages 24-27.
	 * 
	 * @see WEH001602ABPP5N00000 spec pages 35, 41-42
	 * 
	 * @param twolines Whether to display two lines (if false, one line is displayed)
	 * @param fontSize The font size, either Size5x8 or Size5x10 (latter only if not twolines)
	 * @param FontTable One of EN_JP (English/Japanese), WE_EU (Western European) and EN_RU (English/Russian)
	 */
	void setDisplayFunction(bool twolines, FontSize fontSize, FontTable fontTable){
		if(twolines) fontSize = Size5x8;
		send_command(0x30 | (twolines << 3) | (fontSize << 2) | fontTable);
	}
	
	/** Sets whether to display characters or graphis, and whether the display is powered
	 * In character mode, the display can only draw characters (use
	 * setCharacterPosition, writeCharacter and createChar).
	 * In graphic mode, the display can only draw a 100x16 graphic (use
	 * setGraphicsData).
	 * 
	 * @see WEH001602ABPP5N00000 spec pages 22-23, 35, 41
	 * 
	 * @param mode Either Character or Graphic
	 * @param powered Either true or false
	 */
	void setDisplayMode(DisplayMode mode, bool powered){
		send_command(0x13 | (mode << 3) | (powered << 2));
	}
	
	/** Clears the display
	 * This function
	 * - Sets all chars in the DDRAM to the blank character
	 * - Shifts the display window to the origin
	 * - Sets the cursor move direction to increasing
	 * - Sets the DDRAM write address to zero
	 * 
	 * @see WEH001602ABPP5N00000 spec pages 35, 37
	 */
	void clearDisplay();
	
	/** Resets display window shift and DDRAM write address
	 * This function
	 * - Shifts the display window to the origin
	 * - Sets the DDRAM write address to zero
	 * 
	 * @see WEH001602ABPP5N00000 spec pages 35, 37
	 */
	void returnHome(){
		send_command(0x02);
	}
	
	/** Sets the entry mode
	 * This function controls how the DDRAM write address and the display shift
	 * are modified after writing to the DDRAM.
	 * cursorShift determines whether the write address decreases (cursorShift=Left)
	 * or increases (cursorShift=Right) after writing to the DDRAM.
	 * shiftDisplay determines whether the display window is shifted in the
	 * direction specified by cursorShift after writing to the DDRAM.
	 * 
	 * @see WEH001602ABPP5N00000 spec pages 35, 38
	 * 
	 * @param cursorShift Either Left (decrease address) or Right (increase address)
	 * @param shiftDisplay Either true or false
	 */
	void setEntryMode(Direction cursorShift, bool shiftDisplay){
		send_command(0x04 | (cursorShift << 1) | shiftDisplay);
	}
	
	/** Sets whether the display is turned on or off and whether the cursor is visibile/blinking
	 * 
	 * @see WEH001602ABPP5N00000 spec pages 35, 38-40
	 * 
	 * @param display_on Whether the display is powered
	 * @param cursor_visible Whether the cursor is visibile
	 * @param cursor_blink Whether the cursor blinks
	 */
	void controlDisplay(bool display_on, bool cursor_visible, bool cursor_blink){
		send_command(0x08 | (display_on << 2) | (cursor_visible << 1) | cursor_visible);
	}
	
	/** Shifts the cursor left or right
	 * This function allows to shift the cursor left or right without writing
	 * to the DDRAM.
	 * 
	 * @see WEH001602ABPP5N00000 spec pages 35, 41
	 * 
	 * @param direction Either Left or Right
	 */
	void shiftCursor(Direction direction){
		send_command(0x10 | (direction << 2));
	}
	
	/** Shifts the display window left or right
	 * This function allows to shift the display window left or right without
	 * writing to the DDRAM.
	 * 
	 * @see WEH001602ABPP5N00000 spec pages 35, 41
	 * 
	 * @param direction Either Left or Right
	 */
	void shiftDisplay(Direction direction){
		send_command(0x18 | (direction << 2));
	}

	/** Sets the write address of the LCD's Display Data RAM (DDRAM)
	 * The LCD's DDRAM stores 128 char codes: 128 chars on one line or 64 chars
	 * on two lines (@see setDisplayFunction). Call this function to set the
	 * DDRAM address to which the subsequent writeCharacter will write to.
	 * Depending on the configured entry mode (@see setEntryMode) the address
	 * will automatically get de- or increased after the write.
	 * 
	 * @see WEH001602ABPP5N00000 spec pages 12, 13, 22, 42
	 * 
	 * @param col The column, in [0, 127] in one-line mode, in [0, 63] in two-line mode
	 * @param row Either 0 or 1 (the latter only allowed in two-line mode)
	 */
	void setCharacterPosition(uint8_t col, uint8_t row){
		static uint8_t row_offsets[] = { 0x00, 0x40 };
		send_command(0x80 | (col + row_offsets[row & 0x03]));
	}
	
	/** Writes a character code to the DDRAM at the current position
	 * The charcodes are documented in the spec, pages 24-27. The position
	 * can be set by calling setCharacterPosition. After every write, the
	 * position is automatically de- or increased depending on the configured
	 * entry mode (@see setEntryMode).
	 * 
	 * @param charcode The 8-bit charcode
	 */
	void writeCharacter(uint8_t charcode){
		send_data(charcode);
	}
	
	/** Create a custom character
	 * The user can create up to 8 5x8 or 4 5x10 dot custom characters.
	 * 5x10 dot characters can only be used in single-line mode.
	 * Each byte of the data array corresponds to a row of the dot matrix, of
	 * which only the first 5 bits are used. The data array must contain
	 * 8 rows for 5x8 dot characters and 10 rows for 5x10 dot characters.
	 * @note You must call setCharacterPosition again after calling this function
	 * 
	 * @see WEH001602ABPP5N00000 spec pages 16-20
	 * 
	 * @param location The index of the custom character, must be in [0, 7]
	 *                 when creating 5x8 dot characters, and in [0, 3] when
	 *                 creating 5x10 dot characters.
	 * @param data The character data array.
	 * @param rows The number of bytes in the character data array.
	 */
	void createChar(uint8_t index, uint8_t* data, uint8_t rows);
	
	/** Set graphic data for one 8-dot column
	 * The LCD can store a 100x16 dot graphic to display in graphic mode
	 * (@see setDisplayMode). The graphic matrix is a 100x2 array of bytes,
	 * where each byte describes a 8 dots column (hence elements (x,0) and (x,1)
	 * describe the x-th 16-dot vertical column of the display raster).
	 * 
	 * @see WEH001602ABPP5N00000 spec page 23
	 * 
	 * @param x The x coordinate, in [0, 99]
	 * @param row The row, either 0 or 1
	 * @param data Data for one 8-dot column
	 */
	void setGraphicsData(uint8_t x, uint8_t row, uint8_t data);
	
	/** Set the 100x16 dot graphic
	 * See CharacterOLED::setGraphicsData(uint8_t, uint8_t, uint8_t).
	 * This function sets the entire graphic.
	 * 
	 * @param data The graphic data array.
	 */
	void setGraphicsData(uint8_t (&data)[100][2]);
  
private:
	sfr8_t DATA_DDR;
	sfr8_t DATA_PORT;
	sfr8_t DATA_PIN;
	sfr8_t CTRL_DDR;
	sfr8_t CTRL_PORT;
	void send_command(uint8_t command);
	void send_data(uint8_t data);
	void pulseEnable();
	void waitForReady();
};

#endif // HD44780_HPP
