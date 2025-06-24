# 🤖 Ugly Bot
A project exploring communication between an **ESP32-C3 board** and **ROS 2 (Jazzy)** using **micro-ROS** and **PlatformIO**.

<img src="https://github.com/Frapais/ugly-bot/blob/main/3D%20Renders/Profile%20naked.png" alt="profile_naked" width="200"/> <img src="https://github.com/Frapais/ugly-bot/blob/main/3D%20Renders/Profile.png" alt="profile" width="193"/> <img src="https://github.com/Frapais/ugly-bot/blob/main/Photos/IMG_20250624_220913_c.JPG" alt="profile" width="183"/>

---

## 🚀 Running the micro-ROS Agent on Raspberry Pi (ROS 2 side)

Follow these steps to run the micro-ROS Agent and observe messages from the ESP32-C3:

---

### 🧰 Prerequisites

- ROS 2 Jazzy installed on your Raspberry Pi
- micro-ROS Agent built using `micro_ros_setup`
- ESP32-C3 device flashed with `micro_ros_platformio` project

---

### ✅ Step-by-Step Instructions

1. **Open a new terminal and go to your workspace:**

   ```bash
   cd ~/microros_ws
   ```
   
2. **Source the ROS 2 environment:**

    If you've already built the agent:
    ```bash
    source install/local_setup.bash
    ```
  
    If not, first build it with:
    ```bash
      ros2 run micro_ros_setup build_agent.sh
    ```
    📚 Reference: [micro-ROS FreeRTOS Tutorial](https://micro.ros.org/docs/tutorials/core/first_application_rtos/freertos/)
   
3. **Run the micro-ROS Agent (UDP, port 8888):**
   ```bash
   ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888
   ```
   📚 Reference: [micro-ROS Linux Tutorial](https://micro.ros.org/docs/tutorials/core/first_application_linux/)
   
4. **In a new terminal, list active topics:**
   ```bash
   ros2 topic list
   ```
   You should see:
   ```bash
   /micro_ros_platformio_node_publisher
   ```

5. **Echo the topic to see data coming from ESP32-C3:**
   ```bash
   ros2 topic echo /micro_ros_platformio_node_publisher
   ```

## 📡 What’s Happening?
Your ESP32-C3, running a micro-ROS WiFi publisher, is sending messages to your Raspberry Pi, where the micro-ROS agent is converting those into standard ROS 2 messages.

## 🧪 In Progress
This project is under active development. Stay tuned for:
- Sensor data publishing
- Command/control topic subscriptions
- Real-world robot testing

## 🧾 License
This project is open-sourced under the MIT License.

## ✍️ Author
Konstantinos Paraskevas
