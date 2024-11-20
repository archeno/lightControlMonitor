#include <oled_driver.h>
// #include "drv_spi.h"
#include "spi_driver.h"
#include "board.h"
// struct rt_spi_device *spi_dev;
// #define SPI_DEVICE_NAME "spi00"

oled_pos_t g_oled_pos;
#define OLED_RESET_PIN GET_PIN(B, 1)
#define OLED_BUSY_PIN GET_PIN(B, 0)
#define OLED_CS_PIN GET_PIN(A, 4)

#define RESET_PIN_HIGH() rt_pin_write(OLED_RESET_PIN, PIN_HIGH)
#define RESET_PIN_LOW() rt_pin_write(OLED_RESET_PIN, PIN_LOW)

typedef enum
{
  CMD_DISP_ON = 0,     /** ?????  	 	 51H **/
  CMD_DISP_OFF,        /** ?????  		 50H **/
  CMD_DISP_CONTRAST,   /**????????	 52H	*/
  CMD_CLR,             /** ????     58H  **/
  CMD_CHAR_DISP,       /**??????		 5AH	*/
  CMD_LOCAL_CHAR_DISP, /** ????????? 5BH */
  CMD_BMP_DISP,        /** λ????  5DH */
  CMD_PLOT_POINT,      /** ???????  60H*/
  CMD_PLOT_LINE,       /** ?????	      61H*/
  CMD_PLOT_REC_FRAME,  /** ?????ο?  62H*/
  CMD_PLOT_REC_BLOCK,  /** ?????ο?  63H*/
  CMD_PLOT_CIRCLE,     /** ???	  64H*/
  CMD_CURSOR_DISP_ON,  /** ???????  70H*/
  CMD_CUSTOM_CHAR,     /** ????????  81H**/
  CMD_DISP_MODE,       /** ????????? 91H **/
} OLED_CMD_ENUM;

/** ?????б?**/
rt_uint8_t cmd_table[] = {
    0X51, 0X50, 0X52, 0X58, 0X5A, 0X5B, 0X5D,
    0X60, 0X61, 0X62, 0X63, 0X64, 0X70, 0X81, 0X91};
static int rt_hw_spi_OLED_init(void)
{

  // rcu_periph_clock_enable(RCU_GPIOB);
  // rt_hw_spi_device_attach("spi0", "spi00", OLED_CS_PIN);
  // spi_dev = (struct rt_spi_device *)rt_device_find(SPI_DEVICE_NAME);
  // if (!spi_dev)
  // {
  //   rt_kprintf("spi sample run failed! can't find %s device!\n", SPI_DEVICE_NAME);
  //   while (1)
  //     ;
  // }
  // else
  // {
  //   struct rt_spi_configuration cfg;
  //   cfg.data_width = 8;
  //   cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;
  //   cfg.max_hz = 400 * 1000; /* 1M */
  //   rt_spi_configure(spi_dev, &cfg);
  // }
  spi_gpio_init();
  rt_pin_mode(OLED_RESET_PIN, PIN_MODE_OUTPUT);
  rt_pin_write(OLED_RESET_PIN, PIN_HIGH);

  rt_pin_mode(OLED_BUSY_PIN, PIN_MODE_INPUT);

  return RT_EOK;
}

INIT_COMPONENT_EXPORT(rt_hw_spi_OLED_init);

void oled_cmd_send(OLED_CMD_ENUM cmd)
{
  while (rt_pin_read(OLED_BUSY_PIN) == SET)
    ;
  spi_send_byte(cmd_table[cmd]);
  // rt_spi_send(spi_dev, &cmd_table[cmd], 1);
}

void oled_data_send(rt_uint8_t data)
{
  while (rt_pin_read(OLED_BUSY_PIN) == SET)
    ;
  spi_send_byte(data);
  // rt_spi_send(spi_dev, &data, 1);
}

void oled_clear(void)
{
  oled_cmd_send(CMD_CLR);
}

/******** 画圆显示程序********/
void draw_circle(uint8_t x, uint8_t y, uint8_t r, uint8_t fil, uint8_t z)
{

  oled_data_send(0x64);
  oled_data_send(x);   // 垂直圆心地址
  oled_data_send(y);   // 水平圆心地址
  oled_data_send(r);   // 半径
  oled_data_send(fil); // fil=0:空心圆；fil=1:实心圆
  oled_data_send(z);   // 绘点或消点
}
void INTER_chhzes(rt_uint8_t y, rt_uint8_t x, rt_uint8_t m, rt_uint8_t n, rt_uint8_t rev, rt_uint8_t *s)
{
  rt_uint8_t len = rt_strlen((const char *)s);
  oled_cmd_send(CMD_LOCAL_CHAR_DISP);
  oled_data_send(y);   // ??????
  oled_data_send(x);   // ?????
  oled_data_send(m);   // ????????????
  oled_data_send(n);   // ???????????
  oled_data_send(len); // ???????
  oled_data_send(rev); // rev=0:?????????rev=1:???????

  while (*s != '\0')
    oled_data_send(*s++);

} //*/

/******** ????????????? ********/

void oled_displayChar(rt_uint8_t char_with, rt_uint8_t y, rt_uint8_t x, const rt_uint8_t *s)
{
  rt_uint8_t y_point = 16;
  rt_uint8_t x_point = char_with;
  // rt_uint8_t y_point = 24;
  // rt_uint8_t x_point = 24;

  rt_uint8_t coloums = OLED_X_POITNS / x_point; // 240/8 = 30
  rt_uint8_t rows = OLED_Y_POITNS / y_point;    // 128/16 = 8

  rt_uint8_t len = rt_strlen((const char *)s);
  oled_cmd_send(CMD_LOCAL_CHAR_DISP);
  oled_data_send(y * y_point); // ??????
  oled_data_send(x * x_point); // ?????
  oled_data_send(y_point);     // ????????????
  oled_data_send(x_point);     // ???????????
  oled_data_send(len);         // ???????
  oled_data_send(0);           // rev=0:?????????rev=1:???????

  while (*s != '\0')
    oled_data_send(*s++);

  g_oled_pos.x_pos = x + len;
  g_oled_pos.y_pos = y;

} //*/

void oled_clear_line(rt_uint8_t line)
{
  rt_uint8_t y_point = 16;
  rt_uint8_t x_point = 8;
  rt_uint8_t len;
  rt_uint8_t coloums = OLED_X_POITNS / x_point; // 240/8 = 30
  rt_uint8_t rows = OLED_Y_POITNS / y_point;    // 128/16 = 8
  oled_cmd_send(CMD_LOCAL_CHAR_DISP);
  oled_data_send(line * y_point); // ??????
  oled_data_send(0);              // ?????
  oled_data_send(y_point);        // ????????????
  oled_data_send(x_point);        // ???????????
  oled_data_send(coloums);        // ???????
  oled_data_send(0);              // rev=0:?????????rev=1:???????
  len = coloums;
  while (len--)
    oled_data_send(' ');
}

// data : ???????????
// dicNum: С????λ??

static char *num2String(uint16_t data, rt_uint8_t dicNum)
{
  static char buff[64] = {'\0'};
  rt_uint8_t num = 1;
  rt_uint8_t cnt = 0;

  if (data == 0)
  {
    buff[0] = 1;
    buff[1] = '0';
    return buff;
  }
  else
  {
    while (data != 0)
    {

      buff[num++] = data % 10 + '0';
      if (num - 1 == dicNum)
      {
        buff[num++] = '.';
      }
      data /= 10;

      cnt++;
    }

    // λ??С??С???? ????油0
    if (cnt < dicNum)
    {

      while (num - 2 < dicNum)
      {
        buff[num++] = '0';
      }
      buff[num++] = '.';
      buff[num++] = '0';
    }
    else if (cnt == dicNum)
    {
      buff[num++] = '0';
    }

    //        /** λ??С??С?????? ??油0*/
    //		if(num-1 < dicNum)
    //		{
    //			while(num-2< dicNum)
    //			{
    //				buff[num++] = '0';
    //
    //			}
    //			buff[num++] = '.';
    //			buff[num++] = '0';

    //		}
    //		else if((num-1) == dicNum)
    //		{
    //			buff[num++] = '0';
    //		}
    //
  }
  buff[0] = num - 1;
  ;
  return buff;
}

// char hexhex[8];
void oled_displayHex(rt_uint8_t y, rt_uint8_t x, rt_uint16_t data)
{
  char hex[8];
  rt_memset(hex, '0', 8);
  rt_uint8_t y_point = 16;
  rt_uint8_t x_point = 8;
  rt_uint8_t i = 0;
  rt_uint8_t cnt;
  int len = 8;
  while (data > 0)
  {
    hex[i++] = data % 2 + '0';
    data = data / 2;
  }

  oled_cmd_send(CMD_LOCAL_CHAR_DISP);
  oled_data_send(y * y_point); // ??????
  oled_data_send(x * x_point); // ?????
  oled_data_send(y_point);     // ????????????
  oled_data_send(x_point);     // ???????????
  oled_data_send(8);           // ???????
  oled_data_send(0);           // rev=0:?????????rev=1:???????
  cnt = 8;
  while (cnt > 0)
  {

    oled_data_send(hex[cnt - 1]);
    cnt--;
  }
  g_oled_pos.x_pos = x + len;
  g_oled_pos.y_pos = y;
}

void oled_displayHexAppend(rt_uint16_t data)
{
  oled_displayHex(g_oled_pos.y_pos, g_oled_pos.x_pos, data);
}
void oled_displaynumber(rt_uint8_t y, rt_uint8_t x, rt_uint16_t data, rt_uint8_t dicNum)
{
  rt_uint8_t y_point = 16;
  rt_uint8_t x_point = 8;
  rt_uint8_t i;
  rt_uint8_t cnt;

  char *pstr = num2String(data, dicNum);
  rt_uint8_t len = pstr[0];
  oled_cmd_send(CMD_LOCAL_CHAR_DISP);
  oled_data_send(y * y_point); // ??????
  oled_data_send(x * x_point); // ?????
  oled_data_send(y_point);     // ????????????
  oled_data_send(x_point);     // ???????????
  oled_data_send(len);         // ???????
  oled_data_send(0);           // rev=0:?????????rev=1:???????
  cnt = len;
  while (cnt > 0)
  {

    oled_data_send(pstr[cnt]);
    cnt--;
  }
  g_oled_pos.x_pos = x + len;
  g_oled_pos.y_pos = y;

} //*/

void oled_displaynumberAppend(rt_uint16_t data, rt_uint8_t dicNum)
{
  oled_displaynumber(g_oled_pos.y_pos, g_oled_pos.x_pos, data, dicNum);
}

void oled_displayVoltage(rt_uint8_t y, rt_uint8_t x, rt_uint16_t data)
{
  oled_displaynumber(y, x, data, 1);
  oled_displayChar(16, y, g_oled_pos.x_pos, "V          ");
} //*/

void oled_displayVoltageAppend(rt_uint16_t data)
{
  oled_displayVoltage(g_oled_pos.y_pos, g_oled_pos.x_pos, data);
} //*/

void oled_displayCurrent(rt_uint8_t y, rt_uint8_t x, rt_uint16_t data)
{
  oled_displaynumber(y, x, data, 2);
  oled_displayChar(16, y, g_oled_pos.x_pos, "A          ");
} //*/

void oled_displayCurrentAppend(rt_uint16_t data)
{
  oled_displayCurrent(g_oled_pos.y_pos, g_oled_pos.x_pos, data);
} //*/

void oled_displayGBK_24(rt_uint8_t y, rt_uint8_t x, rt_uint8_t *s)
{
  rt_uint8_t y_point = 24;
  rt_uint8_t x_point = 24;
  rt_uint8_t i;
  rt_uint8_t len = rt_strlen((const char *)s);
  oled_cmd_send(CMD_LOCAL_CHAR_DISP);
  oled_data_send(y * y_point); // ?????·
  oled_data_send(x * x_point); // ????·
  oled_data_send(y_point);     // ?·????????
  oled_data_send(x_point);     // ?·????ぐ??
  oled_data_send(len / 2);     // ?·????
  oled_data_send(0);           // rev=0:???????rev=1:·?°???

  while (*s != '\0')
    oled_data_send(*s++);

} //*/

void oled_displayGBK_16(rt_uint8_t y, rt_uint8_t x, rt_uint8_t *s)
{
  rt_uint8_t y_point = 16;
  rt_uint8_t x_point = 16;
  rt_uint8_t i;
  rt_uint8_t start = x;
  rt_uint8_t coloums = OLED_X_POITNS / x_point; // 240/16 = 15
  rt_uint8_t len = rt_strlen((const char *)s);
  oled_cmd_send(CMD_LOCAL_CHAR_DISP);
  oled_data_send(y * y_point); // ?????·
  oled_data_send(0);           // ????·
  oled_data_send(y_point);     // ?·????????
  oled_data_send(x_point);     // ?·????ぐ??
  oled_data_send(coloums);     // ?·????
  oled_data_send(0);           // rev=0:???????rev=1:·?°???
  while (start--)
  {
    oled_data_send(' '); // ??????????
    oled_data_send(' '); // ??????????
    coloums--;
  }

  while (*s != '\0')
    oled_data_send(*s++);

  coloums = coloums - len / 2;
  while (coloums--)
  {
    oled_data_send(' '); // ???????????
    oled_data_send(' '); // ???????????
  }

} //*/

typedef struct
{
  uint8_t total_bytes; // ?????????
  uint8_t char_number; // ???????
} oled_char_t;

void get_char_number(const rt_uint8_t *str, oled_char_t *pstr)
{
  int len = 0;
  int bytes = 0;
  int even_flag = 0;
  const uint8_t *s = str;

  while (*s++ != '\0')
  {
    if (*s >= 0x20 && *s <= 0x7F) // ASCII???
    {
      len++;
    }
    else if (*s >= 0x81 && *s <= 0xFE) // GBK???
    {
      if (even_flag++ % 2 == 0)
      {
        len++;
      }
    }
  }
  pstr->char_number = len;
  pstr->total_bytes = s - 1 - str;
}
void oled_displayMix(rt_uint8_t y, rt_uint8_t x, const rt_uint8_t *s)
{
  rt_uint8_t y_point = 16;
  rt_uint8_t x_point = 16;
  rt_uint8_t start;
  rt_uint8_t i;
  int len;
  rt_uint8_t coloums = OLED_X_POITNS / x_point; // 240/16 = 15

  oled_char_t str_stru;
  get_char_number(s, &str_stru);
  len = str_stru.char_number;
  int leftBytes = 30 - str_stru.total_bytes;

  start = x;

  //  oled_clear_line(y);
  //  rt_uint8_t len = rt_strlen((const char*)s);
  oled_cmd_send(CMD_LOCAL_CHAR_DISP);
  oled_data_send(y * y_point);     // ?????·
  oled_data_send(x * x_point);     // ????·
  oled_data_send(y_point);         // ?·????????
  oled_data_send(x_point);         // ?·????ぐ??
  oled_data_send(len + leftBytes); // ?·????
  oled_data_send(0);               // rev=0:???????rev=1:·?°???
  while (start-- > 0)
  {
    oled_data_send(' ');
    oled_data_send(' ');
  }

  while (*s != '\0')
    oled_data_send(*s++);
  int n = leftBytes - x * 2;
  while (n-- > 0)
  {
    oled_data_send(' ');
    oled_data_send(' ');
  }

} //*/

void INTER_chhzes2(rt_uint8_t y, rt_uint8_t x, rt_uint8_t m, rt_uint8_t n, rt_uint16_t k, rt_uint8_t rev, rt_uint8_t *s)
{
  rt_uint8_t i;
  oled_data_send(0x5B);
  oled_data_send(y);   // ??????
  oled_data_send(x);   // ?????
  oled_data_send(m);   // ????????????
  oled_data_send(n);   // ???????????
  oled_data_send(k);   // ???????
  oled_data_send(rev); // rev=0:?????????rev=1:???????

  if (*s >= 0x20 && *s <= 0x7F) // ASCII???
  {
    for (i = 0; i < k; i++)
      oled_data_send(*s++);
  }
  else if (*s >= 0x81 && *s <= 0xFE) // GBK???
  {
    for (i = 0; i < k; i++)
    {
      oled_data_send(*s++);
      oled_data_send(*s++);
    }
  }
} //*/

void chhzes(uint8_t x, uint8_t y, uint8_t m, uint8_t n, uint8_t k, uint8_t rev, uint8_t *s)
{
  uint8_t i;
  uint32_t j;
  oled_data_send(0x5A);
  oled_data_send(x);   // ??????
  oled_data_send(y);   // ?????
  oled_data_send(m);   // ????????????
  oled_data_send(n);   // ???????????
  oled_data_send(k);   // ???????
  oled_data_send(rev); // rev=0:?????????rev=1:???????
  for (i = 0; i < k; i++)
    // for(j=0;j<(n+7)/8*m;j++)
    for (j = 0; j < 16; j++)
      oled_data_send(*s++);
}

rt_uint16_t volatge = 1893;
void oled_init()
{
  rt_uint8_t data = 0x80;
  RESET_PIN_LOW();
  rt_thread_mdelay(5);
  RESET_PIN_HIGH();
  rt_thread_mdelay(100);
  oled_cmd_send(CMD_DISP_OFF);
  oled_cmd_send(CMD_DISP_ON);
  oled_cmd_send(CMD_CLR);
  //	oled_displayChar(0, 0, "U1: ");
  //	oled_displayVoltageAppend(g_ccr_data.ccr_info.v_scr);
  //	oled_displayChar(1, 0, "I1: ");
  //	oled_displayCurrentAppend(g_ccr_data.ccr_info.i_scr);
  //	oled_displayChar(2, 0, "U2: ");
  //	oled_displayVoltageAppend(g_ccr_data.ccr_info.v_load);
  //	oled_displayChar(3, 0, "I2: ");
  //	oled_displayCurrentAppend(g_ccr_data.ccr_info.i_load);
  // oled_displayVoltage(0, 9, volatge);
  //	oled_displaynumber(0,  9, g_ccr_data.ccr_info.i_scr, 2);//i1
  //	oled_displaynumber(0,  5, g_ccr_data.ccr_info.i_scr, 2);//i1
  //	oled_displaynumber(1,  5, 234, 2);
  //	oled_displaynumber(2,  5, 234, 0);

  //	oled_displayChar(0, 5,
  //	oled_displayChar(0, 0, );
  //	oled_displayChar(0, 0, "3.14");
  ////	oled_display(0, 0, "U2:");
  //	oled_display(0, 5, "220V");
  // oled_display(1, 0, "01234567890123");
  //	//INTER_chhzes2(32 ,0,16,8,3,0,"??????");
  //	oled_display2(2, 0, "??????");

  //	chhzes(32, 0, 32, 16, 15,0, num_table);
  INTER_chhzes(0, 0, 16, 16, 0, "012345678901234");
  //	INTER_chhzes(16, 0, 16, 16,  0, "012345678912345");
  //	INTER_chhzes(32, 0, 16, 16, 0, "012345678912345");
  //	INTER_chhzes(48, 0, 16, 16,  0, "012345678912345");
  //	INTER_chhzes(64, 0, 16, 16, 0, "012345678912345");
  //	INTER_chhzes(80, 0, 16, 16,  0, "012345678912345");
  //	INTER_chhzes(96, 0, 16, 16, 0, "012345678912345");
  //	INTER_chhzes(112, 0, 16, 16,  0, "012345678912345");
  //
  //	chhzes(32, 0, 32, 16, 15,0, num_table);
  //	chhzes(16, 0, 16, 16, 15, 0, num_table);
  //	chhzes(32, 0, 16, 16, 15,0,num_table);
  //	chhzes(48, 0, 16, 16, 15, 0, num_table);
  //	chhzes(64, 0, 16, 16, 15,0, num_table);
  //	chhzes(80, 0, 16, 16, 15, 0, num_table);
  //	chhzes(96, 0, 16, 16, 15,0, num_table);
  //	chhzes(112, 0, 16, 16,15,  0,num_table);
}

/******** 光标显示程序********/
void cursor(uint8_t x, uint8_t y, uint8_t n, uint8_t z)
{
  oled_data_send(0x70);
  oled_data_send(x); // 垂直地址
  oled_data_send(y); // 水平地址
  oled_data_send(n); // 光标水平点列数
  oled_data_send(z); // 绘点或消点
}