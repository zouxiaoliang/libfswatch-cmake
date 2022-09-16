/**
 * @author zouxiaoliang
 * @date 2022/05/13
 */

#include <iostream>
#include <fstream>
#include <easy/FileTailF.h>


int main(int argc, char *argv[]) {
    fsw::easy::FileTailF lw;
    std::ofstream        out("audit.log", std::ios::trunc);
    if (!out.is_open()) {
        return -1;
    }
    lw.async_tailf("/var/log/audit/audit.log", [&](const char* str, size_t length) {
        out << str;
        std::flush(out);
    });

    lw.startup();

    out.close();
    return 0;
}