/**
 * @author zouxiaoliang
 * @date 2022/05/10
 */

#ifndef FILETAILF_H
#define FILETAILF_H

#include <unordered_map>
#include <string>
#include <list>

#include <libfswatch/c++/event.hpp>
#include <libfswatch/c++/monitor.hpp>

namespace fsw {
namespace easy {

class FileTailF {
public:
    typedef std::function<void(const char*, size_t)> Handle;

private:
    struct TailF {
        int               fd{-1};
        std::list<Handle> handles;
    };

public:
    /**
     * @brief Construct a new File Tail F object
     *
     */
    FileTailF();

    /**
     * @brief Destroy the File Tail F object
     *
     */
    ~FileTailF();

    /**
     * @brief
     *
     * @param path
     * @param recv
     * @return true
     * @return false
     */
    bool async_tailf(const std::string& path, Handle recv);

    /**
     * @brief
     *
     */
    void startup();

    /**
     * @brief
     *
     */
    void stop();

private:
    /**
     * @brief
     *
     * @param events
     * @param context
     */
    static void on_events(const std::vector<fsw::event>& events, void* context);

private:
    std::unordered_map<std::string, TailF> m_watchers;
    fsw::monitor*                          m_monitor{nullptr};
};

} // namespace easy
} // namespace fsw

#endif // FILETAILF_H
