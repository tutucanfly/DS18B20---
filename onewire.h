#ifndef _ONEWIRE_H_
#define _ONEWIRE_H_

void Write_DS18B20(unsigned char dat);
unsigned char Read_DS18B20(void);
bit init_ds18b20(void);

#endif
