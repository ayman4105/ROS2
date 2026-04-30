# Task 1: Publisher/Subscriber with Sum Computation

A simple ROS 2 (rclcpp) package demonstrating the basic Publisher/Subscriber pattern with three nodes that work together to compute the sum of two streams.

## 📋 Task Description

Create a ROS 2 package containing three nodes:

- **`publisher_a`** → publishes `std_msgs::msg::Int32` messages on `/topic_a`
- **`publisher_b`** → publishes `std_msgs::msg::Int32` messages on `/topic_b`
- **`sum_subscriber`** → subscribes to both topics, stores the latest values, and computes `sum = a + b`. When both values are available, it logs the result.
- **Bonus:** `sum_subscriber` also publishes the sum on `/topic_sum`.

## 🏗️ System Architecture

```
  publisher_a ──> /topic_a ──┐
                              ├──> sum_subscriber ──> /topic_sum
  publisher_b ──> /topic_b ──┘
```

## 🛠️ Requirements

- Ubuntu 24.04 (or compatible)
- ROS 2 Jazzy
- C++ compiler (GCC)
- `colcon` build tool

## 📁 Project Structure

```
task1_pub_subs/
├── src/
│   └── sum_pkg/
│       ├── CMakeLists.txt
│       ├── package.xml
│       └── src/
│           ├── publisher_a.cpp
│           ├── publisher_b.cpp
│           └── sum_subscriber.cpp
├── build/      (generated)
├── install/    (generated)
└── log/        (generated)
```

## 🚀 Build & Run

### 1. Create the workspace

```bash
mkdir -p ~/ITI/ROS/ROS2/Tasks/task1_pub_subs/src
cd ~/ITI/ROS/ROS2/Tasks/task1_pub_subs
```

### 2. Create the package

```bash
cd src
ros2 pkg create sum_pkg \
  --build-type ament_cmake \
  --dependencies rclcpp std_msgs
```

### 3. Add the source files

Place the three `.cpp` files inside `src/sum_pkg/src/`:

- `publisher_a.cpp`
- `publisher_b.cpp`
- `sum_subscriber.cpp`

### 4. Update `CMakeLists.txt`

Add the executables and install rules:

```cmake
add_executable(publisher_a    src/publisher_a.cpp)
add_executable(publisher_b    src/publisher_b.cpp)
add_executable(sum_subscriber src/sum_subscriber.cpp)

ament_target_dependencies(publisher_a    rclcpp std_msgs)
ament_target_dependencies(publisher_b    rclcpp std_msgs)
ament_target_dependencies(sum_subscriber rclcpp std_msgs)

install(TARGETS
  publisher_a
  publisher_b
  sum_subscriber
  DESTINATION lib/${PROJECT_NAME}
)
```

### 5. Build the workspace

```bash
cd ~/ITI/ROS/ROS2/Tasks/task1_pub_subs
colcon build --packages-select sum_pkg
```

### 6. Source the workspace

```bash
source install/setup.bash
```

> 💡 To make this permanent, add it to `~/.bashrc`:
> ```bash
> echo "source ~/ITI/ROS/ROS2/Tasks/task1_pub_subs/install/setup.bash" >> ~/.bashrc
> ```

## ▶️ Run the Nodes

Open **three separate terminals**:

**Terminal 1 — Publisher A:**
```bash
ros2 run sum_pkg publisher_a
```

**Terminal 2 — Publisher B:**
```bash
ros2 run sum_pkg publisher_b
```

**Terminal 3 — Sum Subscriber:**
```bash
ros2 run sum_pkg sum_subscriber
```

## ✅ Verify

In a fourth terminal, inspect the running system:

```bash
# List active nodes
ros2 node list

# List active topics
ros2 topic list

# Echo the sum topic
ros2 topic echo /topic_sum

# Inspect the subscriber node
ros2 node info /sum_subscriber

# Visualize the graph
ros2 run rqt_graph rqt_graph
```

## 📊 Expected Output

```
[INFO] [sum_subscriber]: a = 5  b = 100  sum = 105
[INFO] [sum_subscriber]: a = 6  b = 100  sum = 106
[INFO] [sum_subscriber]: a = 6  b = 110  sum = 116
...
```

## 🧹 Clean Build (Optional)

To clean and rebuild from scratch:

```bash
cd ~/ITI/ROS/ROS2/Tasks/task1_pub_subs
rm -rf build/ install/ log/
colcon build --packages-select sum_pkg
source install/setup.bash
```

## 📚 Concepts Covered

- ROS 2 Nodes (rclcpp::Node)
- Publishers (`create_publisher`)
- Subscribers (`create_subscription`)
- Wall Timers (`create_wall_timer`)
- Lambda callbacks
- Combined Publisher + Subscriber in one node
- Workspace and package structure
- `colcon` build system

