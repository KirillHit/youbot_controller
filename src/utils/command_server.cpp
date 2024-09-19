#include "youbot_lidar_nav/utils/command_server.hpp"
#include "youbot_lidar_nav/utils/logger.hpp"

namespace ybotln
{

CommandServer &CommandServer::get_commands()
{
    static CommandServer single_instance;
    return single_instance;
}

void CommandServer::add_subscriber(const std::string name)
{
    if (cmds_map.contains(name))
    {
        LOGGER_STREAM(MSG_LVL::WARN,
                      "Such a command server subscriber already exists. Undefined behavior may occur.");
        return;
    }
    
}

ICommandSubscriber::ICommandSubscriber(std::string n_name) : name{n_name}
{
    COMMANDS.add_subscriber(name);
}

void ICommandSubscriber::exec_commands()
{
    std::queue<Command> &cmds = COMMANDS.get(name);
    while (!cmds.empty())
    {
        cmds.front().set_target(this);
        cmds.front().execute();
        cmds.pop();
    }
}

} // namespace ybotln
