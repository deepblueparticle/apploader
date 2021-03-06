/*
 * File Name: plugin_manager.h
 * Author: Stan.Lch
 * Mail: fn.stanc@gmail.com
 * Created Time: Wed 28 Dec 2016 12:06:30 AM CST
 */

#ifndef _PLUGIN_MANAGER_H
#define _PLUGIN_MANAGER_H

#include <string>
#include <cassert>
#include <typeinfo>

namespace uf {

class Module;
class Plugin;

namespace internal {

template<typename T, typename Base>
struct IsBaseOf
{
    using Yes = char;
    struct No { char dummy[2]; };
    static Yes check(Base *base);
    static No ckeck(...);
    enum {
        Value = sizeof(check((T *)0)) == sizeof(Yes)
    };
};

} // namespace internal

#define CHECK_DERIVES(T, Base) \
    static_assert(uf::internal::IsBaseOf<T, Base>::Value, "T is not derived from Base.")

class PluginManager
{
public:
    PluginManager() = default;
    virtual ~PluginManager() = default;
    PluginManager(const PluginManager &) = delete;
    PluginManager &operator=(const PluginManager &) = delete;

    virtual void AppID(int id) = 0;
    virtual int AppID() = 0;
    virtual const std::string &AppName() = 0;
    virtual void AppName(const std::string &n) = 0;

    virtual bool Init(const std::string &plugin_conf_file) = 0;
    virtual bool AfterInit() = 0;
    virtual bool BeforeShutdown() = 0;
    virtual bool Shutdown() = 0;
    virtual void Execute() = 0;

    // for plugin library load/unload
    virtual void Install(Plugin *plugin) = 0;
    virtual void Uninstall(Plugin *plugin) = 0;
    virtual Plugin *FindPlugin(const std::string &name) const = 0;

    virtual void RegisterModule(const std::string &name, Module *module) = 0;
    virtual void UnregisterModule(const std::string &name) = 0;
    virtual Module *FindModule(const std::string &name) const = 0;

    template <typename T>
    void RegisterModule(T *module)
    {
        CHECK_DERIVES(T, Module);
        RegisterModule(typeid(T).name(), module);
    }

    template <typename T>
    void UnregisterModule()
    {
        UnregisterModule(typeid(T).name());
    }

    template <typename T>
    T *FindModule() const
    {
        CHECK_DERIVES(T, Module);
        const char * name = typeid(T).name();
        return (T *)FindModule(name);
    }
};

typedef void (*PluginInstallFunc)(PluginManager *);
typedef void (*PluginUninstallFunc)(PluginManager *);

} // namespace uf
#endif
