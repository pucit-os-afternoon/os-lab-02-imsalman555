#include <pthread.h>  // header file for threads
#include <stdio.h>   // header file for standard input and output.
#include <stdlib.h> // header file for functions involving memory allocation, process control, conversions and others
#include <unistd.h> //header file that provides access to the POSIX operating system API. (fork, pipe and I/O primitives [read, write, close, etc.] )
#include <errno.h>  //header file for error handling

//struct to pass large number arrays
struct matrix
{
    int row;
    int col;
};

void * multiplyer (void *);     //prototype for function multiplyer

int threadno=0;

int** matA;
int** matB;
int** matC;

int noofrowsA=0,noofrowsB=0,noofcolsA=0,noofcolsB=0;

int main(int argc, char ** argv)
{

    //fetching file path from commandline arguments
	char* file1_name=argv[1];
	char* file2_name=argv[2];

	FILE *outfile1, *outfile2;

	//code to open 2 files
	outfile1=fopen(file1_name,"r");
	outfile2=fopen(file2_name,"r");
	if (outfile1==NULL || outfile2==NULL)
	{
        	fprintf(stderr,"File(s) doesn't exists.\n");
        	fclose(outfile1);
        	fclose(outfile2);
        	return 1;
	}
	else
	{
        printf("Files exists.\n");

        char buffer[1000];
        char ch,tempch='\0';

        //code to fill 2 dynamic arrays/matrix with approriate size and given values in a file

        //collecting rows and coloums for matrix A
        while((ch = fgetc(outfile1)) != EOF)
        {
            if (ch!=' ' && (tempch== ' ' || tempch=='\0'))
                noofcolsA++;
            else if (ch!=' ' && tempch>='0' && tempch<'9')
            {
                noofrowsA++;
                while(fgets(buffer,sizeof(buffer),outfile1)!=NULL)
                    noofrowsA++;
            }
            tempch=ch;
        }
        fclose(outfile1);

         //allocating memory dynamically

        matA = (int **)malloc(noofrowsA * sizeof(int *));
        for (int i=0; i<noofrowsA; i++)
        {
            matA[i] = (int *)malloc(noofcolsA * sizeof(int));
        }
        outfile1=fopen(file1_name,"r");
        for (int i=0;i<noofrowsA;i++)
        {
            for (int j=0;j<noofcolsA;j++)
            {
                fscanf(outfile1,"%d",&matA[i][j]);
            }
        }
        fclose(outfile1);

        //printing A

        printf("Matrix A\n");
        for (int i=0;i<noofrowsA;i++)
        {
            for (int j=0;j<noofcolsA;j++)
            {
                printf("%d ",matA[i][j]);
            }
            printf("\n");
        }

        //collecting rows and coloums for matrix B

        buffer[1000];
        tempch='\0';

         while((ch = fgetc(outfile2)) != EOF)
        {
            if (ch!=' ' && (tempch== ' ' || tempch=='\0'))
                noofcolsB++;
            else if (ch!=' ' && tempch>='0' && tempch<'9')
            {
                noofrowsB++;
                while(fgets(buffer,sizeof(buffer),outfile2)!=NULL)
                    noofrowsB++;
            }
            tempch=ch;
        }
        fclose(outfile2);

        //allocating memory dynamically

        matB = (int **)malloc(noofrowsB * sizeof(int *));
        for (int i=0; i<noofrowsB; i++)
         matB[i] = (int *)malloc(noofcolsB * sizeof(int));
        outfile2=fopen(file2_name,"r");
        for (int i=0;i<noofrowsB;i++)
        {
            for (int j=0;j<noofcolsB;j++)
            {
                fscanf(outfile2,"%d",&matB[i][j]);
            }
        }
        fclose(outfile2);

        //printing B

        printf("Matrix B\n");
        for (int i=0;i<noofrowsB;i++)
        {
            for (int j=0;j<noofcolsB;j++)
            {
                printf("%d ",matB[i][j]);
            }
            printf("\n");
        }

        //code to check wether matrix are multiplyable or not

        if (noofrowsA==noofcolsB)
        {
            printf("Multiplication possible.\n\n");

            //allocating memory dynamically
            matC = (int **)malloc(noofrowsA * sizeof(int *));
            for (int i=0; i<noofcolsB; i++)
                matC[i] = (int *)malloc(noofcolsB * sizeof(int));


            //code to created a pool of thread depending on size of operations required on matrixes
            struct matrix C[noofrowsA];

            //creating thread object
            pthread_t mythread[noofrowsA];
            for (int t = 0; t < noofrowsA ; t++)
            {
                //code to invoke function of multiplyer for multiplciation of data elements of matrixes
                C[t].row=t;
                C[t].col=t+1;
                pthread_create(&mythread[t], NULL, multiplyer, (void*)&C[t]);
            }
            for (int t = 0; t < noofrowsA ; t++)
            {
                //code to allocate a 3rd array/matrix with appropriate size and fill it with return values of multiplyer
                pthread_join(mythread[t], NULL);
            }

            //printing C
            printf("Matrix C\n");
            for (int i=0;i<noofrowsA;i++)
            {
                for (int j=0;j<noofcolsB;j++)
                {
                    printf("%d ",matC[i][j]);
                }
                printf("\n");
            }

            //code to store the resultant matrix in a file
            printf("\nWriting Resultant Matrix C into file...\n");
            FILE* infile;
            infile=fopen("resultant matrix.txt","w");
            for (int i=0;i<noofrowsA;i++)
            {
                for (int j=0;j<noofcolsB;j++)
                {
                    fprintf(infile,"%d\t",matC[i][j]);
                }
                fprintf(infile,"\n");
            }
            fclose(infile);
            printf("File has been written..\n");
            printf("\nmain is ended \n");
            return 0;
        }
        else
        {
            printf("Multiplication not possible.\n");
            return 1;
        }
	}
}

void * multiplyer (void * arg)
{
    // this printing could be cumbersome so don't panic if unexpected results are produced
	printf("executing thread # %d\n", threadno++ );

    //code for multiplciation which would be done on args (remmeeber to typecast it back to its origonal type)
	struct matrix *C = (struct matrix *)arg;

	//code to return a single value that would be placed a single index of 3rd matrix
    for (int a = C->row; a < C->col; a++)
    {
        for(int b=0;b<noofrowsA;b++)
        {
            int sum=0;
            for (int c=0;c<noofcolsB;c++)
                sum+=matA[a][c]*matB[c][b];
            matC[a][b]=sum;
        }
    }
    pthread_exit(NULL);
}
