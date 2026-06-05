#pragma once

#include "cal_options.hpp"
#include "flag_table.hpp"

/// 通用命令行解析器。
///
/// 解析器逐 token 遍历 argv，并将每个决策委托给 FlagTable。
/// 它对具体有哪些 flag 一无所知——只知道向 FlagTable 查询描述符、
/// 然后调用其 apply() 方法。
/// 这意味着新增 flag 永远不需要修改本类（开闭原则）。
class OptionParser {
public:
    /// 使用给定的 flag 表解析 argc/argv。
    ///
    /// @throws std::runtime_error 遇到未知 flag、缺少参数或值格式错误时抛出
    [[nodiscard]] CalOptions parse(int argc, char* argv[],
                                   const FlagTable& table) const;
};
