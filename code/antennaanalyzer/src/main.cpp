#include <Arduino.h>
#include "measure.h"
#include "Wire.h"
#include "math.h"
#include "display.h"
#include "pins.h"

impedance_tester imp;
display disp;

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

void calibrate()
{
    imp.run_calibration(disp);
    disp.show_save_screen();
    imp.save_calibration_data();
    disp.clear_screen();
}

void loop() 
{
    float start = 0.1;
    float stop = 30;
    uint32_t steps = 256;

    //imp.print_calibration_data();
    //calibrate();
    //imp.print_calibration_data();
    disp.show_graph_screen(start, stop);
    //imp.set_measurement_parameters(200, 4000);

    for(uint32_t f = start*1000000; f < stop*1000000; f+= (stop*1000000-start*1000000)/steps * 2)
    {
        imp.make_measurement(f);
        float VSWR = imp.get_vswr();
        //disp.draw_impedance(imp.get_impedance());
        disp.graph_add_datapoint(VSWR, f, 2);
    }

    for(;;);
}
