/*
 * Pixel.c
 *
 *  Created on: -----
 *      Author: -----
 */

#include "Pixel.h"



//Table for pixel dots.
//				 dots[X][Y][COLOR]
volatile uint8_t dots[8][8][3]={0};

#define channel *(uint8_t *) 0x41220000
#define control_signals *(uint8_t *) 0x41220008

// Here the setup operations for the LED matrix will be performed
void setup(){

	//Initialize control_signals and channel to
    channel = 0;
    control_signals = 0;

    // Reset the screen by toggling the RSTn bit (bit 0)

    control_signals &= 0b11110;  // Clear RSTn to 0 (disable reset)
    control_signals |= 0b00001;  // Set RSTn to 1 (enable reset)


    //Set the SDA bit (bit 4) to 1 to enable serial data transmission
    control_signals |= 0b10000;  // Set SDA to 1

    // Step 4: Send 6-bit values (all set to 1) in a loop to the DM163 register
    // 24*6 bits (144 bits) need to be transmitted, 24 bytes of 6 bits each
    for (uint8_t t = 0; t <6* 24; t++) {


            control_signals |= 0b01000;  // Set SCK to 1 (rising edge)


            control_signals &= 0b10111;  // Clear SCK to 0 (falling edge)

    }

    // Step 5: Set the SB bit (bit 2) to 1 to enable transmission to the 8-bit register
    control_signals |= 0b00100;  // Set SB to 1


}

//Change value of one pixel at led matrix. This function is only used for changing values of dots array
void SetPixel(uint8_t x,uint8_t y, uint8_t r, uint8_t g, uint8_t b){

	//Hint: you can invert Y-axis quite easily with 7-y
	dots[x][y][0]=b;
	//Write rest of two lines of code required to make this function work properly (green and red colors to array).
	dots[x][y][1]=g;
	dots[x][y][2]= r;

}


//Put new data to led matrix. Hint: This function is supposed to send 24-bytes and parameter x is for channel x-coordinate.
void run(uint8_t x){

	control_signals &= 0b11101; //lat bit 0

	//Write code that writes data to led matrix driver (8-bit data). Use values from dots array

	// Ensure that LAT (bit 1) = 0

	uint8_t temp = 0;

	// 3 nested loops
	// The outermost loop loop through pixels
	for (uint8_t pixel = 0; pixel < 8; pixel++) {

		// The next loop loop through colors
		for (uint8_t color = 0; color < 3; color++) {

			temp = dots[x][pixel][color];

			// The innermost loop loop through the actual color data
			for (uint8_t bit = 0; bit < 8; bit++) {
				if (temp & 0b10000000) control_signals |= 0b10000; //0b1000 0000
				else control_signals &= 0b01111;


				control_signals |= 0b01000; //set SCK to rising edge
				control_signals &= 0b10111; //SCK to falling
				temp<<=1; //shifting 1 bit to left
				//
			}
		}

	}

//	Toggle SCK (bit3) from 0 to 1



	//Hint2: loop iterations are 8,3,8 (pixels,color,8-bit data)

	latch();
	control_signals &= 0b10111; //SCK


}

//Latch signal. See colorsshield.pdf or DM163.pdf in project folder on how latching works
void latch(){
	// Set the latch bit (bit 1) HIGH
    control_signals |= 0b00010;

    // Set the latch bit (bit 1) LOW
    control_signals &= 0b11101;

}


//Set one line (channel) as active, one at a time.
void open_line(uint8_t x){
	switch(x){
		case 0: channel = 0b00000001; break;
		case 1:	channel = 0b00000010; break;
		case 2: channel = 0b00000100; break;
		case 3: channel = 0b00001000; break;
		case 4:	channel = 0b00010000; break;
		case 5: channel = 0b00100000; break;
		case 6: channel = 0b01000000; break;
		case 7:	channel = 0b10000000; break;
		default: channel = 0b00000000;

	}


}



