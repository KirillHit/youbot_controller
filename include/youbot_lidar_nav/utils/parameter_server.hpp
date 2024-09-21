#ifndef PARAMETER_SERVER_HPP
#define PARAMETER_SERVER_HPP

#include <functional>
#include <map>
#include <string>

#define PARAMETERS ybotln::ParameterServer::get_parameters()

namespace ybotln
{

class ParameterServer
{
  public:
    static ParameterServer &get_parameters()
    {
        static ParameterServer single_instance;
        return single_instance;
    };
    template <class T>
    const T &get(const std::string name)
    {
        return get_params_map<T>()[name];
    }
    template <class T>
    void set(const std::string name, const T value)
    {
        get_params_map<T>()[name] = value;
    }
    template <class T>
    bool exist(const std::string name)
    {
        return get_params_map<T>().contains(name);
    }

  private:
    ParameterServer() = default;
    ParameterServer(const ParameterServer &root) = delete;
    ParameterServer &operator=(const ParameterServer &) = delete;
    template <class T>
    std::map<std::string, T> &get_params_map()
    {
        static std::map<std::string, T> parameters;
        return parameters;
    }
};

} // namespace ybotln

#endif // PARAMETER_SERVER_HPP
