/* $Workfile:   BME.h $                                        			    		*/
/* $Revision:   $                                                       			*/
/* $Author:     Pit $                                                   			*/
/* $Date:       16.04.2026 $														*/
/* Description: BME280 Sensor Definitions and API         					*/
/*																					*/
/* Remarks:     GYBMEP 5V – Temperature, Humidity, Pressure Sensor         		*/

#ifndef ___BME_H
#define ___BME_H

#include <stdint.h>
#include <stdbool.h>

/* -----------------------------------------------------------------------
 * BME280 I2C Address
 * ----------------------------------------------------------------------- */
#define BME280_ADDR 0x76

/* -----------------------------------------------------------------------
 * BME280 Register Addresses
 * ----------------------------------------------------------------------- */
#define BME280_REG_ID           0xD0    /**< Chip ID register           */
#define BME280_REG_CALIB_T      0x88    /**< Temperature calibration    */
#define BME280_REG_CALIB_H1     0xA1    /**< Humidity calibration 1     */
#define BME280_REG_CALIB_H2     0xE1    /**< Humidity calibration 2     */
#define BME280_REG_CTRL_HUM     0xF2    /**< Humidity control           */
#define BME280_REG_CTRL_MEAS    0xF4    /**< Measurement control        */
#define BME280_REG_CONFIG       0xF5    /**< Configuration register     */
#define BME280_REG_DATA         0xF7    /**< ADC data (pressure, temp, humidity) */

/* -----------------------------------------------------------------------
 * BME280 Calibration Data
 * ----------------------------------------------------------------------- */
typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
    int32_t  t_fine;    /**< Temperature fine adjustment (used in compensation) */
} bme280_calib_t;

/* -----------------------------------------------------------------------
 * BME280 Sensor Data
 * ----------------------------------------------------------------------- */
typedef struct {
    double temperature;     /**< Temperature in Celsius        */
    double pressure;        /**< Pressure in hPa               */
    double humidity;        /**< Humidity in %                 */
} bme280_data_t;

/* -----------------------------------------------------------------------
 * API Functions
 * ----------------------------------------------------------------------- */

/**
 * @brief  Initialize BME280 sensor.
 *         Reads calibration data and configures measurement settings.
 * @return true on success, false on error.
 */
bool bme280_init(void);

/**
 * @brief  Read temperature, pressure, and humidity from BME280.
 * @param  p_data  Pointer to structure for sensor data.
 * @return true on success, false on error.
 */
bool bme280_read(bme280_data_t *p_data);

/**
 * @brief  Get current calibration data.
 * @return Pointer to calibration structure.
 */
const bme280_calib_t* bme280_get_calib(void);

#endif
