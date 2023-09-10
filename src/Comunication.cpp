#include "Communication.h"
#include "MQTT.h"
#include <string.h>

#ifdef WIND_MQTT

extern MQTTClient mqtt;
extern bool NORTH_LIMIT_REACHED;
extern bool SOUTH_LIMIT_REACHED;
extern bool EAST_LIMIT_REACHED;
extern bool WEST_LIMIT_REACHED;
extern long RECEIVED_WIND_LAST_TIMESTAMP;
extern bool MANUAL_MODE;
extern float RECEIVED_WIND;

void sendData() {
        // read data from sensors
        brightness brightness = readSensors();
        String prefix = PUBLISH_MQTT_TOPIC;
       
        // publish messages
        mqtt.publish(prefix + "north", String(brightness.north));
        mqtt.publish(prefix + "south", String(brightness.south));
        mqtt.publish(prefix + "east", String(brightness.east));
        mqtt.publish(prefix + "west", String(brightness.west));
        mqtt.publish(prefix + "ns_delta", String(brightness.north - brightness.south));
        mqtt.publish(prefix + "ew_delta", String(brightness.east - brightness.west));
        mqtt.publish(prefix + "north_limit_switch", String(NORTH_LIMIT_REACHED));
        mqtt.publish(prefix + "south_limit_switch", String(SOUTH_LIMIT_REACHED));
        mqtt.publish(prefix + "east_limit_switch", String(EAST_LIMIT_REACHED));
        mqtt.publish(prefix + "west_limit_switch", String(WEST_LIMIT_REACHED));
        mqtt.publish(prefix + "wind", String(RECEIVED_WIND));
        mqtt.publish(prefix + "last_wind_timestamp", String(RECEIVED_WIND_LAST_TIMESTAMP));
        mqtt.publish(prefix + "last_wind_timestamp_relative", String(millis() - RECEIVED_WIND_LAST_TIMESTAMP));
        mqtt.publish(prefix + "manual_mode", String(MANUAL_MODE));
}
#endif