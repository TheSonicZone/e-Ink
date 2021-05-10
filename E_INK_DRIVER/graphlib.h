//-------------------------------------------------------------------------------------------------------
//                      Header for graphlib.c
//-------------------------------------------------------------------------------------------------------

#ifndef GRAPHLIB_H_
#define GRAPHLIB_H_

#define DISPLAY_X_SIZE 200
#define DISPLAY_Y_SIZE 200

#define HORIZONTAL_DISPLAY_PIXEL_ORG
#define SMALL_BUFFER                    // Small buffers typ. 2 display lines (for micros with very little RAM). The reason is to be able to draw big fonts too
//#define VERTICAL_DISPLAY_PIXEL_ORG



// Variables
//----------------------
unsigned char DISPLAY_COLUMN;    // DISPLAY_ROW (x-direction in framebuffer)
unsigned char DISPLAY_ROW;       // DISPLAY_COLUMN (y-direction in framebuffer)


#ifdef HORIZONTAL_DISPLAY_PIXEL_ORG
#ifdef SMALL_BUFFER
unsigned char frame_buffer[(DISPLAY_X_SIZE / 8) * 16];                    // Frame buffer small
#else
unsigned char frame_buffer[(DISPLAY_X_SIZE / 8) * DISPLAY_Y_SIZE];        // Frame buffer standard size
#endif
#endif


#ifdef VERTICAL_DISPLAY_PIXEL_ORG
#ifdef SMALL_BUFFER
unsigned char frame_buffer[16 * (DISPLAY_Y_SIZE / 8)];                    // Frame buffer small
#else
unsigned char frame_buffer[DISPLAY_X_SIZE * (DISPLAY_Y_SIZE * 8)];        // Frame buffer standard size
#endif
#endif

// Function Prototypes
//----------------------
void InitFrameBuffer(void);
void WriteFrameBuffer(unsigned char data);
void DrawBar(unsigned char width, unsigned char bitmask);
void ShiftBsline(unsigned char shift);
void SetInverse(unsigned char set, unsigned char mask);
void OutChar(unsigned char ascii_char, const int *fonttype);
void OutString(const char *string, const int *fontname);

#endif /* GRAPHLIB_H_ */
