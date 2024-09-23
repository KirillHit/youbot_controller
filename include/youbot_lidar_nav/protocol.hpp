#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <cstddef>
#include <cstdint>

namespace ybotln
{

const size_t TX_MSG_SIZE = 24;
const size_t RX_MSG_SIZE = 24;

enum class DataId : uint8_t
{
    GO_ROUTE = 0x50,
};

/**
 * @brief GO_ROUTE msg
 *  0 byte: data id (uint8_t)
 *  1 byte: number of commands RouteStep (uint8_t)
 *  2 - ROUTE_STEP_SIZE+2
 */
#pragma pack(push, 1)
struct RouteStepMsg
{
    uint8_t step_num;           // step number
    int16_t longitudinal_vel;   // x/1000 m/s
    int16_t transversal_vel;    // x/1000 m/s
    int16_t angular_vel;        // x/1000 rad/s
    int16_t duration;           // ms
};
#pragma pack(pop)
const size_t ROUTE_STEP_SIZE = sizeof(RouteStepMsg);

} // namespace ybotln

#endif
