/*
 * I2C Example project for the mcb32 toolchain
 * Demonstrates the temperature sensor and display of the Basic IO Shield
 * Make sure your Uno32-board has the correct jumper settings, as can be seen
 * in the rightmost part of this picture:
 * https://reference.digilentinc.com/_media/chipkit_uno32:jp6_jp8.png?w=300&tok=dcceb2
 */

#include <pic32mx.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "i2c-funcs.h"
#include <math.h>

#define DISPLAY_VDD_PORT PORTF
#define DISPLAY_VDD_MASK 0x40
#define DISPLAY_VBATT_PORT PORTF
#define DISPLAY_VBATT_MASK 0x20
#define DISPLAY_COMMAND_DATA_PORT PORTF
#define DISPLAY_COMMAND_DATA_MASK 0x10
#define DISPLAY_RESET_PORT PORTG
#define DISPLAY_RESET_MASK 0x200

char textbuffer[4][16];

static const uint8_t const font[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 2, 5, 2, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 94, 0, 0, 0, 0,
	0, 0, 4, 3, 4, 3, 0, 0,
	0, 36, 126, 36, 36, 126, 36, 0,
	0, 36, 74, 255, 82, 36, 0, 0,
	0, 70, 38, 16, 8, 100, 98, 0,
	0, 52, 74, 74, 52, 32, 80, 0,
	0, 0, 0, 4, 3, 0, 0, 0,
	0, 0, 0, 126, 129, 0, 0, 0,
	0, 0, 0, 129, 126, 0, 0, 0,
	0, 42, 28, 62, 28, 42, 0, 0,
	0, 8, 8, 62, 8, 8, 0, 0,
	0, 0, 0, 128, 96, 0, 0, 0,
	0, 8, 8, 8, 8, 8, 0, 0,
	0, 0, 0, 0, 96, 0, 0, 0,
	0, 64, 32, 16, 8, 4, 2, 0,
	0, 62, 65, 73, 65, 62, 0, 0,
	0, 0, 66, 127, 64, 0, 0, 0,
	0, 0, 98, 81, 73, 70, 0, 0,
	0, 0, 34, 73, 73, 54, 0, 0,
	0, 0, 14, 8, 127, 8, 0, 0,
	0, 0, 35, 69, 69, 57, 0, 0,
	0, 0, 62, 73, 73, 50, 0, 0,
	0, 0, 1, 97, 25, 7, 0, 0,
	0, 0, 54, 73, 73, 54, 0, 0,
	0, 0, 6, 9, 9, 126, 0, 0,
	0, 0, 0, 102, 0, 0, 0, 0,
	0, 0, 128, 102, 0, 0, 0, 0,
	0, 0, 8, 20, 34, 65, 0, 0,
	0, 0, 20, 20, 20, 20, 0, 0,
	0, 0, 65, 34, 20, 8, 0, 0,
	0, 2, 1, 81, 9, 6, 0, 0,
	0, 28, 34, 89, 89, 82, 12, 0,
	0, 0, 126, 9, 9, 126, 0, 0,
	0, 0, 127, 73, 73, 54, 0, 0,
	0, 0, 62, 65, 65, 34, 0, 0,
	0, 0, 127, 65, 65, 62, 0, 0,
	0, 0, 127, 73, 73, 65, 0, 0,
	0, 0, 127, 9, 9, 1, 0, 0,
	0, 0, 62, 65, 81, 50, 0, 0,
	0, 0, 127, 8, 8, 127, 0, 0,
	0, 0, 65, 127, 65, 0, 0, 0,
	0, 0, 32, 64, 64, 63, 0, 0,
	0, 0, 127, 8, 20, 99, 0, 0,
	0, 0, 127, 64, 64, 64, 0, 0,
	0, 127, 2, 4, 2, 127, 0, 0,
	0, 127, 6, 8, 48, 127, 0, 0,
	0, 0, 62, 65, 65, 62, 0, 0,
	0, 0, 127, 9, 9, 6, 0, 0,
	0, 0, 62, 65, 97, 126, 64, 0,
	0, 0, 127, 9, 9, 118, 0, 0,
	0, 0, 38, 73, 73, 50, 0, 0,
	0, 1, 1, 127, 1, 1, 0, 0,
	0, 0, 63, 64, 64, 63, 0, 0,
	0, 31, 32, 64, 32, 31, 0, 0,
	0, 63, 64, 48, 64, 63, 0, 0,
	0, 0, 119, 8, 8, 119, 0, 0,
	0, 3, 4, 120, 4, 3, 0, 0,
	0, 0, 113, 73, 73, 71, 0, 0,
	0, 0, 127, 65, 65, 0, 0, 0,
	0, 2, 4, 8, 16, 32, 64, 0,
	0, 0, 0, 65, 65, 127, 0, 0,
	0, 4, 2, 1, 2, 4, 0, 0,
	0, 64, 64, 64, 64, 64, 64, 0,
	0, 0, 1, 2, 4, 0, 0, 0,
	0, 0, 48, 72, 40, 120, 0, 0,
	0, 0, 127, 72, 72, 48, 0, 0,
	0, 0, 48, 72, 72, 0, 0, 0,
	0, 0, 48, 72, 72, 127, 0, 0,
	0, 0, 48, 88, 88, 16, 0, 0,
	0, 0, 126, 9, 1, 2, 0, 0,
	0, 0, 80, 152, 152, 112, 0, 0,
	0, 0, 127, 8, 8, 112, 0, 0,
	0, 0, 0, 122, 0, 0, 0, 0,
	0, 0, 64, 128, 128, 122, 0, 0,
	0, 0, 127, 16, 40, 72, 0, 0,
	0, 0, 0, 127, 0, 0, 0, 0,
	0, 120, 8, 16, 8, 112, 0, 0,
	0, 0, 120, 8, 8, 112, 0, 0,
	0, 0, 48, 72, 72, 48, 0, 0,
	0, 0, 248, 40, 40, 16, 0, 0,
	0, 0, 16, 40, 40, 248, 0, 0,
	0, 0, 112, 8, 8, 16, 0, 0,
	0, 0, 72, 84, 84, 36, 0, 0,
	0, 0, 8, 60, 72, 32, 0, 0,
	0, 0, 56, 64, 32, 120, 0, 0,
	0, 0, 56, 64, 56, 0, 0, 0,
	0, 56, 64, 32, 64, 56, 0, 0,
	0, 0, 72, 48, 48, 72, 0, 0,
	0, 0, 24, 160, 160, 120, 0, 0,
	0, 0, 100, 84, 84, 76, 0, 0,
	0, 0, 8, 28, 34, 65, 0, 0,
	0, 0, 0, 126, 0, 0, 0, 0,
	0, 0, 65, 34, 28, 8, 0, 0,
	0, 0, 4, 2, 4, 2, 0, 0,
	0, 120, 68, 66, 68, 120, 0, 0,
};

void delay(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

uint8_t spi_send_recvv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 0x01));
	return SPI2BUF;
}

void display_initt() {
	DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
	delay(10);
	DISPLAY_VDD_PORT &= ~DISPLAY_VDD_MASK;
	delay(1000000);
	
	spi_send_recvv(0xAE);
	DISPLAY_RESET_PORT &= ~DISPLAY_RESET_MASK;
	delay(10);
	DISPLAY_RESET_PORT |= DISPLAY_RESET_MASK;
	delay(10);
	
	spi_send_recvv(0x8D);
	spi_send_recvv(0x14);
	
	spi_send_recvv(0xD9);
	spi_send_recvv(0xF1);
	
	DISPLAY_VBATT_PORT &= ~DISPLAY_VBATT_MASK;
	delay(10000000);
	
	spi_send_recvv(0xA1);
	spi_send_recvv(0xC8);
	
	spi_send_recvv(0xDA);
	spi_send_recvv(0x20);
	
	spi_send_recvv(0xAF);
}

void display_stringg(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_updatee() {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_COMMAND_DATA_PORT |= DISPLAY_COMMAND_DATA_MASK;
		
		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;
			
			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

/* Convert 8.8 bit fixed point to string representation*/
char *fixed_to_string(uint16_t num, char *buf) {
	bool neg = false;
	uint32_t n;
	char *tmp;
	
	if(num & 0x8000) {
		num = ~num + 1;
		neg = true;
	}
	
	buf += 4;
	n = num >> 8;
	tmp = buf;
	do {
		*--tmp = (n  % 10) + '0';
		n /= 10;
	} while(n);
	if(neg)
		*--tmp = '-';
	
	n = num;
	if(!(n & 0xFF)) {
		*buf = 0;
		return tmp;
	}
	*buf++ = '.';
	while((n &= 0xFF)) {
		n *= 10;
		*buf++ = (n >> 8) + '0';
	}
	*buf = 0;
	
	return tmp;
}

uint32_t strlenn(char *str) {
	uint32_t n = 0;
	while(*str++)
		n++;
	return n;
}

int main(void) {
	uart_init();
	char buffstr[50];
	uart_write_line("Test");

	uint16_t accelerometer_x;
	uint16_t accelerometer_y;
	uint16_t accelerometer_z;
	uint16_t gyroscope_x;
	uint16_t gyroscope_y;
	uint16_t gyroscope_z;

	char buf1[32], buf2[32],buf3[32],buf4[32], *s, *t;

	/* Set up peripheral bus clock */
	OSCCON &= ~0x180000;
	OSCCON |= 0x080000;
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	
	/* Clear SPIROV*/
	SPI2STATCLR &= ~0x40;
	/* Set CKP = 1, MSTEN = 1; */
        SPI2CON |= 0x60;
	
	/* Turn on SPI */
	SPI2CONSET = 0x8000;
	
	/* Set up i2c */
	I2C1CON = 0x0;
	/* I2C Baud rate should be less than 400 kHz, is generated by dividing
	the 40 MHz peripheral bus clock down */
	I2C1BRG = 0x064;
	I2C1STAT = 0x0;
	I2C1CONSET = 1 << 13; //SIDL = 1
	I2C1CONSET = 1 << 15; // ON = 1
	accelerometer_x = I2C1RCV; //Clear receive buffer
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	
	display_initt();
	char *line1;
	char *line2;
	char *line3;
	char *line4;
	display_stringg(0, "A_X: ");
	display_stringg(1, "A_Y: ");
	display_stringg(2, "A_Z: ");
	display_stringg(3, "");
	display_updatee();

	// Variables
	accelerometer_x = 0;
	accelerometer_y = 0;
	accelerometer_z = 0;
	gyroscope_x = 0;
	gyroscope_y = 0;
	gyroscope_z = 0;
	int angle = 0;
	int angle2 = 0;
	int test_out = 0; 
	

	// Set-up
	i2c_write_register(CTRL3_C, 0b10000001); // Reset device
	i2c_write_register(FUNC_CFG_ACCESS, 0x0);
	i2c_write_register(CTRL1_XL, 0b10100000);
	i2c_write_register(CTRL2_G, 0b10100000);
	i2c_write_register(CTRL3_C, 0b01000000);
	i2c_write_register(CTRL4_C, 0x0);
	i2c_write_register(CTRL5_C, 0x0);
	i2c_write_register(CTRL6_C, 0x0);
	i2c_write_register(CTRL7_G, 0x0);
	i2c_write_register(CTRL8_XL, 0b11010000);
	i2c_write_register(CTRL9_XL, 0x0);
	i2c_write_register(CTRL10_C, 0x0);
	i2c_write_register(FIFO_CTRL3, 0x0);
	i2c_write_register(FIFO_CTRL4, 0x0);
	//

	test_out = i2c_read_register(FUNC_CFG_ACCESS);
	i2c_nack();
	i2c_stop();
	sprintf(buffstr, "%d", test_out);
	uart_write_line(buffstr);

	
	for(;;) {

		accelerometer_x = i2c_read_accel_x();
		accelerometer_y = i2c_read_accel_y();
		accelerometer_z = i2c_read_accel_z();
		gyroscope_x 		= i2c_read_gyro_x();
		gyroscope_y			= i2c_read_gyro_y();
		gyroscope_z			= i2c_read_gyro_z();		
		//angle = (atan(accelerometer_x/accelerometer_y)) * (180/3.14159);
		angle = (acos(accelerometer_z)) * (180/3.14159);
		angle2 = (asin(sqrt(pow(accelerometer_x, 2) + pow(accelerometer_y, 2))) * (180/3.14159));
		// uart_write_line(sqrt(pow(accelerometer_x, 2) + pow(accelerometer_y, 2)));

		sprintf(buffstr, "%d", accelerometer_x);
		uart_write_line(buffstr);
		display_updatee();
		display_updatee();
		
		line1 = fixed_to_string(accelerometer_x, buf1);
		t = line1 + strlenn(line1);
		*t++ = ' ';
		*t++ = 7;
		*t++ = 0;

		line2 = fixed_to_string(accelerometer_y, buf2);
		t = line2 + strlenn(line2);
		*t++ = ' ';
		*t++ = 7;
		*t++ = 0;

		line3 = fixed_to_string(accelerometer_z, buf3);
		t = line3 + strlenn(line3);
		*t++ = ' ';
		*t++ = 7;
		*t++ = 0;

		line4 = fixed_to_string(angle2, buf4);
		t = line4 + strlenn(line4);
		*t++ = ' ';
		*t++ = 7;
		*t++ = 0;
		
		display_string(0, line1);
		display_string(1, line2);
		display_string(2, line3);
		display_string(3, line4);
		display_updatee();
		delay(1000000);
	}
	
	return 0;
}

