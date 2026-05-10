# launch/obstacle.lanch.py
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='obstacle_detection_pkg',
            executable='obsDetect',
            name='obsDetect node',
            output='screen',
        ),
        Node(
            package='obstacle_detection_pkg',
            executable='SafetyNode',
            name='SafetyNode node',
            output='screen',
        ),
    ])