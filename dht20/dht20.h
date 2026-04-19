#ifndef ___DHT20_H
#define ___DHT20_H

#include <stdint.h>
#include <stdbool.h>

/* -----------------------------------------------------------------------
 * DHT20 I2C Address
 * ----------------------------------------------------------------------- */
#define DHT20_ADDR 0x38

/* -----------------------------------------------------------------------
 * DHT20 Commands
 * ----------------------------------------------------------------------- */
#define DHT20_CMD_INIT        0xBE
#define DHT20_CMD_MEASURE     0xAC
#define DHT20_CMD_RESET       0xBA

/* -----------------------------------------------------------------------
 * DHT20 Sensor Data
 * ----------------------------------------------------------------------- */
typedef struct {
    double temperature;   /**< Temperature in Celsius */
    double humidity;      /**< Humidity in %          */
} dht20_data_t;

/* -----------------------------------------------------------------------
 * API Functions
 * ----------------------------------------------------------------------- */
bool dht20_init(void);
bool dht20_read(dht20_data_t *p_data);

#endif
