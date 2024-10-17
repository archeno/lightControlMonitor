#ifndef __I2C_APP_H__
#define __I2C_APP_H__

#define __I2C_APP_H__

#include <rtthread.h>
#include <rtdevice.h>


#define EEPROM_WRITE_DEFAULT_VALUE  0
#define EEPROM_WRITE_DATABASE_VALUE 1


extern struct rt_semaphore g_write_cmd_sem;

void write_sysdata_to_eeprom(rt_uint8_t type);
void read_sysdata_from_eeprom(void);
void reset_eeprom_data(void);
//rt_err_t write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t *data);
//rt_err_t read_regs(struct rt_i2c_bus_device *bus, rt_uint8_t len, rt_uint8_t *buf);
#endif
