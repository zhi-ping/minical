#pragma once

#include <functional>
#include <string>

/// flag 需要的参数类型。
enum class ArgKind {
    None,     // 无参数（如 --help）
    Int,      // 接受一个整数
    DateStr   // 接受 "yyyy-mm" 格式的日期字符串
};

// 前置声明
struct CalOptions;

/// 自描述的命令行 flag。
///
/// 存储 flag 的名称、所需参数类型、以及一个将解析后的值写入 CalOptions
/// 的回调函数（apply）。这使得解析循环可以做到完全通用——永远不需要为某个
/// 具体 flag 写 if-else 分支。
class FlagDescriptor {
public:
    FlagDescriptor(std::string flag_name,
                   ArgKind kind,
                   std::function<void(CalOptions&, const std::string&)> apply_fn);

    std::string flag;       ///< flag 名称（如 "-A"、"" 表示位置参数）
    ArgKind      arg_kind;  ///< 需要的参数类型
    std::function<void(CalOptions&, const std::string&)> apply;  ///< 写入回调
};
