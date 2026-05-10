#include <cstdio>
#include<rclcpp/rclcpp.hpp>
#include<geometry_msgs/msg/twist.hpp>


class velocity : public rclcpp::Node
{
private:
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr subscription_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
  double limit_linear_x = 1.0;
  double limit_angular_z = 1.5;

public:

  velocity() : Node("velocity")
  {
    subscription_ = this->create_subscription<geometry_msgs::msg::Twist>(
      "cmd_vel", 10, std::bind(&velocity::topic_callback, this, std::placeholders::_1));

    publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel_limited", 10);

    


  }

  void topic_callback(const geometry_msgs::msg::Twist::SharedPtr msg) const
  {
    RCLCPP_INFO(this->get_logger(), "I heard: linear x: '%f', angular z: '%f'", msg->linear.x, msg->angular.z);

    if(msg->linear.x > limit_linear_x)
    {
      RCLCPP_WARN(this->get_logger(), "Linear speed exceeds 1.0 m/s");
      msg->linear.x = limit_linear_x;
    }
    if(msg->angular.z > limit_angular_z)
    {
      RCLCPP_WARN(this->get_logger(), "Angular velocity exceeds 1.5 rad/s");
      msg->angular.z = limit_angular_z;
    }
    publisher_->publish(*msg);
  }

};




int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<velocity>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
