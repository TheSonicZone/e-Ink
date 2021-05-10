/*
 * disp_driver.h
 *
 *  Created on: Dec 5, 2020
 *      Author: Sonikku
 */

#ifndef DISP_DRIVER_H_
#define DISP_DRIVER_H_

// Definitions
//--------------------

// EPD1IN54 commands (from Arduino source)
//------------------------------------------------------
#define DRIVER_OUTPUT_CONTROL                       0x01
#define BOOSTER_SOFT_START_CONTROL                  0x0C
#define GATE_SCAN_START_POSITION                    0x0F
#define DEEP_SLEEP_MODE                             0x10
#define DATA_ENTRY_MODE_SETTING                     0x11
#define SW_RESET                                    0x12
#define TEMPERATURE_SENSOR_CONTROL                  0x1A
#define MASTER_ACTIVATION                           0x20
#define DISPLAY_UPDATE_CONTROL_1                    0x21
#define DISPLAY_UPDATE_CONTROL_2                    0x22
#define WRITE_RAM                                   0x24
#define WRITE_VCOM_REGISTER                         0x2C
#define WRITE_LUT_REGISTER                          0x32
#define SET_DUMMY_LINE_PERIOD                       0x3A
#define SET_GATE_TIME                               0x3B
#define BORDER_WAVEFORM_CONTROL                     0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION        0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION        0x45
#define SET_RAM_X_ADDRESS_COUNTER                   0x4E
#define SET_RAM_Y_ADDRESS_COUNTER                   0x4F
#define TERMINATE_FRAME_READ_WRITE                  0xFF


// Function Prototypes
//--------------------
void InitDriver(void);
void InitDisplay(void);
void PollBusy(void);
void SerialOut(unsigned char data);
void WriteCommand(unsigned char cmd);
void WriteData(unsigned char data);
void WriteLUT(const unsigned char *lutdata);
void DoDisplayTest(void);
void DisplayFrame(void);
void MapRAMToDisplay(int x_start, int y_start, int x_end, int y_end);
void SetLocation(int x, int y);
void BlankScreen(void);
void LoadBitmap(const unsigned char *bmp);
void LoadText(unsigned int x, unsigned int y, unsigned char height);


#endif /* DISP_DRIVER_H_ */
