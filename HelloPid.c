#include <unistd.h>
#include <sys/syscall.h>

//Function prototypes
char* buf = "Hello, I am process ";
void itoa(int, char*, int);
void strCat(char*, const char*);
int charCount(char*);
int digitCount(int number);

int main(int argc, char *argv) {
        size_t result;
	// get pid
	int pid = syscall(SYS_getpid);
        // calculate digits in pid
	int digitLen = digitCount(pid);
	// make a char sting the size of the digits in pid 
	char cPid[digitLen];
	//put pid in a char
	itoa(pid, cPid, digitLen);

	//end srtring	
	char end[] = "!\n";
	
	//calculate the length of the final string and make a variable to place it
	int finalLength = charCount(buf)+charCount(cPid)+charCount(end);		
	char finalString[finalLength];
	finalString[0] = '\0';
	
	//now concat all the string
	strCat(finalString,buf);
        strCat(finalString,cPid);
	strCat(finalString,end);

        /* "man 2 write" to see arguments to write syscall */
        result = syscall(SYS_write, 1, finalString, finalLength);

        return (int) result;
}

//Make int a char
void itoa(int number, char* string, int digitLen){
        //int is generated in reverse order
        //but we start from the digitLen -1 index to counteract it.
        do {
                // get a digit of the number and save is string
                // then move to next number
                string [--digitLen] = number % 10 +'0';
        } while ((number /= 10) > 0); // delete the digit that was copied

}

//Concat str to dest
void strCat(char* dest, const char* src){
	//loop through dest string
	while (*dest != '\0')
        	*dest++ ;
	//start putting the src string after dest string
 	do{
        	*dest++ = *src++;
   	 } while (*src != '\0');
	//terminate dest
	*dest++ = '\0';
}

// count length on a c string 
int charCount(char* string) {
	int count = 0;
	do {
		count++;
	} while (*(++string)!= '\0'); 
	return count;
}

//counts number of digits in a number
int digitCount(int number) {
	int count = 0;
	// keep counting until no digits left
	do{
		count++;
	} while ((number/=10) > 0);
	return count;
}

