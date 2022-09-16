/**
 * @author zouxiaoliang
 * @date 2022/05/10
 */

#include <easy/FileTailF.h>


int main(int argc, char* argv[]) {
    std::cout << "Hello world!!!" << std::endl;

    if (argc < 1) {
        std::cout << "usage: " << argv[0] << ", {filepath}" << std::endl;
        return 0;
    }
    fsw::easy::FileTailF fw;
    fw.async_tailf(argv[1], [](const char* str, size_t length) {
        std::cout << str;
        std::flush(std::cout);
    });

    fw.startup();
    return 0;
}
