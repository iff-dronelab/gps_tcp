cmake_minimum_required(VERSION 2.8.3)
project(gps_tcp)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  sensor_msgs
  nav_msgs
)

catkin_package(
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(gps_node
  src/node.cpp
)
target_link_libraries(gps_node
  ${catkin_LIBRARIES}
)

install(TARGETS gps_node
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
