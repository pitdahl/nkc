#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "i2c.h"
#include "dht20.h"

/* -----------------------------------------------------------------------
 * Internal delay (ms)
 * ----------------------------------------------------------------------- */
static void delay_ms(uint16_t ms)
{
    clock_t ticks = ((clock_t)ms * CLOCKS_PER_SEC) / 1000u;
    clock_t start = _clock(NULL);
    while ((_clock(NULL) - start) < ticks) {};
}

/* -----------------------------------------------------------------------
 * Internal I2C Helpers
 * ----------------------------------------------------------------------- */
static void dht20_write(uint8_t *buf, uint8_t len)
{
    oc_i2c_write(DHT20_ADDR, buf, len);
}

static void dht20_read_bytes(uint8_t *buf, uint8_t len)
{
    oc_i2c_read(DHT20_ADDR, buf, len);
}

/* -----------------------------------------------------------------------
 * Init
 * ----------------------------------------------------------------------- */
bool dht20_init(void)
{
    uint8_t cmd[3] = {DHT20_CMD_INIT, 0x08, 0x00};

    dht20_write(cmd, 3);

    iprintf("DHT20 initialisiert an Adresse 0x38\n");
    return true;
}

/* -----------------------------------------------------------------------
 * Read Sensor
 * ----------------------------------------------------------------------- */
bool dht20_read(dht20_data_t *p_data)
{
    if (!p_data) return false;

    uint8_t cmd[3] = {DHT20_CMD_MEASURE, 0x33, 0x00};
    uint8_t data[7];

    /* Trigger measurement */
    dht20_write(cmd, 3);

    /* Warten bis Messung fertig (~80ms laut Datenblatt) */
    delay_ms(80);

    /* Read 7 bytes */
    dht20_read_bytes(data, 7);

    /* Prüfen ob Sensor noch busy */
    if (data[0] & 0x80) {
        return false;
    }

    /* Rohdaten extrahieren */
    uint32_t raw_h = ((uint32_t)data[1] << 12) |
                     ((uint32_t)data[2] << 4) |
                     (data[3] >> 4);

    uint32_t raw_t = ((uint32_t)(data[3] & 0x0F) << 16) |
                     ((uint32_t)data[4] << 8) |
                     data[5];

    /* Umrechnung */
    p_data->humidity = ((double)raw_h / 1048576.0) * 100.0;
    p_data->temperature = ((double)raw_t / 1048576.0) * 200.0 - 50.0;

    return true;
}
