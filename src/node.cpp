#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>
#include <arpa/inet.h>
#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
  uint16_t time;
  double latitude;
  double longitude;
  float altitude;
} Packet;
#pragma pack(pop)

static inline bool openSocket(const std::string &ip_addr,uint16_t port, int *fd_ptr, sockaddr_in *sock_ptr)
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);  // Create a new TCP socket
  if (fd != -1)  // If new socket created
  {
    memset(sock_ptr, 0, sizeof(sockaddr_in));
    sock_ptr->sin_family = AF_INET;
    sock_ptr->sin_port = htons(port);
    sock_ptr->sin_addr.s_addr = htonl(INADDR_ANY);
    inet_aton(ip_addr.c_str(),&sock_ptr->sin_addr);

    if (connect(fd, (sockaddr*)sock_ptr, sizeof(sockaddr)) == 0)
    {
      *fd_ptr = fd;
      return true;
    }
  }
  return false;
}

static inline int readSocket(int fd, void *data, int size)
{
  size_t total = 0;
  size_t bytes_left = size;
  ssize_t n;
  if (fd >= 0)
  {
    memset(data,0,sizeof(Packet));
    while(total < size)
    {
        n = recv(fd, data+total, bytes_left,0);
        if(n == -1)
        {
            size_t total_recv = total;
            return -1;
        }
        total = total + n;
        bytes_left = bytes_left - n;

    }
    return 0;
  }
  return -1;
}

static inline void handlePacket(const Packet *packet, ros::Publisher &pub_fix)
{
    ros::Time stamp = ros::Time::now();

    sensor_msgs::NavSatFix msg_fix;
    msg_fix.header.stamp = stamp;
    msg_fix.header.frame_id = 0;            // No Frame, 1 = global frame
    msg_fix.latitude = packet->latitude;
    msg_fix.longitude = packet->longitude;
    msg_fix.altitude = packet->altitude;
    //msg_fix.status.status = fix_status;
    //msg_fix.status.service = sensor_msgs::NavSatStatus::SERVICE_GPS;
    //msg_fix.position_covariance_type = position_covariance_type;

    pub_fix.publish(msg_fix);
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "gps_node");
  ros::NodeHandle node;                 // A public node handle
  ros::NodeHandle priv_nh("~");

  int port = 3467; 
  priv_nh.getParam("port",port);        // get from param server

  std::string ip_addr ="";
  priv_nh.getParam("ip_addr",ip_addr);  // get from param server

  int fd;
  sockaddr_in sock;
  if (openSocket(ip_addr,port, &fd, &sock))
  {
    ros::Publisher pub_fix = node.advertise<sensor_msgs::NavSatFix>("gps/fix", 2);
    Packet packet;
    sockaddr source;

    while (ros::ok()) // Loop until shutdown
    {
      if (readSocket(fd, &packet, sizeof(packet)) == 0)
      {
          handlePacket(&packet, pub_fix);
      }
      ros::spinOnce(); // handle callbacks
    }
    close(fd); // Close socket
  }
  else
  {
    ROS_FATAL("Failed to open socket");
    ros::WallDuration(1.0).sleep();
  }
  return 0;
}
