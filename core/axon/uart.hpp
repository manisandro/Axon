/** @file uart.hpp
 *  @brief Buffered UART, see Atmega640 documentation chapter 22.
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#ifndef UART_HPP
#define UART_HPP

#include <common.hpp>
#include "buffer.hpp"

#include <stdio.h>

class UART {
public:
	enum EParity { ParityDisabled = 0x00, ParityEven = 0x02, ParityOdd = 0x03 };
	enum EStopBit { Stop1Bit = 0x00, Stop2Bit = 0x01 };
	enum ECharSize { Size5Bit = 0x00, Size6Bit = 0x01, Size7Bit = 0x02, Size8Bit = 0x03/*, Size9Bit = 0x07*/ };

private:
	sfr8_t PRR;
	sfr8_t UDR;
	sfr8_t UCSRA;
	sfr8_t UCSRB;
	sfr8_t UCSRC;
	sfr16_t UBRR;
	const uint8_t PRUSART;

	CBuffer m_rxBuffer, m_txBuffer;
	bool m_receiveOverflow;
	bool m_transmitOverflow;

	int(*fdevSendByte)(char, FILE*);
	int(*fdevGetByte)(FILE*);

	template<UART* uart>
	friend class UARTInitializer;

public:
	UART(sfr8_t _PRR, sfr8_t _UDR, sfr8_t _UCSRA, sfr8_t _UCSRB, sfr8_t _UCSRC, sfr16_t _UBRR, const uint8_t _PRUSART)
	: PRR(_PRR), UDR(_UDR), UCSRA(_UCSRA), UCSRB(_UCSRB), UCSRC(_UCSRC), UBRR(_UBRR), PRUSART(_PRUSART) {};

	/** Sets up the UART
	 * @param baudrate The baudrate
	 * @param rxBufSize The receive buffer size
	 * @param txBufSize The transmit buffer size
	 * @param parity The frame format parity mode, @see EParity
	 * @param stop The frame format stop bit mode, @see EStopBit
	 * @param size The frame format char with mode, @see ECharSize
	 */
	void setup(uint32_t baudrate, uint16_t rxBufSize = 128, uint16_t txBufSize = 128,
	           EParity parity = ParityDisabled, EStopBit stop = Stop1Bit, ECharSize size = Size7Bit);

	/** Reset the UART to its default, inactive state */
	void reset();

	/** Sets the UART baud rate
	 * @param baudrate A baudrate, in bps
	 */
	void setBaudrate(uint32_t baudrate);

	/** Gets a single byte from the receive buffer
	 * @return The read byte
	 */
	uint8_t getByte();

	/** Gets the data from the receive buffer
	 * @param data The data array where to store the bytes
	 * @param nBytes The number of bytes to read
	 */
	void getBytes(uint8_t* data, uint16_t nBytes);

	/** Adds the specified data to the transmission buffer
	 * @param data The data array to send
	 * @param nBytes The length of the data array
	 * @return true on success, false otherwise
	 */
	bool sendBytes(const uint8_t* data, uint16_t nBytes);

	/** Clears the receive buffer */
	void flushReceiveBuffer(){ m_rxBuffer.clear(); }

	/** Clears the transmission buffer */
	void flushTransmitBuffer(){ m_txBuffer.clear(); }

	/** Sets up a write stream for the UART
	 * Example:
	 * @code{.cpp}
	 *   UART& USB = UART1;
	 *   USB.setup(115200);
	 *   FILE usbout = USB.setupWriteStream();
	 *   stdout = &usbout;
	 *   printf("Hello World");
	 * @endcode
	 * @return A FILE handle which can be used with the stdio functions
	 */
	FILE setupWriteStream();

	/** Sets up a read stream for the UART
	 * @return A FILE handle which can be used with the stdio functions
	 */
	FILE setupReadStream();

	/** Get receive overflow
	 * @return Whether a receive overflow occurred during the last receive operation
	 */
	bool receiveOverflow() const{ return m_receiveOverflow; }

	/** Get transmit overflow
	 * @return Whether a transmit overflow occurred during the last send operation
	 */
	bool transmitOverflow() const{ return m_transmitOverflow; }

	/** Returns whether the receive buffer is empty
	 * @return Whether the receive buffer is empty
	 */
	bool receiveBufferEmpty()const{ return m_rxBuffer.size() == 0; }

	/** Returns whether the transmit buffer is empty
	 * @return Whether the transmit buffer is empty
	 */
	bool transmitBufferEmpty() const{ return m_txBuffer.size() == 0; }

	/** Returns the available size in the receive buffer
	 * @return The available number of bytes in the receive buffer
	 */
	uint16_t receiveBufferAvailableSize() const{ return m_rxBuffer.availableSize(); };

	/** Returns the available size in the transmit buffer
	 * @return The available number of bytes in the transmit buffer
	 */
	uint16_t transmitBufferAvailableSize() const{ return m_txBuffer.availableSize(); };

	/** The receive service for the interrupt vector handler */
	static void receiveService(UART& uart);

	/** The transmit service for the interrupt vector handler */
	static void transmitService(UART& uart);
};

extern UART UART0;
extern UART UART1;
extern UART UART2;
extern UART UART3;

#endif
