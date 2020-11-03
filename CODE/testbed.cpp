#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

/******************************************************************************/
// define precision for time measurements, 3 decimal places, i.e. in milliseconds
#define prec 3
// define which program is being tested by defining in the compilation
/******************************************************************************/

#define MAX (LONG_MAX-2)
#define min(a,b) ((a<=b) ? (a) : (b))

#include "Lstring.hpp"
#include "lynarr.hpp"
#ifdef _BSLA
  long* bsla_space=0;
  #include "bsla.cpp"
#endif
#ifdef _TRLA
  #include "debug.hpp"
  #include "Tau.hpp"
  #include "trla.cpp"
#endif

#include <time.h>
clock_t t;



// display ticks as seconds with prec decimal places
// function show_time ----------------------------------------------------------
char* show_time(clock_t t) {
  static char buf[100];
  long i, j;

  sprintf(buf,"%u.%u",(t/CLOCKS_PER_SEC),(t%CLOCKS_PER_SEC));
  for(i=0; buf[i]!='.'; i++);
  buf[i+prec+1]='\0';
  return buf;
}//end show_time



#ifdef _IDLA
// function measurement --------------------------------------------------------
long measurement(Lstring *x,long *la,long string_counter,char* infile,
                 FILE* fpo,long repeat) {
   // turn on the clock here
   t=clock();
   for(long i=0; i<repeat; i++) {
     idla1(*x,la);
   }
   // turn off the clock here
   t = clock()-t;
   // record the time
   fprintf(fpo,"%u repeats, string no. %u, dataset \'%s\', %u ticks\n",
           repeat,string_counter,infile,t);
   #ifndef _silent
   printf("%u repeats, string no. %u, dataset \'%s\', %u ticks\n",
           repeat,string_counter,infile,t);
   #endif
   return t;
}//end measurement
#endif



#ifdef _BSLA
// function measurement --------------------------------------------------------
long measurement(Lstring *x,long *la,long string_counter,char* infile,
                 FILE* fpo,long repeat) {
   static long* space=0;
   long i;
   // turn on the clock here
   t=clock();
   for(i=0; i<repeat; i++) {
     bsla2la(*x,la,space);
   }
   // turn off the clock here
   t = clock()-t;
   // record the time
   fprintf(fpo,"%u repeats, string no. %u, dataset \'%s\', %u ticks\n",
           repeat,string_counter,infile,t);
   #ifndef _silent
   printf("%u repeats, string no. %u, dataset \'%s\', %u ticks\n",
           repeat,string_counter,infile,t);
   #endif

// test correctness
#ifdef _test_correctness
   static long *la1=0;
   if (la1==0) la1=new long[x->getLen()];
   idla1(*x,la1);
   for(i=0; i < x->getLen(); i++) {
     if (la[i]!=la1[i]) {
       #ifdef _silent
         FILE* err;
         err=fopen("silent_testbed.error","w");
         fprintf(err,"ERROR\n");
         fclose(err);
         exit(1);
       #else
         printf("ERROR\n");
         getchar();
	 exit(1);
       #endif
     }
   }
   #ifndef _silent
   printf("PERFECT\n");
   #endif
#endif
   return t;
}//end measurement
#endif



#ifdef _TRLA
// function measurement --------------------------------------------------------
long measurement(Lstring *x,long *la,long string_counter,char* infile,
                 FILE* fpo,long repeat) {
   static Tau* tau=0;
   long i;

   if (tau==0) tau=new Tau(x);

   // turn on the clock here
   t=clock();
   for(long i=0; i<repeat; i++)
     trla(x,tau,la);
   // turn off the clock here
   t = clock()-t;
   // record the time
   fprintf(fpo,"%u repeats, string no. %u, dataset \'%s\', %u ticks\n",
           repeat,string_counter,infile,t);
   #ifndef _silent
   printf("%u repeats, string no. %u, dataset \'%s\', %u ticks\n",
           repeat,string_counter,infile,t);
   #endif

// test correctness
#ifdef _test_correctness
   static long *la1=0;
   if (la1==0) la1=new long[x->getLen()];
   idla1(*x,la1);
   for(i=0; i < x->getLen(); i++) {
     if (la[i]!=la1[i]) {
       #ifndef _silent
       printf("ERROR\n");
       getchar();
       #endif
       FILE* err;
       err=fopen("silent_testbed.error","w");
       fprintf(err,"ERROR\n");
       fclose(err);
       exit(1);
     }
   }
   #ifndef _silent
   printf("PERFECT\n");
   #endif
#endif
   return t;
}//end measurement
#endif



void format(FILE *fp,long ticks,long repeat) {
   double r = (double) ticks / repeat;
   long t=0;
   if (r >= 10) {
      t = 0;
      while(r >= 10) {
         t++;
         r = r/10;
      }
      fprintf(fp,"%1.3fe+%d",r,t);
   }else if (r < 1) {
      t = 0;
      while(r < 1) {
         t--;
         r = r*10;
      }
      fprintf(fp,"%1.3fe%d",r,t);
   }else{
      fprintf(fp,"%1.3f",r);
   }
}



// function main ---------------------------------------------------------------
int main(int argc, char* argv[]) {
  if (argc != 2) {
    #ifndef _silent
      printf("testing algorithm ");
        #ifdef _IDLA
          printf("IDLA");
        #endif
        #ifdef _BSLA
          printf("BSLA");
        #endif
        #ifdef _TRLA
          printf("TRLA");
        #endif
      printf(" in verbose mode\n");
      printf("usage -- %s <infile>\n",argv[0]);
      return 0;
    #else
      FILE* err;
      err=fopen("silent_testbed.error","w");
      fprintf(err,"testing algorithm ");
        #ifdef _IDLA
          fprintf(err,"IDLA");
        #endif
        #ifdef _BSLA
          fprintf(err,"BSLA");
        #endif
        #ifdef _TRLA
          fprintf(err,"TRLA");
        #endif
        fprintf(err," in silent mode\n");
        fprintf(err,"usage -- %s <infile>\n",argv[0]);
        fclose(err);
        return 0;
    #endif
  }
  // so everything is OK

  char infile[200];
  sprintf(infile,"DATA/%s.dat",argv[1]);
  char outfile[200];
  #ifdef _IDLA
     sprintf(outfile,"RESULTS/IDLA/%s.res",argv[1]);
  #endif
  #ifdef _TRLA
     sprintf(outfile,"RESULTS/TRLA/%s.res",argv[1]);
  #endif
  #ifdef _BSLA
     sprintf(outfile,"RESULTS/BSLA/%s.res",argv[1]);
  #endif

  FILE* fpi=fopen(infile,"r");
  if (fpi==0) {
    #ifndef _silent
      printf("can't open input file \"%s\"\n",infile);
    #else
    FILE* err;
    err=fopen("silent_testbed.error","w");
      fprintf(err,"can't open input file \"%s\"\n",infile);
    fclose(err);
    #endif
    return 0;
  }

  FILE* fpo=fopen(outfile,"w");
  if (fpo==0) {
    #ifndef _silent
      printf("can't open output file \"%s\"\n",outfile);
    #else
      FILE* err;
      err=fopen("silent_testbed.error","w");
      fprintf(err,"can't open output file \"%s\"\n",outfile);
      fclose(err);
    #endif
    return 0;
  }

  #ifndef _silent
    printf("processing strings from dataset \'%s\'\n",infile);
    printf("times recorded in file \'%s\'\n",outfile);
    printf("measuring performance of algorithm ");
    #ifdef _IDLA
       printf("IDLA\n");
    #endif
    #ifdef _BSLA
       printf("BSLA\n");
    #endif
    #ifdef _TRLA
       printf("TRLA\n");
    #endif
  #endif
  fprintf(fpo,"processing strings from dataset \'%s\'\n",infile);
  fprintf(fpo,"times recorded in file \'%s\'\n",outfile);
  fprintf(fpo,"measuring performance of algorithm ");
  #ifdef _IDLA
    fprintf(fpo,"IDLA\n");
  #endif
  #ifdef _BSLA
    fprintf(fpo,"BSLA\n");
  #endif
  #ifdef _TRLA
    fprintf(fpo,"TRLA\n");
  #endif

  char c;
  char* buf = 0;
  long i=0;
  bool bufful=false;
  Lstring* x = 0;
  long len;
  long time_sum=0;
  long tmin=0, tmax=0;

  // first get the length by reading the first string
  len=0;
  while(true) {
    c=fgetc(fpi);
    if (feof(fpi)) break;
    if (c=='\0'||c=='\n'||c=='\r') break;
    if (c == '.') len++;
  }
  rewind(fpi);
  long repeat=10000000/len;   // set repeat factor
  long d=len;
  long ds=0;
  while(d > 0) {
    ds++;
    d = d/10;
  }
  long linesize=(ds+1)*len+2;
  char *line = new char[linesize];

  #ifndef _silent
    printf("string length = %lu, repeat factor=%lu\n",len,repeat);
  #endif
  fprintf(fpo,"string length = %lu, repeat factor=%lu\n",len,repeat);
  
  Lstring* s=new Lstring(len);
  long string_counter=0;

  // process the file
  while(fgets(line,linesize,fpi)) {
    for(i=strlen(line); i>=0; i--) {    // deend the line
      if (line[i]=='\0' || line[i]=='\n' || line[i]=='\r') {
        line[i]='\0';
        continue;
      }else{
        break;
      }
    }//endfor
 
    (*s)=line;
    #ifndef _silent
      s->shownl_part(20);
    #endif

    string_counter++;
    long *la=new long[s->getLen()];
    long itim = measurement(s,la,string_counter,infile,fpo,repeat);
    if (tmax == 0 || itim > tmax) tmax = itim;
    if (tmin == 0 || itim < tmin) tmin = itim;
    time_sum += itim;
  }//end while

  // compute average time in ticks
  long aver = time_sum / string_counter;
  #ifndef _silent
    format(stdout,tmin,repeat);
    putchar(',');
    format(stdout,aver,repeat);
    putchar(',');
    format(stdout,tmax,repeat);
    putchar('\n');
  #endif
    format(fpo,tmin,repeat);
    fputc(',',fpo);
    format(fpo,aver,repeat);
    fputc(',',fpo);
    format(fpo,tmax,repeat);
    fputc('\n',fpo);

  fclose(fpi);    
  fclose(fpo);    
  return 0; 
}//end main
