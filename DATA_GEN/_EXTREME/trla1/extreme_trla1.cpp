#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define LEN 5000000


#define MAX LONG_MAX-2
#define min(a,b) (((a)<(b))?(a):(b))


#include "Lstring.hpp"
#include "lynarr.hpp"


Lstring* gen(Lstring& u) {
  //000u00u
  long i, j;
  long size=2*u.len + 5;
  Lstring* x=new Lstring(size);
  j=0;
  x->body[j++]=0;
  x->body[j++]=0;
  x->body[j++]=0;
  for(i=0; i < u.len; i++)
    x->body[j++]=u[i];
  x->body[j++]=0;
  x->body[j++]=0;
  for(i=0; i < u.len; i++)
    x->body[j++]=u[i];
  return x;
}


long printsize(long p) {
  long s=1;
  while(p>=10) {
    s++;
    p=p/10;
  }
  return s;
}


void dot(char c,long size) {
  long i;
  for(i=0; i < size; i++) putchar(c);
}

#define E(b) ((b)+la[(b)]-1)
inline bool is_bad(long b,long* la,Lstring& s) {
  if (b+2<s.len) {
    if (E(b+1) < E(b-1) && E(b+2) < E(b-1)) {
      return true;
    }else{
      return false;
    }
  }else{
    return false;
  }
}


void show_bad(Lstring& s) {
  long* la = idla(s);
  s.shownl();
  long a=0;
  long b=1;
  while(true) {
    if (s[a]>s[b] && s[b]<=s[b+1]) {
      dot(' ',printsize(s[b])+1);
      a=b;
      b=b+1;
      if (b>=s.len) break;
    }else{
      // b is white
      // is it bad ?
      if (is_bad(b,la,s)) {
        dot(' ',printsize(s[a])+2);
        dot('*',printsize(s[b]));
      }else{
        dot(' ',printsize(s[a]+1));
        dot(' ',printsize(s[b]+1));
      }
      a=b+1;
      b=a+1;
      if (b>=s.len) break;
    }
  }//endwhile
  putchar('\n');
}//end show_bad


void datasets(Lstring& x) {
  long d, d1, i;
  FILE *fp;
  char outfile[100];
  static bool init=true;

  if (init) {     //empty all datasets
    init=false;
    long d;
    for(d=10; d<=LEN; d=10*d) {
      sprintf(outfile,"extreme_trla1_%lu.dat",d);
      FILE* fp=fopen(outfile,"w");
      if (5*d <= LEN) {
        sprintf(outfile,"extreme_trla1_%lu.dat",5*d);
        FILE* fp=fopen(outfile,"w");
      }
    }
  }

  for(d=10; d <= x.len; d=10*d) {
    sprintf(outfile,"extreme_trla1_%d.dat",d);
    printf("appending \"%s\"\n",outfile);
    fp=fopen(outfile,"a");
    if (fp == 0) {
      printf("can't open output file \"%s\"\n",outfile);
      exit(0);
    }
    for(i=0; i<d; i++) fprintf(fp,".%lu",x[x.len-d+i]);
    fprintf(fp,"\n");
    fclose(fp);
    d1=5*d;
    if (d1<=x.len) {
      sprintf(outfile,"extreme_trla1_%lu.dat",d1);
      printf("appending \"%s\"\n",outfile);
      fp=fopen(outfile,"a");
        if (fp == 0) {
        printf("can't open output file \"%s\"\n",outfile);
        exit(0);
      }
      for(i=0; i<d1; i++) fprintf(fp,".%lu",x[x.len-d1+i]);
      fprintf(fp,"\n");
      fclose(fp);
    }
  }//end for
}//end datasets


bool comp(Lstring& x,long k) {
  // comparing x[0..k-1], x[k..len-1]
  long i, j;
  for(i=0, j=k; i<=k-1 && j<=x.len-1; i++, j++) {
    if (x[i]<x[j])
      return true;
    else if (x[i]>x[j])
      return false;
    if (i==k-1 && j==x.len-1)
      return false;
    if (i==k-1) 
      return true;
    if (j==x.len-1)
      return false;
  }
  return false;
}

bool is_lyndon(Lstring& x) {
  long k;
  for(k=1; k < x.len; k++) {
    if (! comp(x,k)) return false;
  }
  return true;
}


void next(Lstring* x) {
  for(long i=x->len-1; i>=0; i--) {
    if (x->body[i]==0) {
      x->body[i]=1;
      if (i==0) goto A;
      return;
    }else{
      x->body[i]=0;
      continue;
    }
  }
  A:   delete[] x->body;
  x->len++;
  x->body = new long[x->len];
  for(long i=0; i<x->len; x->body[i++]=0);
  return;
}


int main() {
  Lstring *seed, *x, *y;
  seed=new Lstring(".0");
  long scount=0;
  while(true) {
    if (scount >= 100) break;
    next(seed);
    if (!is_lyndon(*seed)) continue;
    printf("seed: "); seed->shownl();
    scount++;
    x = new Lstring(seed->len);
    *x=*seed;
    y=0;
    while(true) {
      y=gen(*x);
      //if (y->len <= 1000) show_bad(*y);
      if (y->len >= LEN) {
        datasets(*y);
        break;
      }
      //printf("gen: "); y->shownl();
      delete x;
      x=y;
      y=0;
    }//end while
  }

  printf("done\n");
  getchar();
  return 0;
}
