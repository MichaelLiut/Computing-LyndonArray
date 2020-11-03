#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>



// function genrec ----------------------------------
void genrec(long len,long alpha,long* A,long* B,long* string,FILE* fp) {
  long x, i, j;
  static long count=0;

  printf("generating record %u\n",++count);

  Repeat:

  for(i=0; i<alpha; B[i++]=0);

  for(i=0; i<len; i++) {
    x=(long) (rand() % alpha);
    string[i]=A[x];
    B[x]++;
  }//end for

  // did we use all the letters in the alphabet?
  for(i=0; i<alpha; i++)
   if (B[i]==0) goto Repeat;

  for(i=0; i<len; i++) {
   fprintf(fp,".%u",string[i]);
  }
  fprintf(fp,"\n");
}// end genrec



// function genrec1 ----------------------------------
void genrec1(long len,FILE* fp) {
  long x, i, j;
  static long count=0;

  printf("generating unbounded record %u\n",++count);

  for(i=0; i<len; i++)
    fprintf(fp,".%u",(long) (rand() % len));
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

  printf("generating file \"%s\"\n",filename);

  FILE* fp=fopen(filename,"w");
  if (fp==0) {
    printf("can't open output file \"%s\"\n",filename);
    return 0;
  }

  srand((unsigned long)time(NULL));
  long* string = new long[len+1];

  if (alpha == 0 || alpha >= len) { // unbounded integer alphabet
    for(long i=0; i<filesize; i++)
      genrec1(len,fp);
  }else{                            // bounded integer alphabet
    long *A=new long[alpha];
    long last_letter;
    long* B = new long[alpha];
    long x;
  
    for(long i=0; i<filesize; i++) {
      last_letter=-1;
      // generate alphabet of size alpha that is a subset of {0..len}
      // and store it in A
      for(long i=0; i<alpha; i++) {
        while(true) {
          x=(long) (rand() % len);
          if (last_letter == -1) {
            A[++last_letter]=x;
            break; // the while loop
          }else{
            bool found=false;
            for(long j=0; j<=last_letter; j++) {
              if (A[j]==x) {
                found=true;
                break;
              }
            }//end for
            if (found) continue;
            A[++last_letter]=x;
            break; // the while loop
          }
        }//end while
      }//end for
      // now we have the alphabet and we can generate the record
      genrec(len,alpha,A,B,string,fp);
    }//endfor
  }

  fclose(fp);
  printf("%u strings of length %u over alphabet of size %u generated\n",
         filesize,len,alpha);
  return 0;
}//end main
