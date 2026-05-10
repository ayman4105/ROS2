#include"rclcpp/rclcpp.hpp"
#include<sensor_msgs/msg/temperature.hpp>
#include<fstream>
#include<chrono>

class CPU_thermals : public rclcpp::Node{
    private:
        rclcpp::Publisher<sensor_msgs::msg::Temperature>::SharedPtr publisher_;
        rclcpp::TimerBase::SharedPtr timer_;
        std::string file_path_;
    public:
        CPU_thermals() : Node("cpu_thermals"){
            publisher_ = this->create_publisher<sensor_msgs::msg::Temperature>("cpu_temp", 10);
            timer_ = this->create_wall_timer(std::chrono::seconds(1), std::bind(&CPU_thermals::timer_callback, this));
            file_path_ = "/sys/class/thermal/thermal_zone0/temp";
        }
        void timer_callback(){
            std::ifstream file(file_path_);
            if(!file.is_open()){
                RCLCPP_ERROR(this->get_logger(), "Failed to open file: %s", file_path_.c_str());
                return;
            }

        // Read the temperature in millidegrees Celsius all the time and convert it to degrees Celsius
            int temp_milli;
            file >> temp_milli;
            double temp_celsius = temp_milli / 1000.0;
            file.close();

            auto message = sensor_msgs::msg::Temperature();
            message.temperature = temp_celsius;
            message.variance = 0.0;
            message.header.stamp = this->get_clock()->now();
            publisher_->publish(message);


            RCLCPP_INFO(this->get_logger(), "Published CPU Temperature: %.2f °C", temp_celsius);
        
        }
};


int main(int argc, char * argv[]){
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CPU_thermals>());
    rclcpp::shutdown();
    return 0;
}