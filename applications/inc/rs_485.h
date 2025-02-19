#ifndef __RS_485_H__
#define __RS_485_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <board.h>
#include <rtdevice.h>
#include <stdint.h>
#define USART0_DR_PIN GET_PIN(C, 6) // GET_PIN(A, 7)
#define USART2_DR_PIN GET_PIN(A, 15)

    typedef enum
    {
        RS_485_UART0 = 0,
        RS_485_UART2,
        RS_485_NUM
    } RS_485_PORT;

#define USART0_SEND_MODE() rt_pin_write(USART0_DR_PIN, PIN_HIGH)
#define USART0_RECV_MODE() rt_pin_write(USART0_DR_PIN, PIN_LOW)
#define USART2_SEND_MODE() rt_pin_write(USART2_DR_PIN, PIN_HIGH)
#define USART2_RECV_MODE() rt_pin_write(USART2_DR_PIN, PIN_LOW)

    int rs_485_send_data(RS_485_PORT port, uint8_t *buf, int len);
    int rs_485_receive(RS_485_PORT port, uint8_t *buf, int bufsz, int timeout, int bytes_timeout);

#ifdef __cplusplus
}
#endif

#endif