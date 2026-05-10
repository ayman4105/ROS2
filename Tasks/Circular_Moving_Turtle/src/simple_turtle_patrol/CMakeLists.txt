cmake_minimum_required(VERSION 3.8)
project(simple_turtle_patrol)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#  Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(turtlesim REQUIRED)
find_package(std_srvs REQUIRED)
find_package(rosidl_default_generators REQUIRED)

#  Generate Custom Interfaces (Messages) - MUST be before executables
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/RobotStatus.msg"
  DEPENDENCIES geometry_msgs
)

# Add Executable
add_executable(patrol_controller src/patrol_controller.cpp)
ament_target_dependencies(patrol_controller rclcpp geometry_msgs std_srvs)

add_executable(status_publisher src/status_publisher.cpp)
ament_target_dependencies(status_publisher rclcpp turtlesim)

#  Link Executable with the Custom Message
# This allows patrol_controller to use RobotStatus.msg
rosidl_target_interfaces(patrol_controller
  ${PROJECT_NAME} "rosidl_typesupport_cpp"
)

rosidl_target_interfaces(status_publisher
  ${PROJECT_NAME} "rosidl_typesupport_cpp"
)

# Install
install(TARGETS
  patrol_controller
  status_publisher
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch params
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()