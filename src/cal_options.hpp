#pragma once

#include <optional>
#include <utility>
#include <vector>

/// 纯数据结构——保存 cal 命令的每个用户选项。
///
/// `resolve()` 方法是将原始选项值展开为具体、有序的 (年, 月) 列表的
/// 唯一位置。这使得"应该显示什么"的逻辑完全内聚，可以独立于解析和渲染
/// 进行测试。
struct CalOptions {
    int after_months  = 0;   ///< -A：之后显示的月数（不含当前月）
    int before_months = 0;   ///< -B：之前显示的月数（不含当前月）

    /// -m 指定的月份（1–12）
    std::optional<int> month;

    /// 位置参数 yyyy 指定的年份
    std::optional<int> year;

    /// -d yyyy-mm 指定的日期
    std::optional<std::pair<int, int>> date;

    /// -r：每横排显示的月数（默认 3）
    int rows_per_line = 3;

    /// -h / --help：打印帮助信息后退出
    bool show_help = false;

    /// 将选项展开为实际需要显示的 (年, 月) 列表。
    ///
    /// @param  today 代表"今天"的 (年, 月)
    /// @return       有序的待显示月份列表
    /// @throws std::invalid_argument 若解析后的值超出合法范围
    [[nodiscard]] std::vector<std::pair<int, int>> resolve(
        std::pair<int, int> today) const;
};
