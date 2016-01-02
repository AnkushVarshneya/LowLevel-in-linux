#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char **argv) {

    // if proper argument not passed show user correct usage and terminate!
    if(argc != 2) {
        printf("Incorrect Usage!\n");
        printf("Usage: ./13 <name of file>\n");       
        return 1;
    }

    struct stat fileStat;
    
    // populate the fileStat struct will stats of fileName
    // if failed say item not found and terminate!
    char *fileName = argv[1];
    if(stat(fileName,&fileStat) < 0){
        printf("Item not found!\n");
        return 1;
    }
    
    // find the mode
    char *fileType;
    switch (fileStat.st_mode & S_IFMT) {
    case S_IFBLK:  fileType="block device";		break;
    case S_IFCHR:  fileType="character device";	break;
    case S_IFDIR:  fileType="directory";		break;
    case S_IFIFO:  fileType="FIFO/pipe";		break;
    case S_IFLNK:  fileType="symlink";			break;
    case S_IFREG:  fileType="regular file";		break;
    case S_IFSOCK: fileType="socket"; 			break;
    default:       fileType="unknown?";			break;
    }

    //print out the stats
    printf("Stat\n");
    printf("---------------------------\n");
    printf("File: %s \n", fileName);
    printf("Size: %lld\n", (long long) fileStat.st_size);
    printf("Block: %lld\n", (long long) fileStat.st_blocks);
    printf("IO Block: %ld\t%s\n", (long) fileStat.st_blksize, fileType);
    printf("Device: %1$xh/%1$dd\n", (unsigned int) fileStat.st_dev);
    printf("Device(Special): %ld\n", (long) fileStat.st_rdev);
    printf("Inode: %ld\n", (long) fileStat.st_ino);
    printf("Links: %ld\n", (long) fileStat.st_nlink);
    printf("Uid: %ld\n", (long) fileStat.st_uid);
    printf("Gid: %ld\n", (long) fileStat.st_gid);
    printf("Access: %s", ctime(&fileStat.st_atime));
    printf("Modify: %s", ctime(&fileStat.st_mtime));
    printf("Change: %s", ctime(&fileStat.st_ctime));
    
    return 0;
}