#include <kernel/types.h> 
struct stat;
 struct tm;
 
 // system calls
 int fork(void);
 int wait(int*);
 int pipe(int*);
 int write(int, const void*, int);
 int read(int, void*, int);
 int close(int);
 int kill(int);
 int exec(char*, char**);
 int open(const char*, int);
 int mknod(const char*, short, short);
 int unlink(const char*);
 int fstat(int fd, struct stat*);
 int link(const char*, const char*);
 int mkdir(const char*);
 int chdir(const char*);
 int dup(int);
 int getpid(void);
 char* sbrk(int);
 int sleep(int);
 int uptime(void);
 int reboot(int);
 int rdtime(struct tm*);
 int mcreate();
 int mdelete(int);
 int mentry(uint);
 int lseek(int, int, int);
 int permchange(int, int, int, int);
 int access(const char*);
