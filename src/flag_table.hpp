#pragma once

#include <string>
#include <vector>

#include "flag_descriptor.hpp"

/// 纯容器——存储 FlagDescriptor 并按名称查找。
///
/// 本身不包含任何解析逻辑，仅提供注册和查找功能。这保证该类聚焦在单一
/// 职责上（SRP — SOLID 中的 S）。
class FlagTable {
public:
    /// 注册一个 flag 描述符。
    /// 同一名称重复注册时，后面的会静默覆盖前面的。
    void register_flag(const FlagDescriptor& desc);

    /// 按 flag 名称查找描述符（如 "-A"、"-m"、"" 表示位置参数）。
    /// 找不到时返回 nullptr。
    [[nodiscard]] const FlagDescriptor* find(const std::string& token) const;

private:
    std::vector<FlagDescriptor> flags_;
};
