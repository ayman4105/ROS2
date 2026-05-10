import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    # Path to parameter file
    pkg_share = get_package_share_directory('simple_turtle_patrol')
    params_file = os.path.join(pkg_share, 'params', 'patrol_params.yaml')

    return LaunchDescription([
        #  Turtlesim Node
        Node(
            package='turtlesim',
            executable='turtlesim_node',
            name='turtlesim'
        ),
        #  Status Publisher
        Node(
            package='simple_turtle_patrol',
            executable='status_publisher',
            name='status_publisher',
            parameters=[params_file]
        ),
        # Patrol Controller
        Node(
            package='simple_turtle_patrol',
            executable='patrol_controller',
            name='patrol_controller',
            parameters=[params_file]
        )
    ])