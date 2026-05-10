import rclpy
from rclpy.node import Node
import random
from sensor_msgs.msg import Range


class ObstacleDetectionNode(Node):
    def __init__(self):
        super().__init__('obstacle_detection_node')

        # Create publisher ONCE
        self.publisher = self.create_publisher(Range, '/sensor/distance', 10)

        # Timer at 10 Hz (0.1 sec)
        self.timer = self.create_timer(0.1, self.detect_obstacles)

        self.get_logger().info('Sensor Node Started')

    def detect_obstacles(self):
        # Generate random distance
        distance = random.uniform(0.03, 5.0)

        msg = Range()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.header.frame_id = 'sensor_frame'
        msg.radiation_type = Range.INFRARED
        msg.field_of_view = 0.5
        msg.min_range = 0.03
        msg.max_range = 5.0
        msg.range = distance

        self.publisher.publish(msg)

        self.get_logger().info(f'Distance: {distance:.2f} m')


def main():
    rclpy.init()
    node = ObstacleDetectionNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()