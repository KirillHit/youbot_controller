#ifndef COMMAND_SERVER_HPP
#define COMMAND_SERVER_HPP

#include <queue>
#include <map>
#include <string>

#define COMMANDS ybotln::CommandServer::get_commands()

namespace ybotln
{

class Command;

class CommandServer
{
  public:
    static CommandServer &get_commands();
    void add(const std::string name, const Command cmd);
    std::queue<Command> &get(const std::string name);
    void add_subscriber(const std::string name);

  private:
    CommandServer() {}
    CommandServer(const CommandServer &root) = delete;
    CommandServer &operator=(const CommandServer &) = delete;
    std::map<std::string, std::queue<Command>> cmds_map;
};

class ICommandSubscriber
{
  public:
    ICommandSubscriber(std::string n_name);
    void exec_commands();

  private:
    std::string name;
};

class Command
{
  public:
    virtual ~Command() = default;
    virtual void execute() = 0;

    void set_target(ICommandSubscriber *n_sub)
    {
        target = n_sub;
    }

  protected:
    ICommandSubscriber *target;
};

} // namespace ybotln

#endif // COMMAND_SERVER_HPP
