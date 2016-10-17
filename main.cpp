/*
 * GccApplication1.cpp
 *
 * Created: 12.10.2016 9:55:15
 * Author : Admin
 */ 

#include <avr/io.h>
#include <asf.h>
#include <pca9532d.h>
#include <conf_twi.h>

int data[DATA_LENGTH];

uint8_t ls_address;
uint8_t pwm_addr;
uint8_t prsc_addr;

twi_package_t packet;

TWI_Slave_t slave;

twi_options_t m_options;
  
void setup_master_options()
{
	m_options.speed     = TWI_SPEED;
	m_options.chip      = TWI_MASTER_ADDR;
	m_options.speed_reg = TWI_BAUD(sysclk_get_cpu_hz(), TWI_SPEED);
}
 
void set_autoincrementation(uint8_t &adress){
	adress |= 0b00010000;
}

void pca9532d_set_prescaler(uint8_t index) {
	// неверный индекс
	if (index > 1) return;
	
	if (index == 0) {
		prsc_addr = PCA9532D_PSC0;
		}
	else {
		prsc_addr = PCA9532D_PSC1;
		}
}

void pca9532d_set_pwm(uint8_t index) {
	// неверный индекс
	if (index > 1) return;

	if (index == 0) {
		pwm_addr = PCA9532D_PWM0;
	}
	else {
		pwm_addr = PCA9532D_PWM1;		
	}
}

 void pca9532d_set_led_mode(uint8_t index) {	 
	 if (index > 15) return;
	 if (index >= 0 && index <= 3) ls_address = PCA9532D_LS0;
	 if (index >= 4 && index <= 7) ls_address = PCA9532D_LS1;
	 if (index >= 8 && index <= 11) ls_address = PCA9532D_LS2;
	 if (index >= 12 && index <= 15) ls_address = PCA9532D_LS3;
}

void write_to_slave(){
	
	packet.addr_length = 0;
	packet.chip        = TWI_SLAVE_ADDR;
	packet.buffer      = (void *)data;
	packet.length      = DATA_LENGTH;
	packet.no_wait     = false;
	
	TWI_MASTER_PORT.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;
	TWI_MASTER_PORT.PIN1CTRL = PORT_OPC_WIREDANDPULL_gc;
	
	irq_initialize_vectors();
	
	sysclk_enable_peripheral_clock(&TWI_MASTER);
	twi_master_enable(&TWI_MASTER);
	sysclk_enable_peripheral_clock(&TWI_SLAVE);
	twi_master_init(&TWI_MASTER, &m_options);
	
	cpu_irq_enable();
	
	twi_master_write(&TWI_MASTER, &packet);	
}

void test_package_creating()
{
	pca9532d_set_prescaler(0);
	set_autoincrementation(prsc_addr);
}

int main(void)
{
	sysclk_init();
	board_init();
	setup_master_options();
	pca9532d_set_prescaler(0);
	set_autoincrementation(prsc_addr);
	data[0] = prsc_addr;
	data[1] = 0x97;
	data[2] = 0x80;
	data[3] = 0x00;
	data[4] = 0x40;
	data[5] = 0x55;
	data[6] = 0xFA;
	data[7] = 0x00;
	data[8] = 0x00;
	write_to_slave();
}

