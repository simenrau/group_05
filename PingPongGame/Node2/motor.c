#include <avr/interrupt.h>
#include "motor.h"
#include "TWI_master.h"

void motor_init()
{
	MOTOR_DDR |= (1 << MOTOR_PIN_DIR) | (1 << MOTOR_PIN_OE) | (1 << MOTOR_PIN_EN) | (1 << MOTOR_PIN_SEL) | (1 << MOTOR_PIN_DIR);
	
	ENCODER_DDR = 0;

	MOTOR_PORT &= ~(MOTOR_PIN_RST);
	
	_delay_us(20);
	
	MOTOR_PORT |= (1 << MOTOR_PIN_RST);

	TWI_Master_Initialise();
	sei();
}

int16_t motor_read_encoder()
{
	MOTOR_PORT &= ~((1 << MOTOR_PIN_OE) | (1 << MOTOR_PIN_SEL));
	_delay_us(20);
	
	uint8_t high_byte = ENCODER_INPUT;
	
	MOTOR_PORT |= (1 << MOTOR_PIN_SEL);
	_delay_us(20);
	
	uint8_t low_byte = ENCODER_INPUT;

	MOTOR_PORT |= (1 << MOTOR_PIN_OE);

	return ((int16_t)high_byte)<<8 | (low_byte);
}

void motor_velocity(int16_t velocity)
{
	MOTOR_PORT |= (1 << MOTOR_PIN_EN);
	
	uint8_t speed;
	if (velocity < 0)
	{
		MOTOR_PORT |= (1 << MOTOR_PIN_DIR);
		speed = (-velocity) & 0xff;
	}
	if (velocity > 0)
	{
		MOTOR_PORT &= ~(1 << MOTOR_PIN_DIR);
		speed = (velocity) & 0xff;
	}

	uint8_t command = 0b00000000;
	uint8_t msg[] = { MAX520_ADDRESS << 1, command, speed };
	TWI_Start_Transceiver_With_Data(msg, sizeof(msg));
	while (TWI_Transceiver_Busy()) {}
	
}
