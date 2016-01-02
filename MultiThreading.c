#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// The following structure contains the necessary information  
// to allow the function "dotprod" to access its input data
// and place its output into the structure. This structure
// is unchanged from the sequential version.
typedef struct {
    double  *a;
    double  *b;
    double  sum; 
    int     veclen; 
    } DOTDATA;

// The following structure  contains what a thread can return.
typedef struct {
    double  sum;
	int     start;
	int     end;
	int     len;
	long    offset;
	} myret_t;

// Define globally accessible variables the treads and dot product.
#define NUMTHRDS 1000
#define VECLEN 1000
    DOTDATA dotstr; 
    pthread_t callThd[NUMTHRDS];

// The function dotprod is activated when the thread is created.
// As before, all input to this routine is obtained from a structure
// of type DOTDATA and all output from this function is written into
// the myret_t structure and returned. The benefit of this approach
// is apparent for the multi-threaded program: when a thread is created 
// we pass a single argument to the activated function - typically
// this argument is a thread number. All  the other information required
// by the function is accessed from the globally accessible structure. 
void *dotprod(void *arg) {
    // Define and use local variables for convenience.
    int i;
    double *x, *y;
    x = dotstr.a;
    y = dotstr.b;
   
    // define what to return
    myret_t *r = malloc(sizeof(myret_t));
    r->offset = (long)arg;
    r->len = dotstr.veclen;
    r->start =  r->offset*r->len;
    r->end   = r->start + r->len;

    // Perform the dot product and return the result.
    r->sum = 0;
    for (i=r->start; i<r->end ; i++) {
        r->sum += (x[i]*y[i]);
    }
    
    pthread_exit((void*) r);
}
 
// The main program creates threads which do all the work and then 
// print out result upon completion. Before creating the threads,
// the input data is created. Since all threads update a shared 
// structure, we use thread_join to get the some compute by a thread
// and then add in to our structure. The main thread needs to wait
// for all threads to complete before adding to out structure, it 
// waits for each one of the threads. We specify a thread attribute
// value that allow the main thread to join with the threads it creates.
// Note also that we free up handles  when they are no longer needed.
int main (int argc, char *argv[]) {
    long i;
    double *a, *b;
    double threadSum;
    pthread_attr_t attr;
    myret_t *m;

    // Assign storage and initialize values
    a = (double*) malloc (NUMTHRDS*VECLEN*sizeof(double));
    b = (double*) malloc (NUMTHRDS*VECLEN*sizeof(double));
  
    for (i=0; i<VECLEN*NUMTHRDS; i++) {
        a[i]=1;
        b[i]=a[i];
    }

    dotstr.veclen = VECLEN; 
    dotstr.a = a; 
    dotstr.b = b; 
    dotstr.sum=0;

    // Create threads to perform the dotproduct
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Note: we wait for the threads to complete and return its calculated sum.
    // After the thread(s) completed the calculation then only do we add that
    // change to the critical section, i.e. our structures sum variariable.
    // Since we wait for the thread and the main thread does the actual adding
    // we can avoid race conditions and not have to use a lock.
    for(i=0;i<NUMTHRDS;i++) {
        // Each thread works on a different set of data.
        // The offset is specified by 'i'. The size of
        // the data for each thread is indicated by VECLEN.
        pthread_create(&callThd[i], &attr, dotprod, (void *)i);
       
        // After joining wait for other thread to complete
        // and to the total sum and print out.
        // Note: free the return value when done with it.
        pthread_join(callThd[i], (void**) &m);
        dotstr.sum += m->sum;   
        printf("Thread %ld did %d to %d:  mysum=%f global sum=%f\n",
                m->offset,m->start,m->end,m->sum,dotstr.sum);
        free (m);
    }

    pthread_attr_destroy(&attr);

    // print out the results and cleanup
    printf ("Sum =  %f \n", dotstr.sum);
    free (a);
    free (b);
    pthread_exit(NULL);
}