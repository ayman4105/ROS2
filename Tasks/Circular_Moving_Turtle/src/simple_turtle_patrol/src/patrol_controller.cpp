#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_srvs/srv/empty.hpp"

class PatrolController : public rclcpp::Node {
public:
    PatrolController() : Node("patrol_controller") {
        // 1. Declare and initialize parameters
        this->declare_parameter("linear_speed", 1.5);
        this->declare_parameter("angular_speed", 1.0);

        // 2. Create publisher for turtle velocity
        cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

        // 3. Create services to stop and continue movement
        stop_service_ = this->create_service<std_srvs::srv::Empty>(
            "/stop", std::bind(&PatrolController::stop_callback, this, std::placeholders::_1, std::placeholders::_2));
        
        continue_service_ = this->create_service<std_srvs::srv::Empty>(
            "/continue", std::bind(&PatrolController::continue_callback, this, std::placeholders::_1, std::placeholders::_2));

        // 4. Create a timer to control the movement loop at 10Hz
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(100), std::bind(&PatrolController::move_turtle, this));

        RCLCPP_INFO(this->get_logger(), "Patrol Controller has been started.");
    }

private:
    // Callback to stop the turtle
    void stop_callback(const std::shared_ptr<std_srvs::srv::Empty::Request>,
                       std::shared_ptr<std_srvs::srv::Empty::Response>) {
        is_running_ = false;
        RCLCPP_INFO(this->get_logger(), "Stop service called. Turtle stopped.");
    }

    // Callback to resume movement
    void continue_callback(const std::shared_ptr<std_srvs::srv::Empty::Request>,
                           std::shared_ptr<std_srvs::srv::Empty::Response>) {
        is_running_ = true;
        RCLCPP_INFO(this->get_logger(), "Continue service called. Turtle resuming circular path.");
    }

    // Function to publish velocity commands
    void move_turtle() {
        auto twist_msg = geometry_msgs::msg::Twist();

        if (is_running_) {
            // Get current parameter values in case they were updated at runtime
            twist_msg.linear.x = this->get_parameter("linear_speed").as_double();
            twist_msg.angular.z = this->get_parameter("angular_speed").as_double();
        } else {
            // Stop movement
            twist_msg.linear.x = 0.0;
            twist_msg.angular.z = 0.0;
        }

        cmd_vel_pub_->publish(twist_msg);
    }

    // Variables
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Service<std_srvs::srv::Empty>::SharedPtr stop_service_;
    rclcpp::Service<std_srvs::srv::Empty>::SharedPtr continue_service_;
    bool is_running_ = true; // Initial state: running
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PatrolController>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}