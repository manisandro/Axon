/** @file ILI9325.cpp
 *  @brief Driver for the ILI9325 LCD display
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#include "ILI9325.hpp"
#include <util/delay.h>

#include "ILI9325_asciifont.hpp"

#define ILI932X_START_OSC          0x00 /* Driver code read, RO.*/
#define ILI932X_DRIV_OUT_CTRL      0x01 /* Driver output control 1, W.*/
#define ILI932X_DRIV_WAV_CTRL      0x02 /* LCD driving control, W.*/
#define ILI932X_ENTRY_MOD          0x03 /* Entry mode, W.*/
#define ILI932X_RESIZE_CTRL        0x04 /* Resize Control, W.*/
#define ILI932X_DISP_CTRL1         0x07 /* Display Control 1, W.*/
#define ILI932X_DISP_CTRL2         0x08 /* Display Control 2, W.*/
#define ILI932X_DISP_CTRL3         0x09 /* Display Control 3, W.*/
#define ILI932X_DISP_CTRL4         0x0A /* Display Control 4, W.*/
#define ILI932X_RGB_DISP_IF_CTRL1  0x0C /* RGB display interface control 1, W.*/
#define ILI932X_FRM_MARKER_POS     0x0D /* Frame Marker Position, W.*/
#define ILI932X_RGB_DISP_IF_CTRL2  0x0F /* RGB display interface control 2, W.*/
#define ILI932X_POW_CTRL1          0x10 /* Power control 1, W.*/
#define ILI932X_POW_CTRL2          0x11 /* Power control 2, W.*/
#define ILI932X_POW_CTRL3          0x12 /* Power control 3, W.*/
#define ILI932X_POW_CTRL4          0x13 /* Power control 4, W.*/
#define ILI932X_GRAM_HOR_AD        0x20 /* Horizontal GRAM address set, W.*/
#define ILI932X_GRAM_VER_AD        0x21 /* Vertical GRAM address set, W.*/
#define ILI932X_RW_GRAM            0x22 /* Write Data to GRAM, W.*/
#define ILI932X_POW_CTRL7          0x29 /* Power control 7, W.*/
#define ILI932X_FRM_RATE_COL_CTRL  0x2B /* Frame Rate and Color Control, W. */
#define ILI932X_GAMMA_CTRL1        0x30 /* Gamma Control 1, W. */
#define ILI932X_GAMMA_CTRL2        0x31 /* Gamma Control 2, W. */
#define ILI932X_GAMMA_CTRL3        0x32 /* Gamma Control 3, W. */
#define ILI932X_GAMMA_CTRL4        0x35 /* Gamma Control 4, W. */
#define ILI932X_GAMMA_CTRL5        0x36 /* Gamma Control 5, W. */
#define ILI932X_GAMMA_CTRL6        0x37 /* Gamma Control 6, W. */
#define ILI932X_GAMMA_CTRL7        0x38 /* Gamma Control 7, W. */
#define ILI932X_GAMMA_CTRL8        0x39 /* Gamma Control 8, W. */
#define ILI932X_GAMMA_CTRL9        0x3C /* Gamma Control 9, W. */
#define ILI932X_GAMMA_CTRL10       0x3D /* Gamma Control 10, W. */
#define ILI932X_HOR_START_AD       0x50 /* Horizontal Address Start Position, W. */
#define ILI932X_HOR_END_AD         0x51 /* Horizontal Address End Position, W. */
#define ILI932X_VER_START_AD       0x52 /* Vertical Address Start Position, W. */
#define ILI932X_VER_END_AD         0x53 /* Vertical Address End Position, W. */
#define ILI932X_GATE_SCAN_CTRL1    0x60 /* Driver output control 2, W. (???)*/
#define ILI932X_GATE_SCAN_CTRL2    0x61 /* Base Image Display Control, W. (???)*/
#define ILI932X_GATE_SCAN_CTRL3    0x6A /* Vertical Scroll Control, W. (???)*/
#define ILI932X_PART_IMG1_DISP_POS 0x80 /* Partial Image 1 Display Position, W.*/
#define ILI932X_PART_IMG1_START_AD 0x81 /* Partial Image 1 Area (Start Line), W.*/
#define ILI932X_PART_IMG1_END_AD   0x82 /* Partial Image 1 Area (End Line), W.*/
#define ILI932X_PART_IMG2_DISP_POS 0x83 /* Partial Image 2 Display Position, W.*/
#define ILI932X_PART_IMG2_START_AD 0x84 /* Partial Image 2 Area (Start Line), W.*/
#define ILI932X_PART_IMG2_END_AD   0x85 /* Partial Image 2 Area (End Line), W.*/
#define ILI932X_PANEL_IF_CTRL1     0x90 /* Panel Interface Control 1, W.*/
#define ILI932X_PANEL_IF_CTRL2     0x92 /* Panel Interface Control 2, W.*/
#define ILI932X_PANEL_IF_CTRL3     0x93 /* Panel Interface Control 3, W.*/
#define ILI932X_PANEL_IF_CTRL4     0x95 /* Panel Interface Control 4, W.*/
#define ILI932X_PANEL_IF_CTRL5     0x97 /* Panel Interface Control 5, W.*/
#define ILI932X_PANEL_IF_CTRL6     0x98 /* Panel Interface Control 6, W.*/

#define TFTLCD_DELAY50             0xFE
#define TFTLCD_DELAY200            0xFF

#define LCD_CTRL_MASK 0b00011111
#define LCD_CS  0 // Chip Select
#define LCD_RS  1 // Register Select (or C/D - Command/Data)
#define LCD_WR  2 // Write
#define LCD_RD  3 // Read
#define LCD_RST 4 // Reset

// At rot = 0, LCD is in portrait mode
#define TFTWIDTH  240
#define TFTHEIGHT 320

static const PROGMEM uint16_t ILI932x_regValues[] = {
	ILI932X_START_OSC,         0x0001, // R00h: start oscillator
	TFTLCD_DELAY50,            50,
	ILI932X_DRIV_OUT_CTRL,     0x0100, // R01h (Driver Output Control): SS = 1
	ILI932X_DRIV_WAV_CTRL,     0x0700, // R02h (LCD Driving Wave Control): EOR = 1, B/C = 1, D10 = 1
	ILI932X_ENTRY_MOD,         0x0030, // R03h (Entry Mode): AM = 0, I/D0 = 1, I/D1 = 1
	ILI932X_RESIZE_CTRL,       0x0000, // R04h (Resize Control Register): clear
	ILI932X_DISP_CTRL2,        0x0202, // R08h (Display Control 2): BP1 = 1, FP1 = 1
	ILI932X_DISP_CTRL3,        0x0000, // R09h (Display Control 3): clear
	ILI932X_DISP_CTRL4,        0x0000, // R0Ah (Display Control 4): clear
	ILI932X_RGB_DISP_IF_CTRL1, 0x0000, // R0Ch (RGB Display Interface Control 1): clear
	ILI932X_FRM_MARKER_POS,    0x0000, // R0Dh (Frame Marker Position): clear
	ILI932X_RGB_DISP_IF_CTRL2, 0x0000, // R0Ch (RGB Display Interface Control 2): clear
	ILI932X_POW_CTRL1,         0x0000, // R10h (Power Control 1): clear
	ILI932X_POW_CTRL2,         0x0007, // R11h (Power Control 2): VC0 = 1, VC1 = 1, VC2 = 2
	ILI932X_POW_CTRL3,         0x0000, // R12h (Power Control 3): clear
	ILI932X_POW_CTRL4,         0x0000, // R13h (Power Control 3): clear
	TFTLCD_DELAY200,           200,
	ILI932X_POW_CTRL1,         0x1690, // AP0 = 1, APE = 1, BT1 = 1, BT2 = 1, SAP = 1
	ILI932X_POW_CTRL2,         0x0227, // VC0 = 1, VC1 = 1, VC2 = 1, DC01 = 1, DC11 = 1
	TFTLCD_DELAY50,            50,
	ILI932X_POW_CTRL3,         0x001A, // VRH1 = 1, VRH3 = 1, PON = 1
	TFTLCD_DELAY50,            50,
	ILI932X_POW_CTRL4,         0x1800, // VCV3 = 1, VDV4 = 1
	ILI932X_POW_CTRL7,         0x002A, // R29h (Power Control 7): VCM1 = 1, VCM3 = 1, VCM5 = 1
	TFTLCD_DELAY50,            50,
	ILI932X_GAMMA_CTRL1,       0x0000, // R30h (Gamma Control 1): clear
	ILI932X_GAMMA_CTRL2,       0x0000, // R31h (Gamma Control 2): clear
	ILI932X_GAMMA_CTRL3,       0x0000, // R32h (Gamma Control 3): clear
	ILI932X_GAMMA_CTRL4,       0x0206, // R35h (Gamma Control 4): RP0[1] = 1, RP0[2] = 1, VRP1[1] = 1
	ILI932X_GAMMA_CTRL5,       0x0808, // R36h (Gamma Control 5): VRP0[3] = 1, VRP1[3] = 1
	ILI932X_GAMMA_CTRL6,       0x0007, // R37h (Gamma Control 6): KN0[0] = 1, KN0[1] = 1, KN0[2] = 1
	ILI932X_GAMMA_CTRL7,       0x0201, // R38h (Gamma Control 7): KN2[0] = 1, KN3[1] = 1
	ILI932X_GAMMA_CTRL8,       0x0000, // R39h (Gamma Control 8): clear
	ILI932X_GAMMA_CTRL9,       0x0000, // R3Ch (Gamma Control 9): clear
	ILI932X_GAMMA_CTRL10,      0x0000, // R3Dh (Gamma Control 10): clear
	ILI932X_GRAM_HOR_AD,       0x0000, // R20h (GRAM Horizontal Address Set): clear
	ILI932X_GRAM_VER_AD,       0x0000, // R21h (GRAM Vertical Address Set): clear
	ILI932X_HOR_START_AD,      0x0000, // R50h (Horizontal RAM Address Start Position): 0
	ILI932X_HOR_END_AD,        0x00EF, // R51h (Horizontal RAM Address End Position): 239
	ILI932X_VER_START_AD,      0X0000, // R52h (Vertical RAM Address Start Position): 0
	ILI932X_VER_END_AD,        0x013F, // R53h (Vertical RAM Address End Position): 319
	ILI932X_GATE_SCAN_CTRL1,   0xA700, // R60h (Gate Scan Control 1): NL0 = 1, NL1 = 1, NL2 = 1, NL5 = 1, GS = 1
	ILI932X_GATE_SCAN_CTRL2,   0x0003, // R61h (Gate Scan Control 2): REV = 1, VLE = 1
	ILI932X_GATE_SCAN_CTRL3,   0x0000, // R6Ah (Gate Scan Control 3): clear
	ILI932X_PANEL_IF_CTRL1,    0x0010, // R90h (Panel Interface Control 1): RTNI1 = 1
	ILI932X_PANEL_IF_CTRL2,    0x0000, // R92h (Panel Interface Control 2): clean
	ILI932X_PANEL_IF_CTRL3,    0x0003,
	ILI932X_PANEL_IF_CTRL4,    0x1100, // R95h (Panel Interface Control 4): DIVE1 = 1,
	ILI932X_PANEL_IF_CTRL5,    0x0000, // R97h (Panel Interface Control 5): clear
	ILI932X_PANEL_IF_CTRL6,    0x0000,
	ILI932X_DISP_CTRL1,        0x0133, // Display Control (R07h): Power on
};

/** Private members **/
inline void ILI9325::writeCmd(uint16_t cmd)
{
	cbi(CTRL_PORT, LCD_RS);

	DATA_PORT = cmd >> 8;
	cbi(CTRL_PORT, LCD_WR);
	sbi(CTRL_PORT, LCD_WR);

	DATA_PORT = cmd;
	cbi(CTRL_PORT, LCD_WR);
	sbi(CTRL_PORT, LCD_WR);

	sbi(CTRL_PORT, LCD_RS);
}

inline void ILI9325::writeData(uint16_t data)
{
	DATA_PORT = data >> 8;
	cbi(CTRL_PORT, LCD_WR);
	sbi(CTRL_PORT, LCD_WR);

	DATA_PORT = data;
	cbi(CTRL_PORT, LCD_WR);
	sbi(CTRL_PORT, LCD_WR);
}

inline void ILI9325::writeReg(uint16_t cmd, uint16_t data)
{
	cbi(CTRL_PORT, LCD_CS);

	writeCmd(cmd);
	writeData(data);

	sbi(CTRL_PORT, LCD_CS);
}

inline void ILI9325::writeRegRep(uint16_t cmd, uint16_t data, uint32_t count)
{
	cbi(CTRL_PORT, LCD_CS);

	writeCmd(cmd);
	while(count--){
		writeData(data);
	}

	sbi(CTRL_PORT, LCD_CS);
}

inline void ILI9325::rotatePixel(uint16_t x0, uint16_t y0, uint16_t& x, uint16_t& y)
{
	if(m_rotation == ROT_90){
		x = TFTWIDTH - y0 - 1;
		y = x0;
	}else if(m_rotation == ROT_180){
		x = TFTWIDTH - x0 - 1;
		y = TFTHEIGHT - y0 - 1;
	}else if(m_rotation == ROT_270){
		x = y0;
		y = TFTHEIGHT - x0 - 1;
	}else{
		x = x0;
		y = y0;
	}
}

/** Public api functions **/
ILI9325::ILI9325(sfr8_t _CTRL_DDR, sfr8_t _DATA_DDR, sfr8_t _CTRL_PORT, sfr8_t _DATA_PORT, ERot rot)
	: CTRL_PORT(_CTRL_PORT), DATA_PORT(_DATA_PORT)
{
	// Control port as output port, set all pins to high
	set_bits(_CTRL_DDR, 0xFF, LCD_CTRL_MASK);
	set_bits(_CTRL_PORT, 0xFF, LCD_CTRL_MASK);

	// Data port as output port, set to 0
	_DATA_DDR = 0xFF;
	_DATA_PORT = 0x00;

	// Initialization procedure
	reset();

	for(uint8_t i = 0; i < sizeof(ILI932x_regValues)/(2*sizeof(uint16_t)); ++i){
		uint16_t cmd = pgm_read_word(ILI932x_regValues + i*2);
		uint16_t data = pgm_read_word(ILI932x_regValues + i*2 + 1);

		if(cmd == TFTLCD_DELAY50){
			_delay_ms(50);
		}else if(cmd == TFTLCD_DELAY200){
			_delay_ms(200);
		}else{
			writeReg(cmd, data);
		}
	}

	setRotation(rot);
	fillScreen(rgbTo565(255, 255, 255));
}

void ILI9325::reset()
{
	cbi(CTRL_PORT, LCD_RST);
	_delay_ms(10);
	sbi(CTRL_PORT, LCD_RST);
	_delay_ms(100);
}

void ILI9325::setRotation(ERot rot)
{
	m_rotation = rot;

	// Set entry mode (see p 54 ILI9328 docs):
#define LCD_EM_HORI 0b00000000
#define LCD_EM_VERT 0b00001000
#define LCD_EM_INCH 0b00010000
#define LCD_EM_DECH 0b00000000
#define LCD_EM_INCV 0b00100000
#define LCD_EM_DECV 0b00000000
	if(rot == ROT_0){
		m_HorEntryMode = LCD_EM_HORI|LCD_EM_INCH|LCD_EM_INCV;
		m_VerEntryMode = LCD_EM_VERT|LCD_EM_INCH|LCD_EM_INCV;
	}else if(rot == ROT_90){
		m_HorEntryMode = LCD_EM_VERT|LCD_EM_DECH|LCD_EM_INCV;
		m_VerEntryMode = LCD_EM_HORI|LCD_EM_DECH|LCD_EM_INCV;
	}else if(rot == ROT_180){
		m_HorEntryMode = LCD_EM_HORI|LCD_EM_DECH|LCD_EM_DECV;
		m_VerEntryMode = LCD_EM_VERT|LCD_EM_DECH|LCD_EM_DECV;
	}else{/* if rot == ROT_270 */
		m_HorEntryMode = LCD_EM_VERT|LCD_EM_INCH|LCD_EM_DECV;
		m_VerEntryMode = LCD_EM_HORI|LCD_EM_INCH|LCD_EM_DECV;
	}
}

ILI9325::rgb565_t ILI9325::rgbTo565(uint8_t r, uint8_t g, uint8_t b)
{
	rgb565_t c = b >> 3;
	c <<= 6;
	c |= g >> 2;
	c <<= 5;
	c |= r >> 3;
	return c;
}

void ILI9325::drawLine(uint16_t x0, uint16_t y0, uint16_t l, rgb565_t color, EDir dir)
{
	uint16_t x, y;
	rotatePixel(x0, y0, x, y);
	writeReg(ILI932X_ENTRY_MOD, dir == DIR_H ? m_HorEntryMode : m_VerEntryMode);
	writeReg(ILI932X_GRAM_HOR_AD, x);
	writeReg(ILI932X_GRAM_VER_AD, y);
	writeRegRep(ILI932X_RW_GRAM, color, l);
}

void ILI9325::drawPixel(uint16_t x0, uint16_t y0, rgb565_t color)
{
	uint16_t x, y;
	rotatePixel(x0, y0, x, y);
	writeReg(ILI932X_GRAM_HOR_AD, x);
	writeReg(ILI932X_GRAM_VER_AD, y);
	writeReg(ILI932X_RW_GRAM, color);
}

void ILI9325::fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, rgb565_t color) {
	uint16_t x[2], y[2];
	rotatePixel(x0, y0, x[0], y[0]);
	rotatePixel(x0+w-1, y0+h-1, x[1], y[1]);
	writeReg(ILI932X_ENTRY_MOD, m_HorEntryMode);
	uint8_t ixmin = x[0] > x[1];
	uint8_t iymin = y[0] > y[1];
	writeReg(ILI932X_HOR_START_AD, x[ixmin]);
	writeReg(ILI932X_HOR_END_AD, x[!ixmin]);
	writeReg(ILI932X_VER_START_AD, y[iymin]);
	writeReg(ILI932X_VER_END_AD, y[!iymin]);
	writeReg(ILI932X_GRAM_HOR_AD, x[ixmin]);
	writeReg(ILI932X_GRAM_VER_AD, y[iymin]);
	writeRegRep(ILI932X_RW_GRAM, color, (uint32_t)w*h);
	writeReg(ILI932X_HOR_START_AD, 0);
	writeReg(ILI932X_HOR_END_AD, TFTWIDTH-1);
	writeReg(ILI932X_VER_START_AD, 0);
	writeReg(ILI932X_VER_END_AD, TFTHEIGHT-1);
}

void ILI9325::fillScreen(rgb565_t color){
	writeReg(ILI932X_ENTRY_MOD, m_HorEntryMode);
	writeReg(ILI932X_GRAM_HOR_AD, 0);
	writeReg(ILI932X_GRAM_VER_AD, 0);
	writeRegRep(ILI932X_RW_GRAM, color, (uint32_t)TFTWIDTH*TFTHEIGHT);
}

void ILI9325::fillTextExtents(uint16_t x0, uint16_t y0, uint16_t cols, uint16_t rows, uint16_t scale, ILI9325::rgb565_t color)
{
	uint16_t w = scale * cols * (AsciiFont::WIDTH + 1);
	uint16_t h = scale * rows * (AsciiFont::HEIGHT + 1);
	if(m_rotation%2 == 0){
		if(x0 + w >= TFTWIDTH) w = TFTWIDTH - 1 - x0;
		if(y0 + h >= TFTHEIGHT) h = TFTHEIGHT - 1 - y0;
	}else{
		if(x0 + w >= TFTHEIGHT) w = TFTHEIGHT - 1 - x0;
		if(y0 + h >= TFTWIDTH) h = TFTWIDTH - 1 - y0;
	}
	fillRect(x0, y0, w, h, color);
}

void ILI9325::fillPattern(const uint8_t* pgm_pat, uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, rgb565_t fg, rgb565_t bg){
	uint16_t bytesPerRow = w/8 + (w%8 != 0);
	for(uint16_t y = 0; y < h; ++y){
		for(uint16_t rb = 0; rb < bytesPerRow; ++rb){
			uint8_t bits = pgm_read_byte(pgm_pat + y*bytesPerRow + rb);
			for(uint16_t x = rb*8; x < rb*8+8; ++x){
				if(x >= w){
					break;
				}else if(bits & 0x1){
					drawPixel(x0 + x, y0 + y, fg);
				}else{
					drawPixel(x0 + x, y0 + y, bg);
				}
				bits >>= 1;
			}
		}
	}
}

void ILI9325::writeString(uint16_t x0, uint16_t y0, const char* pgm_string, rgb565_t color, uint8_t scale, EAlign align){
	uint16_t x = 0, y = 0;
	uint16_t len = 0;
	for(len = 0; pgm_read_byte(pgm_string + len) != '\0'; ++len);
	if(align == ALIGN_CENTER){
		x0 -= (AsciiFont::WIDTH*scale + 1)*len/2;
	}else if(align == ALIGN_RIGHT){
		x0 -= (AsciiFont::WIDTH*scale + 1)*len;
	}
	for(uint16_t pos = 0; pos < len; ++pos){
		uint8_t c = pgm_read_byte(pgm_string + pos);
		writeChar(x0, y0, x, y, color, scale, c);
	}
}

void ILI9325::writeChar(uint16_t x0, uint16_t y0, uint16_t& x, uint16_t& y, rgb565_t color, uint8_t scale, char c)
{
	if(c == '\n'){
		x = 0;
		y += AsciiFont::HEIGHT + 1;
		return;
	}else if(c == '\r'){
		return;
	}
	for(uint8_t dx = 0; dx < 5; ++dx){
		uint8_t vline = pgm_read_byte(AsciiFont::data + (c*5) + dx);
		for(uint8_t dy = 0; dy < 8; ++dy){
			if(vline & 0x1){
				if(scale == 1){
					drawPixel(x0 + x + dx, y0 + y + dy, color);
				}else{
					fillRect(x0 + scale*(x + dx), y0 + scale*(y + dy), scale, scale, color);
				}
			}
			vline >>= 1;
		}
	}
	x += AsciiFont::WIDTH + 1;
}

ILI9325::Stream ILI9325::createStream(uint16_t x0, uint16_t y0, ILI9325::rgb565_t color, uint8_t scale)
{
	ILI9325::Stream stream = {{0, 0, _FDEV_SETUP_WRITE, 0, 0, ILI9325::printChar, 0, 0}};
	stream.instance = this;
	stream.x0 = x0;
	stream.y0 = y0;
	stream.x = 0;
	stream.y = 0;
	stream.color = color;
	stream.scale = scale;
	return stream;
}

void ILI9325::modifyStream(ILI9325::Stream& stream, uint16_t x0, uint16_t y0, ILI9325::rgb565_t color, uint8_t scale) const
{
	stream.x0 = x0;
	stream.y0 = y0;
	stream.x = 0;
	stream.y = 0;
	stream.color = color;
	stream.scale = scale;
}

int ILI9325::printChar(char c, FILE* fh)
{
	// Since the FILE instance is the first member of the struct, it's also the
	// the address of the struct
	ILI9325::Stream* stream = (ILI9325::Stream*)fh;
	stream->instance->writeChar(stream->x0, stream->y0, stream->x, stream->y, stream->color, stream->scale, c);
	return 0;
}
