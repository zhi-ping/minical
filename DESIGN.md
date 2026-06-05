# lab7 cal 命令设计文档

## 1. 类设计

### 1.1 类图

```
┌──────────────────────────────────────────────┐
│              FlagDescriptor                   │
│──────────────────────────────────────────────│
│  + flag:     string                          │
│  + arg_kind: ArgKind { None, Int, DateStr }  │
│  + apply:    fn(CalOptions&, string)         │
│──────────────────────────────────────────────│
│  （每个 FlagDescriptor 自描述其名称、        │
│    需要的参数类型、以及如何写入 CalOptions）  │
└──────────────────────────────────────────────┘
                    │
                    │ 注册到
                    ▼
┌──────────────────────────────────────────────┐
│               FlagTable                       │
│──────────────────────────────────────────────│
│  + register_flag(FlagDescriptor)             │
│  + find(token) → FlagDescriptor*             │
│──────────────────────────────────────────────│
│  - flags_: vector<FlagDescriptor>            │
│  （纯容器，只负责存储和查找，不包含解析逻辑） │
└──────────────────────────────────────────────┘
                    │
                    │ 驱动
                    ▼
┌──────────────────────────────────────────────┐
│              OptionParser                     │
│──────────────────────────────────────────────│
│  + parse(argc, argv, table) → CalOptions     │
│──────────────────────────────────────────────│
│  （一个 15 行的通用循环，遍历 argv，          │
│    用 table.find() 匹配 flag，委托 apply()   │
│    写入，不关心具体有哪些 flag）              │
└──────────────────────────────────────────────┘
                    │
                    │  creates
                    ▼
┌──────────────────────────────────────────────┐
│               CalOptions                      │
│──────────────────────────────────────────────│
│  + after_months: int      = 0                │
│  + before_months: int     = 0                │
│  + month: optional<int>   = nullopt          │
│  + year: optional<int>    = nullopt          │
│  + date: optional<pair<int,int>> = none      │
│  + rows_per_line: int     = 3                │
│──────────────────────────────────────────────│
│  + resolve(today) → list<YearMonth>          │
└──────────────────────────────────────────────┘
                    │
                    │  drives
                    ▼
┌─────────────────────────────────────────┐
│               Calendar                   │
│─────────────────────────────────────────│
│  + generate(opts: CalOptions) → string  │
│─────────────────────────────────────────│
│  - group_by_rows(months, n)             │
│  - render_row(months) → string           │
└─────────────────────────────────────────┘
                    │
                    │  1..*
                    ▼
┌─────────────────────────────────────────┐
│             MonthCalendar                │
│─────────────────────────────────────────│
│  - year_:  int                          │
│  - month_: int                          │
│─────────────────────────────────────────│
│  + MonthCalendar(year, month)           │
│  + title_line()   → string              │
│  + header_line()  → string              │
│  + week_lines()   → vector<string>      │
│─────────────────────────────────────────│
│  - days_in_month()  → int               │
│  - first_weekday()  → int               │
│  - month_name()     → string            │
└─────────────────────────────────────────┘
```

### 1.2 各类职责

| 类 | 单一职责 |
|----|---------|
| `FlagDescriptor` | 描述一个命令行 flag 的元数据：名称、参数类型、如何写入 `CalOptions` |
| `FlagTable` | 纯容器，存储所有 `FlagDescriptor`，提供按名称查找 |
| `OptionParser` | 通用解析循环：遍历 `argv`，查表匹配 flag，委托 `apply()` 写入，**不关心具体有哪些 flag** |
| `CalOptions` | 存储用户选项的纯数据结构，提供 `resolve()` 方法将选项展开为具体要显示的月份列表 |
| `MonthCalendar` | 给定年、月，生成单个月历的文本块（title行 + header行 + 6行日期） |
| `Calendar` | 接受月份列表、分组、逐排横向拼接，生成最终输出字符串 |

---

## 2. 类关系图

```
   main()
     │
     │ ① 构造 FlagTable（注册所有 flag 的元数据）
     ▼
 FlagDescriptor × N ──► FlagTable
                              │
     │ ② argv + table 传入       │
     ▼                         │
 OptionParser ──────────────────┘
     │
     │ ③ 返回 CalOptions
     ▼
 CalOptions ──► resolve() ──► list<YearMonth>
                                 │
                                 │ ④ 每个 YearMonth 构造一个 MonthCalendar
                                 ▼
                           MonthCalendar × N
                                 │
                                 │ ⑤ 每个吐出等宽文本块
                                 ▼
                            Calendar (编排+拼接)
                                 │
                                 │ ⑥ 最终字符串
                                 ▼
                            std::print()
```

**关系类型：**
- `FlagDescriptor` → `FlagTable`：**聚合**（FlagTable 持有多个 FlagDescriptor）
- `OptionParser` → `FlagTable`：**依赖**（接收 FlagTable 作为参数，只调用 find()）
- `FlagDescriptor.apply` → `CalOptions`：**依赖**（通过回调写入 CalOptions 的字段）
- `OptionParser` → `CalOptions`：**创建**（解析过程中构造并返回）
- `Calendar` → `MonthCalendar`：**聚合**（1 对多）
- `Calendar` → `CalOptions`：**依赖**（接收 CalOptions 作为参数）

---

## 3. 用例图

```
                        ┌──────────────────────────────┐
                        │        cal 命令系统            │
                        │                               │
    ┌──────┐            │  ┌──────────────────────────┐ │
    │ User │───────────►│  │  解析命令行参数            │ │
    └──────┘            │  │  (OptionParser)           │ │
          │             │  └─────────┬────────────────┘ │
          │             │            │                   │
          │             │            ▼                   │
          │             │  ┌──────────────────────────┐ │
          ├────────────►│  │  确定显示范围              │ │
          │             │  │  (CalOptions::resolve)    │ │
          │             │  └─────────┬────────────────┘ │
          │             │            │                   │
          │             │            ▼                   │
          │             │  ┌──────────────────────────┐ │
          ├────────────►│  │  生成月历                  │ │
          │             │  │  (MonthCalendar +         │ │
          │             │  │   Calendar)               │ │
          │             │  └─────────┬────────────────┘ │
          │             │            │                   │
          │             │            ▼                   │
          │             │  ┌──────────────────────────┐ │
          └────────────►│  │  输出格式化结果            │ │
                        │  │  (std::print)             │ │
                        │  └──────────────────────────┘ │
                        └──────────────────────────────┘
```

**用例说明：**

| 用例 | 描述 |
|------|------|
| 解析命令行参数 | 支持 -A, -B, -d, -r, -m, yyyy 共 6 个参数 |
| 确定显示范围 | 根据参数展开为具体的 (年, 月) 列表 |
| 生成月历 | 对每个月生成标题、表头、日期行；横向拼接成一排 |
| 输出结果 | 用 `std::print` / `std::println` 输出到终端 |

---

## 4. 状态图（执行流程）

```
         ┌────────┐
         │ 开始    │
         └───┬────┘
             │
             ▼
     ┌───────────────────────────┐
     │ OptionParser::parse()      │
     │ 遍历 argv，逐参数解析       │
     │ 设置 CalOptions 各字段      │◄──── 循环
     │ 参数错误？→ stderr + exit   │
     └───────────┬───────────────┘
                 │
                 ▼
     ┌───────────────────────────┐
     │ CalOptions::resolve()      │
     │ ┌─────────────────────────┐│
     │ │ 取默认值：today.year     ││
     │ │          today.month    ││
     │ │ 覆盖：-m, -d, yyyy       ││
     │ │ 展开：-A / -B 决定范围   ││
     │ └─────────────────────────┘│
     │ 输出 list<YearMonth>       │
     └───────────┬───────────────┘
                 │
                 ▼
     ┌───────────────────────────┐
     │ Calendar::generate()       │
     │ ┌─────────────────────────┐│
     │ │ 按月分组 (每排N个)       ││
     │ └───────────┬─────────────┘│
     │             ▼              │
     │ ┌─────────────────────────┐│
     │ │ for each 排:             ││
     │ │   for each 月 in 排:    ││
     │ │     MonthCalendar(y,m)  ││
     │ │     → 取 title/header   ││
     │ │         /week_lines()   ││
     │ │   横向拼成一行            ││
     │ │   追加到输出              ││
     │ └─────────────────────────┘│
     └───────────┬───────────────┘
                 │
                 ▼
     ┌───────────────────────────┐
     │ std::println(result)       │
     └───────────┬───────────────┘
                 │
                 ▼
         ┌────────┐
         │ 结束    │
         └────────┘
```

---

## 5. 核心算法伪代码

### 5.1 参数解析：FlagDescriptor 模式

**核心思想：** 每个 flag 只有四个属性不同（名称、参数类型、写哪个字段、怎么转换），把它们封装成 `FlagDescriptor` 对象。解析循环不关心具体有哪些 flag，只负责"查表 → 委托"。

#### 5.1.1 注册 flag（声明式定义）

```
build_cal_flags():
    table = FlagTable()

    // 辅助工厂: 生成"取下一个 int 参数"的 flag
    int_flag = λ(name, CalOptions::member*):
        FlagDescriptor(name, ArgKind::Int,
                       λ(opts, val): opts.*member = stoi(val))

    // 标志参数: -A, -B, -r, -m 结构完全一致
    table.register(int_flag("-A", &CalOptions::after_months))
    table.register(int_flag("-B", &CalOptions::before_months))
    table.register(int_flag("-r", &CalOptions::rows_per_line))
    table.register(int_flag("-m", &CalOptions::month))

    // -d 参数: 值类型特殊 (yyyy-mm)
    table.register(FlagDescriptor("-d", ArgKind::DateStr,
        λ(opts, val):
            pos = val.find('-')
            opts.date = (stoi(val[:pos]), stoi(val[pos+1:]))
    ))

    // 位置参数: 年份，flag 名为空串 ""
    table.register(FlagDescriptor("", ArgKind::Int,
        λ(opts, val): opts.year = stoi(val)))

    return table
```

#### 5.1.2 解析循环（通用逻辑，不随 flag 增加而膨胀）

```
parse_args(argc, argv, table):
    opts = CalOptions()
    i = 1
    while i < argc:
        token = argv[i]

        // 1. 按 flag 名查表
        desc = table.find(token)
        if desc:
            if desc.arg_kind != None:
                i++
                if i >= argc: error("missing argument after", token)
                arg = argv[i]
            // 2. 委托 apply() — 不关心具体写了 CalOptions 的哪个字段
            desc.apply(opts, arg)
        else if isdigit(token[0]):
            // 3. 位置参数: 交给 flag="" 的 descriptor
            table.find("").apply(opts, token)
        else:
            error("unknown option:", token)
        i++

    return opts
```

#### 5.1.3 设计对比

```
坏设计:
  每个 flag 一个 if-else 分支  →  200 行重复代码
  加新 flag → 改 parse 函数     →  违反开闭原则
  参数校验散落各处              →  难以维护

好设计 (FlagDescriptor):
  每个 flag 一个对象            →  每个对象 ~5 行
  加新 flag → table.register()  →  不碰循环逻辑
  flag 和校验封装在 apply 中    →  就近原则，一目了然
```

### 5.2 月历范围确定

```
resolve(opts, today):
    y = opts.year  ?? opts.date.year  ?? today.year
    m = opts.month ?? opts.date.month ?? today.month
    // 如果用户只指定了年份（没有指定月份），显示全年 1~12 月
    if only_year_specified:
        start = (y, 1), count = 12
    else:
        start = (y - (m-1)/12, m - opts.before_months)  // 考虑跨年
        count = 1 + opts.before_months + opts.after_months
    return [start_month, start_month+1, ..., start_month+count-1]
```

### 5.3 单月月历生成

```
generate_month_calendar(year, month):
    title = center(month_name + " " + year, 21)   // "     May 2025"
    header = "Su Mo Tu We Th Fr Sa"
    
    first_wd = weekday_of(year, month, 1)   // 0=Sun
    total_days = days_in_month(year, month)
    
    rows = []
    current_row = ""
    
    // 填充前导空格
    for i = 0 to first_wd - 1:  current_row += "   "
    
    // 填充日期
    for day = 1 to total_days:
        current_row += format("{:>3}", day)
        if (first_wd + day) % 7 == 0:   // 行满
            rows.push(current_row)
            current_row = ""
    
    // 尾部补齐
    if current_row != "":  current_row.pad_to(21)
    
    return {title, header, rows[0..5]}
```

### 5.4 横向拼接

```
render_row(months[]):
    // 每个月的文本块（title + header + 最多6行日期 = 最多8行）
    blocks = []
    max_lines = 0
    for m in months:
        block = [m.title(), m.header()] + m.week_lines()
        max_lines = max(max_lines, block.size())
        blocks.push(block)
    
    // 逐行拼接
    output = ""
    for line = 0 to max_lines - 1:
        for block in blocks:
            if line < block.size():
                output += block[line]
            else:
                output += spaces(21)      // 补齐空行
            output += "   "              // 月间距
        output += "\n"
    return output
```

---

## 6. SOLID 原则应用

| 原则 | 应用方式 |
|------|---------|
| **S - 单一职责** | 6 个类各司其职：`FlagDescriptor` 描述元数据、`FlagTable` 存储和查找、`OptionParser` 通用循环、`CalOptions` 存储结果、`MonthCalendar` 单月生成、`Calendar` 编排拼接 |
| **O - 开闭原则** | 加新参数只需 `table.register_flag(...)` 一行，**不改 OptionParser 循环代码**；加新输出格式只需扩展 `Calendar`，不碰 `MonthCalendar` |
| **L - 里氏替换** | 暂无继承层次，不适用。若后续扩展 flag 类型可引入 `FlagDescriptor` 子类 |
| **I - 接口隔离** | `FlagTable` 只暴露 `find()` 和 `register()`；`MonthCalendar` 只暴露 `title()` / `header()` / `week_lines()` |
| **D - 依赖倒置** | `OptionParser` 依赖 `FlagTable`（抽象容器），不依赖具体 flag；`Calendar` 依赖 `MonthCalendar` 接口

---

## 7. 关于 AI 设计的批评性意见

（此处留空，等你实际使用 AI 工具完成设计后填写）

---

## 8. 参考资料

- [1] OOD: https://zh.wikipedia.org/wiki/面向对象程序设计
- [2] SOLID: https://de.wikipedia.org/wiki/Solid
- [3] fmt (备选): https://github.com/fmtlib/fmt — 若使用 C++23 的 `<format>` 和 `<print>`，则无需依赖
