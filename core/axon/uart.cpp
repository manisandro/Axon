/** @file uart.cpp
 *  @brief Buffered UART, see Atmega640 documentation chapter 22.
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#include "uart.hpp"

void UART::setup(uint32_t baudrate, uint16_t rxBufSize, uint16_t txBufSize,
                 EParity parity, EStopBit stop, ECharSize size)
{
	// Setup buffers
	m_rxBuffer.resize(rxBufSize);
	m_txBuffer.resize(txBufSize);

	// Ensure powered up, see Atmega640 documentation chapter 22.1
	cbi(PRR, PRUSART);

	// Enable receiver and transmitter
	sbi(UCSRB, RXEN0);
	sbi(UCSRB, TXEN0);

	// Set frame format
	UCSRC |= parity << 4;      // Parity mode
	UCSRC |= stop << 3;        // Stop bit mode
	UCSRC |= (size&0x03) << 1; // Character size
	UCSRB |= (size&0x04);      // Character size

	// Enable receive and transmit interrupts
	sbi(UCSRB, RXCIE0);
	sbi(UCSRB, TXCIE0);
	sei();

	setBaudrate(baudrate);
}

void UART::reset()
{
	sbi(PRR, PRUSART);
	cbi(UCSRB, RXCIE0);
	cbi(UCSRB, TXCIE0);
	cbi(UCSRB, RXCIE0);
	cbi(UCSRB, TXCIE0);
}

void UART::setBaudrate(uint32_t baudrate)
{
	// Set UBRR from baud rate, see Atmega640 documentation Table 22-1
	UBRR = (F_CPU + (baudrate*8L))/(baudrate*16L) - 1;
}

uint8_t UART::getByte()
{
	while(m_rxBuffer.size() == 0);
	return m_rxBuffer.pop_front();
}

void UART::getBytes(uint8_t* data, uint16_t nBytes)
{
	for(uint16_t i = 0; i < nBytes; ++i) {
		data[i] = getByte();
	}
}

bool UART::sendBytes(const uint8_t* data, uint16_t nBytes)
{
	for(uint16_t i = 0; i < nBytes; ++i){
		m_transmitOverflow &= m_txBuffer.push_back(data[i]);
	}
	// If transmission unit was inactive, send first byte
	disable_interrupts;
	if(m_txBuffer.size() == nBytes){
		while(bit_is_clear(UCSRA, UDRE0));
		UDR = m_txBuffer.pop_front();
	}
	restore_interrupts;
	return m_transmitOverflow;
}

FILE UART::setupWriteStream(){
// 	FILE fh = FDEV_SETUP_STREAM(putChar, NULL, _FDEV_SETUP_WRITE);
	FILE fh = {0, 0, _FDEV_SETUP_WRITE, 0, 0, fdevSendByte, 0, 0};
	return fh;
}

FILE UART::setupReadStream(){
// 	FILE fh = FDEV_SETUP_STREAM(NULL, getChar, _FDEV_SETUP_READ);
	FILE fh = {0, 0, _FDEV_SETUP_READ, 0, 0, 0, fdevGetByte, 0};
	return fh;
}

void UART::receiveService(UART& uart)
{
	while(bit_is_clear(uart.UCSRA, RXC0));
	uart.m_receiveOverflow = !uart.m_rxBuffer.push_back(uart.UDR);
}

void UART::transmitService(UART& uart)
{
	if(uart.m_txBuffer.size() != 0){
		while(bit_is_clear(uart.UCSRA, UDRE0));
		uart.UDR = uart.m_txBuffer.pop_front();
	}
}

template<UART* uart>
static int _fdevGetByte(FILE*)
{
	return uart->getByte();
}

template<UART* uart>
static int _fdevSendByte(char ch, FILE* fh)
{
	uint8_t byte = ch;
	return !uart->sendBytes(&byte, 1);
}

ISR(USART0_RX_vect)
{
	UART::receiveService(UART0);
}

ISR(USART1_RX_vect)
{
	UART::receiveService(UART1);
}

ISR(USART2_RX_vect)
{
	UART::receiveService(UART2);
}

ISR(USART3_RX_vect)
{
	UART::receiveService(UART3);
}

ISR(USART0_TX_vect)
{
	UART::transmitService(UART0);
}

ISR(USART1_TX_vect)
{
	UART::transmitService(UART1);
}

ISR(USART2_TX_vect)
{
	UART::transmitService(UART2);
}

ISR(USART3_TX_vect)
{
	UART::transmitService(UART3);
}

template<UART* uart>
struct UARTInitializer {
	UARTInitializer(){
		uart->fdevSendByte = _fdevSendByte<uart>;
		uart->fdevGetByte = _fdevGetByte<uart>;
	}
};

UART UART0(PRR0, UDR0, UCSR0A, UCSR0B, UCSR0C, UBRR0, PRUSART0);
UART UART1(PRR1, UDR1, UCSR1A, UCSR1B, UCSR1C, UBRR1, PRUSART1);
UART UART2(PRR1, UDR2, UCSR2A, UCSR2B, UCSR2C, UBRR2, PRUSART2);
UART UART3(PRR1, UDR3, UCSR3A, UCSR3B, UCSR3C, UBRR3, PRUSART3);

UARTInitializer<&UART0> initUART0;
UARTInitializer<&UART1> initUART1;
UARTInitializer<&UART2> initUART2;
UARTInitializer<&UART3> initUART3;
