/** @file buffer.hpp
 *  @brief Multipurpose circular byte buffer.
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <common.hpp>

/** A circular buffer
 *  Byte-buffer class providing an easy and efficient way to store and process a stream of bytes.
 *  The buffers are designed for FIFO operation (first in, first out).
 *  This buffer is not dynamically allocated, it has a user-defined fixed maximum size.
 */
class CBuffer {
private:
	uint8_t* m_dataptr;    //!< Physical memory address where the buffer is stored
	uint16_t m_size;       //!< Allocated size of the buffer
	uint16_t m_datalength; //!< Length of the data stored in the buffer
	uint16_t m_dataindex;  //!< Index where data starts

public:
	/** Construct a buffer */
	CBuffer();

	/** Destroys the buffer */
	~CBuffer();

	/** Get the byte at the specified index */
	uint8_t operator[](uint16_t i) const;

	/** Resizes the buffer
	 * @param size The size in bytes
	 */
	void resize(uint16_t size);

	/** Get and remove the first byte from the front of the buffer
	 *  @return The first byte from the front of the buffer
	 */
	uint8_t pop_front();

	/** Add a byte to the end of the buffer
	 *  @param data The byte to add
	 *  @return true on success, false on failure
	 */
	bool push_back(uint8_t data);

	/** Flush (clear) the contents of the buffer */
	void clear();

	/** Dump (discard) bytes from the front of the buffer
	 *  @param numbytes The number of bytes to discard
	 */
	void pop(uint16_t numbytes);

	/** Get availble size in the buffer
	 *  @return The number of remaining free byte
	 */
	uint16_t availableSize() const;

	/** Get the number of bytes in the buffer */
	uint16_t size() const;
};

#endif
