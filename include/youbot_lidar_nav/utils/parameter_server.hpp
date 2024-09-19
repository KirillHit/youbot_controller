#ifndef PARAMETER_SERVER_HPP
#define PARAMETER_SERVER_HPP

#include <functional>
#include <map>
#include <string>

#define PARAMETERS ybotln::ParameterServer::get_parameters()

namespace ybotln
{

class IParametersObserver
{
  public:
    virtual void handle_event(const std::string name) = 0;
    virtual ~IParametersObserver();
};

class ParameterServer
{
  public:
    static ParameterServer &get_parameters();
    template <class T> const T &get(const std::string name);
    template <class T> void set(const std::string name, const T value);
    template <class T> bool exist(const std::string name);
    void add_observer(const std::string name, IParametersObserver *ref);
    void init_observer(IParametersObserver *observer);
    void remove_observer(const std::string name);
    void remove_observer(IParametersObserver *observer);

  private:
    ParameterServer();
    ParameterServer(const ParameterServer &root) = delete;
    ParameterServer &operator=(const ParameterServer &) = delete;
    template <class T> std::map<std::string, T> &get_params_map();
    std::map<std::string, IParametersObserver *> observers;
    void notify(const std::string name);
};

template <class T> const T &ParameterServer::get(const std::string name)
{
    return get_params_map<T>()[name];
}

template <class T>
void ParameterServer::set(const std::string name, const T value)
{
    get_params_map<T>()[name] = value;
    notify(name);
}

template <class T> bool ParameterServer::exist(const std::string name)
{
    return get_params_map<T>().contains(name);
}

template <class T> std::map<std::string, T> &ParameterServer::get_params_map()
{
    static std::map<std::string, T> parameters;
    return parameters;
}

} // namespace ybotln

#endif // PARAMETER_SERVER_HPP