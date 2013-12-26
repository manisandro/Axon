/** @file message.hpp
 *  @brief Utility function for sending messages via UART
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */


#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <common.hpp>

class UART;

/** Utility functions for reading messages of the form
 * [SIG:2 CLASS:1 ID:1 LENGTH:2 PAYLOAD:LENGTH CHECKSUM:2]
 */
namespace Message {
	/** Receive a message
	 * @param uart   The uart port
	 * @param sig    The message start signature
	 * @param msg    The message, of the format
	 *               [CLASS:1 ID:1 LENGTH:2 PAYLOAD:LENGTH]
	 * @param maxlen The maximum message length
	 * @return       The length of the read signal
	 */
	uint16_t receive(UART& uart, const uint8_t sig[2], uint8_t* msg, uint16_t maxlen);

	/** Send a message
	 * @param uart The uart port
	 * @param sig  The message start signature
	 * @param msg  The message, of the format
	 *             [CLASS:1 ID:1 LENGTH:2 PAYLOAD:LENGTH]
	 * @param len  The message length (including header length)
	 */
	void send(UART& uart, const uint8_t sig[2], const uint8_t* msg, uint16_t len);

	/** Returns whether the message of specified length can be send without a transmit
	 * buffer overflow occuring
	 * @param uart The uart port
	 * @param msglen The message length
	 * @return Whether the message can be sent without the transmit buffer overflowing
	 */
	bool canSend(UART& uart, uint16_t msglen);
}

#endif
