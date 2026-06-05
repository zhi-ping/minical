#include "calendar.hpp"

#include <algorithm>
#include <format>
#include <string>

// ---------------------------------------------------------------------------
// 按行分组
// ---------------------------------------------------------------------------

std::vector<std::vector<MonthCalendar>> Calendar::group_by_rows(
    const std::vector<std::pair<int, int>>& months, int per_row) {

    std::vector<std::vector<MonthCalendar>> rows;
    std::vector<MonthCalendar> current_row;
    current_row.reserve(static_cast<std::size_t>(per_row));

    for (const auto& [y, m] : months) {
        current_row.emplace_back(y, m);
        if (static_cast<int>(current_row.size()) == per_row) {
            rows.push_back(std::move(current_row));
            current_row.clear();
            current_row.reserve(static_cast<std::size_t>(per_row));
        }
    }
    if (!current_row.empty()) {
        rows.push_back(std::move(current_row));
    }
    return rows;
}

// ---------------------------------------------------------------------------
// 渲染单排 —— 将若干月份横向拼接
// ---------------------------------------------------------------------------

std::string Calendar::render_row(const std::vector<MonthCalendar>& row) {
    // 收集该排中每个月的文本块
    struct Block {
        std::string title;
        std::string header;
        std::vector<std::string> weeks;
        int total_lines() const { return 2 + static_cast<int>(weeks.size()); }
    };

    std::vector<Block> blocks;
    blocks.reserve(row.size());
    int max_lines = 0;

    for (const auto& mc : row) {
        Block b;
        b.title  = mc.title_line();
        b.header = mc.header_line();
        b.weeks  = mc.week_lines();
        max_lines = std::max(max_lines, b.total_lines());
        blocks.push_back(std::move(b));
    }

    // 月份之间 3 空格分隔
    std::string spacing(static_cast<std::size_t>(kMonthSpacing), ' ');

    // 逐行拼接
    std::string output;
    for (int line_idx = 0; line_idx < max_lines; ++line_idx) {
        for (std::size_t bi = 0; bi < blocks.size(); ++bi) {
            const auto& b = blocks[bi];

            if (bi > 0) {
                output += spacing;
            }

            if (line_idx == 0) {
                output += b.title;
            } else if (line_idx == 1) {
                output += b.header;
            } else {
                int week_idx = line_idx - 2;
                if (week_idx < static_cast<int>(b.weeks.size())) {
                    output += b.weeks[static_cast<std::size_t>(week_idx)];
                } else {
                    // 用与月历块等宽的空白行补齐
                    output.append(
                        static_cast<std::size_t>(MonthCalendar::kWidth), ' ');
                }
            }
        }
        // 去掉行尾多余空格，保持输出整洁
        while (!output.empty() && output.back() == ' ') {
            output.pop_back();
        }
        output += '\n';
    }

    return output;
}

// ---------------------------------------------------------------------------
// 顶层入口
// ---------------------------------------------------------------------------

std::string Calendar::generate(const CalOptions& opts,
                               std::pair<int, int> today) const {
    auto months = opts.resolve(today);
    auto rows   = group_by_rows(months, opts.rows_per_line);

    std::string result;
    for (std::size_t i = 0; i < rows.size(); ++i) {
        if (i > 0) {
            result += '\n';   // 不同行组之间空一行
        }
        result += render_row(rows[i]);
    }
    return result;
}
