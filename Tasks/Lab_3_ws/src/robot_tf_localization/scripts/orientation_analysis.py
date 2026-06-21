#!/usr/bin/env python3

import math

import rclpy
from rclpy.node import Node
from rclpy.qos import qos_profile_sensor_data

from sensor_msgs.msg import Imu
from nav_msgs.msg import Odometry


class OrientationAnalysis(Node):
    def __init__(self):
        super().__init__('orientation_analysis')

        self.imu_yaw = None
        self.odom_yaw = None

        self.create_subscription(
            Imu,
            '/imu/data',
            self.imu_callback,
            qos_profile_sensor_data
        )

        self.create_subscription(
            Odometry,
            '/odometry/local',
            self.odom_callback,
            qos_profile_sensor_data
        )

        self.timer = self.create_timer(1.0, self.print_yaw_comparison)

        self.get_logger().info('Orientation analysis node started')

    def imu_callback(self, msg):
        self.imu_yaw = self.quaternion_to_yaw(msg.orientation)

    def odom_callback(self, msg):
        self.odom_yaw = self.quaternion_to_yaw(msg.pose.pose.orientation)

    def quaternion_to_yaw(self, q):
        siny_cosp = 2.0 * (q.w * q.z + q.x * q.y)
        cosy_cosp = 1.0 - 2.0 * (q.y * q.y + q.z * q.z)

        yaw = math.atan2(siny_cosp, cosy_cosp)
        return yaw

    def normalize_angle(self, angle):
        while angle > math.pi:
            angle -= 2.0 * math.pi

        while angle < -math.pi:
            angle += 2.0 * math.pi

        return angle

    def print_yaw_comparison(self):
        if self.imu_yaw is None:
            self.get_logger().warn('Waiting for /imu/data...')
            return

        if self.odom_yaw is None:
            self.get_logger().warn('Waiting for /odometry/local...')
            return

        difference = self.normalize_angle(self.imu_yaw - self.odom_yaw)

        imu_yaw_deg = math.degrees(self.imu_yaw)
        odom_yaw_deg = math.degrees(self.odom_yaw)
        difference_deg = math.degrees(difference)

        self.get_logger().info(
            f'IMU yaw = {imu_yaw_deg:.2f} deg | '
            f'Local odom yaw = {odom_yaw_deg:.2f} deg | '
            f'Difference = {difference_deg:.2f} deg'
        )


def main(args=None):
    rclpy.init(args=args)

    node = OrientationAnalysis()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass

    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()

    