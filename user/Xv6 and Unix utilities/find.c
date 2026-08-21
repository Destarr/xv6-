#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h" 

char* get_name(char *path) {
    char *p;
    for(p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    return p;
}

// exec_argv: 存放 -exec 后面的命令及参数数组
// exec_idx: 记录文件路径应该填入参数数组的哪个位置
void find(char *path, char *target, char **exec_argv, int exec_idx) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){
    case T_FILE:
        if(strcmp(get_name(path), target) == 0) {
            // 如果是 -exec 模式，执行命令；否则正常打印
            if(exec_argv != 0) {
                exec_argv[exec_idx] = path;     // 将找到的文件路径放在参数最后
                exec_argv[exec_idx + 1] = 0;    // 标记参数数组结束 (\0)
                
                if(fork() == 0) {
                    // 子进程：执行指定的命令
                    exec(exec_argv[0], exec_argv);
                    fprintf(2, "find: exec %s failed\n", exec_argv[0]);
                    exit(1);
                }
                // 父进程：等待子进程执行完毕
                wait(0);
            } else {
                printf("%s\n", path);
            }
        }
        break;

    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;

            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            if(stat(buf, &st) < 0){
                printf("find: cannot stat %s\n", buf);
                continue;
            }

            // 如果名字匹配目标
            if(strcmp(de.name, target) == 0) {
                if(exec_argv != 0) {
                    exec_argv[exec_idx] = buf;
                    exec_argv[exec_idx + 1] = 0;
                    
                    if(fork() == 0) {
                        exec(exec_argv[0], exec_argv);
                        fprintf(2, "find: exec %s failed\n", exec_argv[0]);
                        exit(1);
                    }
                    wait(0);
                } else {
                    printf("%s\n", buf);
                }
            }

            // 递归进入子目录时，要把 exec 的状态一起传下去
            if(st.type == T_DIR){
                find(buf, target, exec_argv, exec_idx);
            }
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    // 至少需要 3 个参数：find <path> <target>
    if(argc < 3){
        fprintf(2, "Usage: find <path> <target> [-exec cmd ...]\n");
        exit(1);
    }
    
    char *exec_argv[MAXARG];
    int exec_idx = 0;
    char **p_exec_argv = 0;

    // 检查是否带有 -exec 参数
    if (argc >= 4 && strcmp(argv[3], "-exec") == 0) {
        if (argc == 4) {
            fprintf(2, "find: missing command for -exec\n");
            exit(1);
        }
        
        p_exec_argv = exec_argv;
        
        // 提取 -exec 后面的所有参数
        // 例如 find . wc -exec echo hi
        // argc = 6. 从 i=4 开始遍历
        for (int i = 4; i < argc; i++) {
            exec_argv[exec_idx++] = argv[i];
        }
        // 循环结束后，exec_idx 刚好指向数组的下一个空位，正好用来放文件路径！
    }

    // 将解析好的参数传给 find 开始执行
    find(argv[1], argv[2], p_exec_argv, exec_idx);
    exit(0);
}