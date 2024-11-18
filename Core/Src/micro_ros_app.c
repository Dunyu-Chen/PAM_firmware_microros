
#include "micro_ros_app.h"

// external interupt service for adc data periodic acquire
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == adc_instance.busy_pin)
    {
        AD7606_read_data_exti((struct AD7606_Params *) &adc_instance,adc_buffer);
    }

}

// ros executor timer callback
void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
    rcl_ret_t ret = rcl_publish(&publisher, &pub_msg, NULL);
    if (ret != RCL_RET_OK)
    {
        Error_Handler();
    }
    pub_msg.data++;
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

    rmw_qos_profile_t qos_profile = rmw_qos_profile_default;
    qos_profile.reliability = RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT;
    qos_profile.durability = RMW_QOS_POLICY_DURABILITY_VOLATILE;
    qos_profile.history = RMW_QOS_POLICY_HISTORY_KEEP_LAST;
    qos_profile.depth = 5; // 设置depth为5
    qos_profile.deadline.sec = 0; // 设置deadline的秒数部分
    qos_profile.deadline.nsec = 1*1000; // 设置deadline的纳秒部分

    // micro ros initialization
    allocator = rcl_get_default_allocator();

    rclc_support_init(&support, 0, NULL, &allocator);

    rclc_node_init_default(&node, "cubemx_node", "", &support);

    rclc_publisher_init(
        &publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
        "cubemx_publisher",
        &qos_profile);

    rcl_ret_t ret = rclc_timer_init_default(
        &timer,
        &support,
        RCL_MS_TO_NS(2),
        timer_callback);
    pub_msg.data = 0.0;
    executor = rclc_executor_get_zero_initialized_executor();
    rclc_executor_init(&executor,&support.context, 1, &allocator);
    ret = rclc_executor_add_timer(&executor,&timer);

    rclc_executor_spin(&executor);

    for(;;)
    {

    }
}

