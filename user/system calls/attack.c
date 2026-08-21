#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  // 申请大量内存（比如 32 个内存页）
  int sz = 32 * 4096;
  char *mem = sbrk(sz);

  if (mem == (char*)-1) {
    printf("sbrk failed\n");
    exit(1);
  }

  // 遍历我们拿到的这些内存
  for (int i = 0; i < sz; i++) {
    
    // 判断当前字符是不是字母或数字
    if ((mem[i] >= 'a' && mem[i] <= 'z') ||
        (mem[i] >= 'A' && mem[i] <= 'Z') ||
        (mem[i] >= '0' && mem[i] <= '9')) {
        
        int len = 0;
        
        // 看看这个由字母/数字组成的连续字符串有多长
        while ((mem[i + len] >= 'a' && mem[i + len] <= 'z') ||
               (mem[i + len] >= 'A' && mem[i + len] <= 'Z') ||
               (mem[i + len] >= '0' && mem[i + len] <= '9')) {
            len++;
        }

        // 一个合法的 C 字符串必须以 '\0' 结尾，且密码通常不会只有一个字符
        if (mem[i + len] == '\0' && len >= 2) {
            
            // 把系统残留加入黑名单
            if (strcmp(&mem[i], "secret") != 0 && 
                strcmp(&mem[i], "attack") != 0 &&
                strcmp(&mem[i], "sh") != 0 &&
                strcmp(&mem[i], "Hs") != 0 &&
                strcmp(&mem[i], "console") != 0 &&
                strcmp(&mem[i], "0123456789ABCDEF") != 0 &&
                strcmp(&mem[i], "zR") != 0) {
                
                // 找到了！按要求单独打印在一行并退出
                printf("%s\n", &mem[i]);
                exit(0);
            }
        }
        
        // 跳过这段已经扫描过的字符串
        i += len;
    }
  }

  // 如果没找到就返回错误
  exit(1);
}