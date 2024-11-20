#ifndef __SPI_DRIVER_H__
#define __SPI_DRIVER_H__
#include "stdint.h"
#include "gd32f4xx.h"
#define SPI_PORT GPIOA
#define SPI_PROT_CLK RCU_GPIOA
#define SPI_CS_PIN GPIO_PIN_4
#define SPI_CLK_PIN GPIO_PIN_5
#define SPI_SDI_PIN GPIO_PIN_6

#define SPI_CS_LOW() gpio_bit_reset(SPI_PORT, SPI_CS_PIN)
#define SPI_CS_HIGH() gpio_bit_set(SPI_PORT, SPI_CS_PIN)
#define SPI_CLK_LOW() gpio_bit_reset(SPI_PORT, SPI_CLK_PIN)
#define SPI_CLK_HIGH() gpio_bit_set(SPI_PORT, SPI_CLK_PIN)
#define SPI_SDI_LOW() gpio_bit_reset(SPI_PORT, SPI_SDI_PIN)
#define SPI_SDI_HIGH() gpio_bit_set(SPI_PORT, SPI_SDI_PIN)
/**
 * @brief 模拟spi初始化
 *
 * 引脚：cs PA4  clk: PA5 sdi:PA6 reset: PB1 (2.5ms 4msmax)  busy: PB0
 * 模式: mod3  cpol=1 cpha = 1  时钟空闲高电平，上升沿采样
 * 速率：
 *
 */
void spi_gpio_init();

/**
 * @brief spi 发送一个字节数据
 *
 * 发送时序：
 * 1. cs拉低
 * 2. clk 拉低
 * 3. sdi 取字节最高位数据
 * 4. 延时
 * 5. clk 拉高
 * 6. 延时
 * 7. data 右移一位
 * 重复2~7 共8次，1个字节数据发送完成
 * 8. cs拉高
 * @param data
 */
void spi_send_byte(uint8_t data);
#endif