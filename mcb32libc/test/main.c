#include <pic32mx.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

size_t write_uart0(FILE* instance, const char *bp, size_t n);
size_t read_uart0(FILE* instance, char *bp, size_t n);

static struct File_methods _uart_file_methods = {
	.read = read_uart0,
	.write = write_uart0,
};

static FILE _std = {
	.vmt = &_uart_file_methods,
};

FILE * const stdin = &_std;
FILE * const stdout = &_std;
FILE * const stderr = &_std;

size_t write_uart0(FILE *instance, const char *bp, size_t n) {
	while(n--) {
		while(U1STA & (1 << 9));
		if(*bp == '\n') {
			U1TXREG = '\r';
			while(U1STA & (1 << 9));
		}
		U1TXREG = *bp;
		bp++;
	}
	return n;
}

size_t read_uart0(FILE *instance, char *bp, size_t n) {
	while(n--) {
		while(!(U1STA & 0x1));
		*bp = U1RXREG & 0xFF;
		bp++;
	}
	return n;
}

static int calculate_baudrate_divider(int sysclk, int baudrate, int highspeed) {
	int pbclk, uxbrg, divmult;
	unsigned int pbdiv;
	
	divmult = (highspeed) ? 4 : 16;
	/* Periphial Bus Clock is divided by PBDIV in OSCCON */
	pbdiv = (OSCCON & 0x180000) >> 19;
	pbclk = sysclk >> pbdiv;
	
	/* Multiply by two, this way we can round the divider up if needed */
	uxbrg = ((pbclk * 2) / (divmult * baudrate)) - 2;
	/* We'll get closer if we round up */
	if (uxbrg & 1)
		uxbrg >>= 1, uxbrg++;
	else
		uxbrg >>= 1;
	return uxbrg;
}

void init() {
	/* On Uno32, we're assuming we're running with sysclk == 80 MHz */
	/* Periphial bust can run at a maximum of 40 MHz, setting PBDIV to 1 divides sysclk with 2 */
	OSCCON &= ~0x180000;
	OSCCON |= 0x080000;
	
	/* Configure UART1 for 115200 baud, no interrupts */
	U1BRG = calculate_baudrate_divider(80000000, 115200, 0);
	U1STA = 0;
	/* 8-bit data, no parity, 1 stop bit */
	U1MODE = 0x8000;
	/* Enable transmit and recieve */
	U1STASET = 0x1400;
}

void delay(int cyc) {
	volatile int i;
	for(i = cyc; i > 0; i--);
}

char *test[6] = {
	"some",
	"test",
	"strings",
	"for",
	"strlen",
	"function",
};

int main(void) {
	unsigned int i;
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	double a, b;
	
	a = 4.5;
	b = 5.6;
	
	printf("%i %i\n", (int)(a + b), (int)(1000.0*cos(30.0)));
	
	for(i = 0; i < 6; i++)
		printf("string length test %u: \"%s\" has length %i\n", i, test[i], strlen(test[i]));
	
	return 0;
}

