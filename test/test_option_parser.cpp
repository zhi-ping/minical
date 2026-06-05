#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <vector>

#include "cal_options.hpp"
#include "flag_descriptor.hpp"
#include "flag_table.hpp"
#include "option_parser.hpp"

namespace {

FlagTable make_table() {
    FlagTable t;
    t.register_flag(FlagDescriptor("-m", ArgKind::Int,
        [](CalOptions& o, const std::string& v) { o.month = std::stoi(v); }));
    t.register_flag(FlagDescriptor("", ArgKind::Int,
        [](CalOptions& o, const std::string& v) { o.year = std::stoi(v); }));
    return t;
}

struct Args {
    std::vector<std::string> tokens;
    std::vector<char*> argv;
    explicit Args(std::vector<std::string> t) : tokens(std::move(t)) {
        for (auto& s : tokens) argv.push_back(s.data());
    }
    int argc() const { return static_cast<int>(argv.size()); }
    char** data() { return argv.data(); }
};

}  // namespace

// ---- FlagTable ----

TEST(FlagTable, FindRegistered) {
    auto t = make_table();
    ASSERT_NE(t.find("-m"), nullptr);
    EXPECT_EQ(t.find("-m")->flag, "-m");
}

TEST(FlagTable, FindPositional) {
    auto t = make_table();
    ASSERT_NE(t.find(""), nullptr);
    EXPECT_EQ(t.find("")->flag, "");
}

TEST(FlagTable, FindUnknown) {
    EXPECT_EQ(make_table().find("--bogus"), nullptr);
}

// ---- OptionParser ----

TEST(OptionParser, ParseFlag) {
    OptionParser p;
    Args a({"cal", "-m", "5"});
    auto opts = p.parse(a.argc(), a.data(), make_table());
    ASSERT_TRUE(opts.month.has_value());
    EXPECT_EQ(*opts.month, 5);
}

TEST(OptionParser, ParseYear) {
    OptionParser p;
    Args a({"cal", "2025"});
    auto opts = p.parse(a.argc(), a.data(), make_table());
    ASSERT_TRUE(opts.year.has_value());
    EXPECT_EQ(*opts.year, 2025);
}

TEST(OptionParser, ParseMonthAndYear) {
    OptionParser p;
    Args a({"cal", "-m", "5", "2025"});
    auto opts = p.parse(a.argc(), a.data(), make_table());
    ASSERT_TRUE(opts.month.has_value());
    ASSERT_TRUE(opts.year.has_value());
    EXPECT_EQ(*opts.month, 5);
    EXPECT_EQ(*opts.year, 2025);
}

TEST(OptionParser, MissingArgThrows) {
    OptionParser p;
    Args a({"cal", "-m"});
    EXPECT_THROW(p.parse(a.argc(), a.data(), make_table()),
                 std::runtime_error);
}

TEST(OptionParser, UnknownFlagThrows) {
    OptionParser p;
    Args a({"cal", "--bogus"});
    EXPECT_THROW(p.parse(a.argc(), a.data(), make_table()),
                 std::runtime_error);
}

TEST(OptionParser, EmptyArgsUsesDefaults) {
    OptionParser p;
    Args a({"cal"});
    auto opts = p.parse(a.argc(), a.data(), make_table());
    EXPECT_FALSE(opts.month.has_value());
    EXPECT_FALSE(opts.year.has_value());
    EXPECT_EQ(opts.after_months, 0);
    EXPECT_EQ(opts.before_months, 0);
}
