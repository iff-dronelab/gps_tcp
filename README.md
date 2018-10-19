Interface to non-ROS GPS receiver over TCP
===============================

# Description
This ROS node connects to a server sending gps info from a gps device 
over TCP/IP and then converts the info to ROS NavSatFix msg and publishes
on `/gps/fix` topic

### Published Topics
- `/gps/fix` ([sensor_msgs/NavSatFix]) GPS position

### Example usage
```
rosrun gps_tcp gps_node _ip_addr:="192.1.1.1" _port:=4000
```
