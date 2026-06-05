#include "option_parser.hpp"

#include <cctype>
#include <stdexcept>
#include <string>

CalOptions OptionParser::parse(int argc, char* argv[],
                                const FlagTable& table) const {
    CalOptions opts;
    int i = 1;  // 跳过程序名

    while (i < argc) {
        std::string token(argv[i]);

        // 1. 尝试匹配已注册的 flag -------------------------------------------
        const FlagDescriptor* desc = table.find(token);
        if (desc != nullptr) {
            std::string arg;
            if (desc->arg_kind != ArgKind::None) {
                ++i;
                if (i >= argc) {
                    throw std::runtime_error(
                        "'" + token + "' 后面缺少参数");
                }
                arg = argv[i];
            }
            // 委托给描述符的 apply() 回调——我们不需要关心 CalOptions
            // 的哪个字段被写入，也不需要知道值是如何转换的。
            try {
                desc->apply(opts, arg);
            } catch (const std::exception& e) {
                throw std::runtime_error(
                    std::string(token) + " 的值无效: " + e.what());
            }
        }
        // 2. 位置参数：裸年份（如 "2025"）------------------------------------
        else if (std::isdigit(static_cast<unsigned char>(token[0]))) {
            const FlagDescriptor* pos = table.find("");
            if (pos != nullptr) {
                try {
                    pos->apply(opts, token);
                } catch (const std::exception& e) {
                    throw std::runtime_error(
                        std::string("年份 '") + token + "' 无效: " + e.what());
                }
            } else {
                throw std::runtime_error("意外的位置参数 '" + token + "'");
            }
        }
        // 3. 未知选项 --------------------------------------------------------
        else {
            throw std::runtime_error("未知选项: " + token);
        }
        ++i;
    }

    return opts;
}
