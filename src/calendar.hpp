#pragma once

#include <string>
#include <vector>

#include "cal_options.hpp"
#include "month_calendar.hpp"

/// 编排多个月历的生成。
///
/// 接受 (年, 月) 对列表，将每个包装为 MonthCalendar，按横排分组，
/// 再将文本块横向拼接，生成最终输出字符串。
class Calendar {
public:
    /// 同排相邻月份之间的空格数。
    static constexpr int kMonthSpacing = 3;

    /// 根据给定选项生成最终格式化的月历字符串。
    [[nodiscard]] std::string generate(const CalOptions& opts,
                                       std::pair<int, int> today) const;

private:
    /// 将扁平月份列表按每排最多 per_row 个月进行分组。
    static std::vector<std::vector<MonthCalendar>> group_by_rows(
        const std::vector<std::pair<int, int>>& months, int per_row);

    /// 将横排中多个月历块渲染为一行字符串。
    static std::string render_row(const std::vector<MonthCalendar>& row);
};
