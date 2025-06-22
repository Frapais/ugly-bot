# ugly-bot

Steps to run a micro ros agent:
1. In a new terminal, run "cd ~", "cd mircoros_ws"
2. "source install/local_setup.bash" or run "ros2 run micro_ros_setup build_agent.sh" before that. If you find any issues, refer to here: "https://micro.ros.org/docs/tutorials/core/first_application_rtos/freertos/"
3. run micro ros agent: "ros2 run micro_ros_agent micro_ros_agent udp4 --port 8888" For reference, go here: "https://micro.ros.org/docs/tutorials/core/first_application_linux/"
4. in a new cmd: "ros2 topic list" should show the "/micro_ros_platformio_node_publisher"
5. run "ros2 topic echo /micro_ros_platformio_node_publisher" to see the data coming from the esp32-c3
