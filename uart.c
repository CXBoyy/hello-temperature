#include <pic32mx.h>
int calculate_baudrate_divider(int sysclk, int baudrate, int highspeed) {
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

void uart_init() {
    /* On Uno32, we're assuming we're running with sysclk == 80 MHz */
    /* Periphial bust can run at a maximum of 40 MHz, setting PBDIV to 1 divides sysclk with 2 */
    OSCCON &= ~0x180000;
    OSCCON |= 0x080000;

    delay(10000000);
    U1BRG = calculate_baudrate_divider(80000000, 115200, 0);
    U1STA = 0;

    U1MODE = 0x8000;

    U1STASET = 0x400;

}


void uart_write_line(char* str) {
    int i;
    for (i = 0; i < strlen(str); i++)
    {
        while(U1STA & (1 << 9)); //make sure the write buffer is not full 
        U1TXREG = str[i];
    }
    // Write newline and carrage return
    while(U1STA & (1 << 9));
    U1TXREG = '\n';
    while(U1STA & (1 << 9)); 
    U1TXREG = '\r';
}