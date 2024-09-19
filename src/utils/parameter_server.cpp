#include "youbot_lidar_nav/utils/parameter_server.hpp"

namespace ybotln
{

ParameterServer &ParameterServer::get_parameters()
{
    static ParameterServer single_instance;
    return single_instance;
}

void ParameterServer::add_observer(const std::string name,
                                   IParametersObserver *ref)
{
    observers[name] = ref;
}

void ParameterServer::init_observer(IParametersObserver *observer)
{
    for (auto const &[key, val] : observers)
        if (val == observer)
            notify(key);
}

void ParameterServer::remove_observer(const std::string name)
{
    observers.erase(name);
}

void ParameterServer::remove_observer(IParametersObserver *observer)
{
    for (auto const &[key, val] : observers)
        if (val == observer)
            observers.erase(key);
}

ParameterServer::ParameterServer() {}

void ParameterServer::notify(const std::string name)
{
    if (!observers.contains(name))
    {
        return;
    }
    observers[name]->handle_event(name);
}

IParametersObserver::~IParametersObserver()
{
    PARAMETERS.remove_observer(this);
}

} // namespace ybotln