#include "ad7606_driver.h"

void AD7606_init(
  struct AD7606_Params* Params,
  uint8_t Range,
  uint8_t Transmit_mode,
  uint8_t Oversample_mode)
{
	// setup conversion range
	switch(Range){
	    default :
			Error_Handler();
	    case 5 :
			HAL_GPIO_WritePin(Params->range_pin_port,Params->range_pin,GPIO_PIN_RESET);
			break;
	    case 10 :
			HAL_GPIO_WritePin(Params->range_pin_port,Params->range_pin,GPIO_PIN_SET);
			break;
	}
	  //setup transmitting mode
	switch(Transmit_mode){
		default :
			Error_Handler();
			// serial transmitting mode
		case 's' :
			HAL_GPIO_WritePin(Params->ser_pin_port,Params->ser_pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Params->d15_pin_port,Params->d15_pin,GPIO_PIN_RESET);
			break;
		// other mode : refine later if needed
	}
	//setup over-sample mode
	switch(Oversample_mode){
		default :
			Error_Handler();
	    case 0 :
			HAL_GPIO_WritePin(Params->os2_pin_port,Params->os2_pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Params->os1_pin_port,Params->os1_pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Params->os0_pin_port,Params->os0_pin,GPIO_PIN_RESET);
			break;
	    case 2 :
			HAL_GPIO_WritePin(Params->os2_pin_port,Params->os2_pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Params->os1_pin_port,Params->os1_pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Params->os0_pin_port,Params->os0_pin,GPIO_PIN_SET);
			break;
		case 4 :
			HAL_GPIO_WritePin(Params->os2_pin_port,Params->os2_pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Params->os1_pin_port,Params->os1_pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Params->os0_pin_port,Params->os0_pin,GPIO_PIN_RESET);
			break;
	    case 8 :
			HAL_GPIO_WritePin(Params->os2_pin_port,Params->os2_pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Params->os1_pin_port,Params->os1_pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Params->os0_pin_port,Params->os0_pin,GPIO_PIN_SET);
			break;
	    case 16 :
			HAL_GPIO_WritePin(Params->os2_pin_port,Params->os2_pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Params->os1_pin_port,Params->os1_pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Params->os0_pin_port,Params->os0_pin,GPIO_PIN_RESET);
			break;
	    case 32 :
			HAL_GPIO_WritePin(Params->os2_pin_port,Params->os2_pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Params->os1_pin_port,Params->os1_pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Params->os0_pin_port,Params->os0_pin,GPIO_PIN_SET);
			break;
	    case 64 :
			HAL_GPIO_WritePin(Params->os2_pin_port,Params->os2_pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Params->os1_pin_port,Params->os1_pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(Params->os0_pin_port,Params->os0_pin,GPIO_PIN_RESET);
			break;
	}
	// generate reset signal
	AD7606_reset(Params);
}

void AD7606_reset(struct AD7606_Params* Params){
	HAL_GPIO_WritePin(Params->reset_pin_port,Params->reset_pin,GPIO_PIN_RESET);
	HAL_Delay(5);
	HAL_GPIO_WritePin(Params->reset_pin_port,Params->reset_pin,GPIO_PIN_SET);
	HAL_Delay(5);
	HAL_GPIO_WritePin(Params->reset_pin_port,Params->reset_pin,GPIO_PIN_RESET);
}

void AD7606_start_conversion_pwm(struct AD7606_Params* Params,double Period_us){
	// calculate pwm value
	// conversion signal period
	double ca_signal_period = 10.0; //us
	// duty = ca_signal_period / Period
	uint32_t max_pwm_value = Params->ca_timer_handler.Init.Period +1;
	uint32_t pwm_value = (int) max_pwm_value * (ca_signal_period / Period_us);
	// start conversion
	HAL_TIM_PWM_Start(&Params->ca_timer_handler, Params->ca1_timer_channel);
	HAL_TIM_PWM_Start(&Params->ca_timer_handler, Params->ca2_timer_channel);
	__HAL_TIM_SetCompare(&Params->ca_timer_handler, Params->ca1_timer_channel,pwm_value);
	__HAL_TIM_SetCompare(&Params->ca_timer_handler, Params->ca2_timer_channel,pwm_value);
}

void AD7606_read_data_exti(struct AD7606_Params* Params,uint16_t * Receive_Buffer){
	__NOP();
	HAL_GPIO_WritePin(Params->cs_pin_port,Params->cs_pin,GPIO_PIN_RESET);
	HAL_SPI_Receive(&Params->spi_handler, (uint8_t *) Receive_Buffer, 8, 1000);
	HAL_GPIO_WritePin(Params->cs_pin_port,Params->cs_pin,GPIO_PIN_SET);
}
