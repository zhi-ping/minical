#include <chrono>
#include <cstdlib>
#include <print>
#include <stdexcept>
#include <string>

#include "calendar.hpp"
#include "cal_options.hpp"
#include "flag_descriptor.hpp"
#include "flag_table.hpp"
#include "option_parser.hpp"

// ---------------------------------------------------------------------------
// 帮助信息
// ---------------------------------------------------------------------------
void print_help(std::string_view prog_name) {
    std::println("用法: {} [选项] [年份]", prog_name);
    std::println("");
    std::println("命令行日历工具，显示文本格式的月历。");
    std::println("");
    std::println("选项:");
    std::println("  -A N         之后 N 个月（不含当前月，N ≥ 0）");
    std::println("  -B N         之前 N 个月（不含当前月，N ≥ 0）");
    std::println("  -d yyyy-mm   指定年月（如 2025-06）");
    std::println("  -m N         指定月份（1–12）");
    std::println("  -r N         每排显示月数（默认 3）");
    std::println("  -h, --help   打印本帮助信息");
    std::println("");
    std::println("示例:");
    std::println("  {}             显示当月", prog_name);
    std::println("  {} 2025        显示 2025 全年", prog_name);
    std::println("  {} -m 5        显示今年 5 月", prog_name);
    std::println("  {} -A 2 -B 1   显示前 1 月 + 当月 + 后 2 月", prog_name);
    std::println("  {} -r 4 2025   显示 2025 全年，每排 4 个月", prog_name);
}

// ---------------------------------------------------------------------------
// 构建 flag 表 —— 唯一知道具体有哪些 flag 的地方。
// 新增一个 flag 只需在这里加一行 register_flag() 调用，代码库其余部分
// 完全无需修改（开闭原则）。
// ---------------------------------------------------------------------------
FlagTable build_flag_table() {
    FlagTable table;

    // 辅助工厂：创建将整数值写入 CalOptions 某个 int 字段的 flag
    auto int_flag = [](std::string name, auto member_ptr) {
        return FlagDescriptor(
            std::move(name), ArgKind::Int,
            [member_ptr](CalOptions& opts, const std::string& val) {
                int n = std::stoi(val);
                if (n < 0) {
                    throw std::invalid_argument("值不能为负数");
                }
                opts.*member_ptr = n;
            });
    };

    // ----- 帮助 flag --------------------------------------------------------
    auto help_flag = [](std::string name) {
        return FlagDescriptor(
            std::move(name), ArgKind::None,
            [](CalOptions& opts, const std::string& /*val*/) {
                opts.show_help = true;
            });
    };
    table.register_flag(help_flag("-h"));
    table.register_flag(help_flag("--help"));

    // ----- 命名 flag --------------------------------------------------------
    table.register_flag(int_flag("-A", &CalOptions::after_months));
    table.register_flag(int_flag("-B", &CalOptions::before_months));
    table.register_flag(int_flag("-r", &CalOptions::rows_per_line));
    table.register_flag(int_flag("-m", &CalOptions::month));

    // -d yyyy-mm  →  解析为 CalOptions::date
    table.register_flag(FlagDescriptor(
        "-d", ArgKind::DateStr,
        [](CalOptions& opts, const std::string& val) {
            auto dash = val.find('-');
            if (dash == std::string::npos || dash == 0 || dash == val.size() - 1) {
                throw std::invalid_argument(
                    "需要 yyyy-mm 格式，收到 '" + val + "'");
            }
            int y = std::stoi(val.substr(0, dash));
            int m = std::stoi(val.substr(dash + 1));
            if (m < 1 || m > 12) {
                throw std::invalid_argument(
                    "月份必须在 [1, 12] 范围内，收到 " + std::to_string(m));
            }
            opts.date = std::make_pair(y, m);
        }));

    // ----- 位置参数：裸年份 yyyy --------------------------------------------
    table.register_flag(FlagDescriptor(
        "", ArgKind::Int,
        [](CalOptions& opts, const std::string& val) {
            int y = std::stoi(val);
            if (y < 1 || y > 9999) {
                throw std::invalid_argument(
                    "年份必须在 [1, 9999] 范围内，收到 " + std::to_string(y));
            }
            opts.year = y;
        }));

    return table;
}

// ---------------------------------------------------------------------------
// 从系统时钟获取今天的 (年, 月)
// ---------------------------------------------------------------------------
std::pair<int, int> today_ym() {
    auto now = std::chrono::system_clock::now();
    auto local_days = std::chrono::floor<std::chrono::days>(now);
    auto ymd = std::chrono::year_month_day{local_days};
    return {static_cast<int>(ymd.year()),
            static_cast<unsigned>(ymd.month())};
}

// ---------------------------------------------------------------------------
// 主函数
// ---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    try {
        // 1. 构建 flag 注册表
        FlagTable table = build_flag_table();

        // 2. 解析命令行参数
        OptionParser parser;
        CalOptions opts = parser.parse(argc, argv, table);

        // 3. -h / --help 时打印帮助信息并退出
        if (opts.show_help) {
            print_help(argv[0]);
            return EXIT_SUCCESS;
        }

        // 4. 展开为具体月份列表
        auto today = today_ym();
        std::vector<std::pair<int, int>> months = opts.resolve(today);

        // 5. 生成月历
        Calendar cal;
        std::string output = cal.generate(opts, today);

        // 6. 输出
        std::print("{}", output);

        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::println(stderr, "cal: {}", e.what());
        return EXIT_FAILURE;
    }
}
