#ifndef AUBO_HARDWARE_INTERFACE_H
#define AUBO_HARDWARE_INTERFACE_H

// System
#include <memory>
#include <string>
#include <vector>
#include <limits>

#include <algorithm>
#include <utility>

// ros2_control hardware_interface
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
// #include "hardware_interface/visibility_control.h"

// ROS
#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"

#include <aubo_dashboard_msgs/msg/robot_mode.h>

#include <aubo/robot/robot_state.h>
#include "aubo_sdk/rtde.h"
#include "aubo_sdk/rpc.h"
#include "serviceinterface.h"
#include "thread"

using namespace arcs::common_interface;
using namespace arcs::aubo_sdk;
using RtdeRecipeMap =
    std::unordered_map<int, arcs::common_interface::RtdeRecipe>;

namespace aubo_driver {
class AuboHardwareInterface : public hardware_interface::SystemInterface
{
public:
    RCLCPP_SHARED_PTR_DEFINITIONS(AuboHardwareInterface);
    virtual ~AuboHardwareInterface();

    bool OnActive();
    hardware_interface::CallbackReturn on_activate(
        const rclcpp_lifecycle::State &previous_state);
    hardware_interface::CallbackReturn on_init(
        const hardware_interface::HardwareInfo &system_info) final;
    std::vector<hardware_interface::StateInterface> export_state_interfaces()
        final;

    std::vector<hardware_interface::CommandInterface>
    export_command_interfaces() final;

    hardware_interface::return_type read(const rclcpp::Time &time,
                                         const rclcpp::Duration &period) final;
    hardware_interface::return_type write(const rclcpp::Time &time,
                                          const rclcpp::Duration &period) final;

    void readActualQ();

    void setInput(RtdeClientPtr cli);

    bool isServoModeStart();

    bool ServoModeStart();

    int startServoMode();

    int stopServoMode();

    int Servoj(const std::array<double, 6> joint_position_command);

    void configSubscribe(RtdeClientPtr cli);

private:
    std::shared_ptr<RpcClient> rpc_client_{ nullptr };
    std::shared_ptr<RtdeClient> rtde_client_{ nullptr };
    std::vector<std::string> joint_names_;
    std::mutex rtde_mtx_;
    std::string robot_ip_;
    std::string robot_name_;

    std::array<double, 6> aubo_position_commands_;
    std::array<double, 6> aubo_velocity_commands_;
    double speed_scaling_combined_;
    bool controllers_initialized_;
    bool servo_mode_start_{ false };
    bool initialized_;

    std::atomic<bool> robot_program_running_;
    std::atomic<bool> controller_reset_necessary_{ false };
    //    uint32_t runtime_state_;
    std::atomic<bool> position_controller_running_;
    std::atomic<bool> velocity_controller_running_;
    std::atomic<bool> joint_forward_controller_running_;
    std::atomic<bool> cartesian_forward_controller_running_;
    std::atomic<bool> twist_controller_running_;
    std::atomic<bool> pose_controller_running_;

    // topic1
    int line_{ -1 };
    std::vector<double> actual_q_{ std::vector<double>(6, 0) };
    std::vector<double> joint_velocity_{ std::vector<double>(6, 0) };
    std::array<double, 6> actual_q_copy_;
    std::array<double, 6> joint_velocity_copy_;
    std::vector<double> actual_qd_{ std::vector<double>(6, 0) };
    std::vector<double> target_q_{ std::vector<double>(6, 0) };
    std::vector<double> target_qd_{ std::vector<double>(6, 0) };

    std::vector<double> actual_current_{ std::vector<double>(6, 0.1) };
    std::vector<double> actual_current_e{ std::vector<double>(6, 0) };
    std::vector<double> actual_TCP_pose_{ std::vector<double>(6, 0.) };
    std::vector<double> actual_TCP_speed_{ std::vector<double>(6, 0.) };
    std::vector<double> actual_TCP_force_{ std::vector<double>(6, 0.) };
    std::vector<double> target_TCP_pose_{ std::vector<double>(6, 0.) };
    std::vector<double> target_TCP_speed_{ std::vector<double>(6, 0.) };

    RobotModeType robot_mode_ = RobotModeType::NoController;
    SafetyModeType safety_mode_ = SafetyModeType::Normal;
    RuntimeState runtime_state_ = RuntimeState::Stopped;
};
} // namespace aubo_driver

#endif
