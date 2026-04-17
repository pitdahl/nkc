/* $Workfile:   BME.c $                                        			    		*/
/* $Revision:   $                                                       			*/
/* $Author:     Pit $                                                   			*/
/* $Date:       16.04.2026 $														*/
/* Description: BME280 Sensor Driver                           					*/
/*																					*/
/* Remarks:     GYBMEP 5V – Temperature, Humidity, Pressure Sensor         		*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "i2c.h"
#include "bme.h"

/* -----------------------------------------------------------------------
 * Static calibration data
 * ----------------------------------------------------------------------- */
static bme280_calib_t g_calib = {0};

/* -----------------------------------------------------------------------
 * Internal I2C Helper Functions
 * ----------------------------------------------------------------------- */

/**
 * @brief  Read a single register from BME280.
 */
static uint8_t bme_read8(uint8_t reg)
{
    uint8_t data;
    oc_i2c_write_read(BME280_ADDR, &reg, 1, &data, 1);
    return data;
}

/**
 * @brief  Read multiple bytes from BME280.
 */
static void bme_read_bytes(uint8_t reg, uint8_t *buf, uint8_t len)
{
    oc_i2c_write_read(BME280_ADDR, &reg, 1, buf, len);
}

/**
 * @brief  Write a single register to BME280.
 */
static void bme_write8(uint8_t reg, uint8_t data)
{
    uint8_t buf[2] = {reg, data};
    oc_i2c_write(BME280_ADDR, buf, 2);
}

/* -----------------------------------------------------------------------
 * Calibration Functions
 * ----------------------------------------------------------------------- */

/**
 * @brief  Read calibration data from BME280.
 */
static void bme280_read_calib(void)
{
    uint8_t buf[26], hbuf[7];

    /* Read temperature and pressure calibration */
    bme_read_bytes(BME280_REG_CALIB_T, buf, 26);
    g_calib.dig_T1 = (buf[1] << 8) | buf[0];
    g_calib.dig_T2 = (int16_t)((buf[3] << 8) | buf[2]);
    g_calib.dig_T3 = (int16_t)((buf[5] << 8) | buf[4]);
    g_calib.dig_P1 = (buf[7] << 8) | buf[6];
    g_calib.dig_P2 = (int16_t)((buf[9] << 8) | buf[8]);
    g_calib.dig_P3 = (int16_t)((buf[11] << 8) | buf[10]);
    g_calib.dig_P4 = (int16_t)((buf[13] << 8) | buf[12]);
    g_calib.dig_P5 = (int16_t)((buf[15] << 8) | buf[14]);
    g_calib.dig_P6 = (int16_t)((buf[17] << 8) | buf[16]);
    g_calib.dig_P7 = (int16_t)((buf[19] << 8) | buf[18]);
    g_calib.dig_P8 = (int16_t)((buf[21] << 8) | buf[20]);
    g_calib.dig_P9 = (int16_t)((buf[23] << 8) | buf[22]);

    /* Read humidity calibration */
    g_calib.dig_H1 = bme_read8(BME280_REG_CALIB_H1);
    bme_read_bytes(BME280_REG_CALIB_H2, hbuf, 7);
    g_calib.dig_H2 = (int16_t)((hbuf[1] << 8) | hbuf[0]);
    g_calib.dig_H3 = hbuf[2];
    g_calib.dig_H4 = (int16_t)(((hbuf[3] << 4) | (hbuf[4] & 0x0F)));
    g_calib.dig_H5 = (int16_t)(((hbuf[5] << 4) | ((hbuf[4] >> 4) & 0x0F)));
    g_calib.dig_H6 = (int8_t)hbuf[6];
}

/* -----------------------------------------------------------------------
 * Compensation Functions
 * ----------------------------------------------------------------------- */

/**
 * @brief  Compensate temperature value using calibration data.
 */
static double bme280_comp_temp(int32_t adc_T)
{
    int32_t var1, var2;
    var1 = ((((adc_T >> 3) - ((int32_t)g_calib.dig_T1 << 1))) * ((int32_t)g_calib.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)g_calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)g_calib.dig_T1))) >> 12) *
            ((int32_t)g_calib.dig_T3)) >> 14;
    g_calib.t_fine = var1 + var2;
    return (double)g_calib.t_fine / 5120.0;
}

/**
 * @brief  Compensate pressure value using calibration data.
 */
static double bme280_comp_press(int32_t adc_P)
{
    int64_t var1, var2, p;
    var1 = ((int64_t)g_calib.t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)g_calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)g_calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)g_calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)g_calib.dig_P3) >> 8) + ((var1 * (int64_t)g_calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)g_calib.dig_P1) >> 33;

    if (var1 == 0) return 0;

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)g_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)g_calib.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)g_calib.dig_P7) << 4);

    return (double)p / 25600.0;
}

/**
 * @brief  Compensate humidity value using calibration data.
 */
static double bme280_comp_hum(int32_t adc_H)
{
    int32_t v_x1_u32r;
    v_x1_u32r = (g_calib.t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)g_calib.dig_H4) << 20) - (((int32_t)g_calib.dig_H5) * v_x1_u32r)) +
                   ((int32_t)16384)) >> 15) *
                 (((((((v_x1_u32r * ((int32_t)g_calib.dig_H6)) >> 10) *
                      (((v_x1_u32r * ((int32_t)g_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                   ((int32_t)2097152)) * ((int32_t)g_calib.dig_H2) + 8192) >> 14));
    v_x1_u32r = v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                              ((int32_t)g_calib.dig_H1)) >> 4);
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

    return (double)(v_x1_u32r >> 12) / 1024.0;
}

/* -----------------------------------------------------------------------
 * Public API
 * ----------------------------------------------------------------------- */

bool bme280_init(void)
{
    uint8_t id = bme_read8(BME280_REG_ID);
    if (id == 0xFF || id == 0x00) {
        iprintf("ERROR: BME280 not found (ID: 0x%02X)\n", id);
        return false;
    }

    iprintf("BME280 Sensor ID: 0x%02X\n", id);

    /* Configure humidity oversampling */
    bme_write8(BME280_REG_CTRL_HUM, 0x01);

    /* Configure temperature/pressure oversampling and mode */
    bme_write8(BME280_REG_CTRL_MEAS, 0x27);

    /* Configure standby time and filter */
    bme_write8(BME280_REG_CONFIG, 0xA0);

    /* Read calibration data */
    bme280_read_calib();

    return true;
}

bool bme280_read(bme280_data_t *p_data)
{
    if (!p_data) return false;

    uint8_t data[8];
    bme_read_bytes(BME280_REG_DATA, data, 8);

    int32_t adc_P = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | (data[2] >> 4);
    int32_t adc_T = ((int32_t)data[3] << 12) | ((int32_t)data[4] << 4) | (data[5] >> 4);
    int32_t adc_H = ((int32_t)data[6] << 8) | data[7];

    p_data->temperature = bme280_comp_temp(adc_T);
    p_data->pressure = bme280_comp_press(adc_P);
    p_data->humidity = bme280_comp_hum(adc_H);

    return true;
}

const bme280_calib_t* bme280_get_calib(void)
{
    return &g_calib;
}
