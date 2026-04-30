# 📘 ROS 2 Complete Reference Guide

> A comprehensive reference covering ROS 2 fundamentals, with focus on **Topics** and **Messages**, built from Task 1 (Publisher/Subscriber with Sum Computation).

---

## 📑 Table of Contents

1. [What is ROS 2?](#1-what-is-ros-2)
2. [ROS 1 vs ROS 2](#2-ros-1-vs-ros-2)
3. [DDS Middleware](#3-dds-middleware)
4. [Core Concepts Overview](#4-core-concepts-overview)
5. [Nodes](#5-nodes)
6. [Topics — Deep Dive](#6-topics--deep-dive)
7. [Messages — Deep Dive](#7-messages--deep-dive)
8. [Publishers](#8-publishers)
9. [Subscribers](#9-subscribers)
10. [Timers](#10-timers)
11. [Workspace & Package Structure](#11-workspace--package-structure)
12. [CMakeLists.txt Explained](#12-cmakeliststxt-explained)
13. [package.xml Explained](#13-packagexml-explained)
14. [Task 1: Step-by-Step](#14-task-1-step-by-step)
15. [Useful Commands](#15-useful-commands)
16. [Common Pitfalls](#16-common-pitfalls)

---

## 1. What is ROS 2?

**ROS 2** (Robot Operating System 2) is **NOT an operating system** — it's a **framework / middleware** that runs on top of an OS (Linux, Windows, macOS).

```
┌─────────────────────────────────────┐
│      Your Robot Application         │
├─────────────────────────────────────┤
│              ROS 2                  │
│ (Communication + Tools + Libraries) │
├─────────────────────────────────────┤
│         DDS (Middleware)            │
├─────────────────────────────────────┤
│      Linux / Windows / macOS        │
└─────────────────────────────────────┘
```

### What ROS 2 Provides:
- **Communication** between robot components (sensors, motors, cameras...)
- **Tools** like `ros2 run`, `ros2 topic`, `rviz2`, `gazebo`
- **Libraries** for C++ (`rclcpp`) and Python (`rclpy`)

---

## 2. ROS 1 vs ROS 2

```
ROS 1                          ROS 2
─────                          ─────
Master node (roscore)          NO master (DDS-based)
Single point of failure        Distributed, fault-tolerant
TCPROS / UDPROS                DDS (industry standard)
Not real-time                  Real-time support
Linux mostly                   Linux + Windows + macOS
Python 2/3                     Python 3 only
```

The biggest change: **No more `roscore`!** Nodes auto-discover each other via DDS.

---

## 3. DDS Middleware

**DDS** = **Data Distribution Service** — an industry-standard communication protocol used in:
- ✈️ Aerospace (NASA, Lockheed Martin)
- 🚂 Trains (Siemens)
- 🏥 Medical devices
- ⚓ Naval systems
- 🤖 Robotics (ROS 2)

### Why DDS?
1. **Auto-discovery** — nodes find each other via multicast UDP
2. **QoS (Quality of Service)** — fine control over reliability, durability, deadlines
3. **Real-time** — predictable microsecond latency
4. **Security** — DDS-Security for authentication and encryption
5. **Scalability** — thousands of nodes
6. **Interoperability** — different DDS vendors interoperate

### DDS Implementations in ROS 2:
- **Fast DDS** (eProsima) — default in Humble
- **Cyclone DDS** (Eclipse) — default in Jazzy
- **RTI Connext DDS** — commercial

```bash
# Switch DDS implementation
export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp
export RMW_IMPLEMENTATION=rmw_fastrtps_cpp
```

---

## 4. Core Concepts Overview

```
┌───────────────────────────────────────────────────────┐
│              ROS 2 Communication                      │
├───────────────────────────────────────────────────────┤
│                                                       │
│  1. NODES      → Workers (executables)                │
│                                                       │
│  2. TOPICS     → Pub/Sub (continuous data streams)    │
│                                                       │
│  3. SERVICES   → Req/Res (synchronous calls)          │
│                                                       │
│  4. ACTIONS    → Goal/Feedback/Result (long tasks)    │
│                                                       │
└───────────────────────────────────────────────────────┘
```

---

## 5. Nodes

A **Node** is a single program performing one specific task.

```
┌────────────────────────────────────────────────────────┐
│              A ROS 2 Robot System                      │
│                                                        │
│   ┌──────────┐    ┌──────────┐    ┌──────────┐         │
│   │  Node 1  │    │  Node 2  │    │  Node 3  │         │
│   │  Lidar   │    │  Motor   │    │  Camera  │         │
│   │  Driver  │    │ Control  │    │  Driver  │         │
│   └────┬─────┘    └────┬─────┘    └────┬─────┘         │
│        │               │               │               │
│        └───────────────┴───────────────┘               │
│           ROS 2 Communication Layer                    │
└────────────────────────────────────────────────────────┘
```

### Why split a system into nodes?
- ✅ **Modularity** — edit one node without affecting others
- ✅ **Reusability** — use the same lidar node in different robots
- ✅ **Fault tolerance** — if one crashes, others keep running
- ✅ **Mixed languages** — C++ and Python nodes talk seamlessly
- ✅ **Distribution** — run nodes on different machines

### Node Lifecycle (5 stages):

```
   ┌──────────────┐
   │ rclcpp::init │   1. Initialize ROS 2
   └──────┬───────┘
          ▼
   ┌──────────────┐
   │ Create Node  │   2. Instantiate node
   └──────┬───────┘
          ▼
   ┌──────────────┐
   │ Add pubs/    │   3. Setup pubs, subs,
   │ subs/timers  │      timers, services
   └──────┬───────┘
          ▼
   ┌──────────────┐
   │ rclcpp::spin │   4. Keep alive,
   │              │      process callbacks
   └──────┬───────┘
          ▼
   ┌──────────────┐
   │ shutdown()   │   5. Cleanup
   └──────────────┘
```

---

## 6. Topics — Deep Dive

A **Topic** is a **named bus for messages**. It implements the **Publish/Subscribe** pattern.

### Visual Model:

```
                    /topic_name
                  ════════════════
   Publisher ────►│              │────► Subscriber A
                  │              │────► Subscriber B
                  │  (named bus) │────► Subscriber C
                  ════════════════
```

### Many-to-Many Communication:

[Topics support one-to-one, one-to-many, many-to-one, or many-to-many communication patterns](https://docs.ros.org/en/foxy/Tutorials/Beginner-CLI-Tools/Understanding-ROS2-Topics/Understanding-ROS2-Topics.html).

```
Multiple Publishers, Multiple Subscribers:

   Pub 1 ─┐               ┌─► Sub 1
          │               │
   Pub 2 ─┼─► /chatter ───┼─► Sub 2
          │               │
   Pub 3 ─┘               └─► Sub 3
```

### Key Properties:
| Property | Description |
|----------|-------------|
| **Asynchronous** | Publisher doesn't wait for response |
| **Decoupled** | Pub doesn't know subs, sub doesn't know pubs |
| **Many-to-Many** | Any number of pubs/subs on a topic |
| **Typed** | Each topic has ONE message type |

### Topic Naming Rules:
- Must start with `/` (auto-prepended if missing)
- Use `lowercase_with_underscores`
- Examples: `/topic_a`, `/scan`, `/cmd_vel`, `/camera/image_raw`

---

## 7. Messages — Deep Dive

A **Message** is a **typed data structure** sent over a topic.

```
┌──────────────────────────────────────┐
│      Message = Data Structure        │
│                                      │
│  std_msgs/msg/Int32:                 │
│  ┌─────────────────────┐             │
│  │  int32 data         │  ← field    │
│  └─────────────────────┘             │
│                                      │
│  geometry_msgs/msg/Twist:            │
│  ┌─────────────────────┐             │
│  │  Vector3 linear     │  ← field 1  │
│  │  Vector3 angular    │  ← field 2  │
│  └─────────────────────┘             │
└──────────────────────────────────────┘
```

[Messages are described in `.msg` files and consist of two parts: fields and constants. The fields value represents the data sent within the message](https://www.mathworks.com/help/ros/gs/ros2-topics.html).

### Common Message Types:

```
Package          Type            Fields
───────          ────            ──────
std_msgs         Int32           int32 data
std_msgs         Float64         float64 data
std_msgs         String          string data
std_msgs         Bool            bool data

geometry_msgs    Twist           Vector3 linear, Vector3 angular
geometry_msgs    Point           float64 x, y, z
geometry_msgs    Pose            Point position, Quaternion orientation

sensor_msgs      LaserScan       float32[] ranges, ...
sensor_msgs      Image           uint8[] data, uint32 width, height, ...
sensor_msgs      Imu             Quaternion orientation, ...
```

### Inspecting Messages:

```bash
# View message structure
ros2 interface show std_msgs/msg/Int32
# Output:
#   int32 data

ros2 interface show geometry_msgs/msg/Twist
# Output:
#   Vector3 linear
#   Vector3 angular
```

### Message Naming → Include Path Rules:

```
Type in C++ code              Include path
──────────────────            ─────────────
std_msgs::msg::Int32     →    std_msgs/msg/int32.hpp
std_msgs::msg::String    →    std_msgs/msg/string.hpp
geometry_msgs::msg::Twist →   geometry_msgs/msg/twist.hpp

Rule:
  - Replace :: with /
  - Make everything lowercase
  - Add .hpp extension (NOT .h, that's the C version!)
```

### How Topics + Messages Work Together:

```
┌────────────────────────────────────────────────────────┐
│                                                        │
│   Topic = Channel name (a string label)                │
│   Message = The actual data flowing through            │
│                                                        │
│   /battery_level  ←─── topic name                      │
│   std_msgs/Int32  ←─── message type                    │
│   { data: 87 }    ←─── message instance                │
│                                                        │
│   The publisher and subscriber MUST agree on:          │
│     1. Topic name                                      │
│     2. Message type                                    │
│   Otherwise → no communication                         │
│                                                        │
└────────────────────────────────────────────────────────┘
```

---

## 8. Publishers

A **Publisher** sends messages to a topic on a schedule (driven by a timer).

```
PUBLISHER FLOW:
═══════════════
   ┌─────────┐
   │  Timer  │ fires every X seconds
   └────┬────┘
        │
        ▼
   ┌─────────┐
   │ Build   │ create message object
   │   msg   │ fill data fields
   └────┬────┘
        │
        ▼
   ┌─────────┐
   │ Publish │ publisher_->publish(msg)
   └────┬────┘
        │
        ▼
   ┌─────────┐
   │  Topic  │ delivered via DDS
   └─────────┘
```

### Code Pattern (C++):

```cpp
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

using namespace std::chrono_literals;

class MyPublisher : public rclcpp::Node {
public:
    MyPublisher() : Node("my_publisher") {
        // Create publisher
        publisher_ = this->create_publisher<std_msgs::msg::Int32>(
            "topic_name",   // Topic name
            10              // QoS depth (queue size)
        );

        // Create timer that calls callback every 1s
        timer_ = this->create_wall_timer(1s, [this]() {
            auto msg = std_msgs::msg::Int32();
            msg.data = counter_++;
            publisher_->publish(msg);
            RCLCPP_INFO(this->get_logger(), "Published: %d", msg.data);
        });
    }

private:
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    int counter_ = 0;
};
```

### `create_publisher<T>(topic, qos)` Parameters:
| Parameter | Purpose |
|-----------|---------|
| `<T>` | Message type (template) |
| `topic` | Topic name (string) |
| `qos` | Queue depth (int) — buffer size if subscriber is slow |

---

## 9. Subscribers

A **Subscriber** reacts to incoming messages (driven by callbacks).

```
SUBSCRIBER FLOW:
════════════════
   ┌─────────┐
   │  Topic  │ message arrives via DDS
   └────┬────┘
        │
        ▼
   ┌─────────┐
   │Callback │ automatically invoked
   │ fires   │
   └────┬────┘
        │
        ▼
   ┌─────────┐
   │ Process │ read msg->data
   │   msg   │ do something with it
   └─────────┘
```

### Code Pattern (C++):

```cpp
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

class MySubscriber : public rclcpp::Node {
public:
    MySubscriber() : Node("my_subscriber") {
        subscriber_ = this->create_subscription<std_msgs::msg::Int32>(
            "topic_name", 10,
            [this](const std_msgs::msg::Int32::SharedPtr msg) {
                // ⚠️ msg is a SharedPtr → use ->
                RCLCPP_INFO(this->get_logger(),
                            "Received: %d", msg->data);
            }
        );
    }

private:
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr subscriber_;
};
```

### Publisher vs Subscriber:

```
┌────────────────────┬──────────────────┬──────────────────┐
│                    │   PUBLISHER      │   SUBSCRIBER     │
├────────────────────┼──────────────────┼──────────────────┤
│ Direction          │ Sends            │ Receives         │
│ Pattern            │ Active           │ Reactive         │
│ Trigger            │ Timer            │ Incoming msg     │
│ Code uses          │ create_publisher │ create_subscript.│
│ Needs callback?    │ Yes (timer cb)   │ Yes (msg cb)     │
│ Member access in   │ msg.data         │ msg->data        │
│   message          │ (object)         │ (SharedPtr)      │
└────────────────────┴──────────────────┴──────────────────┘
```

### ⚠️ Critical Difference: `.` vs `->`

```cpp
// In Publisher (you build the message):
auto msg = std_msgs::msg::Int32();   // OBJECT
msg.data = 5;                        // Use . (dot)

// In Subscriber callback (msg is a pointer):
void cb(const std_msgs::msg::Int32::SharedPtr msg) {
    int value = msg->data;           // Use -> (arrow)
}
```

---

## 10. Timers

A **Timer** triggers a callback periodically.

### Two types:

```
create_wall_timer:
──────────────────
  Uses REAL-WORLD time
  Independent of simulation
  Best for real hardware

create_timer:
─────────────
  Uses ROS time (/clock)
  Pauses with simulation
  Best for Gazebo simulations
```

### Wall Timer Example:

```cpp
timer_ = this->create_wall_timer(
    1s,                              // period
    [this]() {                       // callback
        // your code
    }
);
```

### Period Formats:

```cpp
500ms                               // 500 milliseconds
1s                                  // 1 second
2s                                  // 2 seconds
std::chrono::milliseconds(500)      // long form
std::chrono::seconds(1)             // long form
```

> 💡 To use shortcuts (`1s`, `500ms`), add at top:
> ```cpp
> using namespace std::chrono_literals;
> ```

---

## 11. Workspace & Package Structure

```
my_workspace/                       ← workspace root
├── src/                           ← YOUR code (you create this)
│   └── my_pkg/                   ← package
│       ├── CMakeLists.txt        ← build config
│       ├── package.xml           ← metadata + deps
│       ├── include/my_pkg/       ← C++ headers
│       └── src/                  ← .cpp files
│           ├── publisher.cpp
│           └── subscriber.cpp
├── build/                        ← colcon-generated
├── install/                      ← colcon-generated
│   └── my_pkg/
│       ├── lib/my_pkg/           ← EXECUTABLES live here
│       │   ├── publisher
│       │   └── subscriber
│       └── share/my_pkg/         ← non-code resources
└── log/                          ← colcon-generated
```

### Underlay vs Overlay:

```
source /opt/ros/jazzy/setup.bash         ← UNDERLAY (base ROS 2)
                ↓
source ~/ws1/install/setup.bash          ← OVERLAY 1
                ↓
source ~/ws2/install/setup.bash          ← OVERLAY 2 (overrides if conflict)

Rule: source from least specific to most specific.
      Last source wins on conflict.
```

---

## 12. CMakeLists.txt Explained

```cmake
cmake_minimum_required(VERSION 3.8)        # Minimum CMake version
project(my_pkg)                            # Project name

# Compiler warnings (good practice)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)         # Build system
find_package(rclcpp REQUIRED)              # ROS 2 C++ library
find_package(std_msgs REQUIRED)            # Message type

# Define executables
add_executable(my_node src/my_node.cpp)
ament_target_dependencies(my_node rclcpp std_msgs)

# Install (so ros2 run can find them)
install(TARGETS
  my_node
  DESTINATION lib/${PROJECT_NAME}          # → install/my_pkg/lib/my_pkg/
)

ament_package()                            # MUST BE LAST
```

### Golden Rule:
**ONE executable per `add_executable`**. Each `.cpp` with a `main()` is its own executable.

```cmake
# ❌ WRONG: two main() functions in one executable!
add_executable(combined src/pub.cpp src/sub.cpp)

# ✅ CORRECT: separate executables
add_executable(pub_node src/pub.cpp)
add_executable(sub_node src/sub.cpp)
```

---

## 13. package.xml Explained

```xml
<?xml version="1.0"?>
<package format="3">
  <!-- Identity -->
  <name>my_pkg</name>                    <!-- MUST match project() -->
  <version>0.0.0</version>
  <description>What this package does</description>
  <maintainer email="you@example.com">Your Name</maintainer>
  <license>Apache-2.0</license>

  <!-- Dependencies -->
  <buildtool_depend>ament_cmake</buildtool_depend>
  <depend>rclcpp</depend>
  <depend>std_msgs</depend>

  <test_depend>ament_lint_auto</test_depend>
  <test_depend>ament_lint_common</test_depend>

  <!-- Build type -->
  <export>
    <build_type>ament_cmake</build_type>   <!-- ament_python for Python -->
  </export>
</package>
```

### Dependency Tags:
| Tag | Purpose |
|-----|---------|
| `<buildtool_depend>` | Tools needed to BUILD (e.g., ament_cmake) |
| `<build_depend>` | Compile-time only |
| `<exec_depend>` | Runtime only |
| `<depend>` | BOTH build + run (most common shortcut) |
| `<test_depend>` | Only for tests |

### Critical Naming Rule:
```
Folder name        →  my_pkg/
CMakeLists.txt     →  project(my_pkg)
package.xml        →  <name>my_pkg</name>

ALL THREE MUST MATCH!
```

---

## 14. Task 1: Step-by-Step

### Architecture:

```
┌────────────────────────────────────────────────────────┐
│                                                        │
│   publisher_a ──► /topic_a (Int32) ──┐                 │
│                                       │                │
│                                       ▼                │
│                            ┌──────────────────┐        │
│                            │  sum_subscriber  │        │
│                            │                  │        │
│                            │  - last_a        │        │
│                            │  - last_b        │        │
│                            │  - sum = a + b   │        │
│                            └────────┬─────────┘        │
│                                     │                  │
│                                     ▼ /topic_sum       │
│   publisher_b ──► /topic_b (Int32) ──┘   (Int32)       │
│                                                        │
└────────────────────────────────────────────────────────┘
```

### Build Steps:

```bash
# 1. Create workspace
mkdir -p ~/ITI/ROS/ROS2/Tasks/task1_pub_subs/src
cd ~/ITI/ROS/ROS2/Tasks/task1_pub_subs

# 2. Create package
cd src
ros2 pkg create sum_pkg \
  --build-type ament_cmake \
  --dependencies rclcpp std_msgs

# 3. Add source files in src/sum_pkg/src/:
#    - publisher_a.cpp
#    - publisher_b.cpp
#    - sum_subscriber.cpp

# 4. Update CMakeLists.txt:
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

# 5. Build
cd ~/ITI/ROS/ROS2/Tasks/task1_pub_subs
colcon build --packages-select sum_pkg

# 6. Source
source install/setup.bash
```

### Run (4 terminals):

```bash
# Terminal 1
ros2 run sum_pkg publisher_a

# Terminal 2
ros2 run sum_pkg publisher_b

# Terminal 3
ros2 run sum_pkg sum_subscriber

# Terminal 4 — verification
ros2 topic list
ros2 topic echo /topic_sum
ros2 node info /sum_subscriber
ros2 run rqt_graph rqt_graph
```

### Key Pattern: Wait for Both Messages

```cpp
// Use flags to know when both topics have produced data
bool has_a_ = false;
bool has_b_ = false;

// In each subscriber callback:
last_X_ = msg->data;
has_X_ = true;
try_compute_sum();

// Helper:
void try_compute_sum() {
    if (has_a_ && has_b_) {              // ← guard!
        int32_t sum = last_a_ + last_b_;
        // compute → publish → log (in this order!)
    }
}
```

### Critical Lesson: "Last Known Value" Pattern

```
Subscribers don't see publishers — they only see MESSAGES.

If you kill publisher_a:
  - sum_subscriber still has last_a_ stored
  - It keeps using that stale value forever
  - publisher_b updates keep producing "wrong" sums

Solution patterns:
  1. Timeout / staleness check (rclcpp::Time)
  2. QoS Deadline policy
  3. QoS Liveliness policy
  4. Manual heartbeat detection
```

---

## 15. Useful Commands

### Node Inspection:
```bash
ros2 node list                        # List active nodes
ros2 node info /node_name             # Detailed info
```

### Topic Inspection:
```bash
ros2 topic list                       # List topics
ros2 topic list -t                    # With message types
ros2 topic info /topic_name           # Pub/sub counts
ros2 topic echo /topic_name           # Print messages
ros2 topic hz /topic_name             # Publishing rate
ros2 topic bw /topic_name             # Bandwidth
ros2 topic pub /topic std_msgs/Int32 "data: 42"   # Manually publish
```

### Interface Inspection:
```bash
ros2 interface show std_msgs/msg/Int32
ros2 interface list                   # All available types
```

### Package Management:
```bash
ros2 pkg list                         # List packages
ros2 pkg executables sum_pkg          # List executables
ros2 pkg create my_pkg --build-type ament_cmake --dependencies rclcpp
```

### Visualization:
```bash
ros2 run rqt_graph rqt_graph          # Visual node graph
rviz2                                  # 3D visualization
```

### Build:
```bash
colcon build                          # Build all packages
colcon build --packages-select my_pkg # Build specific package
colcon build --symlink-install        # Faster rebuilds (Python)
rm -rf build install log              # Clean build
```

---

## 16. Common Pitfalls

### ❌ Pitfall 1: Wrong include path
```cpp
#include "rclcpp.hpp"              // ❌ missing folder
#include "rclcpp/rclcpp.hpp"       // ✅
```

### ❌ Pitfall 2: C header instead of C++
```cpp
#include <std_msgs/msg/int32.h>    // ❌ C version (.h)
#include "std_msgs/msg/int32.hpp"  // ✅ C++ version (.hpp)
```

### ❌ Pitfall 3: Two `main()` in one executable
```cmake
# ❌
add_executable(combined src/pub.cpp src/sub.cpp)
# ✅
add_executable(pub src/pub.cpp)
add_executable(sub src/sub.cpp)
```

### ❌ Pitfall 4: Forgetting to install executables
```cmake
# Without this, ros2 run won't find them!
install(TARGETS my_node DESTINATION lib/${PROJECT_NAME})
```

### ❌ Pitfall 5: Subscriber created before publisher
```cpp
// ❌ Subscriber callback might fire and use null publisher
sub_ = create_subscription(...);
pub_ = create_publisher(...);

// ✅ Always create publishers FIRST
pub_ = create_publisher(...);
sub_ = create_subscription(...);
```

### ❌ Pitfall 6: Local variables for pubs/subs/timers
```cpp
// ❌ Object destroyed when constructor exits
MyNode() : Node("...") {
    auto pub = create_publisher<...>(...);  // local!
}

// ✅ Make them member variables
private:
    rclcpp::Publisher<...>::SharedPtr pub_;  // member
```

### ❌ Pitfall 7: Stale data after publisher dies
Subscribers keep the last value forever. Use timeouts or QoS deadlines for safety-critical data.

### ❌ Pitfall 8: Special characters in folder names
```bash
# ❌ Causes bash to interpret &
mkdir task1_pub&subs

# ✅
mkdir task1_pub_subs
```

### ❌ Pitfall 9: Wrong topic name between pub & sub
```cpp
// Publisher
publisher_ = create_publisher<...>("counter_a", 10);   // ❌

// Subscriber
subscriber_ = create_subscription<...>("topic_a", ...);

// They won't communicate! Names MUST match exactly.
```

### ❌ Pitfall 10: Order of operations in callback
```cpp
// ❌ WRONG: publish stale value, then compute
void try_compute_sum() {
    publish(sum);              // uses OLD sum!
    if (has_a_ && has_b_)
        sum = last_a_ + last_b_;
}

// ✅ CORRECT: compute first, then publish
void try_compute_sum() {
    if (has_a_ && has_b_) {
        sum = last_a_ + last_b_;
        publish(sum);
    }
}
```

---

## 📝 Summary Cheat Sheet

```
┌─────────────────────────────────────────────────────────┐
│                  ROS 2 Workflow                         │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  1. Create workspace + src/ folder                      │
│  2. ros2 pkg create with --dependencies                 │
│  3. Write .cpp files in src/<pkg>/src/                  │
│  4. Update CMakeLists.txt:                              │
│     - add_executable                                    │
│     - ament_target_dependencies                         │
│     - install(TARGETS ...)                              │
│  5. colcon build --packages-select <pkg>                │
│  6. source install/setup.bash                           │
│  7. ros2 run <pkg> <executable>                         │
│  8. ros2 topic echo / list / info to verify             │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

### Required Files for Any New Dependency `X`:

| File | What to add |
|------|-------------|
| `.cpp` | `#include "X/X.hpp"` |
| `CMakeLists.txt` | `find_package(X REQUIRED)` |
| `CMakeLists.txt` | `ament_target_dependencies(my_node X)` |
| `package.xml` | `<depend>X</depend>` |


