#ifndef _Tau_hpp
#define _Tau_hpp

#define nil len+1

#ifdef _mini_debug
#define _dsize_defined
long dsize(long i) {
  long d=1;
  while(true) {
    if (i < 10) break;
    d++;
    i = i/10;
  }
  return d;
}
#endif

class Tau {
public:
  long *array1, *array2, *array3;
  long len;
  long last;
  long *tp;
  long *preindex;
  Lstring* s;
  Lstring* taus;

public:

  Tau(Lstring& s1) {
    s=&s1;
    len=s->getLen();
    array1=new long[3*len];
    array2=array1+len;
    array3=array2+len;
    last=0;
    tp=preindex=0;
    taus=0;
  }

  void reset(Lstring& s1) {
    s=&s1;
    if (len >= s->getLen()) {
      len=s->getLen();
    }else{
      len=s->getLen();
      if (array1) delete[] array1;
      array1=new long[3*len];
    }
    array2=array1+len;
    array3=array2+len;
    last=0;
    tp=preindex=0;
    if (taus) delete taus;
    taus=0;
  }

  Tau(Lstring* s1) {
    s=s1;
    len=s->getLen();
    array1=new long[3*len];
    array2=array1+len;
    array3=array2+len;
    last=0;
    tp=preindex=0;
    taus=0;
  }

  void reset(Lstring* s1) {
    s=s1;
    if (len <= s->getLen()) {
      len=s->getLen();
    }else{
      len=s->getLen();
      if (array1) delete[] array1;
      array1=new long[3*len];
    }
    array2=array1+len;
    array3=array2+len;
    last=0;
    tp=preindex=0;
    if (taus) delete taus;
    taus=0;
  }

  Tau(Tau& t1) {
    len=t1.len;
    last=t1.last;
    s=t1.s;
    array1=new long[3*len];
    array2=array1+len;
    array3=array2+len;
    if (t1.tp==t1.array1)
      tp=array1;
    else if (t1.tp==t1.array2)
      tp=array2;
    else if (t1.tp==t1.array3)
      tp=array3;
    else
      tp=0;
    if (t1.preindex==t1.array1)
      preindex=array1;
    else if (t1.preindex==t1.array2)
      preindex=array2;
    else if (t1.preindex==t1.array3)
      preindex=array3;
    else
      preindex=0;
    for(long i=0; i < len; i++) {
      array1[i]=t1.array1[i];
      array2[i]=t1.array2[i];
      array3[i]=t1.array3[i];
    }
    if (t1.taus)
      taus = new Lstring(*t1.taus);
  }

  ~Tau() {   
	if (array1) delete[] array1;
	if (taus) delete taus;
  }

  long getLast() { return last; }

  Lstring* translate() {
    // tp=array1 is the sorted tau pairs
    long *a1=array2;
    preindex=array3;
    long i, j;

    #ifdef _mini_debug
      printf("input string:        "); 
      s->shownl();
    #endif

    ComputeTauPairs();

    #ifdef _mini_debug
      printf("Tau pairs:           "); 
      shownl();
      printf("preindex should be:  ");
      for(i=0; i<=last; i++)
        printf(" %ld",tp[i]);
      printf("\n");
    #endif

    radixsort();
  

    a1[tp[0]]=0;
    for(i=1; i<=last; i++) {
      if (eq(i,i-1))
        a1[tp[i]]=a1[tp[i-1]];
      else
        a1[tp[i]]=a1[tp[i-1]]+1;
    }
  
    #ifdef _mini_debug
      printf("Sorted Tau pairs:    "); 
      shownl();
      printf("Translated Tau pairs:");
      long d, d1;
      for(i=0; i<=last; i++) {
        if (tp[i]+1<len)
          d=dsize((*s)[tp[i]])+dsize((*s)[tp[i]+1])+4;
        else
          d=dsize((*s)[tp[i]])+5;
        d1=dsize(a1[tp[i]]);
        for(j=0; j<(d-d1)/2; j++) 
          printf(" ");
        printf("%ld",a1[tp[i]]);
        while(j+d1 < d) {
          printf(" ");
          j++;
        }
      }
      printf("\n");
    #endif

    taus = new Lstring(last+1);
    i=j=0;
    (*taus)[j]=a1[0];
    preindex[j++]=0;
    while(true) {
      // last taupair is (s[j],s[j+1])
      if (i+2 >= len) break;
      if ((*s)[i] > (*s)[i+1] && (*s)[i+1] <= (*s)[i+2]) { // we have a V
        (*taus)[j]=a1[i+1];
        i += 1;
        preindex[j++]=i;
      }else{
        (*taus)[j]=a1[i+2];
        i += 2;
        preindex[j++]=i;
      }
    }//endwhile

    #ifdef _mini_debug
      printf("tau string:          "); 
      taus->shownl();
      printf("preindex:            ");
      for(i=0; i<=last; i++)
        printf(" %ld",preindex[i]);
      printf("\n");
    #endif

    return taus;
  }

  void showpair(long i,FILE* fp=stdout) {
    if (tp[i]+1 < len)
      fprintf(fp,"(%ld,%ld)",(*s)[tp[i]],(*s)[tp[i]+1]);
    else
      fprintf(fp,"(%ld,$)",(*s)[tp[i]]);
  }

  void show(FILE *fp=stdout) {
    long i;
    showpair(0,fp);
    for(i=1; i <= last; i++) {
      if (tp[i-1]+1 == tp[i])
          fprintf(fp,"v");
      else
          fprintf(fp," ");
      showpair(i,fp);
    }
  }

  void shownl(FILE* fp=stdout) {
    show(fp);
    fprintf(fp,"\n");
  }

  // are the tau pairs represented by i and j equal
  bool eq(long i,long j) {
    if (tp[i]+1 < len) {        // (u,u)
      if (tp[j]+1 < len) {         // (v,v)
        return (*s)[tp[i]]==(*s)[tp[j]] && (*s)[tp[i]+1]==(*s)[tp[j]+1];
      }else                        // (v,$)
        return false;
    }else{                      // (u,$)
      if (tp[j]+1 < len) {         // (v,v)
        return false;
      }else{                       // (v,$)
        return (*s)[tp[i]]==(*s)[tp[j]];
      }
    }
  }

  void ComputeTauPairs() {
    long i;
    tp=array1;
    tp[0]=0;
    last=0;
    while(true) {
      // last taupair is (array[last],array[last]+1)
      i=tp[last]+2;
      if (i >= len) break;
      if ((*s)[tp[last]] > (*s)[tp[last]+1] && (*s)[tp[last]+1] <= (*s)[i]) { 
        // we have a V
        i=tp[last]+1;
        tp[++last]=i;
        continue;
      }else{
        tp[++last]=i;
      }
    }//endwhile
  }

  void radixsort() {
    long i, j, ns;
    bool dollar;
    long* a1=array2;
    long* a2=array3;

    for(i=0; i < len; i++) {
      a1[i]=0;
      a2[i]=nil;
    }

    #ifdef _test_sort    // makes tp1 copy for debug5()
    Tau tp1(*this);
    #endif

    // sorting by the last pair
    //compute the size of buckets and store it in a1 and set ns, dollar
    dollar=false;
    ns=0;
    for(i=0; i<=last; i++) {
      if (tp[i]+1 < len)
        a1[(*s)[tp[i]+1]]=a1[(*s)[tp[i]+1]]+1;
      else{
        ns=1;
        dollar=true;
      }
    }

    // compute the starts of buckets in a2
    for(i=0; i < len; i++) {
      if (a1[i]==0)
        a1[i]=nil;
      else{
        a2[i]=ns;
        ns=ns+a1[i];
        a1[i]=nil;
      }
    }

    #ifdef _debug
    debug1()
    #endif

    for(i=0; i<=last; i++) {
      j=tp[i]+1;
      if (j < len) {
        // put tp[i] in the bucket for (*s)[j]
        a1[a2[(*s)[j]]]=tp[i];
        a2[(*s)[j]]=a2[(*s)[j]]+1;
      }else{
        // put tp[i] in the bucket for $
        a1[0]=tp[i];
      }
    }

    #ifdef _debug
    debug2()
    #endif

    // now a1[] is tp[] after sorting it by the last letter
    // so we switch tp and a1
    tp=array2;
    a1=array1;
    a2=array3;

    for(i=0; i < len; i++) {
      a1[i]=0;
      a2[i]=nil;
    }
    //compute the size of buckets and store it in a1
    for(i=0; i<=last; i++)
      a1[(*s)[tp[i]]]++;

    // compute the starts of buckets and store it in a2
    for(ns=i=0; i < len; i++) {
      if (a1[i]==0)
        a1[i]=nil;
      else{
        a2[i]=ns;
        ns=ns+a1[i];
        a1[i]=nil;
      }
    }

    #ifdef _debug
    debug3()
    #endif

    // a1 serves as buckets
    for(i=0; i<=last; i++) {
      j=tp[i];
      // put tp[i] in the bucket for (*s)[j]
        a1[a2[(*s)[j]]]=tp[i];
        a2[(*s)[j]]=a2[(*s)[j]]+1;
    }

    // now a1 is tp sorted by the first letter
    tp=array1;
    a1=array2;
    a2=array3;

    #ifdef _debug
    debug4()
    #endif

    #ifdef _test_sort
    debug5()
    #endif

    return;
  }//end radixsort
};

#endif // end _Tau_hpp
