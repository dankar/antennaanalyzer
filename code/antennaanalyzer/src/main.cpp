#include <Arduino.h>
#include "measure.h"
#include "display.h"
#include "menu_system.h"

impedance_tester imp;
display disp;
menu_system menu(imp, disp);

void setup() 
{
    Serial.begin(9600);
    Serial.println("Begin serial...");
    
    disp.init();
    imp.init();

    disp.show_load_screen();
    imp.load_calibration_data();
    //imp.print_calibration_data();
    
}

void loop() 
{
    menu.run();
}
