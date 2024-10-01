#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <cstddef>
#include <cstdint>

namespace ybotln
{

enum class DataId : uint8_t
{
    GO_ROUTE = 0x50,
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
constexpr size_t MAX_ROUTE_STEPS = 32;

constexpr size_t RX_MSG_SIZE = MAX_ROUTE_STEPS * ROUTE_STEP_SIZE + 3;
constexpr size_t TX_MSG_SIZE = 24;

} // namespace ybotln

#endif
