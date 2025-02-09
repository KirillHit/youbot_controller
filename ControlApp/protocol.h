#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <cmath>
#include <cstddef>
#include <cstdint>

enum class DataId : uint8_t {
    GO_ROUTE = 0x50,
    SET_CAMERA = 0x51,
};

/**
 * @brief GO_ROUTE msg
 *  0 byte: data id (uint8_t) - GO_ROUTE
 *  1-2 byte: RouteMsgHeader
 *  3-... byte: RouteStepMsgs
 */
#pragma pack(push, 1)
struct RouteMsgHeader
{
    uint16_t reset_route : 1;
    uint16_t step_count : 15;
};
#pragma pack(pop)
#pragma pack(push, 1)
struct RouteStepMsg
{
    int16_t longitudinal_vel; // x/1000 m/s
    int16_t transversal_vel;  // x/1000 m/s
    int16_t angular_vel;      // x/1000 rad/s
    int16_t duration;         // ms
};
#pragma pack(pop)
constexpr size_t ROUTE_STEP_SIZE = sizeof(RouteStepMsg);
constexpr size_t MAX_ROUTE_STEPS = 1;

#pragma pack(push, 1)
struct StreamSettings
{
    uint8_t camera_dev;
    uint32_t server_ip;
    uint16_t server_port;
    uint16_t pack_size;
    uint16_t frame_width;
    uint16_t frame_height;
    uint16_t frame_interval;
    uint8_t encode_quality;
    uint16_t stream_switch : 1;
    uint16_t delay : 15;
};
#pragma pack(pop)

constexpr size_t TX_MSG_SIZE = std::max(MAX_ROUTE_STEPS * ROUTE_STEP_SIZE + 3,
                                        sizeof(StreamSettings) + 1);
constexpr size_t RX_MSG_SIZE = 24;

#endif
