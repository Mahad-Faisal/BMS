/*
 * ina226.c
 *
 *  Created on: Mar 30, 2026
 *      Author: mahad
 */

#include "ina226.h"
#include "hal_data.h"

/* INA226 I2C address — A0 and A1 tied to GND */
#define INA226_ADDR         0x40

/* Register addresses */
#define INA226_REG_CONFIG   0x00    /* Configuration register */
#define INA226_REG_SHUNT    0x01    /* Shunt voltage register - LSB = 2.5µV */
#define INA226_REG_VBUS     0x02    /* Bus voltage register   - LSB = 1.25mV */
#define INA226_REG_POWER    0x03    /* Power register         - LSB = 25mW (after cal) */
#define INA226_REG_CURRENT  0x04    /* Current register       - LSB = Current_LSB (after cal) */
#define INA226_REG_CALIB    0x05    /* Calibration register */

/*
 * Config register value — 0x0497
 *
 * D15     RST      = 0  no reset
 * D14:D12 reserved = 000
 * D11:D9  AVG      = 010  16 sample averaging
 * D8:D6   VBUSCT   = 010  332µs bus voltage conversion time
 * D5:D3   VSHCT    = 010  332µs shunt voltage conversion time
 * D2:D0   MODE     = 111  continuous shunt and bus voltage measurement
 *
 * Binary: 0000 0100 1001 0111 = 0x0497
 */
#define INA226_CONFIG_VAL   0x0497

/*
 * Calibration register value — 0x0200
 *
 * Formula: CAL = 0.00512 / (Current_LSB × R_shunt)
 *
 * R_shunt    = 0.1Ω  (onboard shunt resistor on INA226 breakout)
 * Current_LSB = 0.001A = 1mA per bit  (chosen for clean math)
 * Max current = 1.2A  (12V / 10Ω dummy load)
 *
 * CAL = 0.00512 / (0.001 × 0.1)
 * CAL = 0.00512 / 0.0001
 * CAL = 51.2 → rounded to 512 = 0x0200
 *
 * Result: 1 LSB of current register = 1mA
 *         1 LSB of power register   = 25mW (25 × Current_LSB)
 */
#define INA226_CALIB_VAL    0x0200

/*
 * Scaling factors for converting raw register values to engineering units
 *
 * Bus voltage:  raw × 0.00125  = voltage in Volts  (1.25mV per LSB)
 * Current:      raw × 0.001    = current in Amps   (1mA per LSB)
 * Power:        raw × 0.025    = power in Watts    (25mW per LSB)
 */
#define INA226_VBUS_LSB     0.00125f    /* V per bit */
#define INA226_CURRENT_LSB  0.0001f   /* was 0.001f */
#define INA226_POWER_LSB    0.0025f   /* 25 × 0.0001 */
volatile bool i2c_tx_complete = false;
volatile bool i2c_rx_complete = false;

/*
void INA226_Init(void){
    static uint8_t buf[3] = {INA226_REG_CONFIG, (INA226_CONFIG_VAL >> 8) & 0xFF , INA226_CONFIG_VAL & 0xFF};
    static uint8_t buf2[3] = {INA226_REG_CALIB, (INA226_CALIB_VAL >> 8) & 0xFF , INA226_CALIB_VAL & 0xFF};

   // R_IIC_MASTER_Open(&g_i2c_master0_ctrl, &g_i2c_master0_cfg);
    R_MSTP->MSTPCRB_b.MSTPB1 = 1U;
    __NOP(); __NOP(); __NOP(); __NOP();
    R_MSTP->MSTPCRB_b.MSTPB1 = 0U;
    __NOP(); __NOP(); __NOP(); __NOP();
    fsp_err_t err = R_IIC_MASTER_Open(&g_i2c_master0_ctrl, &g_i2c_master0_cfg);
       if (err != FSP_SUCCESS)
       {
           __BKPT(0);  // halt here if open failed
       }
       R_IIC_MASTER_Close(&g_i2c_master0_ctrl); // ignore error
       R_IIC_MASTER_Open(&g_i2c_master0_ctrl, &g_i2c_master0_cfg);
    i2c_tx_complete = false;
    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, buf, 3, false);
    while(!i2c_tx_complete) { vTaskDelay(1); }
    i2c_tx_complete = false;
    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, buf2, 3, false);
    while(!i2c_tx_complete) { vTaskDelay(1); }
}
*/
void INA226_Init(void){
    static uint8_t buf[3] = {INA226_REG_CONFIG, (INA226_CONFIG_VAL >> 8) & 0xFF, INA226_CONFIG_VAL & 0xFF};
    static uint8_t buf2[3] = {INA226_REG_CALIB, (INA226_CALIB_VAL >> 8) & 0xFF, INA226_CALIB_VAL & 0xFF};

    R_IIC_MASTER_Open(&g_i2c_master0_ctrl, &g_i2c_master0_cfg);

    i2c_tx_complete = false;
    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, buf, 3, false);
    while(!i2c_tx_complete) { vTaskDelay(1); }

    i2c_tx_complete = false;
    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, buf2, 3, false);
    while(!i2c_tx_complete) { vTaskDelay(1); }
}
void i2c_master_callback(i2c_master_callback_args_t *p_args){
    if(p_args->event == I2C_MASTER_EVENT_TX_COMPLETE)
        i2c_tx_complete = true;
    if(p_args->event == I2C_MASTER_EVENT_RX_COMPLETE)
        i2c_rx_complete = true;

}

float INA226_GetBusVoltage(void){

    static uint8_t buf[1] = {0x02};
    volatile uint8_t busVoltage[2] = {};
    i2c_tx_complete = false;
    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, buf, 1, true);
    while(!i2c_tx_complete) { vTaskDelay(1); }

    i2c_rx_complete = false;

    R_IIC_MASTER_Read(&g_i2c_master0_ctrl, busVoltage, 2, false);
    while(!i2c_rx_complete) { vTaskDelay(1); }


    uint16_t raw = ((uint16_t)busVoltage[0] << 8) | busVoltage[1];
    return (float)raw * INA226_VBUS_LSB;


}

float INA226_GetCurrent(void){
    static uint8_t buf[1] = {INA226_REG_CURRENT};
    volatile uint8_t raw[2] = {};
    i2c_tx_complete = false;
    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, buf, 1, true);
    while(!i2c_tx_complete) { vTaskDelay(1); }
    i2c_rx_complete = false;
    R_IIC_MASTER_Read(&g_i2c_master0_ctrl, raw, 2, false);
    while(!i2c_rx_complete) { vTaskDelay(1); }
    int16_t result = ((uint16_t)raw[0] << 8) | raw[1];
    return (float)result * INA226_CURRENT_LSB;
}

float INA226_GetPower(void){
    static uint8_t buf[1] = {INA226_REG_POWER};
    volatile uint8_t raw[2] = {};
    i2c_tx_complete = false;
    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, buf, 1, true);
    while(!i2c_tx_complete) { vTaskDelay(1); }
    i2c_rx_complete = false;
    R_IIC_MASTER_Read(&g_i2c_master0_ctrl, raw, 2, false);
    while(!i2c_rx_complete) { vTaskDelay(1); }
    uint16_t result = ((uint16_t)raw[0] << 8) | raw[1];
    return (float)result * INA226_POWER_LSB;
}
