/** @file buffer.cpp
 *  @brief Multipurpose circular byte buffer.
 *  @copyright (C) 2012-2013 Sandro Mani manisandro@gmail.com
 *  @section license
 *  Distributed under the GNU Public License, see http://www.gnu.org/licenses/gpl.txt
 */

#include "buffer.hpp"
#include <stdlib.h>

CBuffer::CBuffer(){
	m_dataptr = 0;
	m_size = 0;
	m_dataindex = 0;
	m_datalength = 0;
}

CBuffer::~CBuffer()
{
	free(m_dataptr);
}

void CBuffer::resize(uint16_t size)
{
	disable_interrupts;
	free(m_dataptr);
	m_dataptr = (uint8_t*)malloc(size);
	m_size = size;
	m_dataindex = 0;
	m_datalength = 0;
	restore_interrupts;
}


uint8_t CBuffer::pop_front(){
	uint8_t data = 0;
	disable_interrupts;
	if(m_datalength != 0){
		// Get the first character from buffer
		data = m_dataptr[m_dataindex];
		// Move index down and decrement length
		m_dataindex++;
		if(m_dataindex >= m_size){
			m_dataindex -= m_size;
		}
		m_datalength--;
	}
	restore_interrupts;
	return data;
}

bool CBuffer::push_back(uint8_t data){
	bool success = false;
	disable_interrupts;
	// Make sure the buffer has room
	if(m_datalength < m_size){
		// Save data byte at end of buffer
		m_dataptr[(m_dataindex + m_datalength) % m_size] = data;
		// Increment the length
		m_datalength++;
		success = true;
	}
	restore_interrupts;
	return success;
}

uint8_t CBuffer::operator[](uint16_t i) const
{
	disable_interrupts;
	// return character at index in buffer
	uint8_t data = m_dataptr[(m_dataindex + i)%(m_size)];
	restore_interrupts;
	return data;
}

void CBuffer::clear(){
	disable_interrupts;
	m_datalength = 0;
	restore_interrupts;
}

void CBuffer::pop(uint16_t numbytes){
	disable_interrupts;
	// Dump numbytes from the front of the buffer
	// Are we dumping less than the entire buffer?
	if(numbytes < m_datalength){
		// Move index down by numbytes and decrement length by numbytes
		m_dataindex += numbytes;
		if(m_dataindex >= m_size){
			m_dataindex -= m_size;
		}
		m_datalength -= numbytes;
	}else{
		// Flush the whole buffer
		m_datalength = 0;
	}
	restore_interrupts;
}

uint16_t CBuffer::availableSize() const{
	disable_interrupts;
	uint16_t bytesleft = (m_size - m_datalength);
	restore_interrupts;
	return bytesleft;
}

uint16_t CBuffer::size() const
{
	disable_interrupts;
	uint16_t length = m_datalength;
	restore_interrupts;
	return length;
}

