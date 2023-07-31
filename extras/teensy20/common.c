/*
 * Copyright (c) 2023 Johan Kanflo
 * https://github.com/kanflo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above description, website URL and copyright notice and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "common.h"
#include <string.h>

// Returns true if the packet buf of length len is according to the specs above.
bool packet_ok(uint8_t *buf, uint8_t len)
{
	bool packet_ok = false;
	do {
		if (len != PACKET_SIZE) {
			break;
		}
		if (buf[PACKET_MAGIC0_POS] != PACKET_MAGIC0 || buf[PACKET_MAGIC1_POS] != PACKET_MAGIC1) {
			break;
		}
		packet_ok = true;
	} while(0);

	return packet_ok;
}

// Build a 64 byte packet, fill in the magic, command and data, zero all ofher bytes
void build_packet(uint8_t *buffer, uint8_t command, uint32_t data)
{
	memset((void*) buffer, 0, PACKET_SIZE);

	// send a packet, first 2 bytes are 0xabcd
	buffer[PACKET_MAGIC0_POS] = PACKET_MAGIC0;
	buffer[PACKET_MAGIC1_POS] = PACKET_MAGIC1;

	// Fill in the command and data
	buffer[PACKET_CMD_POS] = command;
	buffer[PACKET_DATA_POS] = (data >> 24) & 0xff;
	buffer[PACKET_DATA_POS+1] = (data >> 16) & 0xff;
	buffer[PACKET_DATA_POS+2] = (data >> 8) & 0xff;
	buffer[PACKET_DATA_POS+3] = data & 0xff;
}
