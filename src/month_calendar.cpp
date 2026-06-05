#include "month_calendar.hpp"

#include <array>
#include <format>
#include <stdexcept>

// ---------------------------------------------------------------------------
// 构造函数
// ---------------------------------------------------------------------------

MonthCalendar::MonthCalendar(int year, int month)
    : year_(year)
    , month_(month) {
    if (month < 1 || month > 12) {
        throw std::invalid_argument(
            std::format("月份 {} 超出 [1, 12] 范围", month));
    }
    if (year < 1 || year > 9999) {
        throw std::invalid_argument(
            std::format("年份 {} 超出 [1, 9999] 范围", year));
    }
}

// ---------------------------------------------------------------------------
// 计算当月天数
// ---------------------------------------------------------------------------

int MonthCalendar::days_in_month() const {
    static constexpr std::array<int, 12> days = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int d = days[static_cast<std::size_t>(month_ - 1)];
    // 二月的闰年调整
    if (month_ == 2) {
        bool leap = (year_ % 4 == 0 && year_ % 100 != 0) || (year_ % 400 == 0);
        if (leap) {
            d = 29;
        }
    }
    return d;
}

// ---------------------------------------------------------------------------
// 计算 1 号的星期几 —— Tomohiko Sakamoto 算法
// 返回值: 0 = 周日 … 6 = 周六
// ---------------------------------------------------------------------------

int MonthCalendar::first_weekday() const {
    int y = year_;
    int m = month_;         // 保持原始的 1-based 月份
    // Sakamoto 算法中一、二月的年份调整
    if (m < 3) {
        y -= 1;
    }
    // Sakamoto 算法的魔数表，按原始月份（1–12）索引
    static constexpr std::array<int, 12> t = {
        0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    return (y + y / 4 - y / 100 + y / 400 + t[static_cast<std::size_t>(m - 1)] + 1) % 7;
}

// ---------------------------------------------------------------------------
// 英文月份名称
// ---------------------------------------------------------------------------

std::string MonthCalendar::month_name() const {
    static constexpr std::array<const char*, 12> names = {
        "January", "February", "March", "April",
        "May", "June", "July", "August",
        "September", "October", "November", "December"};
    return names[static_cast<std::size_t>(month_ - 1)];
}

// ---------------------------------------------------------------------------
// 渲染各行
// ---------------------------------------------------------------------------

std::string MonthCalendar::title_line() const {
    // 例如 "    May 2025"
    std::string title = std::format("{} {}", month_name(), year_);
    // 在 kWidth 宽度内居中
    int pad_total = kWidth - static_cast<int>(title.size());
    if (pad_total <= 0) {
        return title;
    }
    int pad_left  = pad_total / 2;
    int pad_right = pad_total - pad_left;
    return std::string(static_cast<std::size_t>(pad_left), ' ')
        + title
        + std::string(static_cast<std::size_t>(pad_right), ' ');
}

std::string MonthCalendar::header_line() const {
    // "Su Mo Tu We Th Fr Sa" — 每个标签 2 字符，用 1 空格分隔
    // 总计 = 7×2 + 6 = 20 字符，右侧补空格至 kWidth
    static constexpr std::array<const char*, 7> wd = {
        "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
    std::string h;
    for (int i = 0; i < 7; ++i) {
        if (i > 0) h += ' ';
        h += wd[i];
    }
    // 补齐至 kWidth
    h.append(static_cast<std::size_t>(kWidth - h.size()), ' ');
    return h;
}

std::vector<std::string> MonthCalendar::week_lines() const {
    int first_wd  = first_weekday();
    int total     = days_in_month();
    int cells     = first_wd + total;
    int rows      = (cells + 6) / 7;   // 向上取整

    std::vector<std::string> result;
    result.reserve(static_cast<std::size_t>(rows));

    int day = 1;
    for (int r = 0; r < rows; ++r) {
        std::string line;
        line.reserve(static_cast<std::size_t>(kWidth));
        for (int col = 0; col < 7; ++col) {
            if (col > 0) line += ' ';
            if ((r == 0 && col < first_wd) || day > total) {
                line += "  ";   // 空白格（两位空格代替日期数字）
            } else {
                line += std::format("{:>2}", day);
                ++day;
            }
        }
        // 补齐至 kWidth
        line.append(static_cast<std::size_t>(kWidth - line.size()), ' ');
        result.push_back(std::move(line));
    }
    return result;
}
