#include "Communication.h"

#include "MQTT.h"
#include <string.h>

#ifdef WIND_MQTT

extern MQTTClient client;
extern bool NORTH_LIMIT_REACHED;
extern bool SOUTH_LIMIT_REACHED;
extern bool EAST_LIMIT_REACHED;
extern bool WEST_LIMIT_REACHED;
extern long RECEIVED_WIND_LAST_TIMESTAMP;
extern bool MANUAL_MODE;

void sendData() {
        // read data from sensors
        brightness brightness = readSensors();
       
        // publish messages
        client.publish(PUBLISH_MQTT_TOPIC + "north", String(brightness.north));
        client.publish(PUBLISH_MQTT_TOPIC + "south", String(brightness.south));
        client.publish(PUBLISH_MQTT_TOPIC + "east", String(brightness.east));
        client.publish(PUBLISH_MQTT_TOPIC + "west", String(brightness.west));
        client.publish(PUBLISH_MQTT_TOPIC + "north_limit_switch", String(NORTH_LIMIT_REACHED));
        client.publish(PUBLISH_MQTT_TOPIC + "south_limit_switch", String(SOUTH_LIMIT_REACHED));
        client.publish(PUBLISH_MQTT_TOPIC + "east_limit_switch", String(EAST_LIMIT_REACHED));
        client.publish(PUBLISH_MQTT_TOPIC + "west_limit_switch", String(WEST_LIMIT_REACHED));
        client.publish(PUBLISH_MQTT_TOPIC + "last_wind_timestamp", String(RECEIVED_WIND_LAST_TIMESTAMP));
        client.publish(PUBLISH_MQTT_TOPIC + "mode", String(MANUAL_MODE));
}
#endif