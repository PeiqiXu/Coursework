#include <ctype.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
/*This function takes two parameter:
 * mode_n (which is st_mode from struct stat sb)
 * argv (which takes the filename from the ith input)
 * and print the filename as the stat do
 */
void printFileName(mode_t mode_n, char * argv) {
  //if the input is a simLink, use readlink and add null terminator to the end of the target of the link
  if (S_ISLNK(mode_n)) {
    char linktarget[256];  //make enough space to load the target of the link
    ssize_t len = readlink(argv, linktarget, 256);
    linktarget[len] = '\0';
    printf("  File: %s -> %s\n", argv, linktarget);
  }
  else {
    printf("  File: %s\n", argv);
  }
}
/*This function takes one parameter:
 * n_mode (which is st_mode from struct stat sb)
 * and return a pointer to a string of filetype based on the n_mode
 * if the filetype is not included, return unknown?
 */

char * switchFileType(mode_t n_mode) {
  char * p = NULL;
  switch (n_mode & S_IFMT) {
    case S_IFBLK:
      p = strdup("block special file");
      break;
    case S_IFCHR:
      p = strdup("character special file");
      break;
    case S_IFDIR:
      p = strdup("directory");
      break;
    case S_IFIFO:
      p = strdup("fifo");
      break;
    case S_IFLNK:
      p = strdup("symbolic link");
      break;
    case S_IFREG:
      p = strdup("regular file");
      break;
    case S_IFSOCK:
      p = strdup("socket");
      break;
    default:
      p = strdup("unknown?");
      break;
  }
  return p;  //return the pointer allocated by strdup without being freed
}
/*This function takes one parameter:
 * sb (the struct stat sb)
 * prints the total size, the number of 512B blocks allocated to the file ,and blocksize for filesystem I/O
 * and prints the filetype and free it 
 */
void printSize_Blocks_IOBlock(struct stat sb) {
  char * fileType = switchFileType(sb.st_mode);
  printf("  Size: %-10lu\tBlocks: %-10lu IO Block: %-6lu %s\n",
         (unsigned long)sb.st_size,
         (unsigned long)sb.st_blocks,
         (unsigned long)sb.st_blksize,
         fileType);
  free(fileType);
}
/*This function takes one parameter:
 * sb (the struct stat b)
 * prints the ID of the device containing the file, the inode number and the number of hard links
 * and print Device type if the file is a device
 */
void printDevice_Links(struct stat sb) {
  if (S_ISCHR(sb.st_mode) || S_ISBLK(sb.st_mode)) {
    printf("Device: %lxh/%lud\tInode: %-10lu  Links: %-5lu Device type: %d,%d\n",
           (unsigned long)sb.st_dev,
           (unsigned long)sb.st_dev,
           (unsigned long)sb.st_ino,
           (unsigned long)sb.st_nlink,
           major(sb.st_rdev),
           minor(sb.st_rdev));  //major and minor to decompose the file's device ID
  }
  else {
    printf("Device: %lxh/%lud\tInode: %-10lu  Links: %lu\n",
           (unsigned long)sb.st_dev,
           (unsigned long)sb.st_dev,
           (unsigned long)sb.st_ino,
           (unsigned long)sb.st_nlink);
  }
}
/*This function takes one parameter:
 * n_mode (sb.st_mode from struct stat sb)
 * return the char that represents the filetype
 */
char getStr1(mode_t n_mode) {
  switch (n_mode & S_IFMT) {
    case S_IFBLK:
      return 'b';
      break;
    case S_IFCHR:
      return 'c';
      break;
    case S_IFDIR:
      return 'd';
      break;
    case S_IFIFO:
      return 'p';
      break;
    case S_IFLNK:
      return 'l';
      break;
    case S_IFREG:
      return '-';
      break;
    case S_IFSOCK:
      return 's';
      break;
      //if the filetype is unknown?, return and then exit failure in the caller function
    default:
      return '0';
      break;
  }
}
/*This function takes three parameters:
 * n_mode (which is sb.st_mode from struct stat sb)
 * p      (which is the pointer that points to the exact location of somewhere in the permission string)
 * kind   (which is enum kind_t, divide into three kind)
 * according to the kind, write the correct permission for read, write and execute into the permission string
 */
enum kind_t { OWNER, GROUP, OTHER };
void readWriteExecute(mode_t n_mode, char * p, enum kind_t kind) {
  //use three ints to represent the macros we use to compare with the n_mode
  int r = 0;
  int w = 0;
  int x = 0;
  switch (kind) {
    case OWNER:
      r = S_IRUSR;
      w = S_IWUSR;
      x = S_IXUSR;
      break;
    case GROUP:
      r = S_IRGRP;
      w = S_IWGRP;
      x = S_IXGRP;
      break;
    case OTHER:
      r = S_IROTH;
      w = S_IWOTH;
      x = S_IXOTH;
      break;
  }
  //cause the permission string is initialized with all '-', so don't need to consider else if to write in '-'
  if (n_mode & r) {
    *p = 'r';
  }
  if (n_mode & w) {
    *(p + 1) = 'w';
  }
  if (n_mode & x) {
    *(p + 2) = 'x';
  }
}
/*This function takes one parameter:
 * n_mode (which is sb.st_mode from struct stat sb)
 * initialize the permission string with ten '-' 
 * then judge if filetype is unknown
 * set each char appropriately and return the pointer to the permission string without being freed
 */
char * makeStr(mode_t n_mode) {
  char * answer =
      strdup("----------");  //initialize permission string like this can save work to revise
  // exit failure if encounters unknown filetype
  if (getStr1(n_mode) == '0') {
    fprintf(stderr, "Unknown filetype!");
    exit(EXIT_FAILURE);
  }
  answer[0] = getStr1(n_mode);
  readWriteExecute(n_mode, answer + 1, OWNER);
  readWriteExecute(n_mode, answer + 4, GROUP);
  readWriteExecute(n_mode, answer + 7, OTHER);
  return answer;  //not freed here
}
/*This function takes one parameter:
 * sb (struct stat sb)
 * print the Access, user ID and the name of the owner, group ID and group name of the owning group
 * free the permission left in the above part
 */
void printAccess_Uid_Gid(struct stat sb) {
  //exit failure if can not find name for the user ID and group ID
  if (getpwuid(sb.st_uid) == NULL) {
    fprintf(stderr, "No match for user ID!");
    exit(EXIT_FAILURE);
  }
  if (getgrgid(sb.st_gid) == NULL) {
    fprintf(stderr, "No match for group ID!");
    exit(EXIT_FAILURE);
  }
  //get permission string
  char * str = makeStr(sb.st_mode);
  printf("Access: (%04o/%s)  Uid: (%5d/%8s)   Gid: (%5d/%8s)\n",
         sb.st_mode & ~S_IFMT,
         str,
         sb.st_uid,
         getpwuid(sb.st_uid)->pw_name,
         sb.st_gid,
         getgrgid(sb.st_gid)->gr_name);
  free(str);
}

//This function is provided to get the pointer to the string of time information
char * time2str(const time_t * when, long ns) {
  char * ans = malloc(128 * sizeof(*ans));
  char temp1[64];
  char temp2[32];
  const struct tm * t = localtime(when);
  strftime(temp1, 512, "%Y-%m-%d %H:%M:%S", t);
  strftime(temp2, 32, "%z", t);
  snprintf(ans, 128, "%s.%09ld %s", temp1, ns, temp2);
  return ans;
}

/*This function takes one parameter:
 * sb (which is struct stat sb)
 * print the time information for Access, Modify, and Change,
 * and print '-' for birth
 */
void printTime(struct stat sb) {
  char * timestra = time2str(&sb.st_atime, sb.st_atim.tv_nsec);
  printf("Access: %s\n", timestra);
  free(timestra);
  char * timestrm = time2str(&sb.st_mtime, sb.st_mtim.tv_nsec);
  printf("Modify: %s\n", timestrm);
  free(timestrm);
  char * timestrc = time2str(&sb.st_ctime, sb.st_ctim.tv_nsec);
  printf("Change: %s\n", timestrc);
  free(timestrc);
  printf(" Birth: -\n");
}

//This function is meaning to fulfill the same function as stat
int main(int argc, char * argv[]) {
  struct stat sb;
  //test if no input filename
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  //the loop can take arbitrary number of arguments and stats them
  for (int i = 1; argv[i] != NULL; i++) {
    //test if lstat function failure
    if (lstat(argv[i], &sb) == -1) {
      perror("lstat");
      exit(EXIT_FAILURE);
    }
    printFileName(sb.st_mode, argv[i]);
    printSize_Blocks_IOBlock(sb);
    printDevice_Links(sb);
    printAccess_Uid_Gid(sb);
    printTime(sb);
  }
  exit(EXIT_SUCCESS);
}
