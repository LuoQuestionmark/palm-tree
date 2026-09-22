# Palm-Tree

Palm-Tree is a personal project, currently under early development, aim at segmenting and [POS tagging](https://en.wikipedia.org/wiki/Part-of-speech_tagging) Chinese text with UTF8-encoding.

## Motivation

This project is for personal use. I previously consulted [a list of resources](https://www.aclweb.org/aclwiki/Resources_for_Chinese) of computational Chinese resources. Amount the list of "free software", there were 4 software listed:

- `rseg` was written in Ruby;
- `ctbparser` was written in C++ with GB2312 encoding;
- `ZPar` linker to a site that I cannot access;
- `DuDuPlus` provided no code.

Shortly after [getting defeated by ctbparser](https://luoquestionmark.github.io/dev/2026/09/10/defeat-by-ctbparser.html), I decide to create this project my own toy project to perform Chinese segmentation and POS tagging tasks.

## Segmentation / POS tagging guidelines

There are multiple guidelines that I can found on the Internet. I wrote a short [document](./documents/comment-seg-pos.pdf) on this subject. For now it seems that [Universal Dependencies](https://universaldependencies.org/) is a good choice, as I can get free corpus with this guideline; although Chinese Treebank seems to be the de facto Chinese segmentation/POS tagging guideline, I am currently unwilling to pay $300 to access [Chinese Treebank 8.0](https://catalog.ldc.upenn.edu/LDC2013T21).

## Roadmap & Current Status

### Segmentation-POS working process

I consider Chinese segmentation and Chinese POS tagging as two intervened tasks. While POS tagging is only possible *after* a segmentation, it is possible that a single Chinese sentence have multiple segmentations. It is only possible, with syntax, even semantic knowledge to know which segmentation is best. To give an example, considering the joke from decades ago:

> 粮食不卖给共产党

The punchline of this sentence is the fact of its segmentations. Two segmentations “粮食 不卖 给 共产党” and “粮食 不卖给 共产党” have opposite meanings. While both being valid, the (correct) interpretation depends on the general knowledge of the world and semantic distinction.

Base on the observation above, this program is planned to execute the segmentation and POS tagging process in the following order:

1. Perform a "raw" segmentation based on several established rules and a Chinese dictionary.
2. Apply a POS tagging algorithm on given segmentation, return one or multiple segmentation plans.
3. Evaluate these segmentation plans the POS tagging algorithm. Perform certain evolutionary algorithm to refine segmentation result.
4. Return the best segmentation with its proper POS tagging.

### Current status

The project is still under very early stage of development. Currently it can perform the "raw" segmentation (step 1 of previously mentioned process), using a Chinese dictionary and several state machines identifying patterns of:

- cardinal numbers;
- ordinal numbers;
- latin letters (as `[A-Za-z]+`).

Surprisingly, with such simple algorithms, it works pretty well on the example text (the beginning of Chinese `vimtutor zh`):

(apology for the messy `<.>` format)

```
欢迎<.> 阅读<.> 《<.> VIM<.> 教程<.> 》<.> —<.> —<.> 版本<.> 1.7<.> Vim<.> 是<.> 一个<.> 具有<.> 很<.> 多<.> 命令<.> 的<.> 功能<.> 非常<.> 强大<.> 的<.> 编辑器<.> 。<.>
限于<.> 篇幅<.> ，<.> 在<.> 本<.> 教程<.> 当中<.> 就<.> 不详<.> 细<.> 介绍<.> 了<.> 。<.>
本<.> 教程<.> 的<.> 设计<.> 目标<.> 是<.> 讲述<.> 一些<.> 必要<.> 的<.> 基本<.> 命令<.> ，<.> 而<.> 掌握<.> 好<.> 这些<.> 命令<.> ，<.> 您<.> 就<.> 能够<.> 很<.> 容易<.> 地<.> 将<.> Vim<.> 当作<.> 一个<.> 通用<.> 编辑器<.> 来<.> 使用<.> 了<.> 。<.>
完成<.> 本<.> 教程<.> 的<.> 内容<.> 大约<.> 需要<.> 25<.> -<.> 30<.> 分钟<.> ，<.> 取决于<.> 您<.> 训练<.> 的<.> 时间<.> 。<.>
注意<.> ：<.> 每<.> 一<.> 节<.> 的<.> 命令<.> 操作<.> 将会<.> 更改<.> 本文<.> 。<.>
推荐<.> 您<.> 复制<.> 本文<.> 的<.> 一个<.> 副本<.> ，<.> 然后<.> 在<.> 副本<.> 上进<.> 行<.> 训练<.> (<.> 如果<.> 您<.> 是<.> 通过<.> "<.> vimtutor<.> "<.> 来<.> 启动<.> 教程<.> 的<.> ，<.> 那么<.> 本文<.> 就<.> 已经<.> 是<.> 副本<.> 了<.> )<.> 。<.>
切记<.> 一点<.> ：<.> 本<.> 教程<.> 的<.> 设计<.> 思路<.> 是<.> 在<.> 使用<.> 中<.> 进行<.> 学习<.> 的<.> 。<.>
也就是说<.> ，<.> 您<.> 需要<.> 通过<.> 执行<.> 命令<.> 来<.> 学习<.> 它们<.> 本身<.> 的<.> 正确<.> 用法<.> 。<.>
如果<.> 您<.> 只是<.> 阅读<.> 而<.> 不<.> 操作<.> ，<.> 那么<.> 您<.> 可能<.> 会<.> 很<.> 快<.> 遗忘<.> 这些<.> 命令<.> 的<.> ！<.>
好<.> 了<.> ，<.> 现在<.> 请<.> 确定<.> 您<.> 的<.> Shift<.> -<.> Lock<.> (<.> 大小写<.> 锁定<.> 键<.> )<.> 还<.> 没有<.> 按下<.> ，<.> 然后<.> 按键<.> 盘<.> 上<.> 的<.> 字母<.> 键<.> j<.> 足够<.> 多次<.> 来<.> 移动<.> 光标<.> ，<.> 直到<.> 第一<.> 节<.> 的<.> 内容<.> 能够<.> 完全<.> 充满<.> 屏幕<.> 。<.>
第一<.> 讲<.> 第一<.> 节<.> ：<.> 移动<.> 光标<.> 要<.> 移动<.> 光标<.> ，<.> 请<.> 依照<.> 说明<.> 分别<.> 按下<.> h<.> 、<.> j<.> 、<.> k<.> 、<.> l<.> 键<.> 。<.>
^<.> k<.> 提示<.> ：<.> h<.> 的<.> 键<.> 位于<.> 左边<.> ，<.> 每次<.> 按下<.> 就<.> 会<.> 向<.> 左<.> 移动<.> 。<.>
<<.> hl<.> ><.> l<.> 的<.> 键<.> 位于<.> 右边<.> ，<.> 每次<.> 按下<.> 就<.> 会<.> 向<.> 右<.> 移动<.> 。<.>
jj<.> 键<.> 看起来<.> 很<.> 象<.> 一<.> 支<.> 尖端<.> 方向<.> 朝<.> 下<.> 的<.> 箭头<.> 。<.>
v<.> 1.<.> 请<.> 随意<.> 在<.> 屏幕<.> 内<.> 移动<.> 光标<.> ，<.> 直至<.> 您<.> 觉得<.> 舒服<.> 为止<.> 。<.>
2.<.> 按下<.> 下行<.> 键<.> (<.> j<.> )<.> ，<.> 直到<.> 出现<.> 光标<.> 重复<.> 下行<.> 。<.>
-<.> -<.> -<.> ><.> 现在<.> 您<.> 应该<.> 已经<.> 学会<.> 如何<.> 移动<.> 到<.> 下<.> 一<.> 讲<.> 吧<.> 。<.>
3.<.> 现在<.> 请<.> 使用<.> 下行<.> 键<.> ，<.> 将<.> 光标<.> 移动<.> 到<.> 第一<.> 讲<.> 第二<.> 节<.> 。<.>
提示<.> ：<.> 如果<.> 您<.> 不<.> 敢<.> 确定<.> 您<.> 所<.> 按下<.> 的<.> 字母<.> ，<.> 请<.> 按下<.> <<.> ESC<.> ><.> 键<.> 回到<.> 正常<.> (<.> Normal<.> )<.> 模式<.> 。<.>
然后<.> 再次<.> 从<.> 键盘<.> 输入<.> 您<.> 想要<.> 的<.> 命令<.> 。<.>
提示<.> ：<.> 光标<.> 键<.> 应当<.> 也<.> 能<.> 正常工作<.> 的<.> 。<.>
但是<.> 使用<.> hjkl<.> 键<.> ，<.> 在<.> 习惯<.> 之后<.> 您<.> 就<.> 能够<.> 更<.> 快<.> 地<.> 在<.> 屏幕<.> 内<.> 四处<.> 移动<.> 光标<.> 。<.>
真的<.> 是<.> 这样<.> ！<.>
```

### Roadmap for the next steps

Following functions are planned:

- POS tagging on given segmentation;
- POS tagging evaluation based on grammar rules;
    - for instance, "\<verb>\<verb>\<verb>" is not a valid Chinese syntax
- POS tagging - segmentation optimisation over iterations.

Also some auxiliary works:

- give credit to external resources used in the project;
- write documentation & code comments.
