#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data)
{
  // 遍历格式化字符串 fmt 中的每一个字符，直到遇到 '\0' 结束符
  while (*fmt != '\0') {
    switch (*fmt) {
      case 'i':
        // 将 data 强转为 int* (4字节)，解引用获取值，然后 data 后移 4 字节
        printf("%d\n", *(int *)data);
        data += 4;
        break;
        
      case 'p':
        // 强转为 uint64* (64位，8字节整数)，xv6 中使用 %lx 打印十六进制
        printf("%lx\n", *(uint64 *)data);
        data += 8;
        break;
        
      case 'h':
        // 强转为 short* (16位，2字节)
        printf("%d\n", *(short *)data);
        data += 2;
        break;
        
      case 'c':
        // 强转为 char* (8位，1字节)
        printf("%c\n", *(char *)data);
        data += 1;
        break;
        
      case 's':
        // data 指向的是一个指针，所以强转为 char**，解引用后得到实际的字符串首地址 char*
        printf("%s\n", *(char **)data);
        data += 8;
        break;
        
      case 'S':
        // data 剩下的内容直接作为一个 C 字符串打印
        printf("%s\n", data);
        return; // S 打印的是 "the rest of the data"，所以打印完直接结束函数即可
    }
    
    // 移动到 fmt 的下一个格式化字符
    fmt++;
  }
}
