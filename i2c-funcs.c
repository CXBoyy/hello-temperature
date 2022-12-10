/* Address of LSM6DSOX on the I2C bus */
#include <pic32mx.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "i2c-funcs.h"

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

  while(!i2c_send(rgstr));

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

  while(!i2c_send(rgstr));
  while(!i2c_send(data));
  i2c_stop();
}

int i2c_read_accel_x(){
  int data = 0;
  data = i2c_read_register(OUTX_H_A) << 8;
  i2c_nack();
  i2c_stop();

  data |= i2c_read_register(OUTX_L_A);
  i2c_nack();
  i2c_stop();
  return data;
}

int i2c_read_accel_y(){
  int data = 0;
  data = i2c_read_register(OUTY_H_A) << 8;
  i2c_nack();
  i2c_stop();

  data |= i2c_read_register(OUTY_L_A);
  i2c_nack();
  i2c_stop();
  return data;
}

int i2c_read_accel_z(){
  int data = 0;
  data = i2c_read_register(OUTZ_H_A) << 8;
  i2c_nack();
  i2c_stop();

  data |= i2c_read_register(OUTZ_L_A);
  i2c_nack();
  i2c_stop();
  return data;
}

int i2c_read_gyro_x(){
  int data = 0;
  data = i2c_read_register(OUTX_H_G) << 8;
  i2c_nack();
  i2c_stop();

  data |= i2c_read_register(OUTX_L_G);
  i2c_nack();
  i2c_stop();
  return data;
}

int i2c_read_gyro_y(){
  int data = 0;
  data = i2c_read_register(OUTY_H_G) << 8;
  i2c_nack();
  i2c_stop();

  data |= i2c_read_register(OUTY_L_G);
  i2c_nack();
  i2c_stop();
  return data;
}

int i2c_read_gyro_z(){
  int data = 0;
  data = i2c_read_register(OUTZ_H_G) << 8;
  i2c_nack();
  i2c_stop();

  data |= i2c_read_register(OUTZ_L_G);
  i2c_nack();
  i2c_stop();
  return data;
}