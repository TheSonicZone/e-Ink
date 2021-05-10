//-----------------------------------------------------------------------------------------------------------------------------------------------
//
//                      Name: graphlib.c
//                  Driver for LCD graphics displays of all kinds
//                          Author: Sonikku
//
//                  Version 2020
//        New Features:
//                 Uses frame buffer as most displays benefit from this. On small microcontrollers this frame buffer is 8 lines high typically
//
//------------------------------------------------------------------------------------------------------------------------------------------------

#include <msp430.h>
#include "graphlib.h"
#include <stdbool.h>

unsigned char inverse_mode;
unsigned char masking;
unsigned char BsShift;





// Variables





//---------------------------------------------------------------
// Name: InitFrameBuffer
// Function: Initialise the frame buffer to a clear screen i.e. all pixels off
// Parameters: void
// Returns: void
//-----------------------------------------------------------------
void InitFrameBuffer(void){
    unsigned int n;

        DISPLAY_ROW = 0;
        DISPLAY_COLUMN = 0;
#ifdef SMALL_BUFFER
    for (n = 0; n < (DISPLAY_X_SIZE / 8) * 16; n++){
        frame_buffer[n] = 0;
    }
#else
    for (n = 0; n < DISPLAY_X_SIZE * (DISPLAY_Y_SIZE * 8); n++){
        frame_buffer[n] = 0;
    }

#endif


}


//----------------------------------------------------------------
// Name: WriteFrameBuffer
// Function: Write a byte into the display buffer
// Parameters: Pixel data in the VERTICAL orientation
// Returns: void
//----------------------------------------------------------------
void WriteFrameBuffer(unsigned char data){

    // Calculate where in array to write and write the byte there
    unsigned int location = (DISPLAY_ROW * DISPLAY_X_SIZE) + DISPLAY_COLUMN;
    frame_buffer[location] = data;
    DISPLAY_COLUMN++;
}

//----------------------------------------------------------------
// Name: DrawBar
// Function: Draw a horizontal bar at the specified XY location
// Parameters: Bar Width (pixels), Bar Pattern Mask
// Returns: void
//----------------------------------------------------------------
void DrawBar(unsigned char width, unsigned char bitmask){
    unsigned char r;

    for (r = 0; r < width; r++){
        WriteFrameBuffer(bitmask);
    }
}


//---------------------------------------------------------
// Name: ShiftBsline
// Function: Raise baseline of text by passed parameter
// Parameter: Shift value
// Returns: void
//--------------------------------------------------------
void ShiftBsline(unsigned char shift){

    BsShift = shift;

}

//-----------------------------------------------------------------------------------------------------
// Name: SetInverse
// Function: Set the inverse text mode
// Parameter: Option (TRUE = Set inverse, FALSE = Clear inverse), Masking
// Returns: void
//----------------------------------------------------------------------------------------------------
void SetInverse(unsigned char set, unsigned char mask){

    masking = mask;
    if(set == true){
        inverse_mode = true;
    } else {
        inverse_mode = false;
    }
}
//----------------------------------------------------------------------------------------------------------------------
// Name: OutChar (renamed from PutChar to prevent conflicts with similar named functions in other libs esp. Linux code)
// Function: Put a character of a specified font on the display at the specified location
// Parameters: character (ASCII), Font Type (int)
// Effects: DISP_COLUMN is updated
// Returns: void
// Updated: 22/7/17 - Removed a variable that is not used (font-header)- it is instead accessed directly
//-----------------------------------------------------------------------------------------------------------------------
void OutChar(unsigned char ascii_char, const int *fonttype){
                    unsigned char *px;                      /* Declare pointer variable */
                    unsigned int font_ptr;                  /* Font array address storage */
                    unsigned char rcnt;
                    unsigned char run_length;
                    unsigned char char_height;
                    unsigned char ref_height;
                    unsigned char glyph;
                    px = 0;                                 /* Reset pointer at runtime */
                    font_ptr = fonttype[ascii_char];
                    px = (px + (font_ptr));                 /* Set pointer to point to font data */

 /* Process character's header information */
                    run_length = (*px & 0x1F);              /* Extract run-length value */
                    char_height = (*px & 0xC0);             /* Extract cell height bits */
                    char_height = ((char_height >> 6) & 0x03);
                    ref_height = char_height;
                    char_height++;                          /* Offset by 1 */
                    px++;                                   /* Point to next byte (start of font data) */

/* Start drawing the character on the LCD */
                    while(char_height != 0){
                              for (rcnt = 0; rcnt < run_length; rcnt++){

                                      glyph = *px;

                                      // Process baseline shift, if any
                                      if(BsShift > 0){
                                          glyph = glyph >> BsShift;
                                      }

                                      // Process masking, if any
                                      if(inverse_mode == true){
                                          glyph = ~glyph & masking;
                                      }
                                      WriteFrameBuffer(glyph);           // Put glyph data on display
                                      px++;                         // Increment pointer
                              }
                              if(inverse_mode == true){
                                  WriteFrameBuffer(0xFF & masking);
                              } else {
                                  WriteFrameBuffer(0x00);           // Inter-character whitespace
                              }
                              char_height--;
                              if (char_height != 0){
                                        DISPLAY_ROW++;      /* If more than 1 row high, switch to next line */
                                        DISPLAY_COLUMN = DISPLAY_COLUMN - run_length;
                                        DISPLAY_COLUMN--;
                              }
                    }
// Restore original display row
                    DISPLAY_ROW = (DISPLAY_ROW - ref_height);

}

//----------------------------------------------------------------------------------------------
// Name: OutString
// Function: Output string at specified location on display
// Parameters: String, Pointer to Font Index Table
// Returns: void
//----------------------------------------------------------------------------------------------
void OutString(const char *string, const int *fontname){
          unsigned int textptr = 0;
          for (;;){
                    if ((string[textptr]) != 0){
                    OutChar(string[textptr], fontname);
                    textptr++;
                    } else {
                        break;
                    }
          }
}
