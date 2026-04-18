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

<<<<<<< HEAD
#define BUS_SPEED  40000
#define I2C_SPEED  100
#define I2CP_PRER(speed_khz) (BUS_SPEED/(5*speed_khz)-1)

/*-------------------------------------------------------------*/
/* Delay Funktion */
void delay_ms(const uint16_t ms) {
    clock_t ticks = ((clock_t)ms * CLOCKS_PER_SEC) / 1000u;
    clock_t start = _clock(NULL);
    while ((_clock(NULL) - start) < ticks) {};
}

/*-------------------------------------------------------------*/
/* Grad-Zeichen definieren */
static inline __attribute__((always_inline))
void GDP_define_char(const uint8_t ch, const uint8_t* p_char) {
    GDP.ctrl2 |= (1u << 4u);

    const uint16_t chr_addr = ((uint16_t)(ch - ' ')) * CHAR_SIZE;

    GDP.xh = (uint8_t)(chr_addr >> 8u) & 0xFFu;
    GDP.xl = (uint8_t)(chr_addr & 0xFFu);

    for (uint16_t i = 0u; i < CHAR_SIZE; i++) {
        GDP.char_def = *p_char++;
    }
}

/*-------------------------------------------------------------*/
int main(int argc, char **argp, char **envp)
{
    /* ggf. auskommentieren falls Tastatur Probleme macht */
=======
#define BUS_SPEED  40000    /**< 40 MHz */
#define I2C_SPEED  100      /**< 100 kHz */
#define I2CP_PRER(speed_khz) (BUS_SPEED/(5*speed_khz)-1)

int main(int argc, char **argp, char **envp)
{
>>>>>>> d55544c819019099ec79e8a3a517bbcc6d153a1b
    DISABLE_CPU_INTERRUPTS;

    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

<<<<<<< HEAD
    /*---------------------------------------------------------*/
    /* I2C initialisieren */
=======
    /* Initialize I2C Bus */
>>>>>>> d55544c819019099ec79e8a3a517bbcc6d153a1b
    uint16_t prescaler = I2CP_PRER(I2C_SPEED);
    oc_i2c_init(prescaler);
    iprintf("I2C Init done\n");

<<<<<<< HEAD
    /*---------------------------------------------------------*/
    /* I2C Bus scannen */
    uint8_t found[10] = {0};
    uint8_t found_cnt = 0;

    oc_i2c_scan(found, sizeof(found), &found_cnt);

    iprintf("Folgende %u Geraete auf dem I2C Bus gefunden\n", found_cnt);
    for (uint8_t i = 0; i < found_cnt; i++) {
        iprintf("  Geraet an Adresse: 0x%02X\n", found[i]);
    }

    /*---------------------------------------------------------*/
    /* BME280 initialisieren */
    if (!bme280_init()) {
        iprintf("ERROR: BME280 Initialisierung fehlgeschlagen\n");
        oc_i2c_disable();
        return 1;
    }

    iprintf("BME280 Sensor initialisiert\n\n");

    /*---------------------------------------------------------*/
    /* Grad-Zeichen */
    static const uint8_t char_degree[CHAR_SIZE] =
        {0x02u, 0x05u, 0x05u, 0x02u, 0x00u};

    GDP_define_char(0x7F, char_degree);

    /*---------------------------------------------------------*/
    bool abort_flag = false;
    char key;
    iprintf("Die Messwerte werden alle 2s erneuert, Abbruch mit Taste x \n");
    /* Hauptschleife */
    while (!abort_flag) {

        bme280_data_t sensor_data;

        if (bme280_read(&sensor_data)) {

            int t_int = (int)sensor_data.temperature;
            int t_dec = abs((int)((sensor_data.temperature - t_int) * 100));

            int p_int = (int)sensor_data.pressure;
            int p_dec = abs((int)((sensor_data.pressure - p_int) * 100));

            int h_int = (int)sensor_data.humidity;
            int h_dec = abs((int)((sensor_data.humidity - h_int) * 100));

            iprintf("Temperatur: %d.%02d %cC >>> "
                    "Luftdruck: %d.%02d hPa >>> "
                    "Luftfeuchtigkeit: %d.%02d %%\n",
                    t_int, t_dec, 0x7F,
                    p_int, p_dec,
                    h_int, h_dec);
                    
        } else {
            iprintf("ERROR: Lesen der Sensor Daten fehlgeschlagen\n");
        }

        /*-----------------------------------------------------*/
        /* 2 Sekunden warten – aber Eingabe prüfen */
        for (int i = 0; i < 200; i++) {
            delay_ms(10);

            if (gp_csts() != 0) {
                key = gp_ci();

                if (key == 'x' || key == 'X') {
                    abort_flag = true;
                    break;
                }
            }
        }
    }

    /*---------------------------------------------------------*/
    /* Aufräumen */
    oc_i2c_disable();
    GDP.ctrl2 &= ~(1u << 4u);

=======
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

        /* Delay ca. 2 Sekunden */
        for (volatile long i = 0; i < 2000000L; i++);
    }

    oc_i2c_disable();
>>>>>>> d55544c819019099ec79e8a3a517bbcc6d153a1b
    return 0;
}
