# Artistic Style (astyle) 插件 —— Qt Creator 20.0.2

A plugin for Qt Creator used to format source files with
[Artistic Style](https://astyle.sourceforge.net/).

把 [Artistic Style](https://astyle.sourceforge.net/)（下称 AStyle）以**静态库**方式内嵌进 Qt Creator，
不依赖任何外部可执行文件、不需要临时文件、不 fork 进程，直接对**当前文件**或**当前工程的全部源文件**
做格式化。

- 参考工程：<https://gitee.com/zhouliqiu/qtc_astyle>（AStyle 2.04 + Qt Creator 8 时代的代码，已无法在新版下编译）
- AStyle 上游源码：<https://gitlab.com/saalen/astyle>
- 本插件内嵌版本：**AStyle 3.6.20**（MIT 许可），提交见 `3rdparty/astyle/VERSION.txt`
- 仓库地址：<https://github.com/electri/qtc-astyle>

> **仓库根目录 = Qt Creator 源码树里的 `src/plugins/astyle/`。**
> 插件必须和 IDE 一起编译（`add_qtc_plugin` 宏、同 Qt、同编译器），所以用法是把它放进源码树：

```bash
cd <qtcreator-source>
git clone https://github.com/electri/qtc-astyle.git src/plugins/astyle

# 确认 src/plugins/CMakeLists.txt 里有这一行（按字母序插入）
grep -q 'add_subdirectory(astyle)' src/plugins/CMakeLists.txt \
  || sed -i '/add_subdirectory(autotest)/i add_subdirectory(astyle)' src/plugins/CMakeLists.txt
```

然后按第 2 节重新 cmake 配置 + 构建 `Astyle` 目标即可。

---

## 1. 目录结构

```
（仓库根 = 源码树的 src/plugins/astyle/）
├── CMakeLists.txt                     # add_qtc_plugin(Astyle ...)
├── Astyle.json.in                     # 插件元数据（版本/依赖由 CMake 填充）
├── astyle.qrc                         # 设置分类图标
├── astyleconstants.h                  # 菜单/动作/设置页 ID
├── astyletr.h
├── astyleformatter.h/.cpp             # 核心：调用 AStyle 库、BOM/编码/换行处理、选项校验
├── astyleoptionhelp.h/.cpp            # 每个选项的悬浮提示（由官方手册生成，含 Before/After 对照）
├── astylesettings.h/.cpp              # 选项页（AspectContainer）+ .astylerc 查找 + 选项拼装
├── astyleplugin.cpp                   # IPlugin：菜单、动作、单文件格式化
├── astyleprojectdialog.h/.cpp         # 整工程格式化对话框：文件列表/勾选/进度/逐文件结果
├── images/settingscategory_astyle.png
├── tools/gen_option_help.py           # 从 AStyle 官方手册重新生成上面的提示文本
├── tools/fill_translations.py         # 译文表 + 把译文回填进 .ts
├── tools/merge_into_qtcreator_ts.py   # 把插件的 .ts 上下文合并进 qtcreator_zh_CN.ts
├── translations/astyle_zh_CN.ts       # 中文文案（上下文 QtC::Astyle，282 条）
├── tests/standalone/                  # 独立的核心单元测试（不参与 Qt Creator 构建）
└── 3rdparty/astyle/                   # AStyle 源码（ASTYLE_LIB 静态库，AstyleLib 目标）
```

---

## 2. 构建

插件在 Qt Creator 源码树内构建，与 IDE 同 Qt / 同编译器，这是唯一可靠的发布方式。

```bash
# 已配置好的构建目录
cmake --build <build-dir> --target Astyle
```

Windows 下需要 MSVC 环境：从 **x64 Native Tools Command Prompt for VS** 里执行上面的命令，
或先在普通 cmd 里 `call "<VS>\VC\Auxiliary\Build\vcvars64.bat"`。

产物：

```
<build-dir>/lib/qtcreator/plugins/Astyle.dll
```

安装（覆盖到已安装的 IDE 也可）：

```
<QtCreator>/lib/qtcreator/plugins/Astyle.dll
```

启动后可在 *Help > About Plugins* 中看到 “Artistic Style”。

> **插件 Id 必须全小写**：`Astyle.json.in` 里的 `"Id"` 是 `astyle`（目标名 / DLL 文件名仍是 `Astyle`，
> 二者无需一致）。Qt Creator 的 `PluginSpec` 会在 `id.isLower()` 校验失败时直接拒绝加载，
> 弹窗提示 `Plugin id "Astyle" must be lowercase`。
>
> 另外：`Q_PLUGIN_METADATA(FILE "Astyle.json")` 只在 **moc 重跑**时才会重新读取 json，
> 所以只改 `Astyle.json.in` 后必须顺手 `touch astyleplugin.cpp` 再构建，否则 ninja 会认为无事可做。

### 核心单元测试（可选）

```bash
cmake -S tests/standalone -B build/astest -G Ninja \
      -DCMAKE_PREFIX_PATH=<Qt 6 prefix>
cmake --build build/astest
build/astest/astyle-core-test
```

测试直接编译插件里的 `astyleformatter.cpp`，覆盖：

- 设置页可能产生的**每一个** AStyle 开关都能被库识别（防止拼写错误导致运行期静默失败）
- CRLF / LF / 末行无换行符的处理，行尾风格保持
- GBK、UTF-8、UTF-8 BOM、UTF-16 LE BOM 格式化前后字节完整
- 二次格式化幂等（同一文件反复格式化结果不变）

---

## 3. 使用

菜单 **Tools > Artistic Style**：

| 菜单项 | 说明 | 快捷键 |
| --- | --- | --- |
| Format Current File | 格式化当前编辑器里的整个文件，**保留光标、折叠、滚动位置，可 Ctrl+Z 撤销** | `Ctrl+Alt+F` |
| Format Current Project... | 弹出**工程文件列表对话框**，勾选要格式化的文件后执行，直接写盘 | — |
| Artistic Style Options... | 打开本插件的选项页 | — |

选项页位置：**Edit > Preferences > Artistic Style**（左侧独立分类）。

### 整工程格式化对话框

`Format Current Project...` 不再是“一句确认框全量格式化”，而是先列出工程里所有匹配扩展名的源文件：

```
┌───────────────────────────────────────────────────────────────┐
│ 项目：[ plugin.pro            ▼ ]   ← 打开多个工程时才出现      │
│ 筛选：[                        ]   ← 按文件名或目录过滤          │
├───────────────────────────────────────────────────────────────┤
│ ☑ **头文件 (12)**                                             │
│    ☑ astyleformatter.h     src/plugins/astyle                 │
│    ☑ astyleoptionhelp.h    src/plugins/astyle    有未保存的更改 │
│ ☑ **源文件 (9)**                                              │
│    ☑ astyleplugin.cpp      src/plugins/astyle                 │
│ ☐ **其他文件 (2)**                                            │
│    ☐ astyle.qrc            src/plugins/astyle                 │
├───────────────────────────────────────────────────────────────┤
│ [██████░░░░░░]                    已选择 21 / 23 个文件        │
│                              [格式化] [停止] [关闭]            │
└───────────────────────────────────────────────────────────────┘
```

- 按**头文件 / 源文件 / 其他文件**分组，组节点三态勾选（`Qt::PartiallyChecked`），点组即全选/全不选；
- 第三列实时显示每个文件的结果：`已改变` / `未改变` / `已跳过：<原因>` / `失败：<原因>` / `未处理`；
- 运行中可 `停止`（`QFuture::cancel()`，已完成的结果保留），此时 `关闭` 先禁用；
- 有未保存修改的文件会在列表里先标出来；点“格式化”时再询问 **保存并格式化 / 跳过这些文件 / 取消**，
  选“跳过”的文件会被自动取消勾选，绝不覆盖未保存的编辑器内容；
- 目录列显示**相对工程根目录**的路径，工程外的文件退化为绝对路径。

设计参考 `gitee.com/zhouliqiu/qtc_astyle` 的 `CDlgFormatProject`（分组 + 勾选 + 状态列 + 进度），
但文件来源改为 IDE 的 `Project::files(Project::SourceFiles)`——**不依赖 qmake 私有 API**，
CMake / qmake / Meson 工程都能用；进度和状态直接从 `QFutureWatcher::resultReadyAt()` 取，不再用独立线程类。

### 选项页

插件**只格式化 C++**，所以选项页只保留对 C++ 有意义的开关：所有源文件都以 `--mode=c`
（AStyle 里的 C/C++ 模式）交给库，Java / C# / Objective-C / JS 模式与它们的专属选项
（`--mode=java`、`--pad-method-colon`、`--join-lines` 之类）都不再出现。选项页按功能分了 7 页：

| 分页 | 内容 |
| --- | --- |
| General | 预定义风格（`--style=`）、缩进方式与宽度、强制制表符宽度、续行缩进、条件语句最小缩进、续行最大缩进、最大行宽与度量方式、指针/引用对齐 |
| Indentation | 类、访问修饰符、switch、case、namespace、括号后缩进、标签、第 1 列注释、预处理块/宏定义/条件编译的缩进，Tab 转空格 |
| Spacing | 运算符、逗号、圆括号内/外/首个/空括号、函数头的填充；去掉括号与分号前的多余空格；`#include` 与 `!` 的填充方式 |
| Braces | namespace/class/inline/extern "C"/do-while 的括号附着、收尾括号断行、增删括号、返回类型的附着与断行 |
| Line Breaks | 块间空行、单行头断行、`else if` 断行（含“不额外缩进”）、逻辑运算符后断行、类成员间空行、保留单行块/语句 |
| Blank Lines and Misc | 删除函数内空行、用缩进填充空行、连续空行上限、合并模板右尖括号、去掉块注释的 `*` 前缀 |
| Files and Configuration | 参与整工程格式化的扩展名、`.astylerc` 支持、指定配置文件、附加自由选项 |

一共覆盖 AStyle 库模式下 **65 个对 C++ 有效**的开关；剩下没进界面的开关及原因见第 7 节。

### 悬浮提示

每个勾选框（以及 `Align pointers` / `Align references` 的每个条目）的悬浮提示包含：

1. 选项名与命令行短写，如 `--indent-switches / -S`；
2. **Artistic Style 官方手册原文**对该选项作用的描述；
3. 手册给出的**代码对照**，左右并排：左边 `Before`、右边 `After`，一眼看出勾选后的差异。

例如 `Indent switch blocks` 的提示：

```
--indent-switches / -S
Indent 'switch' blocks so that the 'case X:' statements are indented in the
switch block. The entire case block is indented.

Before                         After
switch (foo)                   switch (foo)
{                              {
case 1:                            case 1:
    a += 1;                            a += 1;
    break;                             break;
...
```

提示文本不是手写的，而是 `tools/gen_option_help.py` 从 **AStyle 官方手册**解析生成的，
落在 `astyleoptionhelp.cpp`。手册比内置的 3.6.20 略新，所以脚本只取手册的**说明文字与示例**，
选项拼写一律用内置库 `ASOptions::parseOption()` 真正接受的写法
（例如手册里的 `--no-indent-if-after-else` 在本库中仍是 `--break-elseifs=no-indent`）。
升级 AStyle 或手册时重新跑一次即可：

```bash
curl -sSLo /tmp/astyle.html https://astyle.sourceforge.net/astyle.html
python tools/gen_option_help.py /tmp/astyle.html astyleoptionhelp.cpp
```

### 选项生效顺序（后者覆盖前者）

1. 选项页里的开关
2. 语言模式：**固定追加 `--mode=c`**（插件只做 C++；后缀只决定哪些文件参与格式化，不影响模式）
3. `.astylerc`：从**目标文件所在目录逐级向上**查找 `.astylerc` / `astylerc`（会一直找到磁盘根和主目录）
4. 指定的配置文件（勾选 “Use specific configuration file” 时）
5. “Additional options” 里手写的开关

因此工程里放一份 `.astylerc` 就能覆盖全局偏好，而 `Additional options` 永远拥有最终决定权。

---

## 4. 实现要点

**为什么不用 `ASConsole` / 外部进程？**
`ASTYLE_LIB` 模式下编译的 AStyle 只保留库 API：`ASFormatter`、`ASBeautifier`、`ASEnhancer`、`ASOptions`，
控制台前端（`main()`、`ASConsole`、`--suffix`/`--recursive` 等文件管理选项）被裁掉。插件通过官方 C 入口
`AStyleMain()` 调用，选项串由 `ASOptions` 解析——**与命令行完全一致的选项语法**，包括 `.astylerc` 的注释与空白规则。

**编码与换行**
- 所有磁盘文件按**原始字节**送进库、原样取回，因此 GBK 等双字节编码不会损坏（AStyle 只增删 ASCII 空白/换行）。
- UTF-8 BOM：剥离后格式化、结果再补回，避免 BOM 参与首行缩进计算。
- UTF-16 LE/BE BOM：先转 UTF-8 格式化，再按原字节序写回（`AStyleMain` 只接受窄字符串）。
- 行尾风格默认跟随原文件（AStyle 的 `LINEEND_DEFAULT`，逐行沿用输入的行尾），CRLF 工程不会被改成 LF。

**单文件格式化**
`TextEditor::updateEditorText()` 做差分替换而非整篇重设，光标、折叠块、滚动条位置都保留，且进入 undo 栈。

**整工程格式化**
- 先弹文件列表对话框（见上），只对勾选的文件动手。
- 在 UI 线程一次性拼好每个文件的选项并**预校验**（同一份配置只校验一次），有错立刻中止并提示，不会毁掉半个工程。
- `QtConcurrent::mapped` 并行格式化（AStyle 库无共享可变状态，线程安全），
  每个结果通过 `QFutureWatcher::resultReadyAt(int)` 回到 UI 线程，逐条更新列表状态和进度条。
- 回写规则：
  - 文件在编辑器中打开 → 走 `TextDocument`/`updateEditorText` + `save()`，**不会**弹“文件已被外部修改”的对话框；
  - 文件未打开 → `FileSaver` 原子写盘；
  - 文件有**未保存修改** → 跳过并统计，绝不覆盖用户未保存的内容。
- 结束后在 General Messages 中给出汇总：`N formatted, M unchanged, K skipped, J failed`，并列出失败原因。

---

## 5. 与旧插件 qtc_astyle 的差异

| 维度 | qtc_astyle（gitee） | 本插件 |
| --- | --- | --- |
| Qt Creator | 8.x，`QTextCodec` 时代 | **20.0.2**，Qt 6.11，未使用任何已废弃 API |
| AStyle | 2.04（内嵌源码） | **3.6.20**（最新，MIT） |
| 选项页 | 多张 `.ui` 表单 + `QSettings` | `AspectContainer` + `Layouting`，与 IDE 设置页原生风格一致，支持即时 apply/cancel 脏标记 |
| 选项传递 | 自行拼接命令行字符串 | 官方 `ASOptions` 解析 + 运行前校验，错误立即反馈 |
| 整工程 | `QThread` + qmake 私有 API 取 `.pro` 文件表，自带 `CDlgFormatProject` | `QtConcurrent` 并行；文件表走 `Project::files()`，自带同样思路的勾选对话框（分组/筛选/状态列/停止） |
| 编码 | `QStringConverter::System`（只认本地编码） | 字节级透传 + BOM 专项处理（GBK/UTF-8/UTF-16 均可） |
| 单文件 | 重设整篇文本 | 差分替换，保留光标/折叠，可撤销 |
| 构建 | 独立工程 `find_package(QtCreator)` | 源码树内 `add_qtc_plugin`，与 IDE 同 Qt/同编译器 |

---

## 6. 中文翻译

Qt Creator 没有“每个插件一个 `.qm`”的机制：所有插件、所有库的文案最终都合进
`share/qtcreator/translations/qtcreator_<lang>.ts`，构建期 `lrelease` 编译成
`qtcreator_<lang>.qm`，`main.cpp` 启动时按界面语言加载 `resourcePath()/translations` 下的它。
本插件按同样的方式接入，不需要在插件里自己装 `QTranslator`，也不需要改插件元数据。

| 文件 | 作用 |
| --- | --- |
| `translations/astyle_zh_CN.ts` | 插件自己的 199 条文案（上下文 `QtC::Astyle`），可单独审阅 |
| `tools/fill_translations.py` | 译文表（源文 → 中文）+ 回填脚本 |
| `tools/merge_into_qtcreator_ts.py` | 把上述上下文合并进 `qtcreator_zh_CN.ts` |

重新生成（改了源码里的 `tr()` 之后）：

```bash
cd src/plugins/astyle

# 1. 从源码提取，生成/更新插件的 .ts。注意源文件必须写在 -ts 之前，
#    否则 lupdate 会把它们当成输出文件，报 "has no recognized extension"
lupdate -locations relative -no-ui-lines *.cpp *.h -ts translations/astyle_zh_CN.ts

# 2. 回填译文，脚本会报出漏掉的条目（输出 messages=199 missing=0 才算完整）
python tools/fill_translations.py translations/astyle_zh_CN.ts translations/astyle_zh_CN.ts

# 3. 合并进主 .ts（按上下文名替换、按 lupdate 的排序插入，其余内容一个字节不动，可重复执行）
python tools/merge_into_qtcreator_ts.py translations/astyle_zh_CN.ts \
       ../../share/qtcreator/translations/qtcreator_zh_CN.ts

# 4. 重编 .qm 到构建目录（IDE 启动时从这里加载）
lrelease ../../share/qtcreator/translations/qtcreator_zh_CN.ts \
         -qm ../../build/qt611_vs2022_release/share/qtcreator/translations/qtcreator_zh_CN.qm
```

翻译范围：

- 菜单与动作（`Artistic Style`、`格式化当前文件`、`格式化当前项目...`）、设置页全部标签/分组、
  所有提示消息、进度与结果统计；
- 65 条选项 tooltip 的**手册说明**与对照标题（`变为：`、`变为（align-pointer=type）：`）。
  说明文字在 `astyleoptionhelp.cpp` 里用 `QT_TRANSLATE_NOOP("QtC::Astyle", ...)` 标记，
  运行时经 `QCoreApplication::translate()` 查找；
- **不翻译**：选项名（`--indent-classes / -C`）、Before/After 代码示例、以及 `Before`/`After`
  两个列头——前两者必须与 AStyle 输出一致，后者参与等宽字体的列对齐。

切换语言：`编辑 > 首选项 > 环境 > 界面语言`，或启动参数 `-ui-language zh_CN`。

> 只把 `Astyle.dll` 拷到另一份 Qt Creator 安装里时，那份安装的
> `share/qtcreator/translations/qtcreator_zh_CN.qm` 里没有这 282 条，界面会回落成英文；
> 需要同步更新那边的 `.qm`（或按上面的步骤重新生成）。

---

## 7. 已知限制

- 只支持整文件格式化（不做选区/局部格式化）。
- 没有“保存时自动格式化”（需要的话可以在 `EditorManager::aboutToSave` 上做扩展，与 Beautifier 插件类似）。
- 未提供 qbs 构建脚本（Qt Creator 20 主线使用 CMake）。
- 只格式化 **C++**（含 C 源文件，AStyle 的 `--mode=c` 同时覆盖两者）。Java / C# / Objective-C /
  JS / GHC 模式及其专属选项（`--mode=java`、`--mode=cs`、`--mode=objc`、`--pad-method-colon`、
  `--align-method-colon`、`--pad-param-type` 等）已从界面移除，插件不会按后缀切换语言。
- AStyle 3.6 中 `--pad-brackets`、`--indent-lambda`、`--squeeze-ws`、`--preserve-ws`、`--lineend`、
  `--suffix` 属于**控制台专有**选项（`astyle_main.cpp` 的 `#ifdef ASTYLE_LIB` 之前被截断），库模式下
  一律被拒，所以没有放进界面。
- `--indent=none` 与 `--remove-braces=one-line` 库模式**接受**，但官方手册没有为它们写章节，
  没法给出与其它开关一致的“说明 + Before/After”提示，因此也没放进界面；需要时写进
  “Additional options” 即可。
- `.astylerc` 中的 `--options=`、`--suffix=`、`--recursive` 等文件管理选项在库模式下无效。

---

## 8. 许可

- 插件本身与 Qt Creator 一致：**GPL-3.0-only WITH Qt-GPL-exception-1.0**，
  全文见 `LICENSES/LICENSE.GPL3-EXCEPT`。
- `3rdparty/astyle/` 内为 Artistic Style 上游源码，**MIT** 许可，
  见 `3rdparty/astyle/LICENSE.md`（副本：`LICENSES/MIT.txt`）。
- 本插件与 The Qt Company 无关，未获其背书。
