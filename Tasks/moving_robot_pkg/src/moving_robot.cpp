#include <cstdio>
#include<rclcpp/rclcpp.hpp>
#include<nav_msgs/msg/odometry.hpp>
// #include<chrono.hpp>



class moving_robot : public rclcpp::Node
{
private:
  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr publisher_;

public:

  moving_robot() : Node("moving_robot")
  {
    publisher_ = this->create_publisher<nav_msgs::msg::Odometry>("odom", 10);
    auto timer_callback = [this]() -> void {
      auto message = nav_msgs::msg::Odometry();
      message.header.stamp = this->get_clock()->now();
      message.header.frame_id = "odom";
      message.child_frame_id = "base_link";
      message.pose.pose.position.x += 0.1;
      message.pose.pose.orientation.w = 1.0;
      message.pose.pose.position.y = 0.0;
      message.pose.pose.position.z = 0.0;
      publisher_->publish(message);
    };
    auto timer = this->create_wall_timer(std::chrono::seconds(1), timer_callback);
  }


};




int main(int argc, char ** argv)
{
  
  return 0;
}
