/**
 * @author zouxiaoliang
 * @date 2022/05/10
 */

#include <easy/FileTailF.h>


int main(int argc, char* argv[]) {
    std::cout << "Hello world!!!" << std::endl;

    fsw::easy::FileTailF lw;
    lw.async_tailf("/var/log/audit/audit.log", [](const char* str, size_t length) {
        std::cout << str;
    });

    lw.startup();
    return 0;
}
