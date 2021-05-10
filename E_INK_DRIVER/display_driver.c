//--------------------------------------------------------------------------------------------------------------------------
//
//                      Name: display_driver.c
//                  Low level driver for e-ink Display
//                          Author: Sonikku
//
//              Derived from an Arduino sketch provided by WaveShare and other sources
//                           IDENTIFIED as GDEW0154Z04
//              The datasheet for the GDEW device is FAR BETTER than the half-baked crappy Chinese "datasheet" from Waveshare
//              The GDE datasheet also explains what the LUTs are for
//              This also helps tremendously: https://github.com/soonuse/gdep015oc1_1.54inch_e-paper/blob/master/stm32/BSP/epd1in54.c
//---------------------------------------------------------------------------------------------------------------------------

// What is "partial update" and "full update"?
// Former only partially writes the e-ink, latter fully writes it (familiar transitions as seen on Amazon kindle display)

#include <msp430.h>
#include "disp_driver.h"
#include "graphlib.h"






// Look-up tables as used by the display
// These are basically back-to-back command blocks
//---------------------------------------------------
const unsigned char lut_full_update[] ={
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22,
    0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88,
    0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51,
    0x35, 0x51, 0x51, 0x19, 0x01, 0x00
};

const unsigned char lut_partial_update[] ={
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Definitions on GPIO pins for display interfacing
//---------------------------------------------------


#define BUSY_IN BIT1            // P1.1 as input
#define RESET BIT2              // P1.2 as output
#define D_C BIT4                // P1.4 as output
#define CS  BIT5                // P1.5 as output
#define CLK BIT0                // P2.0 as output
#define DATA BIT1               // P2.1 as output


// Other definitions
//------------------
#define DISPLAY_X_SIZE 200
#define DISPLAY_Y_SIZE 200

// Functions
//-------------

// Name: InitDriver
// Function: Called at startup to configure the MSP430 GPIO pins to proper states
//-------------------------------------------------------------------------------
void InitDriver(void){

    // Initialise all the pins

    P2OUT &= ~CLK;              // CLK LOW
    P2OUT &= ~DATA;             // DATA LOW
    P1OUT |= RESET;             // RESET HIGH
    P1OUT |= CS;                // CHIP SELECT HIGH (inactive state)
    P1OUT &= ~D_C;              // DATA/COMMAND pin LOW

}

//---------------------------------------------------------------------
// Name: Reset
// Function: Reset the display
// Parameters: void
// Returns: void
//---------------------------------------------------------------------
void Reset(void){
    P1OUT &= ~RESET;            // Reset LOW
    __delay_cycles(30);
    P1OUT |= RESET;             // Reset HIGH
    __delay_cycles(80);
}

//---------------------------------------------------------------------
// Name: InitDisplay
// Function: Initialise the display to a ready condition
// Parameters: void
// Returns: void
//----------------------------------------------------------------------
void InitDisplay(void){

        Reset();
        PollBusy();
        WriteCommand(DRIVER_OUTPUT_CONTROL);
        WriteData((DISPLAY_Y_SIZE - 1) & 0xFF);             // MUX setting
        WriteData(((DISPLAY_Y_SIZE - 1) >> 8) & 0xFF);      // MUX setting bit 8
        WriteData(0x00);                                    // Scan Direction G0 -> G299

        WriteCommand(BOOSTER_SOFT_START_CONTROL);
        WriteData(0xD7);
        WriteData(0xD6);
        WriteData(0x9D);
        WriteCommand(WRITE_VCOM_REGISTER);
        WriteData(0xA8);                     // VCOM 7C
        WriteCommand(SET_DUMMY_LINE_PERIOD);
        WriteData(0x1A);                     // 4 dummy lines per gate
        WriteCommand(SET_GATE_TIME);
        WriteData(0x08);                     // 2us per line
        WriteCommand(DATA_ENTRY_MODE_SETTING);
        WriteData(0x03);                     // X increment; Y increment
        WriteLUT(lut_full_update);       // Now depending on the update mode, a look-up-table is written // TODO - determine what is optimal for this use case

}

//----------------------------------------------------------------------------------------------------------------------
// Name: DisplayText
// Function: Takes the small text buffer and builds it into the main display buffer write at the desired line location
// Parameters: Location on display where the text must appear (x; y)
//             Font height (0 = single; 1 = double)
//             Note that this leverages the display hardware, therefore using minimal resources on the micro
//----------------------------------------------------------------------------------------------------------------------
void LoadText(unsigned int x, unsigned int y, unsigned char height){

    unsigned char linecnt;
    unsigned int g = 0;
    unsigned char h = 0;
    unsigned char temp = 0;
    unsigned char bitpos = 1;
    unsigned char bpos = 0x80;
    unsigned int target_addr = 0;
    unsigned char frame_data;

    MapRAMToDisplay(0, 0, DISPLAY_X_SIZE - 1, DISPLAY_Y_SIZE - 1);
    SetLocation(x, y);
    WriteCommand(WRITE_RAM);


    // Need to write 8 horizontal lines
    // Each horizontal line is 25 bytes of data
    for (linecnt = 0; linecnt < 8; linecnt++){
        for (g = 0; g < 25; g++){
            for (h = 0; h < 8; h++){
                frame_data = frame_buffer[target_addr];           // get frame buffer byte
                if ((frame_data & bitpos) == bitpos){
                    temp |= bpos;
                } else {
                    temp &= ~bpos;
                }
                target_addr++;
                bpos >>= 1;
            }
            WriteData(~temp);
            bpos = 0x80;

        }

        target_addr = 0;
        bitpos <<= 1;

    }

    target_addr = 200;
    bitpos = 1;
    bpos = 0x80;

    if (height > 0){
        for (linecnt = 0; linecnt < 8; linecnt++){
            for (g = 0; g < 25; g++){
                for (h = 0; h < 8; h++){
                    frame_data = frame_buffer[target_addr];           // get frame buffer byte
                    if ((frame_data & bitpos) == bitpos){
                        temp |= bpos;
                    } else {
                        temp &= ~bpos;
                    }
                    target_addr++;
                    bpos >>= 1;
                }
                WriteData(~temp);
                bpos = 0x80;
            }

            target_addr = 200;
            bitpos <<= 1;
        }
    }

}


//-----------------------------------------------------------------
// Name: LoadBitmap
// Function: Load LCDImager compatible bitmap
//----------------------------------------------------------------------
void LoadBitmap(const unsigned char *bmp){

    unsigned int n = 0;


    //unsigned char y = 0;
    //unsigned char toggle = 0;
    // So, similar to the ST7535, we need to set up the XY position, then we can write to RAM
    MapRAMToDisplay(0, 0, DISPLAY_X_SIZE - 1, DISPLAY_Y_SIZE - 1);
    SetLocation(0, 0);
    WriteCommand(WRITE_RAM);
    // Attention, can't use a for() loop here- the MSP430 compiler tries to inline it and it doesn't work then
 //   while (n < 50){
 //       WriteData(~bmp[n]);
 //      n++;
 //   }
    // Use this code if --opt_level=off
    for (n = 0; n < ((DISPLAY_X_SIZE / 8) * DISPLAY_Y_SIZE); n++){
        WriteData(~bmp[n]);
    }


}

//----------------------------------------------------------------------
// Name: DoDisplayTest
// Function: Put crap in buffers to get some kind of display
//----------------------------------------------------------------------
void DoDisplayTest(void){
    unsigned int n;
    unsigned int r;
    unsigned char y = 0;
    unsigned char toggle = 0;
    // So, similar to the ST7535, we need to set up the XY position, then we can write to RAM
    MapRAMToDisplay(0, 0, DISPLAY_X_SIZE - 1, DISPLAY_Y_SIZE - 1);
    SetLocation(0, 0);

    // Draw big blocks of checkeboard
    WriteCommand(WRITE_RAM);

    for (r = 0; r < 200; r++){

        for (n = 0; n < 25; n++){
            if (toggle == 0){
                WriteData(0x00);
                toggle = 1;
            } else {
                WriteData(0xFF);
                toggle = 0;
            }
        }

        y++;
        if (y < 8){
            if (toggle == 1){
                toggle = 0;
            } else {
                toggle = 1;
            }
        } else {
            y = 0;
        }

    }


}

//----------------------------------------------------------------------
// Name: BlankScreen
// Function: Fill buffer with data that clears the entire display
//----------------------------------------------------------------------
void BlankScreen(void){
    unsigned int n;
    // So, similar to the ST7535, we need to set up the XY position, then we can write to RAM
    MapRAMToDisplay(0, 0, DISPLAY_X_SIZE - 1, DISPLAY_Y_SIZE - 1);
    SetLocation(0, 0);


    WriteCommand(WRITE_RAM);
    for (n = 0; n < ((DISPLAY_X_SIZE / 8) * DISPLAY_Y_SIZE); n++){
        WriteData(0xFF);
    }

    MapRAMToDisplay(0, 0, DISPLAY_X_SIZE - 1, DISPLAY_Y_SIZE - 1);
    SetLocation(0, 0);


    WriteCommand(WRITE_RAM);
    for (n = 0; n < ((DISPLAY_X_SIZE / 8) * DISPLAY_Y_SIZE); n++){
        WriteData(0xFF);
    }

}

//---------------------------------------------------------------------
// Name: SetLocation
// Function: Specify where data will be written in DISPLAY BUFFER
//---------------------------------------------------------------------
void SetLocation(int x, int y){
    WriteCommand(SET_RAM_X_ADDRESS_COUNTER);
    WriteData((x >> 3) & 0xFF);
    WriteCommand(SET_RAM_Y_ADDRESS_COUNTER);        // This is so similar to the ST7565???
    WriteData(y & 0xFF);
    WriteData((y >> 8) & 0xFF);
    PollBusy();

}

//---------------------------------------------------------------------
// Name: MapRAMToDisplay
// Function: Configure the controller to the display active area
//--------------------------------------------------------------------
void MapRAMToDisplay(int x_start, int y_start, int x_end, int y_end){
    WriteCommand(SET_RAM_X_ADDRESS_START_END_POSITION);
    WriteData((x_start >> 3) & 0xFF);
    WriteData((x_end >> 3) & 0xFF);
    WriteCommand(SET_RAM_Y_ADDRESS_START_END_POSITION);
    WriteData(y_start & 0xFF);
    WriteData((y_start >> 8) & 0xFF);
    WriteData(y_end & 0xFF);
    WriteData((y_end >> 8) & 0xFF);
}

//---------------------------------------------------------------------
// Name: DisplayFrame
// Function: Transfer memory contents to glass
//---------------------------------------------------------------------
void DisplayFrame(void){
    WriteCommand(DISPLAY_UPDATE_CONTROL_2);
    WriteData(0xC4);
    WriteCommand(MASTER_ACTIVATION);
    WriteCommand(TERMINATE_FRAME_READ_WRITE);
    PollBusy();
}

//-----------------------------------------------------------------------------------
// Name: WriteLUT
// Function: Write given data to look-up-table
// Since the Chinese don't disclose the controller chip, not sure what this is for
//-----------------------------------------------------------------------------------
void WriteLUT(const unsigned char *lutdata){
    unsigned char n;
    WriteCommand(WRITE_LUT_REGISTER);
    for (n = 0; n < 30; n++){
        WriteData(lutdata[n]);
    }



}
//---------------------------------------------------------------------
// Name: PollBusy
// Function: Poll BUSY signal, and return when LOW = IDLE
// Parameters: void
// Returns: void
//---------------------------------------------------------------------
void PollBusy(void){
    while ((P1IN & BUSY_IN) != 0){
        ;
    }
}

//---------------------------------------------------------------------
// Name: WriteCommand
// Function: Write a command to the e-ink controller
// Parameters: Command code
// Returns: void
//----------------------------------------------------------------------
void WriteCommand(unsigned char cmd){

    P1OUT &= ~D_C;                  // Data/Command pin = 0
    SerialOut(cmd);

}

//---------------------------------------------------------------------
// Name: WriteData
// Function: Write data to the e-ink controller
// Parameters: Data to be written
// Returns: void
//----------------------------------------------------------------------
void WriteData(unsigned char data){

    P1OUT |= D_C;               // Data/Command pin = 1
    SerialOut(data);

}

//---------------------------------------------------------------------
// Name: SerialOut
// Function: Transfer byte on serial interface
// Parameter: Data to be sent
//----------------------------------------------------------------------
void SerialOut(unsigned char data){
    unsigned char dout;
    unsigned char n;
    P1OUT &= ~CS;           // Lower CS
   // __delay_cycles(2);      // was 4
    dout = data;
    for (n = 0; n < 8; n++){

        // Copy MSB of data to port PIN
        if ((dout & 0x80) == 0x80){
            P2OUT |= DATA;
        } else {
            P2OUT &= ~DATA;
        }
        dout = dout << 1;
        // Clock the bit
        P2OUT |= CLK;
       // __delay_cycles(2);   // was 10
        P2OUT &= ~CLK;
       // __delay_cycles(2);
    }

    P1OUT |= CS;


}


