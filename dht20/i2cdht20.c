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
#include "dht20.h"


#define BUS_SPEED  40000
#define I2C_SPEED  100
#define I2CP_PRER(speed_khz) (BUS_SPEED/(5*speed_khz)-1)

/*-------------------------------------------------------------*/
void delay_ms(const uint16_t ms) {
    clock_t ticks = ((clock_t)ms * CLOCKS_PER_SEC) / 1000u;
    clock_t start = _clock(NULL);
    while ((_clock(NULL) - start) < ticks) {};
}

/*-------------------------------------------------------------*/
int main(void)
{
    DISABLE_CPU_INTERRUPTS;

    setvbuf(stdout, NULL, _IONBF, 0);

    /* I2C Init */
    uint16_t prescaler = I2CP_PRER(I2C_SPEED);
    oc_i2c_init(prescaler);

    iprintf("I2C Init done\n");

    /* Scan */
    uint8_t found[10] = {0};
    uint8_t found_cnt = 0;

    oc_i2c_scan(found, sizeof(found), &found_cnt);

    iprintf("Gefundene Geraete:\n");
    for (uint8_t i = 0; i < found_cnt; i++) {
        iprintf("  0x%02X\n", found[i]);
    }

    /* Init DHT20 */
    if (!dht20_init()) {
        iprintf("DHT20 Init fehlgeschlagen\n");
        return 1;
    }

    iprintf("DHT20 bereit\n\n");

    /* Loop */
    while (1) {

        dht20_data_t data;

        if (dht20_read(&data)) {

            int t_int = (int)data.temperature;
            int t_dec = abs((int)((data.temperature - t_int) * 100));

            int h_int = (int)data.humidity;
            int h_dec = abs((int)((data.humidity - h_int) * 100));

            iprintf("Temp: %d.%02d C  |  Feuchte: %d.%02d %%\n",
                    t_int, t_dec,
                    h_int, h_dec);
        }
        else {
            iprintf("Fehler beim Lesen\n");
        }

        delay_ms(2000);
    }

    return 0;
}
