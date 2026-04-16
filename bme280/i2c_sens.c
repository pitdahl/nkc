/* $Workfile:   I2C_SENS.c $                                   			    		*/
/* $Revision:   $                                                       			*/
/* $Author:     Pit $                                                   			*/
/* $Date:       16.04.2026 $														*/
/* Description: I2C Sensor Test Application                    					*/
/*																					*/
/* Remarks:     GYBMEP 5V – Temperature, Humidity, Pressure Sensor         		*/

#include <stdio.h>
#include <sys/ndrclock.h>
#include <sys/file.h>
#include <sys/path.h>
#include <sys/m68k.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "../../nkc_common/nkc/nkc.h"
#include "i2c.h"
#include "bme.h"

#define BUS_SPEED  40000    /**< 40 MHz */
#define I2C_SPEED  100      /**< 100 kHz */
#define I2CP_PRER(speed_khz) (BUS_SPEED/(5*speed_khz)-1)

int main(int argc, char **argp, char **envp)
{
    DISABLE_CPU_INTERRUPTS;

    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    /* Initialize I2C Bus */
    uint16_t prescaler = I2CP_PRER(I2C_SPEED);
    oc_i2c_init(prescaler);
    iprintf("I2C Init done\n");

    /* Scan for devices on I2C bus */
    uint8_t found[10] = {0};
    uint8_t found_cnt = 0;
    oc_i2c_scan(found, sizeof(found), &found_cnt);
    iprintf("Found %u devices on I2C bus\n", found_cnt);
    for (uint8_t i = 0; i < found_cnt; i++) {
        iprintf("  Device at Address: 0x%02X\n", found[i]);
    }

    /* Initialize BME280 sensor */
    if (!bme280_init()) {
        iprintf("ERROR: BME280 initialization failed\n");
        oc_i2c_disable();
        return 1;
    }
    iprintf("BME280 sensor initialized\n\n");

    /* Main measurement loop */
    while (1) {
        bme280_data_t sensor_data;

        if (bme280_read(&sensor_data)) {
            int t_int = (int)sensor_data.temperature;
            int t_dec = (int)((sensor_data.temperature - t_int) * 100);
            int p_int = (int)sensor_data.pressure;
            int p_dec = (int)((sensor_data.pressure - p_int) * 100);
            int h_int = (int)sensor_data.humidity;
            int h_dec = (int)((sensor_data.humidity - h_int) * 100);

            iprintf("Temp: %d.%02d C | Druck: %d.%02d hPa | Feuchte: %d.%02d %%\n",
                    t_int, t_dec, p_int, p_dec, h_int, h_dec);
        } else {
            iprintf("ERROR: Failed to read sensor data\n");
        }

        /* Simple delay */
        for (volatile long i = 0; i < 10000000L; i++);
    }

    oc_i2c_disable();
    return 0;
}