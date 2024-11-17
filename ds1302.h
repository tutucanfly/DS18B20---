#ifndef _DS1302_H_
#define _DS1302_H_

void Write_Ds1302_Byte( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302_Byte ( unsigned char address );

#endif