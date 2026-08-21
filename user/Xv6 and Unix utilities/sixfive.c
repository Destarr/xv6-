#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 定义题目要求的合法分隔符
char *seps = " -\r\t\n./,";

// 处理单个文件的核心函数
void process(int fd) {
    char c;
    int state = 0; // 初始状态为 0 (等待中)
    int val = 0;   // 用于累加计算当前的数字值

    // read(fd, &c, 1) 每次读取 1 个字符，如果读到文件末尾 (EOF) 会返回 0
    while (read(fd, &c, 1) > 0) {
        
        // strchr 如果在 seps 里找到了字符 c，会返回它的地址；找不到返回 0
        if (strchr(seps, c) != 0) {
            if (state == 1) { 
                // 如果之前是在正常采集中，说明现在刚好截断了一个完整的数字
                // 检查它是否是 5 或 6 的倍数
                if (val % 5 == 0 || val % 6 == 0) {
                    printf("%d\n", val);
                }
            }
            // 无论之前是数字还是乱码，遇到分隔符后，全部清零，回到等待状态
            state = 0;
            val = 0;
        } 
        
        // 遇到 0-9 的数字
        else if (c >= '0' && c <= '9') {
            if (state == 0) { 
                // 从等待状态直接遇到数字，说明是一个新数字的开头
                state = 1;
                val = c - '0'; // 将字符转换为真实数字
            } else if (state == 1) { 
                // 正在采集中，把新读取的个位数拼接到原数字上 (比如 1 和 0 拼成 10)
                val = val * 10 + (c - '0');
            }
            // 如果 state == 2 (报废中)，即使遇到数字也不做任何处理
        } 
        
        // 遇到除数字和合法分隔符以外的其他字符
        else {
            state = 2; // 标记当前这一串连续字符为“报废”状态
        }
    }

    // 题目提示：文件的开头和结尾也是隐式的分隔符。
    // 如果文件结束时，我们恰巧还在“正常采集中”状态，需要做最后一次检查
    if (state == 1) {
        if (val % 5 == 0 || val % 6 == 0) {
            printf("%d\n", val);
        }
    }
}

int main(int argc, char *argv[]) {
    int fd;

    // 如果用户运行 "sixfive" 但没有提供文件参数，默认从标准输入读取 (Unix 惯例)
    if (argc <= 1) {
        process(0);
        exit(0);
    }

    // 遍历用户传入的所有文件路径
    for (int i = 1; i < argc; i++) {
        // 使用系统调用 open 打开文件，参数 0 表示以只读模式打开
        if ((fd = open(argv[i], 0)) < 0) {
            // 如果打开失败，打印错误信息到标准错误 (fd=2) 并退出
            fprintf(2, "sixfive: cannot open %s\n", argv[i]);
            exit(1);
        }
        
        // 把打开的文件描述符交给处理函数
        process(fd);
        
        // 处理完毕后关闭文件
        close(fd);
    }

    // 正常退出
    exit(0);
}