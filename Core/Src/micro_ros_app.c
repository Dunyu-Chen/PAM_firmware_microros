
#include "micro_ros_app.h"

void StartROSApp(void *argument)
{
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

    // setup message
    pub_msg.data = (float) 0.0;
    for(;;)
    {
        rcl_ret_t ret = rcl_publish(&publisher, &pub_msg, NULL);
        if (ret != RCL_RET_OK)
        {
            Error_Handler();
        }
        pub_msg.data += (float) 1.0;
        osDelay(100);
    }
}

