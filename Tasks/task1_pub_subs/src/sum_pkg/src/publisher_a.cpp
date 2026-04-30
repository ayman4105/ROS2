#include<rclcpp/rclcpp.hpp>
#include<std_msgs/msg/int32.hpp>
#include<chrono>



class PublisherA : public rclcpp::Node
{

private:

    rclcpp::Publisher<std_msgs::msg::Int32> :: SharedPtr publisher_; 
    rclcpp::TimerBase::SharedPtr timer_ ;
    int32_t counter_a = 0;          



public:
    PublisherA() : Node("publisher_a")
    {
        publisher_ = this->create_publisher<std_msgs::msg::Int32>("topic_a", 10);
        timer_ = this->create_wall_timer(std::chrono::seconds(1),
                 [this](){
                    auto msg = std_msgs::msg::Int32();
                    msg.data = counter_a ; 
                    publisher_->publish(msg);
                    RCLCPP_INFO(this->get_logger(), "counter_a : %i", msg.data);

                    counter_a +=1;
                    if(counter_a == 1000){
                        counter_a = 0;
                    }

                }
            );

    }

};


int main(int argc , char *argv[]){

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<PublisherA>());
    rclcpp::shutdown();

    return 0;
}

