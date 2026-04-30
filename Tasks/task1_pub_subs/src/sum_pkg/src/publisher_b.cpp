#include<rclcpp/rclcpp.hpp>
#include<std_msgs/msg/int32.hpp>
#include<chrono>



class PublisherB : public rclcpp::Node
{

private:

    rclcpp::Publisher<std_msgs::msg::Int32> :: SharedPtr publisher_; 
    rclcpp::TimerBase::SharedPtr timer_ ;
    int32_t counter_b = 0;          



public:
    PublisherB() : Node("publisher_b")
    {
        publisher_ = this->create_publisher<std_msgs::msg::Int32>("topic_b", 10);
        timer_ = this->create_wall_timer(std::chrono::seconds(1),
                 [this](){
                    auto msg = std_msgs::msg::Int32();
                    msg.data = counter_b ; 
                    publisher_->publish(msg);
                    RCLCPP_INFO(this->get_logger(), "counter_b : %i", msg.data);

                    counter_b +=1;
                    if(counter_b == 1000){
                        counter_b = 0;
                    }

                }
            );

    }

};


int main(int argc , char *argv[]){

    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<PublisherB>());
    rclcpp::shutdown();

    return 0;
}

