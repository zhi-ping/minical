#include "cal_options.hpp"

#include <stdexcept>

namespace {

/// 将 (年, 月) 偏移 delta 个月（可为负数）。
/// 月份为 1-based，跨年时年份会自动调整。
std::pair<int, int> add_months(std::pair<int, int> ym, int delta) {
    int y = ym.first;
    int m = ym.second + delta;          // m 可能 < 1 或 > 12

    if (m > 0) {
        // 正向：整除和取模直接可用
        y += (m - 1) / 12;
        m = (m - 1) % 12 + 1;
    } else {
        // m <= 0 时需要向下取整语义
        int years_back = (-m) / 12 + 1;
        y -= years_back;
        m += years_back * 12;
    }
    return {y, m};
}

} // 匿名命名空间

std::vector<std::pair<int, int>> CalOptions::resolve(
    std::pair<int, int> today) const {

    // --- 确定年份 -----------------------------------------------------------
    int y = year.value_or(
        date.has_value() ? date->first : today.first);

    // --- 确定月份 -----------------------------------------------------------
    int m = month.value_or(
        date.has_value() ? date->second : today.second);

    // --- 校验 ---------------------------------------------------------------
    if (m < 1 || m > 12) {
        throw std::invalid_argument("月份必须在 [1, 12] 范围内");
    }

    // --- 判断用户是否只指定了年份 -------------------------------------------
    // "只指定了年份" ⇔ 用户显式给出了年份，且未指定 -m 和 -d。
    // 若无此判断，裸 `cal`（无参数）会错误地显示全年 12 个月。
    bool only_year_specified = year.has_value()
                            && !month.has_value()
                            && !date.has_value();

    std::pair<int, int> start;
    int count;

    if (only_year_specified) {
        // 显示全年，忽略 -A / -B（与标准 cal 行为一致）
        start = {y, 1};
        count = 12;
    } else {
        // 以解析出的月份为中心，向前后扩展
        start = add_months({y, m}, -before_months);
        count = 1 + before_months + after_months;
    }

    // --- 构建月份列表 -------------------------------------------------------
    std::vector<std::pair<int, int>> result;
    result.reserve(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i) {
        result.push_back(add_months(start, i));
    }
    return result;
}
