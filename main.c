/******************************************************************************
  Phase 1: Basic Time & Date, 1-second ISR, Large Display
  (No Menu, No Step Counting, No Graph)
*******************************************************************************/

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "System/system.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"

// ---------------------------------------------------------------------------
// Global time/date structure updated by Timer1 ISR
// ---------------------------------------------------------------------------
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

// For simplicity: 28-day February, no year tracking
static const uint8_t daysInMonth[12] = {
    31, // January
    28, // February (fixed 28 days)
    31, // March
    30, // April
    31, // May
    30, // June
    31, // July
    31, // August
    30, // September
    31, // October
    30, // November
    31  // December
};

// ---------------------------------------------------------------------------
// Timer1: 1-second interrupt to increment time
// ---------------------------------------------------------------------------
static void InitializeTimer1(void)
{
    // Stop Timer1
    T1CONbits.TON = 0;      
    // Use internal clock (Fcy), 1:256 prescaler
    T1CONbits.TCS = 0;      
    T1CONbits.TCKPS = 3;    // 1:256 prescale
    // (Fcy = ~4 MHz if default FRC; adjust if your system uses a different clock)
    // PR1 calculation for ~1 second:
    // If Fcy=4 MHz, then Timer1 freq = 4 MHz / 256 = 15625 Hz
    // For 1-second, PR1 = 15625 - 1 = 15624
    PR1 = 15624;
    TMR1 = 0;

    // Clear and enable the interrupt
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    IPC0bits.T1IP = 4; // priority

    // Start Timer1
    T1CONbits.TON = 1;
}

// Simple helper to increment time & date once per second
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
                // Check if we passed the end of the month
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

// ---------------------------------------------------------------------------
// Draw Time & Date in large digits
// Called by the ISR each second (or could be from main)
// ---------------------------------------------------------------------------
static void DrawTimeDate(void)
{
    // Clear just a portion of the screen for a quick refresh,
    // or do a full screen clear if you prefer. 
    // We'll do a quick rectangle clear for demonstration:
    //   (top-left x=0, y=0, bottom-right x=95, y=50)
    oledC_DrawRectangle(0, 0, 95, 50, OLEDC_COLOR_BLACK);

    // Format: "HH:MM:SS" in 24-hour format
    char timeString[16];
    sprintf(timeString, "%02u:%02u:%02u", 
            currentTime.hour, currentTime.minute, currentTime.second);

    // Format: "DD/MM"
    char dateString[16];
    sprintf(dateString, "%02u/%02u", currentTime.day, currentTime.month);

    // We can draw the time in scale=2 (bigger), date in scale=1
    // Adjust positions as you like
    oledC_DrawString(2, 2,    2, 2, (uint8_t*)timeString, OLEDC_COLOR_WHITE);
    oledC_DrawString(4, 30,   1, 1, (uint8_t*)dateString, OLEDC_COLOR_YELLOW);
}

// ---------------------------------------------------------------------------
// Timer1 ISR: increments time, updates display
// ---------------------------------------------------------------------------
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    // Clear interrupt flag
    IFS0bits.T1IF = 0;

    // 1) Update the time
    IncrementTime();

    // 2) Update the OLED time & date display
    //    (If flicker occurs, consider double-buffer or partial redraw.)
    DrawTimeDate();
}

// ---------------------------------------------------------------------------
// MAIN - Phase 1
// ---------------------------------------------------------------------------
int main(void)
{
    // Initialize board & OLED
    SYSTEM_Initialize();
    oledC_setColumnAddressBounds(0, 95);
    oledC_setRowAddressBounds(0, 95);

    // Optional: set entire background to black
    oledC_startWritingDisplay();
    for(uint16_t i=0; i<96*96; i++)
    {
        oledC_sendColorInt(OLEDC_COLOR_BLACK);
    }
    oledC_stopWritingDisplay();

    // Initialize Timer1 for 1-second ticks
    InitializeTimer1();

    // (We do not implement step counting or menu yet in Phase 1)
    // The watch time will auto-update every second via the ISR.

    while(1)
    {
        // Do nothing here for now.
        // All time updates & screen draws are in the ISR.
    }

    return 0; 
}
