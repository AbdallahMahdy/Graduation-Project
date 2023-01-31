
#ifndef GPS_H_
#define GPS_H_

#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "USART.h"
#define Buffer_Size 150
#define degrees_buffer_size 20

float ratof(char*);
void convert_time_to_UTC();
void convert_to_degrees(char *);

void get_gpstime();
void get_latitude(uint16_t);
void get_longitude(uint16_t);
void get_altitude(uint16_t );



#endif /* GPS_H_ */
