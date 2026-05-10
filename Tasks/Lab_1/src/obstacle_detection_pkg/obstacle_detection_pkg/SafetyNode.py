import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Range
from std_msgs.msg import Bool


class SafetyNode(Node):
    def __init__(self):
        super().__init__('safety_node')

        self.subscription = self.create_subscription(
            Range,
            '/sensor/distance',
            self.callback,
            10
        )

        self.publisher = self.create_publisher(Bool, '/cmd/stop', 10)

        self.get_logger().info('Safety Node Started')

    def callback(self, msg):
        stop_msg = Bool()

        if msg.range < 2.0:
            stop_msg.data = True
        else:
            stop_msg.data = False

        self.publisher.publish(stop_msg)

        self.get_logger().info(f'Distance: {msg.range:.2f} -> Stop: {stop_msg.data}')


def main():
    rclpy.init()
    node = SafetyNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()