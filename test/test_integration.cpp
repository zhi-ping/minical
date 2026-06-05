#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "calendar.hpp"
#include "cal_options.hpp"
#include "flag_descriptor.hpp"
#include "flag_table.hpp"
#include "option_parser.hpp"

namespace {

FlagTable full_table() {
    FlagTable t;
    auto int_f = [](std::string n, auto mp) {
        return FlagDescriptor(std::move(n), ArgKind::Int,
            [mp](CalOptions& o, const std::string& v) { o.*mp = std::stoi(v); });
    };
    t.register_flag(int_f("-A", &CalOptions::after_months));
    t.register_flag(int_f("-B", &CalOptions::before_months));
    t.register_flag(int_f("-r", &CalOptions::rows_per_line));
    t.register_flag(int_f("-m", &CalOptions::month));
    t.register_flag(FlagDescriptor("-d", ArgKind::DateStr,
        [](CalOptions& o, const std::string& v) {
            auto p = v.find('-');
            o.date = {std::stoi(v.substr(0, p)), std::stoi(v.substr(p + 1))};
        }));
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

// ---- 端到端 ----

TEST(Integration, FullYear2025) {
    OptionParser p;
    Calendar cal;
    Args a({"cal", "2025"});
    auto opts = p.parse(a.argc(), a.data(), full_table());
    std::string out = cal.generate(opts, {2025, 5});
    EXPECT_NE(out.find("January"), std::string::npos);
    EXPECT_NE(out.find("December"), std::string::npos);
}

TEST(Integration, SingleMonth) {
    OptionParser p;
    Calendar cal;
    Args a({"cal", "-m", "5"});
    auto opts = p.parse(a.argc(), a.data(), full_table());
    std::string out = cal.generate(opts, {2025, 5});
    EXPECT_NE(out.find("May"), std::string::npos);
    EXPECT_NE(out.find("2025"), std::string::npos);
}

TEST(Integration, RowsPerLine) {
    OptionParser p;
    Args a({"cal", "-r", "4", "2025"});
    auto opts = p.parse(a.argc(), a.data(), full_table());
    EXPECT_EQ(opts.rows_per_line, 4);
}

TEST(Integration, VariousInvocations) {
    OptionParser p;
    Calendar cal;
    std::vector<std::vector<std::string>> cases = {
        {"cal", "2025"},
        {"cal", "-m", "5"},
        {"cal", "-A", "2"},
        {"cal", "-B", "2"},
        {"cal", "-r", "4", "2025"},
        {"cal", "-d", "2025-05"},
        {"cal", "-m", "3", "-A", "1", "-B", "1"},
    };
    for (const auto& c : cases) {
        Args a(c);
        auto opts = p.parse(a.argc(), a.data(), full_table());
        std::string out = cal.generate(opts, {2025, 5});
        EXPECT_FALSE(out.empty());
    }
}

TEST(Integration, ErrorCases) {
    OptionParser p;
    auto t = full_table();
    {
        Args a({"cal", "-m"});
        EXPECT_THROW(p.parse(a.argc(), a.data(), t), std::runtime_error);
    }
    {
        Args a({"cal", "--bogus"});
        EXPECT_THROW(p.parse(a.argc(), a.data(), t), std::runtime_error);
    }
    {
        Args a({"cal", "-d", "bad"});
        EXPECT_THROW(p.parse(a.argc(), a.data(), t), std::runtime_error);
    }
}
