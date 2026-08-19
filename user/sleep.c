#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // argc 记录了命令行参数的个数，argv 是一个字符串数组。
    if (argc != 2) {
        // 如果用户忘记传入数字，或者传入了多余的参数，打印错误提示
        fprintf(2, "Usage: sleep <ticks>\n");
        exit(1); // 异常退出，按惯例传 1
    }

    // 将字符串参数转换为整数
    int ticks = atoi(argv[1]);

    // 直接调用 xv6 内核提供的 pause 系统调用，并传入转换好的 ticks
    pause(ticks);

    // 正常退出程序
    exit(0); 
}