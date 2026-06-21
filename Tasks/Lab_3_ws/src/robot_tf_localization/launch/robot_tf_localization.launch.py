from launch import LaunchDescription
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import PathJoinSubstitution
import os
from ament_index_python.packages import get_package_share_directory

pkg_dir = get_package_share_directory('robot_tf_localization')

rviz_config = os.path.join( pkg_dir, 'config', 'robot_tf.rviz' )


def static_tf(name, x, y, z, yaw, parent, child):
    return Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name=name,
        arguments=[
            '--x', str(x),
            '--y', str(y),
            '--z', str(z),
            '--roll', '0.0',
            '--pitch', '0.0',
            '--yaw', str(yaw),
            '--frame-id', parent,
            '--child-frame-id', child
        ]
    )


def generate_launch_description():
    pkg_share = FindPackageShare('robot_tf_localization')

    ekf_config = PathJoinSubstitution([
        pkg_share,
        'config',
        'ekf.yaml'
    ])

    return LaunchDescription([

        static_tf(
            'base_footprint_to_base_link',
            0.0, 0.0, 0.05,
            0.0,
            'base_footprint',
            'base_link'
        ),

        static_tf(
            'base_link_to_imu_link',
            0.50, -0.10, 0.10,
            0.0,
            'base_link',
            'imu_link'
        ),

        static_tf(
            'base_link_to_gps_link',
            0.20, 0.0, 0.25,
            0.0,
            'base_link',
            'gps_link'
        ),

        static_tf(
            'base_link_to_ultrasonic1_link',
            0.50, 0.15, 0.10,
            0.785398,
            'base_link',
            'ultrasonic1_link'
        ),

        static_tf(
            'base_link_to_ultrasonic2_link',
            0.50, 0.0, 0.10,
            0.0,
            'base_link',
            'ultrasonic2_link'
        ),

        static_tf(
            'base_link_to_ultrasonic3_link',
            0.50, -0.15, 0.10,
            -0.785398,
            'base_link',
            'ultrasonic3_link'
        ),

        static_tf(
            'base_link_to_ultrasonic4_link',
            -0.10, 0.15, 0.10,
            2.35619,
            'base_link',
            'ultrasonic4_link'
        ),

        static_tf(
            'base_link_to_ultrasonic5_link',
            -0.10, 0.0, 0.10,
            3.14159,
            'base_link',
            'ultrasonic5_link'
        ),

        static_tf(
            'base_link_to_ultrasonic6_link',
            -0.10, -0.15, 0.10,
            -2.35619,
            'base_link',
            'ultrasonic6_link'
        ),

        Node(
            package='robot_localization',
            executable='ekf_node',
            name='ekf_filter_node',
            output='screen',
            parameters=[ekf_config],
            remappings=[
                ('odometry/filtered', '/odometry/local')
            ]
        ),

        Node(
            package='robot_tf_localization',
            executable='orientation_analysis.py',
            name='orientation_analysis',
            output='screen'
        ),

        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            output='screen',
            arguments=['-d', rviz_config]
        )
    ])

