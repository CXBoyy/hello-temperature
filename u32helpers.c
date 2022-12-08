/* u32helpers.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-30 by F Lundevall

   For copyright and licensing, see file COPYING */

#include <stddef.h>   /* Declarations of integer sizes and the like, part 1 */
#include <stdint.h>   /* Declarations of integer sizes and the like, part 2 */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */

/* Declare all functions in advance. */

void savebyte( char s[], void * a );
void saveword( char s[], void * a );
static void saveinfo( char s[], void * ap, size_t ns );
void showinfo( void );
void u32init( void );

void labwork(void);
void quicksleep(int cyc);

/* Declare text buffer for display output */
extern char textbuffer[4][16];

char textbuffer[4][16];


/* Declare a helper function which is local to this file */
static void num32asc( char * s, int, int ); 

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)


/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n, int sz ) 
{
  int i;
  int bits;
  if( sz == 4 ) bits = 28;
  else if( sz == 2 ) bits = 12;
  else if( sz == 1 ) bits = 4;
  else bits = 0;
  if( bits )
    for( i = bits; i >= 0; i -= 4 )
      *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
  else
    for( i = 0; i < 8; i += 1 )
      *s++ = "bad size"[ i ];
}

/*
 * nextprime
 * 
 * Return the first prime number larger than the integer
 * given as a parameter. The integer must be positive.
 */
#define PRIME_FALSE   0     /* Constant to help readability. */
#define PRIME_TRUE    1     /* Constant to help readability. */

/*
 * itoa
 * 
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 * 
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 * 
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 * 
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 * 
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 * 
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 * 
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 * 
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )

void u32init(void) {
	/* Set up peripheral bus clock */
        /* OSCCONbits.PBDIV = 1; */
        OSCCONCLR = 0x100000; /* clear PBDIV bit 1 */
	OSCCONSET = 0x080000; /* set PBDIV bit 0 */
	
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
	/* SPI2STAT bit SPIROV = 0; */
	SPI2STATCLR = 0x40;
	/* SPI2CON bit CKP = 1; */
        SPI2CONSET = 0x40;
	/* SPI2CON bit MSTEN = 1; */
	SPI2CONSET = 0x20;
	/* SPI2CON bit ON = 1; */
	SPI2CONSET = 0x8000;
}

/* Here follows a lot of complicated code to display stuff */

#define NUMENTRIES (64)
#define LINELENGTH (16)
char str[NUMENTRIES][LINELENGTH];
unsigned int  add[NUMENTRIES];
unsigned int  dat[NUMENTRIES];
unsigned int  siz[NUMENTRIES];
int  flg[NUMENTRIES];
static int numused = 0;

void savebyte( char s[], void * a )
{
  saveinfo( s, a, 1 );
}

void saveword( char s[], void * a )
{
  saveinfo( s, a, 4 );
}

static void saveinfo( char s[], void * ap, size_t ns )
{
  char c;
  int i;
  int a = (unsigned int) ap;
  int n = (unsigned int) ns;
  if( numused > NUMENTRIES ) return;
  for( i = 0; i < LINELENGTH; i += 1 )
  {
    c = *s;
    s += 1;
    if( c )
      str[ numused ][ i ] = c;
    else
      str[ numused ][ i ] = 0;
  }
  str[ numused ][ LINELENGTH - 1 ] = 0; /* Always terminate a string */
  add[ numused ] = a;
  siz[ numused ] = n;
  if( n == 4 )
  {
    if( a & 3 )
      flg[ numused ] = 2; /* non-aligned */
    else
    {
      dat[ numused ] = *( (unsigned int *) ap);
      flg[ numused ] = 1; /* aligned */
    }
  }
  if( n == 2 )
  {
    if( a & 1 )
      flg[ numused ] = 2; /* non-aligned */
    else
    {
      dat[ numused ] = (unsigned int) *( (uint16_t *) ap);
      flg[ numused ] = 1; /* aligned */
    }
  }
  if( n == 1 )
  {
    dat[ numused ] = (unsigned int) *( (uint8_t *) ap);
    flg[ numused ] = 1; /* aligned */
  }
  if( ( n < 1 ) || ( n == 3 ) || ( n > 4 ) )
    flg[ numused ] = 4; /* weird size */
  numused += 1;
  return;
}

void showinfo( void )
{
  int current = 0;
  char tmp[ LINELENGTH ];
  char * cp;
  char * tp;

  u32init(); /* Initialize display */

  *( (volatile int *) 0xbf8860c8 ) = ( 0x3f << 5 ); /* ugly */

  if( numused <= 0 )
  {
    display_string( 3, "empty" );
    display_update();
    return;
  }

  while( 1 )
  {
    display_string( 0, str[ current ] );
    display_string( 1, "Addr" );
    num32asc( &textbuffer[1][6], add[ current ], 4 );
    if( flg[ current ] & 6 )
    {
      if( flg[ current ] & 2 ) display_string( 2, "Unaligned data" );
      if( flg[ current ] & 4 ) display_string( 2, "Bad data-size" );
    }
    else
    {
      display_string( 2, "Data" );
      num32asc( &textbuffer[2][6], dat[ current ], siz[ current ] );
    }
    tp = tmp;
    cp = itoaconv( current + 1 );
    if( *cp ) *tp++ = *cp++;
    if( *cp ) *tp++ = *cp++;
    if( *cp ) *tp++ = *cp++;
    *tp++ = 0x20;
    *tp++ = 'o';
    *tp++ = 'f';
    *tp++ = 0x20;
    cp = itoaconv( numused );
    if( *cp ) *tp++ = *cp++;
    if( *cp ) *tp++ = *cp++;
    if( *cp ) *tp++ = *cp++;
    *tp = 0;
    display_string( 3, tmp );
    display_update();
    while( 1 )
    {
      if( ( *( (volatile int *) 0xbf8860d0) >> 5 ) & 4 )
      {
	current -= 1;
	break;
      }
      if( ( *( (volatile int *) 0xbf8860d0) >> 5 ) & 2 )
      {
	current = 0;
        break;
      }
      if( ( *( (volatile int *) 0xbf8860d0) >> 5 ) & 1 )
      {
	current += 1;
	break;
      }
    }
    if( current < 0 ) current = numused - 1;
    if( current >= numused ) current = 0;
    while( ( *( (volatile int *) 0xbf8860d0) >> 5 ) & 7 );
  }
}

