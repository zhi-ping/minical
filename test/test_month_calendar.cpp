#include <gtest/gtest.h>

#include "month_calendar.hpp"

// ---- 构造校验 ----

TEST(MonthCalendar, ConstructValid) {
    EXPECT_NO_THROW(MonthCalendar(2025, 1));
    EXPECT_NO_THROW(MonthCalendar(2025, 12));
    EXPECT_NO_THROW(MonthCalendar(1, 1));
    EXPECT_NO_THROW(MonthCalendar(9999, 12));
}

TEST(MonthCalendar, ConstructInvalidMonth) {
    EXPECT_THROW(MonthCalendar(2025, 0), std::invalid_argument);
    EXPECT_THROW(MonthCalendar(2025, 13), std::invalid_argument);
}

TEST(MonthCalendar, ConstructInvalidYear) {
    EXPECT_THROW(MonthCalendar(0, 1), std::invalid_argument);
    EXPECT_THROW(MonthCalendar(10000, 1), std::invalid_argument);
}

TEST(MonthCalendar, Getters) {
    MonthCalendar mc(2025, 5);
    EXPECT_EQ(mc.year(), 2025);
    EXPECT_EQ(mc.month(), 5);
}

// ---- 标题行 ----

TEST(MonthCalendar, TitleLineWidth) {
    for (int m = 1; m <= 12; ++m) {
        MonthCalendar mc(2025, m);
        EXPECT_EQ(mc.title_line().size(),
                  static_cast<std::size_t>(MonthCalendar::kWidth));
    }
}

TEST(MonthCalendar, TitleContainsMonthAndYear) {
    std::string title = MonthCalendar(2025, 5).title_line();
    EXPECT_NE(title.find("May"), std::string::npos);
    EXPECT_NE(title.find("2025"), std::string::npos);
}

// ---- 表头行 ----

TEST(MonthCalendar, HeaderLineWidth) {
    EXPECT_EQ(MonthCalendar(2025, 1).header_line().size(),
              static_cast<std::size_t>(MonthCalendar::kWidth));
}

TEST(MonthCalendar, HeaderContainsAbbreviations) {
    std::string h = MonthCalendar(2025, 1).header_line();
    EXPECT_NE(h.find("Su"), std::string::npos);
    EXPECT_NE(h.find("Mo"), std::string::npos);
    EXPECT_NE(h.find("Sa"), std::string::npos);
}

// ---- 日期行 ----

TEST(MonthCalendar, WeekLinesWidth) {
    MonthCalendar mc(2025, 5);
    for (const auto& line : mc.week_lines()) {
        EXPECT_EQ(line.size(),
                  static_cast<std::size_t>(MonthCalendar::kWidth));
    }
}

TEST(MonthCalendar, May2025RowCount) {
    // 5 月 1 号是周四 (4), 31 天 → 35 格 → 5 行
    EXPECT_EQ(MonthCalendar(2025, 5).week_lines().size(), 5u);
}

TEST(MonthCalendar, Feb2026RowCount) {
    // 2 月 1 号是周日 (0), 28 天 → 28 格 → 4 行
    EXPECT_EQ(MonthCalendar(2026, 2).week_lines().size(), 4u);
}

TEST(MonthCalendar, FirstDayInFirstRow) {
    auto weeks = MonthCalendar(2025, 5).week_lines();
    bool found = weeks[0].find(" 1") != std::string::npos
              || weeks[0].find("1 ") != std::string::npos;
    EXPECT_TRUE(found);
}

TEST(MonthCalendar, LastDayInLastRow) {
    auto weeks = MonthCalendar(2025, 5).week_lines();
    EXPECT_NE(weeks.back().find("31"), std::string::npos);
}

TEST(MonthCalendar, LeapYearFeb) {
    auto weeks = MonthCalendar(2020, 2).week_lines();
    bool found = false;
    for (const auto& line : weeks) {
        if (line.find("29") != std::string::npos) { found = true; break; }
    }
    EXPECT_TRUE(found);
}
