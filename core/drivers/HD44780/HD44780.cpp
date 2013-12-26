/** @file HD44780.cpp
 *  @brief Driver for HD44780 OLED LCD controllers
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#include "HD44780.hpp"
#include <util/delay.h>

#define RS_PIN 0 // Register select, LOW: command, HIGH: data
#define RW_PIN 1 // Read/Write select, LOW: write, HIGH: read
#define E_PIN  2 // activated by a HIGH pulse.

HD44780::HD44780(sfr8_t _CTRL_DDR, sfr8_t _DATA_DDR, sfr8_t _CTRL_PORT, sfr8_t _DATA_PORT, sfr8_t _DATA_PIN)
	: CTRL_DDR(_CTRL_DDR),
	  DATA_DDR(_DATA_DDR),
	  CTRL_PORT(_CTRL_PORT),
	  DATA_PORT(_DATA_PORT),
	  DATA_PIN(_DATA_PIN)
{
	// Set control and data pins to output
	CTRL_DDR = CTRL_DDR & 0xF8;
	DATA_DDR = 0x00;
	
	// Set all control pins to low
	CTRL_PORT = CTRL_PORT & 0xF8;
	
	// Wait for power stabilization
	_delay_ms(15);
	setDisplayFunction(true, Size5x8, WE_EU);
	controlDisplay(true, true, true);
	clearDisplay();
	setEntryMode(Left, true);
}

void HD44780::clearDisplay()
{
	send_command(0x01);
	// http://electronics.stackexchange.com/questions/26120/hd44780-how-long-do-i-have-to-wait-for-busy-flag-to-reset
	_delay_ms(2);
}


void HD44780::createChar(uint8_t index, uint8_t* data, uint8_t rows)
{
	send_command(0x40 | (index << 3));
	// Recall: address counter is automatically increased after each write
	for(uint8_t i = 0; i < rows; ++i){
		send_data(data[i]);
	}
}

void HD44780::setGraphicsData(uint8_t x, uint8_t row, uint8_t data){
	send_command(0x80 | (x & 0x7f)); // GXA
	send_command(0x40 | row); // GYA
	send_data(data);
}

void HD44780::setGraphicsData(uint8_t (&data)[100][2]){
	for(uint8_t y = 0; y < 2; ++y){
		for(uint8_t x = 0; x < 100; ++x){
			setGraphicsData(x, y, data[x][y]);
		}
	}
}

void HD44780::send_command(uint8_t command)
{
	cbi(CTRL_PORT, RS_PIN);
	DATA_PORT = command;
	pulseEnable();
	waitForReady();
}

void HD44780::send_data(uint8_t data)
{
	sbi(CTRL_PORT, RS_PIN);
	DATA_PORT = data;
	pulseEnable();
	waitForReady();
}

void HD44780::pulseEnable()
{
	// http://www.avrfreaks.net/index.php?name=PNphpBB2&file=printview&t=106134&start=0
	_delay_us(40);
	sbi(CTRL_PORT, E_PIN);
	_delay_us(230);
	cbi(CTRL_PORT, E_PIN);
	_delay_us(10);
}

void HD44780::waitForReady()
{
	// http://electronics.stackexchange.com/questions/26120/hd44780-how-long-do-i-have-to-wait-for-busy-flag-to-reset
	_delay_us(40);
/*
	DATA_DDR = 0x00
	sbi(CTRL_PORT, RW_PIN);
	
	bool busy = true;
	do {
		_delay_ms(1);
		sbi(CTRL_PORT, E_PIN);
		_delay_ms(1);
		busy = ((DATA_PIN & 0x80) != 0);
		cbi(CTRL_PORT, E_PIN);
	} while(busy);
	
	DATA_DDR = 0xFF;
	cbi(CTRL_PORT, RW_PIN);
*/
}
