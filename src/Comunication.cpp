#include "Communication.h"
<<<<<<< HEAD

=======
>>>>>>> b4ec83e (Add support to send data over mqtt)
#include "MQTT.h"
#include <string.h>

#ifdef WIND_MQTT

<<<<<<< HEAD
extern MQTTClient client;
=======
extern MQTTClient mqtt;
>>>>>>> b4ec83e (Add support to send data over mqtt)
extern bool NORTH_LIMIT_REACHED;
extern bool SOUTH_LIMIT_REACHED;
extern bool EAST_LIMIT_REACHED;
extern bool WEST_LIMIT_REACHED;
extern long RECEIVED_WIND_LAST_TIMESTAMP;
extern bool MANUAL_MODE;
<<<<<<< HEAD
=======
extern float RECEIVED_WIND;
>>>>>>> b4ec83e (Add support to send data over mqtt)

void sendData() {
        // read data from sensors
        brightness brightness = readSensors();
<<<<<<< HEAD
       
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
=======
        String prefix = PUBLISH_MQTT_TOPIC;
       
        // publish messages
        mqtt.publish(prefix + "north", String(brightness.north));
        mqtt.publish(prefix + "south", String(brightness.south));
        mqtt.publish(prefix + "east", String(brightness.east));
        mqtt.publish(prefix + "west", String(brightness.west));
        mqtt.publish(prefix + "north_limit_switch", String(NORTH_LIMIT_REACHED));
        mqtt.publish(prefix + "south_limit_switch", String(SOUTH_LIMIT_REACHED));
        mqtt.publish(prefix + "east_limit_switch", String(EAST_LIMIT_REACHED));
        mqtt.publish(prefix + "west_limit_switch", String(WEST_LIMIT_REACHED));
        mqtt.publish(prefix + "wind", String(RECEIVED_WIND));
        mqtt.publish(prefix + "last_wind_timestamp", String(RECEIVED_WIND_LAST_TIMESTAMP));
        mqtt.publish(prefix + "manual_mode", String(MANUAL_MODE));
>>>>>>> b4ec83e (Add support to send data over mqtt)
}
#endif