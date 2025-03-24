
#include <stdlib.h>
#include <stdio.h>
#include "System/system.h"
#include "System/delay.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"

typedef unsigned char MODE;
#define COLOR_DISPLAY  0
#define COMPOSITE_DISPLAY !COLOR_DISPLAY
#define UNINITIALIZED  2
static int index = 0;
static uint16_t background_color;



uint16_t brightness[3] = {1023, 1023, 1023};
bool isBalanced = false;
bool c_isBalanced = false;

static void oledC_clearScreen(void) {
    uint8_t x, y;
    oledC_setColumnAddressBounds(0, 96);
    oledC_setRowAddressBounds(0, 96);
    for (x = 0; x < 96; x++) {
        for (y = 0; y < 96; y++) {
            oledC_sendColorInt(background_color);
        }
    }
}

static void oledC_setBackground(uint16_t color) {
    background_color = color;
    oledC_clearScreen();
}

void User_Initialize(void) {
    TRISBbits.TRISB12 = 1;
    ANSBbits.ANSB12 = 1;
    AD1CON1bits.SSRC = 0;
    AD1CON1bits.FORM = 0;
    AD1CON1bits.ASAM = 0;
    AD1CON1bits.ADSIDL = 0;
    AD1CON1bits.DMABM = 0;
    AD1CON1bits.DMAEN = 0;
    AD1CON1bits.MODE12 = 0;

    AD1CON2 = 0x0000;
    AD1CON3bits.ADCS = 0xFF;
    AD1CON3bits.SAMC = 0x10;
    AD1CON3bits.ADRC = 0x00;
    AD1CON3bits.EXTSAM = 0x00;
    AD1CON3bits.PUMPEN = 0x00;

    TRISAbits.TRISA11 = 1;
    TRISAbits.TRISA12 = 1;
    TRISAbits.TRISA8 = 0;
    TRISAbits.TRISA9 = 0;

    AD1CHSbits.CH0SA = 8;
    AD1CON1bits.ADON = 1;
}

void RedLed() {
    RPOR13bits.RP26R = 13;
    OC1RS = 1023;
    OC1CON2bits.SYNCSEL = 0x1F;
    OC1CON2bits.OCTRIG = 0;
    OC1CON1bits.OCTSEL = 0b111;
    OC1CON1bits.OCM = 0b110;
    OC1CON2bits.TRIGSTAT = 1;
    OC1R = 0;
}

void GreenLed() {
    RPOR13bits.RP27R = 14;
    OC2RS = 1023;
    OC2CON2bits.SYNCSEL = 0x1F;
    OC2CON2bits.OCTRIG = 0;
    OC2CON1bits.OCTSEL = 0b111;
    OC2CON1bits.OCM = 0b110;
    OC2CON2bits.TRIGSTAT = 1;
    OC2R = 0;
}

void BlueLed() {
    RPOR11bits.RP23R = 15;
    OC3RS = 1023;
    OC3CON2bits.SYNCSEL = 0x1F;
    OC3CON2bits.OCTRIG = 0;
    OC3CON1bits.OCTSEL = 0b111;
    OC3CON1bits.OCM = 0b110;
    OC3CON2bits.TRIGSTAT = 1;
    OC3R = 0;
}
void turnRedOn(int potentio) {
    OC1R = potentio;
}

void turnGreenOn(int c_value) {
    OC2R = c_value;
}

void turnBlueOn(int c_value) {
    OC3R = c_value;
}

void Color_Initialize() {
    RedLed();
    GreenLed();
    BlueLed();
    /*turnRedOn(0);
   turnGreenOn(0);
    turnBlueOn(0);*/
}

MODE changeMode() {
    static MODE c_mode = UNINITIALIZED;

    turnRedOn(0);
    turnGreenOn(0);
    turnBlueOn(0);

    if (c_mode == UNINITIALIZED) {
       
        isBalanced = false;
        c_isBalanced = false;
        c_mode = COLOR_DISPLAY;
    } else if (c_mode == COMPOSITE_DISPLAY)
    {
        
        isBalanced = false;
        c_isBalanced = false;
            
        if (index == 0)
        {
            turnRedOn(brightness[0]);
            
            
        } else if (index == 1) {
            turnGreenOn(brightness[1]);
            
        } else if (index == 2) 
        {
            turnBlueOn(brightness[2]);
            
        }
        c_mode = COLOR_DISPLAY;
        
    } else {
        
        isBalanced = false;
        c_isBalanced = false;
        turnRedOn(brightness[0]);
        turnGreenOn(brightness[1]);
        turnBlueOn(brightness[2]);
        c_mode = COMPOSITE_DISPLAY;
    }

    return c_mode;
}

void turnColorOn(int index)
{
    switch (index) {
        case 0:
            turnRedOn(brightness[0]);
            break;

        case 1:
            turnGreenOn(brightness[1]);
            break;

        case 2:
            turnBlueOn(brightness[2]);
            break;
    }
    isBalanced = false;
}
void changeColor() {
    
    switch (index) {
        case 0: 
            brightness[0] = OC1R;  
            turnRedOn(0);              
            break;

        case 1:
            brightness[1] = OC2R;
            turnGreenOn(0);              
            break;

        case 2: 
            brightness[2] = OC3R;  
            turnBlueOn(0);             
            break;
    }

    index = (index + 1) % 3;
    turnColorOn(index);
}



void changeColorValue(int c_value) {
    switch (index) {
        case 0: 
            if (c_value == brightness[0]) {
                isBalanced = true;
            }
            if (isBalanced) {
                turnRedOn(c_value);
            }
            break;

        case 1: 
            if (c_value == brightness[1]) {
                isBalanced = true;
            }
            if (isBalanced) {
                turnGreenOn(c_value);
            }
            break;

        case 2: 
            if (c_value == brightness[2]) {
                isBalanced = true;
            }
            if (isBalanced) {
                turnBlueOn(c_value);
            }
            break;
    }
}


void displayCOMPOSITE(int potentio) {
    static int oldPotentiometer = 0;
    float potentiometerPercentage = 0;
    if(oldPotentiometer == potentio)
    {
        c_isBalanced = true;
    }else if(c_isBalanced)
    {
        potentiometerPercentage = potentio / 1023.0f; 
        turnRedOn(brightness[0] * potentiometerPercentage);
        turnGreenOn(brightness[1] * potentiometerPercentage);
        turnBlueOn(brightness[2] * potentiometerPercentage);
        oldPotentiometer = potentio;
    }
    
}


int main(void) {
    static int oldPotentio = 0;
    int potentio = 0;
    MODE currentMode = UNINITIALIZED;  
   
    SYSTEM_Initialize();
    User_Initialize();
    Color_Initialize();
    oledC_setBackground(OLEDC_COLOR_ORANGE);

    while (1) 
    {
        if (PORTAbits.RA11 == 0) 
        {
            LATAbits.LATA8 = 1;
        } else if (LATAbits.LATA8 == 1)
        {
            LATAbits.LATA8 = 0;
            currentMode = changeMode();
        }

        if (PORTAbits.RA12 == 0) 
        {
            LATAbits.LATA9 = 1;
        } else if (LATAbits.LATA9 == 1) 
        {
            LATAbits.LATA9 = 0;
            if (currentMode == COLOR_DISPLAY) 
            {
                 changeColor();
            }
        }


        AD1CON1bits.SAMP = 1;
        for (int i = 0; i < 1000; i++);
        AD1CON1bits.SAMP = 0;

        while (!AD1CON1bits.DONE);
        potentio = ADC1BUF0;
        if(abs(potentio - oldPotentio) > 5 ){
                if (currentMode == COLOR_DISPLAY)
            {
                changeColorValue(potentio);
                oldPotentio = potentio;
            } else if (currentMode == COMPOSITE_DISPLAY) 
            {
                displayCOMPOSITE(potentio);
                oldPotentio = potentio;
            }
        }
        
    }

    return 1;
}
