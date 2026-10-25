"""Fill the Chinese translations of the Artistic Style plugin into a .ts file.

The .ts file itself is produced by lupdate:

    lupdate -locations relative -no-ui-lines <plugin sources> -ts astyle_zh_CN.ts
    python fill_translations.py astyle_zh_CN.ts astyle_zh_CN.ts

Keeping the texts in this script makes the translation reviewable as a plain
diff and keeps the .ts file reproducible from the sources: rerun lupdate, rerun
this script, and every message is translated again.

Encoding: the .ts files (like all Qt Linguist files) are UTF-8. This script is
UTF-8 as well. Do not confuse this with the GBK sources of the plugin.
"""

import html
import re
import sys

# source text -> Chinese translation
TRANSLATIONS = {
    # --- messages and menu entries (astyleplugin.cpp) -----------------------
    "is not a text document": "不是文本文档",
    "has unsaved changes": "有未保存的更改",
    "Artistic Style": "Artistic Style",
    "&Artistic Style": "Artistic Style(&A)",
    "Format &Current File": "格式化当前文件(&C)",
    "Format Current &Project...": "格式化当前项目(&P)...",
    "Artistic Style &Options...": "Artistic Style 选项(&O)...",
    "Artistic Style does not understand the configured options, "
    "nothing was formatted:":
        "Artistic Style 无法识别所配置的选项，未进行任何格式化：",
    "Fix the options in Edit > Preferences > Artistic Style.":
        "请在“编辑 > 首选项 > Artistic Style”中修正这些选项。",
    "No editor is open.": "没有已打开的编辑器。",
    "\"%1\" is already formatted.": "“%1”已经是格式化后的状态。",
    "Formatted \"%1\".": "已格式化“%1”。",
    "No project is open.": "没有已打开的项目。",
    "Project \"%1\" has no source files with the configured extensions.":
        "项目“%1”中没有符合所配置扩展名的源文件。",
    "Format Project": "格式化项目",
    "Skipped \"%1\": %2.": "已跳过“%1”：%2。",
    "Failed \"%1\": %2.": "处理“%1”失败：%2。",
    "Artistic Style: %1 formatted, %2 unchanged, %3 skipped, %4 failed.":
        "Artistic Style：已格式化 %1 个，未改动 %2 个，跳过 %3 个，失败 %4 个。",
    "The run was canceled.": "操作已取消。",
    "Artistic Style reported error %1.": "Artistic Style 报告错误 %1。",
    "The file contains binary data and is not formatted.":
        "该文件包含二进制数据，未进行格式化。",

    # --- groups -------------------------------------------------------------
    "Formatting Style": "格式化风格",
    "Indentation": "缩进",
    "Spacing and Alignment": "空格与对齐",
    "Line Breaking": "换行",
    "Other": "其他",
    "Files and Configuration": "文件与配置",

    # --- predefined styles --------------------------------------------------
    "Style:": "风格：",
    "Predefined formatting style, the equivalent of --style=<name>.":
        "预定义的格式化风格，等同于 --style=<name>。",
    "Allman": "Allman",
    "Braces on their own line (BSD style).": "大括号独占一行（BSD 风格）。",
    "Java": "Java",
    "Braces attached to the header of the block.": "大括号紧跟代码块头部。",
    "K&R": "K&R",
    "Braces attached to the header, but function braces on their own line.":
        "大括号紧跟代码块头部，但函数的大括号独占一行。",
    "Stroustrup": "Stroustrup",
    "Like K&R, but closing braces get their own line.":
        "与 K&R 类似，但右大括号独占一行。",
    "Whitesmith": "Whitesmith",
    "Indented, with braces on their own line.": "整体缩进，大括号独占一行。",
    "VTK": "VTK",
    "Whitesmith with indented braces and switches.":
        "Whitesmith 风格，大括号与 switch 语句一并缩进。",
    "Ratliff": "Ratliff",
    "Braceless style where the braces are indented.":
        "大括号随代码缩进的“无括号”风格。",
    "GNU": "GNU",
    "Indented braces, extra indentation for blocks.":
        "大括号缩进，代码块再额外缩进一级。",
    "Linux": "Linux",
    "Like K&R with eight spaces indentation.": "与 K&R 类似，使用 8 空格缩进。",
    "Horstmann": "Horstmann",
    "Run-in braces, indented bodies.": "大括号随行，块体缩进。",
    "1TBS": "1TBS",
    "The One True Brace Style.": "“唯一正确的大括号风格”（1TBS）。",
    "Google": "Google",
    "Google C++ style.": "Google C++ 风格。",
    "Mozilla": "Mozilla",
    "Mozilla style.": "Mozilla 风格。",
    "WebKit": "WebKit",
    "WebKit style.": "WebKit 风格。",
    "Pico": "Pico",
    "Run-in braces, opening brace follows the first statement.":
        "大括号随行，左大括号紧随第一条语句。",
    "Lisp": "Lisp",
    "Run-in braces with closing braces at the end of the statement.":
        "大括号随行，右大括号位于语句末尾。",
    "No style": "不指定风格",
    "Do not change the brace style.": "不改变大括号风格。",

    # --- indentation --------------------------------------------------------
    "Indent with:": "缩进方式：",
    "The equivalent of --indent=spaces, --indent=tab or --indent=force-tab.":
        "等同于 --indent=spaces、--indent=tab 或 --indent=force-tab。",
    "Spaces": "空格",
    "Indent with spaces.": "使用空格缩进。",
    "Tabs": "制表符",
    "Indent with tabs, but use spaces for continuation lines.":
        "使用制表符缩进，续行使用空格。",
    "Force tabs": "强制制表符",
    "Indent with tabs everywhere possible.": "在可能的地方全部用制表符缩进。",
    "Indent width:": "缩进宽度：",
    "Number of spaces per indentation step.": "每一级缩进所占的空格数。",
    "Continuation indent:": "续行缩进：",
    "Indent for continuation lines inside parentheses.":
        "圆括号内续行的缩进量。",
    "unchanged": "保持不变",
    "Maximum code length:": "最大行宽：",
    "Break lines that are longer than this. 0 keeps the lines.":
        "超过此长度的行会被折行；0 表示不折行。",
    "unlimited": "不限制",
    "Indent class and struct blocks": "缩进 class 与 struct 块",
    "Indent access modifiers": "缩进访问修饰符",
    "Indent switch blocks": "缩进 switch 块",
    "Indent case blocks": "缩进 case 块",
    "Indent namespace blocks": "缩进 namespace 块",
    "Indent labels": "缩进标签",
    "Indent preprocessor blocks": "缩进预处理块",
    "Indent define directives": "缩进 #define 指令",
    "Indent conditional directives": "缩进条件编译指令",
    "Indent comments at column 1": "缩进位于第 1 列的注释",

    # --- spacing and alignment ---------------------------------------------
    "Pad operators": "运算符两侧加空格",
    "Pad commas": "逗号后加空格",
    "Pad parentheses outside": "圆括号外侧加空格",
    "Pad parentheses inside": "圆括号内侧加空格",
    "Pad first parenthesis outside": "首个圆括号外侧加空格",
    "Pad headers": "头部关键字后加空格",
    "Remove extra parentheses padding": "去除多余的括号内空格",
    "Pad include directives": "#include 后加空格",
    "Convert tabs to spaces": "把制表符转换为空格",
    "Align pointers:": "指针符号对齐：",
    "Attach '*' and '&' to the type, to the name, or put them between both. "
    "Hover an entry for a sample.":
        "把 '*' 和 '&' 贴到类型上、贴到变量名上，或放在两者之间。"
        "将鼠标悬停在选项上可查看示例。",
    "Unchanged": "保持不变",
    "Leave the pointer and reference alignment untouched.":
        "不改变指针与引用符号的对齐方式。",
    "Type": "类型",
    "Middle": "中间",
    "Name": "名称",
    "Align references:": "引用符号对齐：",
    "Align '&' separately from pointers. The entries behave like the ones above.":
        "'&' 与指针分开设置；各条目含义与上面相同。",
    "Follow pointer": "与指针一致",
    "None": "不处理",

    # --- line breaking ------------------------------------------------------
    "Break blocks": "代码块之间插入空行",
    "Break blocks everywhere": "所有代码块之间都插入空行",
    "Break one line headers": "拆开单行的头部语句",
    "Break else if": "拆分 else if",
    "Keep one line blocks": "保留单行代码块",
    "Keep one line statements": "保留单行语句",
    "Attach namespace braces": "namespace 大括号随行",
    "Attach class braces": "class 大括号随行",
    "Attach inline braces": "内联大括号随行",
    "Attach extern \"C\" braces": "extern \"C\" 大括号随行",
    "Delete empty lines": "删除多余空行",

    # --- files and configuration -------------------------------------------
    "File extensions:": "文件扩展名：",
    "Space separated list of file extensions that are formatted when formatting "
    "a project.":
        "以空格分隔的扩展名列表；格式化整个项目时只处理这些类型的文件。",
    "Use .astylerc files found next to the file or in the home directory":
        "使用源文件所在目录或用户主目录中的 .astylerc",
    "Options from such a file are applied after the options above, so they can "
    "override them.":
        "该文件里的选项会在上述选项之后应用，因此可以覆盖它们。",
    "Use specific configuration file:": "使用指定的配置文件：",
    "Artistic Style options (*.astylerc *.rc);;All files (*)":
        "Artistic Style 选项文件 (*.astylerc *.rc);;所有文件 (*)",
    "Additional options:": "附加选项：",
    "Free form AStyle options, one or more per line. They are appended last and "
    "therefore win over everything above.":
        "自由填写的 AStyle 选项，每行一个或多个。它们最后追加，"
        "因此优先级高于上面所有设置。",

    # --- project file list dialog (astyleprojectdialog.cpp) -----------------
    "&Project:": "项目(&P)：",
    "Fi&lter:": "筛选(&L)：",
    "Filter the file list": "筛选文件列表",
    "File": "文件",
    "Directory": "目录",
    "Status": "状态",
    "&Format": "格式化(&F)",
    "&Stop": "停止(&S)",
    "&Close": "关闭(&C)",
    "Header Files": "头文件",
    "Source Files": "源文件",
    "Other Files": "其他文件",
    "%1 of %2 files selected": "已选择 %1 / %2 个文件",
    "Changed": "已改变",
    "Unchanged": "未改变",
    "Failed: %1": "失败：%1",
    "Skipped: %1": "已跳过：%1",
    "Skipped: unsaved changes": "已跳过：有未保存的更改",
    "Not processed": "未处理",
    "Stopping...": "正在停止...",
    "Unsaved changes": "有未保存的更改",
    "Unsaved Changes": "未保存的更改",
    "%1 of the selected files are open with unsaved changes.":
        "所选文件中有 %1 个已在编辑器中打开且有未保存的更改。",
    "Save them first to include them in the run, otherwise they are skipped.":
        "先保存这些文件才能参与格式化，否则它们会被跳过。",
    "&Save and Format": "保存并格式化(&S)",
    "&Skip These Files": "跳过这些文件(&K)",

    # --- tool tips: the wording of the Artistic Style manual -----------------
    # Wrapped in QT_TRANSLATE_NOOP in astyleoptionhelp.cpp; the code samples
    # next to them stay untranslated because AStyle prints them as they are.
    "Indent 'class' and 'struct' blocks so that the entire block is indented. "
    "The struct blocks are indented only if an access modifier, 'public:', "
    "'protected:' or 'private:', is declared somewhere in the struct. This "
    "option is effective for C++ files only.":
        "缩进 'class' 与 'struct' 块，使整个块向右缩进。只有在 struct 中声明了"
        "'public:'、'protected:' 或 'private:' 之类的访问修饰符时，struct 块才会"
        "被缩进。该选项仅对 C++ 文件有效。",
    "Indent 'class ' and 'struct' access modifiers, 'public:', 'protected:' and "
    "'private:', one half indent. The rest of the class is not indented. This "
    "option is effective for C++ files only. If used with indent-classes this "
    "option will be ignored.":
        "把 'class' 与 'struct' 的访问修饰符 'public:'、'protected:'、'private:' "
        "缩进半个缩进量，类的其余部分不缩进。该选项仅对 C++ 文件有效。"
        "与 indent-classes 一起使用时，本选项将被忽略。",
    "Indent 'switch' blocks so that the 'case X:' statements are indented in "
    "the switch block. The entire case block is indented.":
        "缩进 'switch' 块，使 'case X:' 语句在 switch 块内缩进，"
        "整个 case 块也随之一并缩进。",
    "Indent 'case X:' blocks from the 'case X:' headers. Case statements not "
    "enclosed in blocks are NOT indented.":
        "从 'case X:' 标号处开始缩进 case 块。未被花括号包围的 case 语句"
        "不会缩进。",
    "Add extra indentation to namespace blocks. This option has no effect on "
    "Java files. It will also indent CORBA IDL module statements.":
        "为 namespace 块增加额外缩进。该选项对 Java 文件无效。"
        "它同时也会缩进 CORBA IDL 的 module 语句。",
    "Add extra indentation to labels so they appear 1 indent less than the "
    "current indentation, rather than being flushed to the left (the default).":
        "为标签增加额外缩进，使其比当前缩进少一级，而不是像默认那样顶格左对齐。",
    "becomes (with indented 'error:'):": "变为（'error:' 标签已缩进）：",
    "Indent preprocessor blocks at brace level zero and immediately within a "
    "namespace. There are restrictions on what will be indented. Blocks within "
    "methods, classes, arrays, etc., will not be indented. Blocks containing "
    "braces or multi-line define statements will not be indented. Without this "
    "option the preprocessor block is not indented.":
        "缩进位于花括号层级零以及紧邻 namespace 内的预处理块。可缩进的范围有限制："
        "方法、类、数组等内部的块不会缩进；包含花括号或跨行 define 语句的块"
        "也不会缩进。不启用该选项时，预处理块不缩进。",
    "Indent multi-line preprocessor definitions ending with a backslash. Should "
    "be used with --convert-tabs for proper results. Does a pretty good job, "
    "but cannot perform miracles in obfuscated preprocessor definitions. "
    "Without this option the preprocessor statements remain unchanged.":
        "缩进以反斜杠结尾的多行预处理定义。建议与 --convert-tabs 一起使用以取得"
        "正确结果。通常效果不错，但对刻意混淆的预处理定义无能为力。"
        "不启用该选项时，预处理语句保持原样。",
    "Indent preprocessor conditional statements to the same level as the source "
    "code.":
        "把预处理条件语句缩进到与源代码相同的层级。",
    "Indent C++ comments beginning in column one. By default C++ comments "
    "beginning in column one are assumed to be commented-out code and not "
    "indented. This option will allow the comments to be indented with the code.":
        "缩进从第 1 列开始的 C++ 注释。默认情况下，从第 1 列开始的注释被视为"
        "被注释掉的代码，不参与缩进；启用本选项后，这些注释会随代码一起缩进。",
    "Insert space padding around operators. This will also pad commas. Any end "
    "of line comments will remain in the original column, if possible. Note "
    "that there is no option to unpad. Once padded, they stay padded.":
        "在运算符两侧加空格，逗号也会一并加空格。行尾注释会尽量保持在原列位置。"
        "注意：没有取消加空格的选项，一旦加上就不会自动去掉。",
    "Insert space padding after commas. This is not needed if pad-oper is used. "
    "Any end of line comments will remain in the original column, if possible. "
    "Note that there is no option to unpad. Once padded, they stay padded.":
        "在逗号后加空格。若已使用 pad-oper 则无需本选项。行尾注释会尽量保持在"
        "原列位置。注意：没有取消加空格的选项，一旦加上就不会自动去掉。",
    "Insert space padding around parens on the outside only. Parens that are "
    "empty will not be padded. Any end of line comments will remain in the "
    "original column, if possible. This can be used with pad-paren=none below "
    "to remove unwanted spaces.":
        "仅在圆括号外侧加空格，空圆括号不加。行尾注释会尽量保持在原列位置。"
        "可与下面的 pad-paren=none 配合，用于去掉不需要的空格。",
    "Insert space padding around paren on the inside only. Any end of line "
    "comments will remain in the original column, if possible. This can be used "
    "with pad-paren=none below to remove unwanted spaces.":
        "仅在圆括号内侧加空格。行尾注释会尽量保持在原列位置。"
        "可与下面的 pad-paren=none 配合，用于去掉不需要的空格。",
    "Insert space padding around the first paren in a series on the outside "
    "only. Parens that are empty will not be padded. Any end of line comments "
    "will remain in the original column, if possible. This can be used with "
    "pad-paren=none below to remove unwanted spaces. If used with pad-paren or "
    "pad-paren=out, this option will be ignored. If used with pad-paren=in, the "
    "result will be the pad-paren.":
        "仅为一连串圆括号中的第一个括号外侧加空格，空圆括号不加。行尾注释会尽量"
        "保持在原列位置。可与下面的 pad-paren=none 配合，用于去掉不需要的空格。"
        "与 pad-paren 或 pad-paren=out 同时使用时本选项会被忽略；"
        "与 pad-paren=in 同时使用时，效果与 pad-paren 相同。",
    "Insert space padding between a header (e.g. 'if', 'for', 'while'...) and "
    "the following paren. Any end of line comments will remain in the original "
    "column, if possible. This can be used with pad-paren=none to remove "
    "unwanted spaces.":
        "在头部关键字（如 'if'、'for'、'while' 等）与其后的圆括号之间加空格。"
        "行尾注释会尽量保持在原列位置。可配合 pad-paren=none 去掉不需要的空格。",
    "Remove extra space padding around parens on the inside and outside. Any "
    "end of line comments will remain in the original column, if possible. This "
    "option can be used in combination with the paren padding options pad-paren, "
    "pad-paren=out, pad-paren=in, and pad-header above. Only padding that has "
    "not been requested by other options will be removed. For example, if a "
    "source has parens padded on both the inside and outside, and you want "
    "inside only. You need to use pad-paren=none to remove the outside padding, "
    "and pad-paren=in to retain the inside padding. Using only pad-paren=in "
    "would not remove the outside padding.":
        "去掉圆括号内外多余的空白。行尾注释会尽量保持在原列位置。本选项可与上面"
        "的 pad-paren、pad-paren=out、pad-paren=in 和 pad-header 组合使用，"
        "只会去掉未被其他选项要求的空格。例如：源码中圆括号内外都有空格，而你"
        "只想保留内侧的空格，则需要用 pad-paren=none 去掉外侧空格，再用 "
        "pad-paren=in 保留内侧空格；只写 pad-paren=in 无法去掉外侧空格。",
    "becomes (with no padding option requested):":
        "变为（未请求任何加空格选项）：",
    "Insert space padding after include directives and removes superfluous "
    "whitespace.":
        "在 include 指令之后加空格，并去掉多余的空白。",
    "Pad empty lines around header blocks (e.g. 'if', 'for', 'while'...).":
        "在头部块（如 'if'、'for'、'while' 等）前后插入空行。",
    "Pad empty lines around header blocks (e.g. 'if', 'for', 'while'...). Treat "
    "closing header blocks (e.g. 'else', 'catch'), structs and functions as "
    "stand-alone blocks.":
        "在头部块（如 'if'、'for'、'while' 等）前后插入空行。"
        "把结尾头部块（如 'else'、'catch'）以及 struct 和函数也视为独立的块。",
    "Break one line headers (e.g. 'if', 'while', 'else', ...) from a statement "
    "residing on the same line. If the statement is enclosed in braces, the "
    "braces will be formatted according to the requested brace style. A "
    "multi-statement line will NOT be broken if keep-one-line-statements is "
    "requested. One line blocks will NOT be broken if keep-one-line-blocks is "
    "requested and the header is enclosed in the block.":
        "把与语句写在同一行的头部关键字（如 'if'、'while'、'else' 等）拆到单独"
        "一行。若语句带花括号，花括号按所选的大括号风格格式化。若启用了 "
        "keep-one-line-statements，多语句行不会被拆分；若启用了 "
        "keep-one-line-blocks 且头部位于该单行块内，则该单行块不会被拆分。",
    "Break \"else if\" header combinations into separate lines. This option has "
    "no effect if keep-one-line-statements is used, the \"else if\" statements "
    "will remain as they are. If this option is NOT used, \"else if\" header "
    "combinations will be placed on a single line.":
        "把组合在一起的 \"else if\" 头部拆成单独的行。若使用了 "
        "keep-one-line-statements，本选项无效，\"else if\" 语句保持原样。"
        "若不使用本选项，组合的 \"else if\" 会被放在同一行。",
    "Don't break one-line blocks.": "不拆分单行块。",
    "Don't break complex statements and multiple statements residing on a "
    "single line.":
        "不拆分复杂语句，也不拆分写在同一行上的多条语句。",
    "Attach braces to a namespace statement. This is done regardless of the "
    "brace style being used. It will also attach braces to CORBA IDL module "
    "statements.":
        "把大括号附着到 namespace 语句上，不论当前使用哪种大括号风格。"
        "对 CORBA IDL 的 module 语句同样生效。",
    "the brace is always attached to a namespace statement:":
        "大括号始终附着在 namespace 语句上：",
    "Attach braces to a class statement. This is done regardless of the brace "
    "style being used.":
        "把大括号附着到 class 语句上，不论当前使用哪种大括号风格。",
    "the brace is always attached to a class statement:":
        "大括号始终附着在 class 语句上：",
    "Attach braces to class and struct inline function definitions. This option "
    "has precedence for all styles except Horstmann and Pico (run-in styles). "
    "It is effective for C++ files only.":
        "把大括号附着到 class 与 struct 的内联函数定义上。除 Horstmann 和 Pico"
        "（随行风格）之外，本选项对其他所有风格都优先。仅对 C++ 文件有效。",
    "all braces are attached to class and struct inline method definitions:":
        "所有大括号都附着在 class 与 struct 的内联方法定义上：",
    "Attach braces to a braced extern \"C\" statement. This is done regardless "
    "of the brace style being used. This option is effective for C++ files "
    "only. An extern \"C\" statement that is part of a function definition is "
    "formatted according to the requested brace style. Braced extern \"C\" "
    "statements are unaffected by the brace style and this option is the only "
    "way to change them.":
        "把大括号附着到带花括号的 extern \"C\" 语句上，不论当前使用哪种大括号"
        "风格。仅对 C++ 文件有效。作为函数定义一部分的 extern \"C\" 语句按所选"
        "的大括号风格格式化；带花括号的 extern \"C\" 语句不受大括号风格影响，"
        "只能用本选项改变。",
    "this option attaches braces to a braced extern \"C\" statement:":
        "本选项把大括号附着到带花括号的 extern \"C\" 语句上：",
    "Converts tabs into spaces in the non-indentation part of the line. The "
    "number of spaces inserted will maintain the spacing of the tab. The "
    "current setting for spaces per tab is used. It may not produce the "
    "expected results if convert-tabs is used when changing spaces per tab. "
    "Tabs are not replaced within quotes.":
        "把行内非缩进部分的制表符转换为空格。插入的空格数会保持制表符原有的对"
        "齐宽度，取用当前的“每制表符空格数”设置。若在改变每制表符空格数的同时"
        "使用 convert-tabs，结果可能不符合预期。引号内的制表符不会被替换。",
    "Eliminates unnecessary empty lines from within functions and methods. "
    "Empty lines outside of nested functions and methods, as well as lines "
    "specified by break-block options, are kept intact.":
        "删除函数与方法内部多余的空行。嵌套函数、方法之外的空行，以及由 "
        "break-block 类选项指定的行都保持不变。",
    "Attach a pointer or reference operator (*, &, or ^) to either the variable "
    "type (left) or variable name (right), or place it between the type and "
    "name (middle). The spacing between the type and name will be preserved, if "
    "possible. This option is for C/C++, C++/CLI, and C# files. To format "
    "references separately, use the following align-reference option.":
        "把指针或引用运算符（*、& 或 ^）附着到变量类型上（type）、附着到变量名"
        "上（name），或放在类型与名称之间（middle）。类型与名称之间的间距会尽量"
        "保持。该选项适用于 C/C++、C++/CLI 与 C# 文件。若要单独设置引用符，"
        "请使用下面的 align-reference 选项。",
    "becomes (with align-pointer=type):": "变为（align-pointer=type）：",
    "becomes (with align-pointer=middle):": "变为（align-pointer=middle）：",
    "becomes (with align-pointer=name):": "变为（align-pointer=name）：",
    "This option will align references separate from pointers. Pointers are not "
    "changed by this option. If pointers and references are to be aligned the "
    "same, use the previous align-pointer option. The option "
    "align-reference=none will not change the reference alignment. The other "
    "options are the same as for align-pointer. This option is for C/C++, "
    "C++/CLI, and C# files.":
        "该选项把引用符与指针符分开设置，指针不受本选项影响。若希望引用与指针"
        "采用相同的对齐方式，请使用上面的 align-pointer 选项。"
        "align-reference=none 表示不改变引用符的对齐方式，其他取值与 "
        "align-pointer 相同。该选项适用于 C/C++、C++/CLI 与 C# 文件。",
    "becomes (with align-reference=type):": "变为（align-reference=type）：",
    "becomes (with align-reference=middle):": "变为（align-reference=middle）：",
    "becomes (with align-reference=name):": "变为（align-reference=name）：",
    "becomes:": "变为：",
    "Result:": "结果：",
}

MESSAGE_RE = re.compile(
    r"(<message>.*?<source>(?P<source>.*?)</source>.*?)"
    r"<translation(?P<attrs>[^>]*)>(?P<body>.*?)</translation>",
    re.S)


def xml_escape(text):
    # Same escaping as lupdate/QXmlStreamWriter uses, so that a later lupdate
    # run does not rewrite the translations just because of &apos; vs '.
    return (text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
            .replace('"', "&quot;").replace("'", "&apos;"))


def fill(content):
    stats = {"found": 0, "missing": []}

    def replace(match):
        source = html.unescape(match.group("source"))
        stats["found"] += 1
        translation = TRANSLATIONS.get(source)
        if translation is None:
            stats["missing"].append(source)
            return match.group(0)
        return (match.group(1)
                + "<translation>" + xml_escape(translation) + "</translation>")

    return MESSAGE_RE.sub(replace, content), stats


def main(argv):
    if len(argv) != 3:
        print(__doc__)
        return 2

    in_path, out_path = argv[1], argv[2]
    with open(in_path, "r", encoding="utf-8") as handle:
        content = handle.read()

    content, stats = fill(content)

    with open(out_path, "w", encoding="utf-8", newline="\n") as handle:
        handle.write(content)

    print("messages: %d, missing: %d" % (stats["found"], len(stats["missing"])))
    for source in stats["missing"]:
        print("  missing translation: %r" % source)
    return 0 if not stats["missing"] else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
