/**
 * @author zouxiaoliang
 * @date 2022/05/10
 */

#include "FileTailF.h"


#include <fstream>
#include <iostream>

#include <libfswatch/c++/monitor_factory.hpp>

#include <libfswatch/c++/libfswatch_exception.hpp>

#include <libfswatch/c++/path_utils.hpp>
#include <libfswatch/c/error.h>
#include <libfswatch/c/libfswatch.h>
#include <libfswatch/c/libfswatch_log.h>

#include <fcntl.h>
#include <unistd.h>

fsw::easy::FileTailF::FileTailF() {
    //
}

fsw::easy::FileTailF::~FileTailF() {
    stop();
}

bool fsw::easy::FileTailF::async_tailf(const std::string& path, Handle recv) {
    auto found = m_watchers.find(path);
    if (m_watchers.end() != found) {
        found->second.handles.push_back(recv);
    } else {
        auto inserted = m_watchers.emplace(path, TailF{});
        if (!inserted.second) {
            return false;
        }
        if (0 == access(path.c_str(), F_OK)) {
            struct stat s;
            if (0 == ::stat(path.c_str(), &s)) {
                int fd = open(path.c_str(), O_RDONLY);
                if (-1 != fd) {
                    lseek(fd, 0, SEEK_END);
                    inserted.first->second.fd = fd;
                }
            }
        }
        inserted.first->second.handles.push_back(recv);
    }

    if (nullptr != m_monitor && m_monitor->is_running()) {
        // restart monitor;
    }
    return true;
}

void fsw::easy::FileTailF::startup() {
    if (nullptr != m_monitor) {
        return;
    }

    std::vector<std::string> paths;
    for (const auto& item : m_watchers) {
        // std::cout << " -- watch " << item.first << ", obsover: " << item.second.handles.size() << std::endl;
        paths.push_back(item.first);
    }

    m_monitor = fsw::monitor_factory::create_monitor(
        fsw_monitor_type::system_default_monitor_type, paths, FileTailF::on_events, this);
    if (nullptr == m_monitor) {
        return;
    }
    try {
        std::vector<fsw_event_type_filter> filter{
            {fsw_event_flag::Updated},
            {fsw_event_flag::Removed},
            {fsw_event_flag::Renamed},
            {fsw_event_flag::MovedTo},
            {fsw_event_flag::MovedFrom}};
        m_monitor->set_event_type_filters(filter);
        m_monitor->start();
    } catch (fsw::libfsw_exception& ex) {
        // no idea.
    }

    for (auto& item : m_watchers) {
        if (-1 != item.second.fd) {
            close(item.second.fd);
        }
        item.second.fd = -1;
    }
}

void fsw::easy::FileTailF::stop() {
    if (nullptr != m_monitor && m_monitor->is_running()) {
        m_monitor->stop();
        for (int i = 0; i < 3 && m_monitor->is_running(); ++i) {
            sleep(1);
        }
        delete m_monitor;
        m_monitor = nullptr;
    }
}

void fsw::easy::FileTailF::on_events(const std::vector<fsw::event>& events, void* context) {
    char buffer[129] = {0};

    FileTailF* self = static_cast<FileTailF*>(context);

    for (const auto& event : events) {
        const auto& path = event.get_path();
        // std::cout << " -- " << path << " has changed" << std::endl;
        auto found = self->m_watchers.find(path);
        if (self->m_watchers.end() == found) {
            continue;
        }

        int&  fd      = found->second.fd;
        auto& handles = found->second.handles;

        for (const auto& flag : event.get_flags()) {
            // std::cerr << " -- " << path << " has changed, flag: " << event.get_event_flag_name(flag) <<
            // std::endl;
            if (flag == fsw_event_flag::Updated || flag == fsw_event_flag::Renamed || flag == fsw_event_flag::MovedTo) {
                if (-1 == fd) {
                    fd = open(event.get_path().c_str(), O_RDONLY);
                    if (-1 == fd) {
                        // std::cout << "open " << event.get_path() << ", failed" << std::endl;
                        continue;
                    }
                }

                size_t s = 0;
                while (0 != (s = read(fd, buffer, 128))) {
                    // std::cout << "recv message length: " << s << std::endl;
                    buffer[s] = 0;
                    for (auto& handle : handles) {
                        // std::cout << "handle message " << buffer << std::endl;
                        handle(buffer, s);
                    }
                }
                if (0 != access(event.get_path().c_str(), F_OK)) {
                    if (-1 != fd) {
                        close(fd);
                        fd = -1;
                    }
                }
            }

            if (flag == fsw_event_flag::Removed || flag == fsw_event_flag::Renamed || flag == fsw_event_flag::MovedTo) {
                if (-1 != fd) {
                    close(fd);
                    fd = -1;
                }
            }
        }
    }
}
