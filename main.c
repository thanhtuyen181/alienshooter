#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xgpiops.h"
#include "xttcps.h"
#include "xscugic.h"
#include "xparameters.h"
#include "Pixel.h"
#include "Interrupt_setup.h"

#define enable_interrupts


volatile uint8_t ship_x = 3;
volatile uint8_t bullet_y;
volatile uint8_t bullet_x; // Make sure to declare bullet_x here
volatile uint8_t ongoing_shoot = 0;
static uint8_t direction = 1;
static uint position = 0; // Variable to track the current position
volatile uint8_t alien_hit = 0;
volatile uint8_t hitcount = 0;
volatile uint8_t score = 0;
volatile game_over = 0;

extern void blinker(void);
int main() {
    init_platform();
#ifdef enable_interrupts
    init_interrupts();
#endif
    setup();
    Xil_ExceptionEnable();


    // Try to avoid writing any code in the main loop.
    while (1) {
    	 blinker();
    }

    cleanup_platform();
    return 0;
}

void DrawShip(uint8_t ship_x) {
    SetPixel(ship_x, 7, 0, 255, 0);
    SetPixel(ship_x + 1, 7, 0, 255, 0);
    SetPixel(ship_x + 2, 7, 0, 255, 0);
    SetPixel(ship_x + 1, 6, 0, 255, 0);
}

void ClearShip(uint8_t ship_x) {
    SetPixel(ship_x, 7, 0, 0, 0);
    SetPixel(ship_x + 1, 7, 0, 0, 0);
    SetPixel(ship_x + 2, 7, 0, 0, 0);
    SetPixel(ship_x + 1, 6, 0, 0, 0);
}

void updateBullet(uint8_t x, uint8_t y) {
    SetPixel(x, y, 255, 0, 255); // Magenta color for the bullet
}

void ClearBullet(uint8_t x, uint8_t y) {
    SetPixel(x, y, 0, 0, 0); // Clear the bullet pixel
}

// Timer interrupt handler for LED matrix update. Frequency is 800 Hz
volatile uint8_t current_channel = 0; // global

void TickHandler(void *CallBackRef) {
    uint32_t StatusEvent;
    Xil_ExceptionDisable();
    //blinker();

    if (current_channel > 8) {
        current_channel = 0;
    }
    open_line(9);
    run(current_channel);  // Update current channel's data to LED matrix
    open_line(current_channel); // Turn on corresponding channel
    current_channel = (current_channel + 1) % 8; // Move to the next channel

    StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
    XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);
    Xil_ExceptionEnable();
}

// Timer interrupt for moving alien, shooting... Frequency is 10 Hz by default
void TickHandler1(void *CallBackRef) {
    uint32_t StatusEvent;


    SetPixel(position, 0, 0, 0, 0); // Clear the previous position of the alien

    if (game_over) {
            StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
            XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);
            return;
        }

    if (direction == 1) {
        position += 1;
        if (position == 8) {
            direction = 2;
        }
    } else if (direction == 2) {
        position -= 1;
        if (position == 0) {
            direction = 1;
        }
    }

    if (ongoing_shoot) {
        ClearBullet(bullet_x, bullet_y);
        bullet_y -= 1;
        if (bullet_y < 0) {
            ongoing_shoot = 0; // Stop shooting when the bullet goes off-screen
            bullet_y = 5; // Reset bullet position
        } else if (bullet_y == 0 && position == bullet_x) {
            ongoing_shoot = 0; // Stop shooting when the alien is hit
            alien_hit = 1;
            hitcount+=1;// Mark alien as hit
            score+=1;  //add points for hitting the alien
            //display the score
            SetPixel(7,score,0,0,255);

            ClearBullet(bullet_x, bullet_y); // Clear the bullet after hitting the alien
            bullet_y = 5; // Reset bullet position
            if (score==3){

            	game_over=1;
            	ShowEndScreen("You win!");
            	ClearBullet(bullet_x, bullet_y); // Clear the bullet after hitting the alien


            }
        } else {
            updateBullet(bullet_x, bullet_y); // Update bullet position
        }
    }




    SetPixel(position, 0, 255, 0, 0); // Red color for the moving column (alien)


    StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
    XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);
}
void ShowEndScreen(const char *message) {
	// Clear the entire screen first
	    for (uint8_t x = 0; x < 8; x++) {
	        for (uint8_t y = 0; y < 8; y++) {
	            SetPixel(x, y, 0, 0, 0); // Turn off all pixels
	        }
	    }

	    // Draw "N"
	    SetPixel(0, 2, 255, 255, 255);
	    SetPixel(0, 3, 255, 255, 255);
	    SetPixel(0, 4, 255, 255, 255);
	    SetPixel(0, 5, 255, 255, 255);
	    SetPixel(0, 6, 255, 255, 255);
	    SetPixel(1, 2, 255, 255, 255);
	    SetPixel(2, 2, 255, 255, 255);
	    SetPixel(2, 3, 255, 255, 255);
	    SetPixel(2, 4, 255, 255, 255);
	    SetPixel(2, 5, 255, 255, 255);
	    SetPixel(2, 6, 255, 255, 255);

	    // Draw "I"
	    SetPixel(3, 2, 255, 255, 255);
	    SetPixel(3, 3, 255, 255, 255);
	    SetPixel(3, 4, 255, 255, 255);
	    SetPixel(3, 5, 255, 255, 255);
	    SetPixel(3, 6, 255, 255, 255);

	    // Draw "C"
	    SetPixel(5, 2, 255, 255, 255);
	    SetPixel(4, 3, 255, 255, 255);
	    SetPixel(4, 4, 255, 255, 255);
	    SetPixel(4, 5, 255, 255, 255);
	    SetPixel(5, 6, 255, 255, 255);

	    // Draw "E"
	    SetPixel(6, 2, 255, 255, 255);
	    SetPixel(7, 2, 255, 255, 255);
	    SetPixel(6, 3, 255, 255, 255);
	    SetPixel(6, 4, 255, 255, 255);
	    SetPixel(7, 4, 255, 255, 255);
	    SetPixel(6, 5, 255, 255, 255);
	    SetPixel(6, 6, 255, 255, 255);
	    SetPixel(7, 6, 255, 255, 255);



	    // Print the message to the console for debugging
	    xil_printf("%s\n", message);
}

void RestartGame() {
    // Clear the entire LED matrix
    for (uint8_t x = 0; x < 8; x++) {
        for (uint8_t y = 0; y < 8; y++) {
            SetPixel(x, y, 0, 0, 0); // Turn off all pixels
        }
    }

    // Reset game variables
    score = 0;
    hitcount = 0;

    game_over = 0; // Game state reset
    ongoing_shoot = 0; // Bullet inactive
    position = 0;  // Reset alien to starting position
    direction = 1; // Alien moving to the right initially

    // Reset ship position
    ship_x = 3; // Starting position
    DrawShip(ship_x); // Redraw the ship

    xil_printf("Game Restarted!\n");
}

void ButtonHandler(void *CallBackRef, u32 Bank, u32 Status) {
    switch (Status) {
        case 0b1: // Move ship to the right
            if (ship_x < 5) {
                ClearShip(ship_x);
                ship_x += 1;
                DrawShip(ship_x);
            }
            break;
        case 0b10: // Move ship to the left
            if (ship_x > 0) {
                ClearShip(ship_x);
                ship_x -= 1;
                DrawShip(ship_x);
            }
            break;
        case 0b100: // Shoot bullet
            if (!ongoing_shoot) {
                ongoing_shoot = 1;
                bullet_x = ship_x + 1;
                bullet_y = 5; // Initial bullet position just above the ship
                updateBullet(bullet_x, bullet_y); // Update bullet position
            }
            break;
        case 0b1000:
        	RestartGame();
        	break;
        default:
            break;
    }
}
