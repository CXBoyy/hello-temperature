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

#define DISPLAY_VDD_PORT PORTF
#define DISPLAY_VDD_MASK 0x40
#define DISPLAY_VBATT_PORT PORTF
#define DISPLAY_VBATT_MASK 0x20
#define DISPLAY_COMMAND_DATA_PORT PORTF
#define DISPLAY_COMMAND_DATA_MASK 0x10
#define DISPLAY_RESET_PORT PORTG
#define DISPLAY_RESET_MASK 0x200

/* Address of the temperature sensor on the I2C bus */
#define LSM6DSOX_ADDR 0x6A

/* LSM6DSOX internal registers */
typedef enum LSM6DSOX_REG LSM6DSOX_REG;
enum LSM6DSOX_REG {
	FUNC_CFG_ACCESS = 0x1,
	PIN_CTRL = 0x2,
	S4S_TPH_L = 0x4,
	S4S_TPH_H = 0x5,
	S4S_RR = 0x6,
	FIFO_CTRL1 = 0x7,
	FIFO_CTRL2 = 0x8,
	FIFO_CTRL3 = 0x9,
	FIFO_CTRL4 = 0x0A,
	COUNTER_BDR_REG1 = 0x0B,
	COUNTER_BDR_REG2 = 0x0C,
	INT1_CTRL = 0x0D,
	INT2_CTRL = 0x0E,
	WHO_AM_I = 0x0F,
	CTRL1_XL = 0x10,
	CTRL2_G = 0x11,
	CTRL3_C = 0x12,
	CTRL4_C = 0x13,
	CTRL5_C = 0x14,
	CTRL6_C = 0x15,
	CTRL7_G = 0x16,
	CTRL8_XL = 0x17,
	CTRL9_XL = 0x18,
	CTRL10_C = 0x19,
	ALL_INT_SRC = 0x1A,
	WAKE_UP_SRC = 0x1B,
	TAP_SRC = 0x1C,
	D6D_SRC = 0x1D,
	STATUS_REG = 0x1E,
	OUT_TEMP_L = 0x20,
	OUT_TEMP_H = 0x21,
	OUTX_L_G = 0x22,
	OUTX_H_G = 0x23,
	OUTY_L_G = 0x24,
	OUTY_H_G = 0x25,
	OUTZ_L_G = 0x26,
	OUTZ_H_G = 0x27,
	OUTX_L_A = 0x28,
	OUTX_H_A = 0x29,
	OUTY_L_A = 0x2A,
	OUTY_H_A = 0x2B,
	OUTZ_L_A = 0x2C,
	OUTZ_H_A = 0x2D,
	EMB_FUNC_STATUS_MAINPAGE = 0x35,
	FSM_STATUS_A_MAINPAGE = 0x36,
	FSM_STATUS_B_MAINPAGE = 0x37,
	MLC_STATUS_MAINPAGE = 0x38,
	STATUS_MASTER_MAINPAGE = 0x39,
	FIFO_STATUS1 = 0x3A,
	FIFO_STATUS2 = 0x3B,
	TIMESTAMP0 = 0x40,
	TIMESTAMP1 = 0x41,
	TIMESTAMP2 = 0x42,
	TIMESTAMP3 = 0x43,
	UI_STATUS_REG_OIS = 0x49,
	UI_OUTX_L_G_OIS = 0x4A,
	UI_OUTX_H_G_OIS = 0x4B,
	UI_OUTY_L_G_OIS = 0x4C,
	UI_OUTY_H_G_OIS = 0x4D,
	UI_OUTZ_L_G_OIS = 0x4E,
	UI_OUTZ_H_G_OIS = 0x4F,
	UI_OUTX_L_A_OIS = 0x50,
	UI_OUTX_H_A_OIS = 0x51,
	UI_OUTY_L_A_OIS = 0x52,
	UI_OUTY_H_A_OIS = 0x53,
	UI_OUTZ_L_A_OIS = 0x54,
	UI_OUTZ_H_A_OIS = 0x55,
	TAP_CFG0 = 0x56,
	TAP_CFG1 = 0x57,
	TAP_CFG2 = 0x58,
	TAP_THS_6D = 0x59,
	INT_DUR2 = 0x5A,
	WAKE_UP_THS = 0x5B,
	WAKE_UP_DUR = 0x5C,
	FREE_FALL = 0x5D,
	MD1_CFG = 0x5E,
	MD2_CFG = 0x5F,
	S4S_ST_CMD_CODE = 0x60,
	S4S_DT_REG = 0x61,
	I3C_BUS_AVB = 0x62,
	INTERNAL_FREQ_FINE = 0x63,
	UI_INT_OIS = 0x6F,
	UI_CTRL1_OIS = 0x70,
	UI_CTRL2_OIS = 0x71,
	UI_CTRL3_OIS = 0x72,
	X_OFS_USR = 0x73,
	Y_OFS_USR = 0x74,
	Z_OFS_USR = 0x75,
	FIFO_DATA_OUT_TAG = 0x78,
	FIFO_DATA_OUT_X_L = 0x79,
	FIFO_DATA_OUT_X_H = 0x7A,
	FIFO_DATA_OUT_Y_L = 0x7B,
	FIFO_DATA_OUT_Y_H = 0x7C,
	FIFO_DATA_OUT_Z_L = 0x7D,
	FIFO_DATA_OUT_Z_H = 0x7E
};


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

/* Wait for I2C bus to become idle */
void i2c_idle() {
	while(I2C1CON & 0x1F || I2C1STAT & (1 << 14)); //TRSTAT
}

/* Send one byte on I2C bus, return ack/nack status of transaction */
bool i2c_send(uint8_t data) {
	i2c_idle();
	I2C1TRN = data;
	i2c_idle();
	return !(I2C1STAT & (1 << 15)); //ACKSTAT
}

/* Receive one byte from I2C bus */
uint8_t i2c_recv() {
	i2c_idle();
	I2C1CONSET = 1 << 3; //RCEN = 1
	i2c_idle();
	I2C1STATCLR = 1 << 6; //I2COV = 0
	return I2C1RCV;
}

/* Send acknowledge conditon on the bus */
void i2c_ack() {
	i2c_idle();
	I2C1CONCLR = 1 << 5; //ACKDT = 0
	I2C1CONSET = 1 << 4; //ACKEN = 1
}

/* Send not-acknowledge conditon on the bus */
void i2c_nack() {
	i2c_idle();
	I2C1CONSET = 1 << 5; //ACKDT = 1
	I2C1CONSET = 1 << 4; //ACKEN = 1
}

/* Send start conditon on the bus */
void i2c_start() {
	i2c_idle();
	I2C1CONSET = 1 << 0; //SEN
	i2c_idle();
}

/* Send restart conditon on the bus */
void i2c_restart() {
	i2c_idle();
	I2C1CONSET = 1 << 1; //RSEN
	i2c_idle();
}

/* Send stop conditon on the bus */
void i2c_stop() {
	i2c_idle();
	I2C1CONSET = 1 << 2; //PEN
	i2c_idle();
}

int i2c_read_register(int rgstr){
	int data = 0;
	do {
			i2c_start();
		} while(!i2c_send(LSM6DSOX_ADDR << 1));

		do {

		} while(!i2c_send(rgstr));

		//display_string(2,"-----");
		//display_updatee();

		/* Now send another start condition and address of the temperature sensor with
		read mode (lowest bit = 1) until the temperature sensor sends
		acknowledge condition */
		do {
			i2c_start();
		} while(!i2c_send((LSM6DSOX_ADDR << 1) | 1));
		
		data = i2c_recv();

		return data;
}

void i2c_write_register(int rgstr, int data){
	do {
			i2c_start();
		} while(!i2c_send(LSM6DSOX_ADDR << 1));

		do {

		} while(!i2c_send(rgstr));

		do {

		} while(!i2c_send(data));
		i2c_stop();
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
	char buf[32], *s, *t;

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
	display_stringg(0, "Temperature:");
	display_stringg(1, "");
	display_stringg(2, "");
	display_stringg(3, "");
	display_updatee();

	int accelerometer_y = 0;
	int accelerometer_z = 0;
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
		uart_write_line("----");
		test_out = i2c_read_register(CTRL8_XL);
		i2c_nack();
		i2c_stop();
		sprintf(buffstr, "%d", test_out);
		uart_write_line(buffstr);
		uart_write_line("----");

		accelerometer_x = i2c_read_register(OUTX_H_A) << 8;
		i2c_nack();
		i2c_stop();


		accelerometer_x |= i2c_read_register(OUTX_L_A);
		i2c_nack();
		i2c_stop();
		sprintf(buffstr, "%d", accelerometer_x);
		uart_write_line(buffstr);
		// display_stringg(2, "test");
		// display_updatee();
		//display_string(1, "ack");
		//i2c_ack();
		display_updatee();
		/* To stop receiving, send nack and stop */
		//display_string(2, "nack");
		display_updatee();
		
		s = fixed_to_string(accelerometer_x, buf);
		t = s + strlenn(s);
		*t++ = ' ';
		*t++ = 7;
		*t++ = 'C';
		*t++ = 0;
		
		display_string(1, s);
		display_string(2, accelerometer_x);
		display_updatee();
		delay(1000000);
	}
	
	return 0;
}

