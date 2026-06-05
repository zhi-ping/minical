#pragma once

#include <string>
#include <vector>

/// 生成单个月的格式化文本块。
///
/// 给定年份和月份，此类计算标题行（如 "      May 2025"）、
/// 星期表头（"Su Mo Tu We Th Fr Sa"）以及 4–6 行日期。
/// 每行恰好 kWidth 个字符宽，以便多个 MonthCalendar 块可以并排放置。
class MonthCalendar {
public:
    /// 本类生成的所有行的规范宽度。
    static constexpr int kWidth = 21;

    /// 为给定的 1-based 月份构造月历块。
    /// @param year  年份，如 2025
    /// @param month 月份（1=一月, 12=十二月）
    MonthCalendar(int year, int month);

    [[nodiscard]] int year() const { return year_; }
    [[nodiscard]] int month() const { return month_; }

    /// "     May 2025" 格式的标题行（居中，恰好 kWidth 列）
    [[nodiscard]] std::string title_line() const;

    /// "Su Mo Tu We Th Fr Sa" 星期缩写表头
    [[nodiscard]] std::string header_line() const;

    /// 4–6 行日期字符串，每行恰好 kWidth 列。
    /// 部分填满的行会用空格补齐。
    [[nodiscard]] std::vector<std::string> week_lines() const;

private:
    int year_;
    int month_;

    /// 本月的天数（28–31）。
    [[nodiscard]] int days_in_month() const;

    /// 本月 1 号是星期几（0=周日 … 6=周六）。
    [[nodiscard]] int first_weekday() const;

    /// 英文月份名称（如 "January"）。
    [[nodiscard]] std::string month_name() const;
};
