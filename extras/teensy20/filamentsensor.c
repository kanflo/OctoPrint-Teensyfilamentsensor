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

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "usb_rawhid.h"
#include "common.h"

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

#define LED_PIN     (6)
#define SENSOR_PIN  (6)

// Is it time to send a packet to the host?
volatile bool g_do_output = false;
// Number of filement sensor pulses we have detected
volatile uint32_t g_filament_counter = 0;
// Current pin level
volatile bool g_filament_pin_level = 0;


static void set_led(bool on)
{
	if (on) {
		PORTD |= 1 << LED_PIN;
	} else {
		PORTD &= ~(1 << LED_PIN);
	}
}

static bool get_filament_pin_level(void)
{
	return PINF & (1 << SENSOR_PIN) ? true : false;
}

// Set the LED
static void handle_led_command(uint32_t state)
{
	switch (state) {
		case LED_ON:
			set_led(true);
			break;
		case LED_OFF:
			set_led(false);
			break;
	}
}

// Reset the device into the boot loader (halfkay).
// This is handy if you embed the Teensy and cannot reach the reset button.
static void handle_reset_command(void)
{
	cli();
	// Disable watchdog, if enabled
	// Disable all peripherals
	UDCON = 1;
	USBCON = (1 << FRZCLK);  // Disable USB
	UCSR1B = 0;
	_delay_ms(5);
#if defined(__AVR_AT90USB162__)                // Teensy 1.0
    DDRB = 0; DDRC = 0; DDRD = 0;
    TIMSK0 = 0; TIMSK1 = 0;
    asm volatile("jmp 0x1F00");
#elif defined(__AVR_ATmega32U4__)              // Teensy 2.0
    DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0;
    TIMSK0 = 0; TIMSK1 = 0; TIMSK3 = 0; TIMSK4 = 0;
    ADCSRA = 0;
    asm volatile("jmp 0x3F00");
#elif defined(__AVR_AT90USB646__)              // Teensy++ 1.0
    DDRA = 0; DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0;
    TIMSK0 = 0; TIMSK1 = 0; TIMSK2 = 0; TIMSK3 = 0;
    ADCSRA = 0;
    asm volatile("jmp 0x7E00");
#elif defined(__AVR_AT90USB1286__)             // Teensy++ 2.0
    DDRA = 0; DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0;
    TIMSK0 = 0; TIMSK1 = 0; TIMSK2 = 0; TIMSK3 = 0;
    ADCSRA = 0;
    asm volatile("jmp 0xFE00");
#else
 #error "Unknown board"
#endif
}

// Parse a packet sent from the host
static void parse_out_packet(uint8_t *buf, uint8_t len)
{
	if(packet_ok(buf, len)) {
		uint32_t data = ((uint32_t) (buf[PACKET_DATA_POS])   << 24) |
		                ((uint32_t) (buf[PACKET_DATA_POS+1]) << 16) |
						(buf[PACKET_DATA_POS+2] <<  8) |
						buf[PACKET_DATA_POS+3];
		switch(buf[PACKET_CMD_POS]) {
			case CMD_RESET_COUNTER:
				g_filament_counter = 0;
				handle_led_command(0);
				break;
			case CMD_LED:
				handle_led_command(data);
				break;
			case CMD_RESET:
				handle_reset_command();
				break;
		}
	} else {
		// Not a whole lot we can do.
	}
}

// Main program
int main(void)
{
	int8_t r;
	uint8_t buffer[PACKET_SIZE];

	// Set for 16 MHz clock
	CPU_PRESCALE(0);

	// The on board LED is connected to PD6
	DDRD |= 1 << LED_PIN;
	set_led(true);

    // Filament sensor is connected to PF6
    DDRF &= ~(1 << SENSOR_PIN);
    // Enable pull up to eliminate flutter in case the sensor is disconnected
    PORTF |= 1 << SENSOR_PIN;
    g_filament_pin_level = get_filament_pin_level();

	// Initialize the USB, and then wait for the host to set configuration.
	// If the Teensy is powered without a PC connected to the USB port,
	// this will wait forever.
	usb_init();
	while (!usb_configured()) /* wait */ ;

	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	_delay_ms(1000);

    // Configure timer 0 to generate a timer overflow interrupt every
    // 256*1024 clock cycles, or approx 61 Hz when using 16 MHz clock
    TCCR0A = 0x00;
    TCCR0B = 0x05;
    TIMSK0 = (1 << TOIE0);

	while (1) {
        if (g_filament_pin_level != get_filament_pin_level()) {
            g_filament_pin_level = get_filament_pin_level();
            g_filament_counter++;
            set_led(g_filament_pin_level);
        }

		r = usb_rawhid_recv(buffer, 0);
		if (r > 0) {
			parse_out_packet((uint8_t*) buffer, (uint8_t) r);
		}
		// Time to send filament counter
		if (g_do_output) {
			g_do_output = false;
			build_packet((uint8_t*) buffer, CMD_COUNTER, g_filament_counter);
			usb_rawhid_send(buffer, 50);
		}
	}
}

// This interrupt routine is run approx 61 times per second.
ISR(TIMER0_OVF_vect)
{
	static uint8_t output_count = 0;
	// Set the g_do_output variable every ~0.5 seconds
	if (output_count++ > 30) {
		output_count = 0;
		g_do_output = true;
	}
}
