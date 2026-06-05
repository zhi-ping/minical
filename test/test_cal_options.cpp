#include <gtest/gtest.h>

#include "cal_options.hpp"

using YM = std::pair<int, int>;

// ---- 仅指定年份 — 显示全年 ----

TEST(CalOptions, OnlyYearFullYear) {
    CalOptions o;
    o.year = 2025;
    auto v = o.resolve({2025, 5});
    ASSERT_EQ(v.size(), 12u);
    EXPECT_EQ(v[0],  YM(2025, 1));
    EXPECT_EQ(v[11], YM(2025, 12));
}

// ---- 仅指定月份 — 使用当年 ----

TEST(CalOptions, OnlyMonthUsesCurrentYear) {
    CalOptions o;
    o.month = 5;
    auto v = o.resolve({2025, 6});
    ASSERT_EQ(v.size(), 1u);
    EXPECT_EQ(v[0], YM(2025, 5));
}

// ---- -A ----

TEST(CalOptions, AfterMonths) {
    CalOptions o;
    o.month = 5;
    o.after_months = 2;
    auto v = o.resolve({2025, 5});
    ASSERT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], YM(2025, 5));
    EXPECT_EQ(v[1], YM(2025, 6));
    EXPECT_EQ(v[2], YM(2025, 7));
}

// ---- -B ----

TEST(CalOptions, BeforeMonths) {
    CalOptions o;
    o.month = 5;
    o.before_months = 2;
    auto v = o.resolve({2025, 5});
    ASSERT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], YM(2025, 3));
    EXPECT_EQ(v[1], YM(2025, 4));
    EXPECT_EQ(v[2], YM(2025, 5));
}

TEST(CalOptions, BeforeAndAfter) {
    CalOptions o;
    o.month = 5;
    o.before_months = 1;
    o.after_months = 1;
    auto v = o.resolve({2025, 5});
    ASSERT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], YM(2025, 4));
    EXPECT_EQ(v[1], YM(2025, 5));
    EXPECT_EQ(v[2], YM(2025, 6));
}

// ---- 跨年 ----

TEST(CalOptions, BeforeCrossYear) {
    CalOptions o;
    o.month = 1;
    o.before_months = 2;
    auto v = o.resolve({2025, 1});
    ASSERT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], YM(2024, 11));
    EXPECT_EQ(v[1], YM(2024, 12));
    EXPECT_EQ(v[2], YM(2025, 1));
}

TEST(CalOptions, AfterCrossYear) {
    CalOptions o;
    o.month = 12;
    o.after_months = 2;
    auto v = o.resolve({2025, 12});
    ASSERT_EQ(v.size(), 3u);
    EXPECT_EQ(v[0], YM(2025, 12));
    EXPECT_EQ(v[1], YM(2026, 1));
    EXPECT_EQ(v[2], YM(2026, 2));
}

// ---- -d 与 -m 优先级 ----

TEST(CalOptions, MonthOverridesDateMonth) {
    CalOptions o;
    o.date = {2025, 3};
    o.month = 5;
    auto v = o.resolve({2025, 5});
    ASSERT_EQ(v.size(), 1u);
    EXPECT_EQ(v[0], YM(2025, 5));  // -m 优先于 -d 的月份
}

// ---- 无参数 — 显示当月 ----

TEST(CalOptions, NoArgsCurrentMonth) {
    CalOptions o;
    auto v = o.resolve({2025, 12});
    ASSERT_EQ(v.size(), 1u);
    EXPECT_EQ(v[0], YM(2025, 12));
}
