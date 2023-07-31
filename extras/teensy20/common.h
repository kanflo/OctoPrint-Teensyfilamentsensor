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

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdbool.h>
#include <stdint.h>

// USB vendor and product IDs. Note that these are "borrowed" and may collide with products you own.
#define USB_VID   (0x04c9)  // AT&T Microelectronics - OBSOLETE
#define USB_PID   (0x1234)

#define PACKET_SIZE  (64)
/*
    This is the 64 byte packet structure

    00 Magic 0xab
    01 Magic 0xcd
	02 Command (0:nop, 1:set LED, 2:set button delay, 3:button state 128:reset device)
	03 Data
		command 1:
			LED state (0:off, 1:on, 2:blink, 4:blink fast for error indication)
	   	command 2:
			Time needed to press the button for the device to report button down in "ticks"
			where 61 ticks equals one second.
		command 3:
			Read button state from device:
			0 : button pressed
			1 : button not pressed
    ..
    62 16 bit counter, most significant byte
    63 16 bit counter, least significant byte
*/

#define PACKET_MAGIC0         (0xab)
#define PACKET_MAGIC1         (0xcd)

#define CMD_NOP               (0)
#define CMD_LED               (1)
#define CMD_COUNTER           (2)
#define CMD_RESET_COUNTER     (3)
#define CMD_RESET             (128)

#define LED_OFF               (0)
#define LED_ON                (1)

#define PACKET_MAGIC0_POS    (0)
#define PACKET_MAGIC1_POS    (1)
#define PACKET_CMD_POS       (2)
#define PACKET_DATA_POS      (3)

// Returns true if the packet buf of length len is according to the specs above.
bool packet_ok(uint8_t *buf, uint8_t len);

// Build a 64 byte packet, fill in the magic, command and data, zero all other bytes
void build_packet(uint8_t *buffer, uint8_t command, uint32_t data);

#endif /* end of include guard: __COMMON_H__ */
