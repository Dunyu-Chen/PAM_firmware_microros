#ifndef INC_MICRO_ROS_APP_H_
#define INC_MICRO_ROS_APP_H_
#include "main.h"
#include "cmsis_os.h"
#include <usart.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <uxr/client/transport.h>
#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>

#include <std_msgs/msg/float32.h>
#include <std_msgs/msg/int32.h>

#include "ad7606_driver.h"

bool cubemx_transport_open(struct uxrCustomTransport * transport);
bool cubemx_transport_close(struct uxrCustomTransport * transport);
size_t cubemx_transport_write(struct uxrCustomTransport* transport, const uint8_t * buf, size_t len, uint8_t * err);
size_t cubemx_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);

void * microros_allocate(size_t size, void * state);
void microros_deallocate(void * pointer, void * state);
void * microros_reallocate(void * pointer, size_t size, void * state);
void * microros_zero_allocate(size_t number_of_elements, size_t size_of_element, void * state);

//
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_publisher_t publisher;

rcl_timer_t timer;
rclc_executor_t executor;


std_msgs__msg__Float32 pub_msg;

struct AD7606_Params adc_instance;
volatile uint16_t adc_buffer[8];
volatile float adc_voltage[8];
#endif /* INC_MICRO_ROS_APP_H_ */
