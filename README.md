# fos仿Linux文件系统

## 一.设计思路

### 1.一切皆文件

底层设计时，参考Linux的“一切皆文件”的思路进行设计。文本文件、目录、软链接等皆使用文件方式存储，并使用统一的读写控制。包括报头和报文，其中报头包括占用标记、计数器、下地址、创建时间和修改时间。不同的文件的报文不同，报文也就是文件的内容（content）。我们设计每一块的大小为1024字节，其中报头占12字节。对于小文件来说，一块可以存储所有的content，则其下地址为FF；对于超大型文件来说，一个文件可能需要多块才能存储，那么就可以通过下地址来形成链表作隐式连接，最后一块的下地址为FF。

### 2.隐式链接

系统大部分文件为流文件，仅支持顺序存取，因此选用隐式链接的方式来保存超大型文件。方便文件扩展，不会有磁盘碎片化的问题。

### 3.成组链接分配

空白块号登记不占用额外空间,只借用每组的最后一个空白块。 当前可分配的物理块号存放在卷资源表中,因此绝大部分的分配和回收工作是在主存中进行,可以节省时间

### 4.模拟磁盘外存

使用一个4MB大小的二进制文件模拟磁盘，每个块大小为1KB，一共4096个块，磁盘支持随机读写，在读写时不需要把整个4MB的磁盘读入缓存。

### 5.系统整体架构



## 二.底层存储设计

### 1.基础文件存储方式

文件最基础的存储结构，不同的文件的content不同，但“表头”都相同

![基础文件存储方式](https://raw.githubusercontent.com/gongzhaoxu/fos/master/img/%E5%9F%BA%E7%A1%80%E6%96%87%E4%BB%B6%E5%AD%98%E5%82%A8%E6%96%B9%E5%BC%8F.png)

### 2.目录存储方式

目录记录以块号+块内编号为主键查找索引节点，此方式方便对索引节点进行随机查找以及增删，目录文件为基础文件的派生类。

![目录存储方式](https://raw.githubusercontent.com/gongzhaoxu/fos/master/img/%E7%9B%AE%E5%BD%95%E5%AD%98%E5%82%A8%E6%96%B9%E5%BC%8F.png)

### 3.索引节点存储方式

索引节点派生类为索引顺序文件，包括文件物理地址、权限信息。

![索引节点存储方式](https://raw.githubusercontent.com/gongzhaoxu/fos/master/img/%E7%B4%A2%E5%BC%95%E8%8A%82%E7%82%B9%E5%AD%98%E5%82%A8%E6%96%B9%E5%BC%8F.png)

### 4.用户存储方式

用户文件为基础文件的派生类

![用户存储方式](https://raw.githubusercontent.com/gongzhaoxu/fos/master/img/%E7%94%A8%E6%88%B7%E5%AD%98%E5%82%A8%E6%96%B9%E5%BC%8F.png)

## 三.指令设计

### 1.多重寻址

![多重寻址](https://raw.githubusercontent.com/gongzhaoxu/fos/master/img/%E5%A4%9A%E9%87%8D%E5%AF%BB%E5%9D%80%E6%96%B9%E5%BC%8F.png)

### 2.目录相关指令

![目录相关指令](https://raw.githubusercontent.com/gongzhaoxu/fos/master/img/%E7%9B%AE%E5%BD%95%E7%9B%B8%E5%85%B3%E6%8C%87%E4%BB%A4.png)

### 3.文件相关指令

![文件相关指令1](https://raw.githubusercontent.com/gongzhaoxu/fos/master/img/%E6%96%87%E4%BB%B6%E7%9B%B8%E5%85%B3%E6%8C%87%E4%BB%A4%EF%BC%881%EF%BC%89.png)

![文件相关指令2](https://raw.githubusercontent.com/gongzhaoxu/fos/master/img/%E6%96%87%E4%BB%B6%E7%9B%B8%E5%85%B3%E6%8C%87%E4%BB%A4%EF%BC%882%EF%BC%89.png)

### 4.系统指令

![系统指令1](https://raw.githubusercontent.com/gongzhaoxu/fos/master/img/%E5%85%B6%E4%BB%96%E6%8C%87%E4%BB%A4%EF%BC%881%EF%BC%89.png)

![系统指令2](https://raw.githubusercontent.com/gongzhaoxu/fos/master/img/%E5%85%B6%E4%BB%96%E6%8C%87%E4%BB%A4%EF%BC%882%EF%BC%89.png)

## 四.工程文件解释

![工程文件解释](https://raw.githubusercontent.com/gongzhaoxu/fos/master/img/%E5%B7%A5%E7%A8%8B%E6%96%87%E4%BB%B6%E8%A7%A3%E9%87%8A.png)