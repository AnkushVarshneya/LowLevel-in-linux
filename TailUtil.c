#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **argv) {

    // if proper argument not passed show user correct usage and terminate!
    if(argc != 3) {
        printf("Incorrect Usage!\n");
        printf("Usage: ./mytail <number of lines> <name of file>\n");       
        return 1;
    }

    struct stat fileStat;

    // populate the fileStat struct will stats of fileName
    // if failed say item not found and terminate!
    char *fileName = argv[2];
    if(stat(fileName,&fileStat) < 0){
        printf("Item not found!\n");
        return 1;
    }

    //get the number of line as integer	
    int lineNumber = 0;
    int i;
    for (i = 0; argv[1][i] != '\0'; ++i) {
        lineNumber = lineNumber*10 + argv[1][i] - '0';
    }   
    
    // the buffer
    char *buffer;
    write(1, buffer, 0);
    
    // size of file
    int lastIndex = (int)fileStat.st_size;
    
    // assume a line size
    int estimatedLineSize = 5;

    off_t indexToStop = 0;
    
    // check where to stop	
    while (1){
        // open the file
        int fileHandle  = open(fileName, O_RDONLY);
        
        // seek up the file on estimatedLineSize*lineNumber from end
        lseek(fileHandle, lastIndex-estimatedLineSize*lineNumber, SEEK_SET);

        // read from curr pos to end 
        read(fileHandle, buffer, estimatedLineSize*lineNumber);

        // close the file
        close(fileHandle);

        // calculate number of lines in buffer
        // also do a increase for final line
        int currLineCount =0;
        for(i = 0; buffer[i] != '\0'; ++i) {
            if(buffer[i] == '\n'){
                currLineCount++;
            }
        }

        // if the guess didn't work increase the guess by 2.
        // get a extra line and chop off later as the first line may be partial
        if((lineNumber+1)>currLineCount){
            estimatedLineSize +=1;
        }
        // otherwise write the answer to stdout (with file descriptor=1)
        else {
            //find cut off range ie first \n
            int cutOff;            
            for(cutOff = 0; buffer[cutOff] != '\n'; cutOff++) {}
            cutOff++;
            // change buffer to cut off
            int j;
            for(j = 0; buffer[j+cutOff] != '\0'; j++) {
                buffer[j] = buffer[j+cutOff];
            }
            buffer[j] = '\0';
            //printf("-------%d\t%d\t%d---------\n", currLineCount, estimatedLineSize, lineNumber);
            
            //write to stdout
            write(1, buffer, j);
            break;
        }
    }
    return 0;
}