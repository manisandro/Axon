/** @file message.cpp
 *  @brief Utility function for sending messages via UART
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#include "message.hpp"
#include <axon/uart.hpp>

static inline void compute_checksum(const uint8_t* msg, uint16_t len, uint8_t& c1, uint8_t& c2){
	c1 = c2 = 0;
	for(uint16_t i = 0; i < len; ++i){
		c1 += msg[i];
		c2 += c1;
	}
}

uint16_t Message::receive(UART& uart, const uint8_t sig[2], uint8_t* msg, uint16_t maxlen)
{
	// Message structure: [SIG:2 CLASS:1 ID:1 LENGTH:2 PAYLOAD:LENGTH CHECKSUM:2]
	// Find message start
	uint8_t c1 = 0, c2 = uart.getByte();
	while(true){
		c1 = c2;
		c2 = uart.getByte();
		if(c1 == sig[0] && c2 == sig[1]){ break; }
	}
	// Read message header
	uart.getBytes(msg, 4);
	uint16_t len = *(uint16_t*)(msg+2);
	// Check whether buffer overflow would occur
	if(len + 4 > maxlen){
		return 0;
	}
	// Read payload
	uart.getBytes(msg + 4, len);
	// Read checksum
	uint8_t check[2];
	uart.getBytes(check, 2);
	/// Compute checksum
	compute_checksum(msg, len+4, c1, c2);
	if(check[0] == c1 && check[1] == c2){
		return len + 4;
	}
	return 0;
}

void Message::send(UART& uart, const uint8_t sig[2], const uint8_t* msg, uint16_t len)
{
	// Send message start
	uart.sendBytes(sig, 2);
	// Send message
	uart.sendBytes(msg, len);
	// Compute and send checksum
	uint8_t checksum[2];
	compute_checksum(msg, len, checksum[0], checksum[1]);
	uart.sendBytes(checksum, 2);
}

bool Message::canSend(UART& uart, uint16_t msglen)
{
	return uart.transmitBufferAvailableSize() >= msglen + 4; // + 2 header + 2 checksum
}
