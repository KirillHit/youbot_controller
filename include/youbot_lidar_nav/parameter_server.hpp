#ifndef PARAMETER_SERVER_HPP
#define PARAMETER_SERVER_HPP

#include <map>
#include <string>

#include "logger.hpp"

namespace ybotln
{

class ParameterServer
{
  public:
    static ParameterServer &get_parameters()
    {
        static ParameterServer single_instance;
        return single_instance;
    }
    template <class T> const T& get(const std::string name);
    template <class T> void set(const std::string name, const T value)
    {
        if (name.empty())
        {
            LOGGER_STREAM(MSG_LVL::ERROR, "Parameter name cannot be empty!");
        }

        static std::map<std::string, T> parameters;
        parameters[name] = value;

        LOGGER_STREAM(MSG_LVL::DEBUG,
                      "The parameter \""
                          << name << "\" is assigned the value:" << value);
    }

  private:
    ParameterServer()
    {
    }
    ParameterServer(const ParameterServer &root) = delete;
    ParameterServer &operator=(const ParameterServer &) = delete;
};

} // namespace ybotln

#endif // PARAMETER_SERVER_HPP