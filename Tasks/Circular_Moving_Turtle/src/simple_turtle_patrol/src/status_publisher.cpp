#include "rclcpp/rclcpp.hpp"
#include "turtlesim/msg/pose.hpp"
#include "simple_turtle_patrol/msg/robot_status.hpp"
#include <cmath>

class StatusPublisher : public rclcpp::Node {
public:
    StatusPublisher() : Node("status_publisher"), last_theta_(0.0), total_rotation_(0.0) {
        // Declare parameter for publishing rate (Hz)
        this->declare_parameter("status_rate", 5.0);
        double rate = this->get_parameter("status_rate").as_double();

        //  Subscriber to get the turtle's pose
        pose_sub_ = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose", 10, std::bind(&StatusPublisher::pose_callback, this, std::placeholders::_1));

        // Publisher for our custom RobotStatus message
        status_pub_ = this->create_publisher<simple_turtle_patrol::msg::RobotStatus>("/robot/status", 10);

        // Timer to publish status at the specified rate
        auto interval = std::chrono::milliseconds(static_cast<int>(1000.0 / rate));
        timer_ = this->create_wall_timer(interval, std::bind(&StatusPublisher::publish_status, this));

        RCLCPP_INFO(this->get_logger(), "Status Publisher has been started.");
    }

private:
    void pose_callback(const turtlesim::msg::Pose::SharedPtr msg) {
        current_pose_ = *msg;

        // Calculate lap count logic:
        // Detect how much the angle changed since last callback
        double delta_theta = current_pose_.theta - last_theta_;

        // Correct for angles jumping between -PI and PI
        if (delta_theta > M_PI) delta_theta -= 2.0 * M_PI;
        if (delta_theta < -M_PI) delta_theta += 2.0 * M_PI;

        total_rotation_ += delta_theta;
        last_theta_ = current_pose_.theta;

        // 2*PI radians = 1 full circle
        lap_count_ = static_cast<int>(std::abs(total_rotation_) / (2.0 * M_PI));
    }

    void publish_status() {
        auto status_msg = simple_turtle_patrol::msg::RobotStatus();

        // Fill data into the custom message
        status_msg.pose.x = current_pose_.x;
        status_msg.pose.y = current_pose_.y;
        status_msg.pose.theta = current_pose_.theta;

        // If linear velocity is near zero, we consider it "stopped"
        status_msg.state = (std::abs(current_pose_.linear_velocity) > 0.01) ? "running" : "stopped";

        // Dummy temperature calculation: base 25°C + speed factor
        status_msg.temperature = 25.0f + (std::abs(current_pose_.linear_velocity) * 5.0f);
        
        status_msg.lap_count = lap_count_;

        status_pub_->publish(status_msg);
    }

    // ROS 2 Objects
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_sub_;
    rclcpp::Publisher<simple_turtle_patrol::msg::RobotStatus>::SharedPtr status_pub_;
    rclcpp::TimerBase::SharedPtr timer_;

    // Tracking variables
    turtlesim::msg::Pose current_pose_;
    double last_theta_;
    double total_rotation_;
    int lap_count_ = 0;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<StatusPublisher>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}