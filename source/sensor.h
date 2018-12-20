#ifndef __SENSOR_H__
#define __SENSOR_H__

//#include "sereg/serial.h"

#define SENSOR_BASE  0x20
#define SENSOR_HUMIDITY  (SENSOR_BASE + 18)

#define SENSOR_END  (SENSOR_BASE + 20)

#define FAN_BASE    (0x40)
#define FAN_END     (FAN_BASE + 4)

#define VALVE_BASE  (0x60)
#define VALVE_END   (VALVE_BASE + 4)

#endif

