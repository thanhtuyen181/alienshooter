//ARM INSTRUCTIONS
//Instructions are conditional
//32-bit instruction


//How to load data from memory:
//LDR R0, = 0x41200000	(Load address to register. You can use any global variable defined in main.c, or variable found here)
//LDR R1, [R0] 			(Load data from address, in this case 32-bit data)
//LDRB R1, [R0] 		(Load data from address, in this case 8-bit data)
//******
//LDR R0, =0x123456		(Load address to register, Use only if you know the memory address)
//LDR R1, [R0]			(load data from address, in this case 32-bit data)
//LDRB R1, [R0]			(load data from address, in this case 8-bit data)

//How to store data to memory:
//STR R1,[R0]			(R1 contains the data and R0 contains the address where  to store, 32-bit)
//STRB R1,[R0]			(R1 contains the data and R0 contains the address where  to store, 8-bit)


//.data
	//dir: .byte 1		//8-bit variable for direction
	                    // Add position here


//.text

.global blinker


blinker:
	PUSH {R4-R6, LR}       // Save registers R4-R6 and the link register

    LDR R2, =0x41200000    // Base address for LEDs
    LDR R3, =0b0001        // Start with LD0 (Bit 0)
    MOV R4, #0             // Direction flag: 0 for left, 1 for right

main_loop:
    STR R3, [R2]           // Write the LED state to the memory-mapped address
    BL delay               // Call delay function

    CMP R4, #0             // Check the direction flag
    BEQ shift_left         // If 0, shift left
    B shift_right          // Otherwise, shift right

shift_left:
    LSLS R3, R3, #1        // Shift left to the next LED
    CMP R3, #0b1000        // Check if LD3 is reached
    BNE main_loop          // If not, continue
    MOV R4, #1             // Set direction to right
    B main_loop

shift_right:
    LSRS R3, R3, #1        // Shift right to the previous LED
    CMP R3, #0b0001        // Check if LD0 is reached
    BNE main_loop          // If not, continue
    MOV R4, #0             // Set direction to left
    B main_loop

delay:
    PUSH {R0, LR}          // Save R0 and LR
    LDR R0, =0xFFFFFFF      // Load delay counter (adjust for visibility)
delay_loop:
    SUBS R0, R0, #1        // Decrement delay counter
    BNE delay_loop         // Repeat until counter reaches 0
    POP {R0, LR}           // Restore R0 and LR
    BX LR                  // Return from delay

    POP {R4-R6, LR}        // Restore saved registers
    BX LR                  // Return from blinker
