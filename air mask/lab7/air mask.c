// Program: Air Mask Control
// Author: Jinhan Yang
// Date: Dec 7, 2023
// Purpose: Control fan speed using a joystick and display percentage speed

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "c:\\Program Files (x86)\\LabJack\\Drivers\\LabJackUD.h"

LJ_ERROR lj_cue = 0;
LJ_HANDLE lj_handle = 0;
int OFF = 0, ON = 1;
int Joy_Y = 1; 
int Display_PIN = 6; 
int Motor_PIN = 7;   


void updateDisplay(int percentage);
void controlFanSpeed(int percentage);
void wait(unsigned int delay);

int main() {
    int counter = 0;
    double serialnum, yvoltage = 0, threshold_high = 4.5, threshold_low = 0.5;
    int fanSpeedPercentage = 0; 


    lj_cue = OpenLabJack(LJ_dtU3, LJ_ctUSB, "1", 1, &lj_handle);
    lj_cue = ePut(lj_handle, LJ_ioPIN_CONFIGURATION_RESET, 0, 0, 0);
    lj_cue = eGet(lj_handle, LJ_ioGET_CONFIG, LJ_chSERIAL_NUMBER, &serialnum, 0);
    printf("Serial Number: %.0lf \n\n", serialnum);


    while (counter < 20000) { 
        // S4 - Read joystick Y-axis voltage
        lj_cue = eGet(lj_handle, LJ_ioGET_AIN, Joy_Y, &yvoltage, 0);
        printf("Joystick Y Voltage: %.3lf\n", yvoltage);

        // S5 - Determine fan speed percentage
        if (yvoltage >= threshold_high) {
            fanSpeedPercentage = 100; 
        }
        else if (yvoltage <= threshold_low) {
            fanSpeedPercentage = 10; 
        }
        else {
            fanSpeedPercentage = (int)((yvoltage / threshold_high) * 100); 
        }


        updateDisplay(fanSpeedPercentage);
        controlFanSpeed(fanSpeedPercentage);

        // Wait before next iteration
        wait(100000); 
        counter++;
    }

    // S7 - Reset settings, print Done, pause, close LabJack, exit
    lj_cue = ePut(lj_handle, LJ_ioPIN_CONFIGURATION_RESET, 0, 0, 0); 
    printf("\nDone\n");
    system("pause");
    close();

    return 0;
}
// Action: Update the digital display to show speed percentage
void updateDisplay(int percentage) {
    printf("Fan Speed: %d%%\n", percentage); 
    lj_cue = ePut(lj_handle, LJ_ioPUT_DAC, Display_PIN, percentage / 10.0, 0); 
}

// Action: Adjust the fan speed based on percentage
void controlFanSpeed(int percentage) {
    double voltage = (percentage / 100.0) * 5.0; 
    lj_cue = ePut(lj_handle, LJ_ioPUT_DAC, Motor_PIN, voltage, 0); 
}

// Action: Waste CPU cycles to create a delay
void wait(unsigned int delay) {
    for (unsigned int i = 0; i < delay; i++) {}
}
