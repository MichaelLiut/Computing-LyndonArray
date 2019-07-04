#ifndef _debug_hpp
#define _debug_hpp

#ifdef _debug
#define debug1() { \
  long i; \
  if (dollar) printf("bucket for $ starts at 0\n"); \
  for(i=0; i < len; i++) { \
    if (a2[i]==nil) continue; \
    printf("bucket for %ld starts at %u\n",i,a2[i]); \
  } \
}


#define debug2() { \
  long i; \
  printf("tau pairs sorted by last letter\n"); \
  for(i=0; i<=last; i++) \
    if (a1[i]+1< s->getLen()) \
      printf("(%ld,%ld) ",(*s)[a1[i]],(*s)[a1[i]+1]); \
    else \
      printf("(%ld,$) ",(*s)[a1[i]]); \
  printf("\n"); \
}

#define debug3() { \
  long i; \
  for(i=0; i < s->getLen(); i++) { \
    if (a2[i]==nil) continue; \
    printf("bucket for %ld starts at %u\n",i,a2[i]); \
  } \
}

#define debug4() { \
  long i; \
  printf("tau pairs sorted by first letter\n"); \
  for(i=0; i<=last; i++) \
    if (tp[i]+1 < s->getLen()) \
      printf("(%ld,%ld) ",(*s)[tp[i]],(*s)[tp[i]+1]); \
    else \
      printf("(%ld,$) ",(*s)[tp[i]]); \
  printf("\n"); \
}
#endif

#ifdef _test_sort
bool lexsmleq(long i1,long i2,Lstring& s) {
  if (s[i1]<s[i2]) return true;
  if (s[i1]>s[i2]) return false;
  // hence s[i1]==s[i2]
  if (i1+1==s.getLen()) return true;
  if (i2+1==s.getLen()) return false;
  if (s[i1+1]<=s[i2+1]) return true;
  return false;
}

#define debug5() { \
  long i, j, x; \
  for(i=last; i>0; i--) { \
    for(j=0; j<i; j++) { \
      if (! lexsmleq(tp1[j],tp1[j+1],(*s))) { \
       x=tp1[j]; \
       tp1[j]=tp1[j+1]; \
       tp1[j+1]=x; \
       continue; \
     } \
    } \
  } \
  for(i=0; i<=last; i++) { \
    if (tp1[i] != tp[i]) { \
      printf("sort problem\n"); \
      printf("radix sort:\n"); \
      s->shownl(); \
      printf("bubble sort:\n"); \
      tp1.shownl(); \
      getchar(); \
      exit(1); \
    } \
  } \
  printf("radix sort OK\n"); \
}
#endif

#endif  // end _debug_hpp
