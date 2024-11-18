
#include "micro_ros_app.h"

// external interupt service for adc data periodic acquire
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == adc_instance.busy_pin)
    {
        AD7606_read_data_exti((struct AD7606_Params *) &adc_instance,adc_buffer);
    }

}

void StartROSApp(void *argument)
{
    // set up ad7606
    adc_instance.busy_pin = BUSY_Pin;
    adc_instance.busy_pin_port = BUSY_GPIO_Port;
    adc_instance.reset_pin = RST_Pin;
    adc_instance.reset_pin_port = RST_GPIO_Port;
    adc_instance.cs_pin = CS_Pin;
    adc_instance.cs_pin_port = CS_GPIO_Port;
    adc_instance.ca_timer_handler = htim2;
    adc_instance.ca1_timer_channel = TIM_CHANNEL_1;
    adc_instance.ca2_timer_channel = TIM_CHANNEL_4;
    adc_instance.spi_handler = hspi1;
    adc_instance.range_pin = RANGE_Pin;
    adc_instance.range_pin_port = RANGE_GPIO_Port;
    adc_instance.ser_pin = SER_Pin;
    adc_instance.ser_pin_port = SER_GPIO_Port;
    adc_instance.d15_pin = D15_Pin;
    adc_instance.d15_pin_port = D15_GPIO_Port;
    adc_instance.os0_pin = OS0_Pin;
    adc_instance.os0_pin_port = OS0_GPIO_Port;
    adc_instance.os1_pin = OS1_Pin;
    adc_instance.os1_pin_port = OS1_GPIO_Port;
    adc_instance.os2_pin = OS2_Pin;
    adc_instance.os2_pin_port = OS2_GPIO_Port;
    AD7606_init(
        (struct AD7606_Params *) &adc_instance,
        5,
        's',
        0);
    AD7606_reset((struct AD7606_Params *) &adc_instance);

    AD7606_start_conversion_pwm((struct AD7606_Params *) &adc_instance,1000);
    // set up micro ros transport
    rmw_uros_set_custom_transport(
        true,
        (void *) &huart1,
        cubemx_transport_open,
        cubemx_transport_close,
        cubemx_transport_write,
        cubemx_transport_read);

    // set up default allocator
    rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
    freeRTOS_allocator.allocate = microros_allocate;
    freeRTOS_allocator.deallocate = microros_deallocate;
    freeRTOS_allocator.reallocate = microros_reallocate;
    freeRTOS_allocator.zero_allocate =  microros_zero_allocate;
    if (!rcutils_set_default_allocator(&freeRTOS_allocator))
    {
        Error_Handler();
    }

    // micro ros initialization
    allocator = rcl_get_default_allocator();
    rclc_support_init(&support, 0, NULL, &allocator);
    rclc_node_init_default(&node, "cubemx_node", "", &support);
    rclc_publisher_init_default(
        &publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
        "cubemx_publisher");


    for(;;)
    {
        adc_value = adc_buffer[0];
        if (adc_value>=32768){
            adc_value = adc_value - 65535; //nagtive voltage
        }

        adc_voltage[0] = (adc_value/32767.0)*5.0 ;
        pub_msg.data = adc_voltage[0];
        rcl_ret_t ret = rcl_publish(&publisher, &pub_msg, NULL);
        if (ret != RCL_RET_OK)
        {
            Error_Handler();
        }

        osDelay(100);
    }
}

