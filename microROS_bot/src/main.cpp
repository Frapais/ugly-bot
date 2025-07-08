#include <Arduino.h>
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <geometry_msgs/msg/twist.h>
#include <PID_v1.h>
#include <encoders.h>

// ROS node config
const char* kNodeName = "uglybot_diffdrive_node";
const char* kSubscriberTopic = "cmd_vel";
const int kExecutorTimeout = 10;  // ms

// Motor pins (adjust as needed)
#define INTA_1 6
#define INTA_2 7
#define INTB_1 8
#define INTB_2 9

#define MAX_PWM 255
#define TICKS_PER_METER 1492
#define CONTROL_INTERVAL 10 // ms
#define WHEEL_BASE 0.16
#define WHEEL_DIAMETER 0.068

// PID variables
double SetpointL, SetpointR, InputL, InputR, OutputL, OutputR;
double Kpl = 1.0, Kil = 0.3, Kdl = 0.05;
double Kpr = 1.1, Kir = 0.3, Kdr = 0.05;
PID myPIDL(&InputL, &OutputL, &SetpointL, Kpl, Kil, Kdl, DIRECT);
PID myPIDR(&InputR, &OutputR, &SetpointR, Kpr, Kir, Kdr, DIRECT);

// Motion control variables (updated by ROS callback)
volatile double velocityForward = 0.0; // m/s
volatile double velocityRotate = 0.0;  // rad/s

unsigned long lastControlTime = 0;
unsigned long lastCmdVelTime = 0;
const unsigned long CMD_VEL_TIMEOUT = 500; // ms

// micro-ROS entities
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_subscription_t subscriber;
geometry_msgs__msg__Twist twist_msg;

// Connection management
enum class ConnectionState {
  kWaitingForAgent,
  kConnecting,
  kConnected,
  kDisconnected
};
ConnectionState connection_state = ConnectionState::kWaitingForAgent;

// Forward declarations
void HandleConnectionState();
bool CreateEntities();
void DestroyEntities();
void SubscriptionCallback(const void* msgin);

void setup() {
  Serial.begin(115200);
  Serial.println("[INIT] Starting micro-ROS diffdrive node...");

  // Motor pins
  pinMode(INTA_1, OUTPUT);
  pinMode(INTA_2, OUTPUT);
  pinMode(INTB_1, OUTPUT);
  pinMode(INTB_2, OUTPUT);

  encodersSetup();

  // PID setup
  myPIDL.SetMode(AUTOMATIC);
  myPIDR.SetMode(AUTOMATIC);
  myPIDL.SetSampleTime(CONTROL_INTERVAL);
  myPIDR.SetSampleTime(CONTROL_INTERVAL);
  myPIDL.SetOutputLimits(-MAX_PWM, MAX_PWM);
  myPIDR.SetOutputLimits(-MAX_PWM, MAX_PWM);

  InputL = updateEncoderL();
  InputR = updateEncoderR();
  SetpointL = InputL;
  SetpointR = InputR;

  // micro-ROS Serial transport
  set_microros_serial_transports(Serial);

  delay(2000);
  connection_state = ConnectionState::kWaitingForAgent;
  lastControlTime = millis();
}

void loop() {
  HandleConnectionState();

  // Timeout: if no cmd_vel received recently, stop the robot
  if (millis() - lastCmdVelTime > CMD_VEL_TIMEOUT) {
    velocityForward = 0.0;
    velocityRotate = 0.0;
    // Reset setpoints to current encoder values to stop motion
    SetpointL = updateEncoderL();
    SetpointR = updateEncoderR();
  }

  // PID control loop (runs always, even if not connected)
  unsigned long currentTime = millis();
  if (currentTime - lastControlTime >= CONTROL_INTERVAL) {
    lastControlTime = currentTime;

    // Calculate wheel velocities
    double velocityL = velocityForward - (velocityRotate * WHEEL_BASE / 2.0);
    double velocityR = velocityForward + (velocityRotate * WHEEL_BASE / 2.0);

    // Calculate target encoder ticks for each wheel
    double targetTicksL = velocityL * TICKS_PER_METER * (CONTROL_INTERVAL / 1000.0);
    double targetTicksR = velocityR * TICKS_PER_METER * (CONTROL_INTERVAL / 1000.0);

    // Update PID setpoints
    SetpointL += targetTicksL;
    SetpointR += targetTicksR;

    // Update encoder inputs
    InputL = updateEncoderL();
    InputR = updateEncoderR();

    // Compute PID outputs
    myPIDL.Compute();
    myPIDR.Compute();

    // Drive left motor
    if (OutputL > 0) {
      analogWrite(INTA_1, OutputL);
      analogWrite(INTA_2, 0);
    } else {
      analogWrite(INTA_1, 0);
      analogWrite(INTA_2, -OutputL);
    }

    // Drive right motor
    if (OutputR > 0) {
      analogWrite(INTB_1, OutputR);
      analogWrite(INTB_2, 0);
    } else {
      analogWrite(INTB_1, 0);
      analogWrite(INTB_2, -OutputR);
    }
  }
  delay(1); // Prevent tight loop
}

void HandleConnectionState() {
  switch (connection_state) {
    case ConnectionState::kWaitingForAgent:
      if (RMW_RET_OK == rmw_uros_ping_agent(200, 3)) {
        Serial.println("[ROS] Agent found, establishing connection...");
        connection_state = ConnectionState::kConnecting;
      }
      break;

    case ConnectionState::kConnecting:
      if (CreateEntities()) {
        Serial.println("[ROS] Connected and ready!");
        connection_state = ConnectionState::kConnected;
      } else {
        Serial.println("[ROS] Connection failed, retrying...");
        connection_state = ConnectionState::kWaitingForAgent;
      }
      break;

    case ConnectionState::kConnected:
      if (RMW_RET_OK != rmw_uros_ping_agent(200, 3)) {
        Serial.println("[ROS] Agent disconnected!");
        connection_state = ConnectionState::kDisconnected;
      } else {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(kExecutorTimeout));
      }
      break;

    case ConnectionState::kDisconnected:
      DestroyEntities();
      Serial.println("[ROS] Waiting for agent...");
      connection_state = ConnectionState::kWaitingForAgent;
      break;
  }
}

bool CreateEntities() {
  allocator = rcl_get_default_allocator();

  if (rclc_support_init(&support, 0, NULL, &allocator) != RCL_RET_OK) {
    return false;
  }
  if (rclc_node_init_default(&node, kNodeName, "", &support) != RCL_RET_OK) {
    return false;
  }
  if (rclc_subscription_init_default(
        &subscriber, &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        kSubscriberTopic) != RCL_RET_OK) {
    return false;
  }
  if (rclc_executor_init(&executor, &support.context, 1, &allocator) != RCL_RET_OK) {
    return false;
  }
  if (rclc_executor_add_subscription(&executor, &subscriber, &twist_msg, &SubscriptionCallback, ON_NEW_DATA) != RCL_RET_OK) {
    return false;
  }
  return true;
}

void DestroyEntities() {
  rcl_subscription_fini(&subscriber, &node);
  rclc_executor_fini(&executor);
  rcl_node_fini(&node);
  rclc_support_fini(&support);
}

// ROS callback: update velocities for control loop
void SubscriptionCallback(const void* msgin) {
  const geometry_msgs__msg__Twist* msg = (const geometry_msgs__msg__Twist*)msgin;
  velocityForward = msg->linear.x;
  velocityRotate  = msg->angular.z;
  lastCmdVelTime = millis(); // Update timestamp on every message

  // If both velocities are zero, reset setpoints to current encoder values
  if (velocityForward == 0.0 && velocityRotate == 0.0) {
    SetpointL = updateEncoderL();
    SetpointR = updateEncoderR();
  }
}
