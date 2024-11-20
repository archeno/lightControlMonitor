#include "spi_driver.h"

static uint32_t us_ticks;
// 延时指定的微秒数
static void delay_us(uint32_t us)
{
    uint32_t start = SysTick->VAL;
    uint32_t ticks = us * us_ticks;
    uint32_t elapsed;

    do
    {
        uint32_t current = SysTick->VAL;
        elapsed = (start >= current) ? (start - current) : (start + (0xFFFFFF - current));
    } while (elapsed < ticks);
}
/**
 * @brief 模拟spi初始化
 *
 * 引脚：cs PA4  clk: PA5 sdi:PA6 reset: PB1 (2.5ms 4msmax)  busy: PB0
 * 模式: mod3  cpol=1 cpha = 1  时钟空闲高电平，上升沿采样
 * 速率：
 *
 */
void spi_gpio_init()
{
    rcu_periph_clock_enable(SPI_PROT_CLK);
    gpio_mode_set(SPI_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, SPI_CLK_PIN | SPI_SDI_PIN | SPI_CS_PIN);
    gpio_output_options_set(SPI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, SPI_CLK_PIN | SPI_SDI_PIN | SPI_CS_PIN);
    SPI_CS_HIGH();
    SPI_CLK_HIGH();

    // get usticks
    us_ticks = SystemCoreClock / 1000000;
}

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
void spi_send_byte(uint8_t data)
{
    SPI_CS_LOW();
    for (int i = 0; i < 8; i++)
    {
        SPI_CLK_LOW();
        if (data & 0x80)
        {
            SPI_SDI_HIGH();
        }
        else
        {
            SPI_SDI_LOW();
        }
        SPI_CLK_HIGH();
        delay_us(2);
        data <<= 1;
    }
    SPI_CS_HIGH();
}