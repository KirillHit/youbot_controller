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

#pragma pack(push, 1)
struct RouteMsg
{
    int16_t longitudinal_vel;   // x/1000 m/s
    int16_t transversal_vel;    // x/1000 m/s
    int16_t angular_vel;        // x/1000 rad/s
    int16_t duration;           // ms

};
#pragma pack(pop)

} // namespace ybotln

#endif
