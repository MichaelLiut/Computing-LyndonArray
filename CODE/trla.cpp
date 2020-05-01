#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <ctime>

#define min(a,b) ((a<=b) ? (a) : (b))
#define MAX (LONG_MAX-2)

//#define _mini_debug
//#define _debug
//#define _test_sort
#ifdef _test_sort
  #include "Lstring.hpp"
  #include "debug.hpp"
  #include "Tau.hpp"
  #include "lynarr.hpp"
#else
  #ifdef _debug
    #include "Lstring.hpp"
    #include "debug.hpp"
    #include "Tau.hpp"
    #include "lynarr.hpp"
  #endif
  #include "Lstring.hpp"
  #include "Tau.hpp"
  #include "lynarr.hpp"
#endif

#define e(i) i+la[i]-1


void showla(long *la,long len) {
  for(long i=0; i<len; i++) {
  if (la[i]>=0)
     printf("%ld ",la[i]);
  else
    printf("? ",la[i]);
  }
  printf("\n");
}


// function FillGaps ---------------------------------------------------------
void FillGaps(long* la,Lstring* s) {
  long i, A, B, Aend, Bend, frame_end;

  for(i=s->getLen()-1; i>0; i--) {
    if (la[i]>=0) continue;
    if (i==s->getLen()) {
      la[i]=1;
      continue;
    }
    if ((*s)[i]>(*s)[i+1]) {
      la[i]=1;
      continue;
    }
    if ((*s)[i]==(*s)[i+1] && la[i+1]==1) {
      la[i]=1;
      continue;
    }
    // so s[i]<=s[i+1]
    la[i]=la[i+1]+1; // extend [i+1 .. e(i+1)] to [i .. e(i+1)]
    frame_end=e(i-1);
    while(e(i)<frame_end) {
      A=i;
      Aend=e(i);
      B=Aend+1;
      Bend=e(B);
      // must compare A=[i .. e(i)] and B=[e(i)+1 .. e(e(i)+1)]
      // if A >= B break the loop
      // if A < B, join A+B and continue
      while(true) {
        if ((*s)[A]>(*s)[B]) goto lab;
        if ((*s)[A]<(*s)[B]) {
          la[i]=la[i]+la[i+la[i]]; // join, and start anew
          break;
        }
        // so s[A]=s[B]
        if (A==Aend && B < Bend) {
          la[i]=la[i]+la[i+la[i]]; // join, start anew
          break;
        }else if (A<=Aend && B==Bend) {
          goto lab;
        }else{
          A++;
          B++;
          continue;
        }
      }//endwhile
    }//endwhile
    lab: ;
  }
}//end FillGaps


// function trla -------------------------------------------------------------
void trla(Lstring* s,Tau* t,long *la) {
  long i, j, k;
  t->reset(s);
  t->translate();

  // base case
  if (s->getLen() < 10) {
  idla1(*s,la);
  return;
  }
  // recursive case
  long* la1=new long[t->taus->getLen()];
  Tau* t1=new Tau(t->taus);
  trla(t->taus,t1,la1);
  
  // the loop below may miss the very last element
  // so let us pre-set it.
  la[t->len-1]=1;
  j=0;
  for(i=0; i < t->taus->getLen(); i++) {
  if (t->preindex[i]>j) la[j++]=-1;
  if ((k=t->preindex[i+la1[i]-1]) >= (t->len-1))
    la[j]=k-j+1;
  else if ((*(s))[k+1] > (*(s))[j])
    la[j]=k-j+2;
  else
    la[j]=k-j+1;
  j++;
  }
  delete[] la1;
  delete t1;

  FillGaps(la,s);
}//end trla


#ifdef _stand_alone
// function main -------------------------------------------------------------
int main() {
  long size=50;
  char infile[]="eng50.asc";

  long d=size;
  long ds=0;
  while(d>0) {
  ds++;
  d=d/10;
  }
  long linesize=(ds+1)*size;
  FILE* fp=fopen(infile,"r");
  if (fp==0) {
  printf("can't open \"%s\"\n",infile);
  getchar();
  return 0;
  }
  char* line=new char[linesize];
  Lstring* s=new Lstring(size);
  Tau* t=new Tau(s);
  long* la=new long[size];
  while(fgets(line,linesize,fp)) {
    for(long i=strlen(line); i>=0; i--) { 
      if (line[i]=='\0' || line[i]=='\n' || line[i]=='\r') {
      line[i]='\0';
      continue;
      }else{
      break;
      }
    }
    (*s)=line;
    trla(s,t,la);
  
    long* la1=idla(*s);
    for(long i=0; i<s->getLen(); i++) {
      if (la[i]!=la1[i]) {
      printf("pruser\n");
      getchar();
      return 0;
      break;
      }
    }
    s->shownl();
    showla(la,s->getLen());
    printf("PERFECT\n");
  }//endwhile

  printf("done\n");
  getchar();
  return 0;
}//end main
#endif
