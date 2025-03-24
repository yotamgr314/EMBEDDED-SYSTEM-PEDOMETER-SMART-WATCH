#ifndef _XTAL_FREQ
#define _XTAL_FREQ 4000000UL
#endif
#include <libpic30.h>    // Provides __delay_ms() and __delay_us()

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "System/system.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"

// ---------------------------------------------------------------------------
// Global Definitions and Structures
// ---------------------------------------------------------------------------

// Time/Date structure
typedef struct {
    uint8_t day;
    uint8_t month;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} WatchTime_t;

// Global time variable (initialized to 01/01 00:00:00)
static volatile WatchTime_t currentTime = {
    .day    = 1,
    .month  = 1,
    .hour   = 0,
    .minute = 0,
    .second = 0
};

// Days per month (February fixed to 28 days)
static const uint8_t daysInMonth[12] = {
    31, // January
    28, // February
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

// Define the two modes for the watch display.
typedef enum {
    TIME_MODE = 0,
    PEDOMETER_MODE = 1
} WatchMode_t;

volatile WatchMode_t currentMode = TIME_MODE;

// Simulated pedometer variables.
static volatile uint16_t stepsPerMinute = 0;
static volatile bool iconToggle = false;  // Used to alternate the animated icon

// ---------------------------------------------------------------------------
// Timer1 Setup: 1-second interrupt
// ---------------------------------------------------------------------------
static void InitializeTimer1(void)
{
    T1CONbits.TON = 0;      // Stop Timer1
    T1CONbits.TCS = 0;      // Use internal clock (Fcy)
    T1CONbits.TCKPS = 3;    // Prescaler 1:256
    // Assuming Fcy ~4 MHz, Timer1 frequency = 4MHz / 256 = 15625 Hz.
    // For a 1-second interval, PR1 = 15625 - 1 = 15624.
    PR1 = 15624;
    TMR1 = 0;
    
    IFS0bits.T1IF = 0;      // Clear Timer1 interrupt flag
    IEC0bits.T1IE = 1;      // Enable Timer1 interrupt
    IPC0bits.T1IP = 4;      // Set interrupt priority

    T1CONbits.TON = 1;      // Start Timer1
}

// ---------------------------------------------------------------------------
// Helper function to increment time/date once per second
// ---------------------------------------------------------------------------
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

// ---------------------------------------------------------------------------
// Function to draw the Time & Date display (TIME_MODE)
// ---------------------------------------------------------------------------
static void DrawTimeDate(void)
{
    // Clear a portion of the screen (top area) where time/date is shown.
    oledC_DrawRectangle(0, 0, 95, 50, OLEDC_COLOR_BLACK);

    // Format time as "HH:MM:SS" (24-hour format)
    char timeString[16];
    sprintf(timeString, "%02u:%02u:%02u", currentTime.hour, currentTime.minute, currentTime.second);

    // Format date as "DD/MM"
    char dateString[16];
    sprintf(dateString, "%02u/%02u", currentTime.day, currentTime.month);

    // Draw time (scale 2 for larger digits) and date (scale 1)
    oledC_DrawString(2, 2, 2, 2, (uint8_t*)timeString, OLEDC_COLOR_WHITE);
    oledC_DrawString(4, 30, 1, 1, (uint8_t*)dateString, OLEDC_COLOR_YELLOW);
}

// ---------------------------------------------------------------------------
// Function to draw the Pedometer display (PEDOMETER_MODE)
// ---------------------------------------------------------------------------
static void DrawPedometerDisplay(void)
{
    // Clear the full display to black.
    oledC_DrawRectangle(0, 0, 95, 95, OLEDC_COLOR_BLACK);

    // Draw an animated icon in the top-left corner.
    // When iconToggle is true, draw a filled circle; else draw a ring.
    if (iconToggle)
    {
        oledC_DrawCircle(15, 15, 10, OLEDC_COLOR_GREEN);
    }
    else
    {
        oledC_DrawRing(15, 15, 10, 2, OLEDC_COLOR_GREEN);
    }

    // Display the simulated steps count.
    char stepsString[20];
    sprintf(stepsString, "Steps: %u", stepsPerMinute);
    oledC_DrawString(2, 40, 1, 1, (uint8_t*)stepsString, OLEDC_COLOR_WHITE);
}

// ---------------------------------------------------------------------------
// Button debouncing: Checks if S1 (RA11) is pressed (active low)
// ---------------------------------------------------------------------------
static bool IsS1Pressed(void)
{
    if (!PORTAbits.RA11)
    {
        __delay_ms(50); // Debounce delay
        if (!PORTAbits.RA11)
            return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Timer1 Interrupt Service Routine (ISR)
// Updates the time and refreshes the display based on current mode.
// ---------------------------------------------------------------------------
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0; // Clear Timer1 interrupt flag

    // Update the global time.
    IncrementTime();

    if (currentMode == TIME_MODE)
    {
        // In TIME_MODE, update the time/date display.
        DrawTimeDate();
    }
    else if (currentMode == PEDOMETER_MODE)
    {
        // In PEDOMETER_MODE, simulate step count update.
        stepsPerMinute += 5;       // Increase simulated step count
        iconToggle = !iconToggle;  // Toggle animated icon state
        DrawPedometerDisplay();
    }
}

// ---------------------------------------------------------------------------
// Main Function - Merged Phase 1 and Phase 2
// ---------------------------------------------------------------------------
int main(void)
{
    // Initialize system, OLED, etc.
    SYSTEM_Initialize();

    // Set OLED boundaries (assumes a 96x96 display)
    oledC_setColumnAddressBounds(0, 95);
    oledC_setRowAddressBounds(0, 95);

    // Clear OLED display entirely (set background to black)
    oledC_startWritingDisplay();
    for (uint16_t i = 0; i < 96 * 96; i++)
    {
        oledC_sendColorInt(OLEDC_COLOR_BLACK);
    }
    oledC_stopWritingDisplay();

    // Configure S1 button on RA11 as input.
    TRISAbits.TRISA11 = 1;

    // Initialize Timer1 for 1-second interrupts.
    InitializeTimer1();

    // Main loop: poll S1 to toggle between TIME_MODE and PEDOMETER_MODE.
    while (1)
    {
        if (IsS1Pressed())
        {
            // Toggle display mode on button press.
            if (currentMode == TIME_MODE)
            {
                currentMode = PEDOMETER_MODE;
            }
            else
            {
                currentMode = TIME_MODE;
            }
            // Delay to prevent bouncing or multiple toggles.
            __delay_ms(500);
        }
    }
    
    return 0;
}
