#include"rclcpp/rclcpp.hpp"
#include<std_msgs/msg/int32.hpp>


class SumSubscriber : public rclcpp::Node{

private:

int32_t sum = 0;
int last_a_ = 0;
int last_b_ = 0 ; 
bool has_a_  = false;
bool has_b_ = false ; 

rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr sum_publisher_;
rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscriber_a_;
rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscriber_b_;



public:

SumSubscriber() : Node("sum_subscriber"){

    sum_publisher_ = this->create_publisher<std_msgs::msg::Int32>(
        "topic_sum",
        10
    );

    subscriber_a_ = this->create_subscription<std_msgs::msg::Int32> (
        "topic_a",
        10,
        [this](const std_msgs::msg::Int32::SharedPtr msg){
           last_a_ = msg->data;
           has_a_ = true;
           try_compute_sum();
        }

    );

    subscriber_b_ = this->create_subscription<std_msgs::msg::Int32> (
        "topic_b",
        10,
        [this](const std_msgs::msg::Int32::SharedPtr msg){
           last_b_ = msg->data;
           has_b_ = true;
           try_compute_sum();
        }

    );
}

void try_compute_sum() {

    if (has_a_ && has_b_){
        sum = last_a_ + last_b_;
        RCLCPP_INFO(this->get_logger(), "a = %d  b = %d sum = %d" , last_a_ , last_b_ , sum);
    }

    auto msg = std_msgs::msg::Int32();
    msg.data = sum;
    sum_publisher_->publish(msg);
    RCLCPP_INFO(this->get_logger() , "sum in topic = %d" , msg.data);

}

};


int main(int argc , char* argv[]){

    rclcpp::init(argc , argv);
    rclcpp::spin(std::make_shared<SumSubscriber>());
    rclcpp::shutdown();

    return 0;
}