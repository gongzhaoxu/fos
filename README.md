# fos仿Linux文件系统

## 一.设计思路

### 1.一切皆文件

底层设计时，参考Linux的“一切皆文件”的思路进行设计。文本文件、目录、软链接等皆使用文件方式存储，并使用统一的读写控制。

### 2.隐式链接

系统大部分文件为流文件，仅支持顺序存取，因此选用隐式链接的方式来保存超大型文件。方便文件扩展，不会有磁盘碎片化的问题。

### 3.成组链接分配

空白块号登记不占用额外空间,只借用每组的最后一个空白块。 当前可分配的物理块号存放在卷资源表中,因此绝大部分的分配和回收工作是在主存中进行,可以节省时间

### 4.模拟磁盘外存

使用一个4MB大小的二进制文件模拟磁盘，每个块大小为1KB，一共4096个块，磁盘支持随机读写，在读写时不需要把整个4MB的磁盘读入缓存。

## 二.底层存储设计


