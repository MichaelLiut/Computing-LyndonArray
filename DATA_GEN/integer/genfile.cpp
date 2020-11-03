#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>


// function condense -----------------------------------------
void condense(long *string,long len,long alpha,long *A,long *B) {
  long i, j, beta;
  bool found=false;

  for(beta=i=0; i<alpha; i++) {
    if (A[i]==0) {
      B[i]=-1;
      found=true;
    }else
      B[i]=beta++;
  }
  if (! found) { // string is already condensed
    return;
  }
  for(i=0; i<len; i++) {
    string[i]=B[string[i]];
  }
  // check for condensedness
  for(i=0; i<alpha; A[i++]=0);
  for(i=0; i<len; i++)
    A[string[i]]++;
  // find first 0
  for(i=0; !(i==alpha||A[i]==0); i++);
  if (i==alpha) return; // string condensed
  for(j=i+1; j<alpha; j++) {
    if (A[j]!=0) {
      printf("condense error on string:\n");
      for(i=0; i<len; i++)
        printf(".%u",string[i]);
      putchar('\n');
      exit(0);
    }
  }
}//end condense



// function genrec ----------------------------------
void genrec(long len,long alpha,long* string,long* A,long* B,FILE* fp) {
  long x, i, j;

  for(i=0; i<alpha; i++) {
    A[i]=0;
  }
  for(i=0; i<len; i++) {
    x=(long) (rand() % len);
    if (alpha>0) x=(x % alpha);
    string[i]=x;
    A[x]++;
 }//end for

 condense(string,len,alpha,A,B);
 for(i=0; i<len; i++) {
   fprintf(fp,".%u",string[i]);
 }
 fprintf(fp,"\n");
}// end genrec



// function main -------------------------------------------------------
int main(int argc,char* argv[]) {
  if (argc!=5) {
    printf("usage -- %s <string_len> <alphabet_size> <file_size>"
           " <filename>\n",argv[0]); 
    return 0;
  }
  long len = atoi(argv[1]);
  long alpha = atoi(argv[2]);
  long filesize=atoi(argv[3]);
  char *filename;
  long fl=strlen(argv[4]);
  filename=new char[fl+1];
  strcpy(filename,argv[4]);

  FILE* fp=fopen(filename,"w");
  if (fp==0) {
    printf("can't open output file \"%s\"\n",filename);
    return 0;
  }

  long *A=new long[alpha];
  long *B=new long[alpha];
  long* string=new long[len];

  srand((unsigned long)time(NULL));
  for(long i=0; i<filesize; i++)
    genrec(len,alpha,string,A,B,fp);

  fclose(fp);
  printf("%u strings of length %u over alphabet of size %u generated\n",
         filesize,len,alpha);
  return 0;
}//end main
