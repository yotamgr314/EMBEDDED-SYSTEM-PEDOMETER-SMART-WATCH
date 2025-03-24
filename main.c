/******************************************************************************
  Complete main.c
  - Updates time/date every second via Timer1 ISR.
  - Supports toggling between 24‑hour and 12‑hour display modes using a button on RA11.
  - Uses a per‑character update method for the time and date.
  - In 12‑hour mode, after updating the time string, it explicitly clears the 
    rightmost two columns (columns 94-95) of the time display region to remove
    any leftover white pixels.
  
  Note:
  - This code assumes that the OLED driver APIs (oledC_DrawString, oledC_DrawRectangle, etc.)
    are available.
  - The __delay_ms() and __delay_us() functions require inclusion of <libpic30.h>
    and a proper _XTAL_FREQ definition.
*******************************************************************************/

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <libpic30.h>   // For __delay_ms() and __delay_us()

#include "System/system.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"

//---------------------------------------------------------------------
// Global Time/Date Structure
//---------------------------------------------------------------------
typedef struct {
    uint8_t day;
    uint8_t month;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} WatchTime_t;

static volatile WatchTime_t currentTime = {
    .day    = 1,
    .month  = 1,
    .hour   = 0,
    .minute = 0,
    .second = 0
};

// For simplicity, February is fixed to 28 days.
static const uint8_t daysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//---------------------------------------------------------------------
// Global display mode: false = 24-hour; true = 12-hour (with AM/PM)
// Volatile because it's modified in main() and read in the ISR.
volatile bool use12HourFormat = false;

//---------------------------------------------------------------------
// Timer1 Initialization (assumes Fcy ~4MHz)
//---------------------------------------------------------------------
static void InitializeTimer1(void)
{
    T1CONbits.TON = 0;      // Stop Timer1
    T1CONbits.TCS = 0;      // Use internal clock
    T1CONbits.TCKPS = 3;    // 1:256 prescaler
    // With Fcy ~4MHz, Timer1 frequency = 4MHz/256 = 15625 Hz.
    // For a 1-second period, PR1 = 15625 - 1 = 15624.
    PR1 = 15624;
    TMR1 = 0;
    IFS0bits.T1IF = 0;      // Clear Timer1 interrupt flag
    IEC0bits.T1IE = 1;      // Enable Timer1 interrupt
    IPC0bits.T1IP = 4;      // Set Timer1 interrupt priority
    T1CONbits.TON = 1;      // Start Timer1
}

//---------------------------------------------------------------------
// Increment the global time by one second (with rollover logic)
//---------------------------------------------------------------------
static void IncrementTime(void)
{
    currentTime.second++;
    if (currentTime.second >= 60)
    {
        currentTime.second = 0;
        currentTime.minute++;
        if (currentTime.minute >= 60)
        {
            currentTime.minute = 0;
            currentTime.hour++;
            if (currentTime.hour >= 24)
            {
                currentTime.hour = 0;
                currentTime.day++;
                uint8_t mIndex = (currentTime.month - 1) % 12;
                if (currentTime.day > daysInMonth[mIndex])
                {
                    currentTime.day = 1;
                    currentTime.month++;
                    if (currentTime.month > 12)
                    {
                        currentTime.month = 1;
                    }
                }
            }
        }
    }
}

//---------------------------------------------------------------------
// Helper: Per-character update function.
// Compares newStr with oldStr and only redraws changed characters.
// Uses a fixed 5x8 font scaled by scale_x and scale_y with 1-pixel spacing.
// For the last character in the new string, clears only the character width.
// If the old string is longer than the new one, clears the extra region.
//---------------------------------------------------------------------
static void DrawStringChanged(uint8_t x, uint8_t y, uint8_t scale_x, uint8_t scale_y,
                                char *newStr, char *oldStr,
                                uint16_t fgColor, uint16_t bgColor)
{
    const uint8_t charWidth = 5 * scale_x;
    const uint8_t spacing = 1;             // Fixed spacing between characters.
    const uint8_t charHeight = 8 * scale_y;
    
    uint8_t newLen = (uint8_t)strlen(newStr);
    uint8_t oldLen = (uint8_t)strlen(oldStr);
    
    // For each character in the new string:
    for (uint8_t i = 0; i < newLen; i++) {
        char newChar = newStr[i];
        char oldChar = (i < oldLen) ? oldStr[i] : '\0';
        if (newChar != oldChar) {
            uint8_t posX = x + i * (charWidth + spacing);
            // For the last character, clear only the character width (no extra spacing).
            uint8_t widthToClear = (i == newLen - 1) ? charWidth : (charWidth + spacing);
            oledC_DrawRectangle(posX, y, posX + widthToClear - 1, y + charHeight - 1, bgColor);
            if (newChar != '\0') {
                char temp[2] = {newChar, '\0'};
                oledC_DrawString(posX, y, scale_x, scale_y, (uint8_t*)temp, fgColor);
            }
        }
    }
    // If the old string was longer than the new one, clear the extra region.
    if (oldLen > newLen) {
        uint8_t posX = x + newLen * (charWidth + spacing);
        uint8_t extraWidth = (oldLen - newLen) * (charWidth + spacing) - spacing;
        oledC_DrawRectangle(posX, y, posX + extraWidth - 1, y + charHeight - 1, bgColor);
    }
    // Update the stored old string.
    strcpy(oldStr, newStr);
}

//---------------------------------------------------------------------
// Updated DrawTimeDate: Formats time/date and updates the OLED using
// per-character update for both time and date.
// For the time string, after the update, clear the rightmost columns
// to ensure no extra white line appears when in 12-hour mode.
//---------------------------------------------------------------------
static void DrawTimeDate(void)
{
    static char lastTimeString[20] = "";
    static char lastDateString[16] = "";
    
    char newTimeString[20];
    char newDateString[16];
    
    // Format the time string.
    if (use12HourFormat)
    {
        uint8_t hour12 = currentTime.hour % 12;
        if (hour12 == 0)
            hour12 = 12;
        const char *period = (currentTime.hour < 12) ? "AM" : "PM";
        sprintf(newTimeString, "%02u:%02u:%02u %s", hour12, currentTime.minute, currentTime.second, period);
    }
    else
    {
        sprintf(newTimeString, "%02u:%02u:%02u", currentTime.hour, currentTime.minute, currentTime.second);
    }
    
    // Format the date string.
    sprintf(newDateString, "%02u/%02u", currentTime.day, currentTime.month);
    
    // Update the time display at (2,2) with scale 2 (white on black).
    DrawStringChanged(2, 2, 2, 2, newTimeString, lastTimeString, OLEDC_COLOR_WHITE, OLEDC_COLOR_BLACK);
    
    // --- Extra Patch for 12-hour mode ---
    // If in 12-hour mode, explicitly clear the rightmost two columns (94 and 95)
    // in the time display area. For scale 2 and font height 16 (8*2), clear region:
    if (use12HourFormat) {
        // Clear columns 94 to 95 for rows from y=2 to y=2+15.
        oledC_DrawRectangle(94, 2, 95, 2 + (8 * 2) - 1, OLEDC_COLOR_BLACK);
    }
    
    // Update the date display at (4,30) with scale 1 (yellow on black).
    DrawStringChanged(4, 30, 1, 1, newDateString, lastDateString, OLEDC_COLOR_YELLOW, OLEDC_COLOR_BLACK);
}

//---------------------------------------------------------------------
// Timer1 ISR: Called every second to update time and refresh the display.
//---------------------------------------------------------------------
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;  // Clear Timer1 interrupt flag
    IncrementTime();
    DrawTimeDate();
}

//---------------------------------------------------------------------
// Main: Initialize system, clear OLED screen, start timer, and poll button.
//---------------------------------------------------------------------
int main(void)
{
    // Initialize system (clock, pins, OLED, etc.)
    SYSTEM_Initialize();
    
    // Set OLED boundaries.
    oledC_setColumnAddressBounds(0, 95);
    oledC_setRowAddressBounds(0, 95);
    
    // Clear the entire OLED screen to black.
    oledC_startWritingDisplay();
    for (uint16_t i = 0; i < 96 * 96; i++) {
        oledC_sendColorInt(OLEDC_COLOR_BLACK);
    }
    oledC_stopWritingDisplay();
    
    // Initialize Timer1 for 1-second updates.
    InitializeTimer1();
    
    // Main loop: Poll button on RA11 (active low) to toggle display mode.
    bool lastButtonState = true;  // Assume button is not pressed.
    while (1)
    {
        bool currentButtonState = PORTAbits.RA11;  // Read button state.
        if (lastButtonState && (currentButtonState == false))
        {
            __delay_ms(50);  // Debounce delay.
            if (PORTAbits.RA11 == false)
            {
                use12HourFormat = !use12HourFormat;
            }
        }
        lastButtonState = currentButtonState;
    }
    
    return 0;
}
