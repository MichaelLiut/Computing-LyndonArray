#ifndef _Lstring_hpp
#define _Lstring_hpp

class Lstring {
private:
  long* body;
  long len;
private: // local utility methods
  long myatoi(const char* s, long start, long term) {
    long res=0;
    long i;
    for(i=start; i<term; i++) 
      res = 10*res+(s[i]-'0');
    return res;
  }
  void parse_string(const char* s) {
    // if the string starts with . the string is in dot-notation, 
    //e.g.     ".12.345.1.56.7"
    // otherwise it is a normal notation "123451567"

    long i, slen, lastdot, j;

    // case dot-notation
    if (s[0]=='.') {
      lastdot=0;
      len=0;
      slen=1;
      for(i=1; s[i]!='\0'; i++) {
        slen++;
        if ('0'<=s[i] && s[i]<='9') continue;
        if (s[i]=='.') {
          if (lastdot+1==i) {
            printf("input string double dot error\n");
            exit(1);
          }
          len++;
          if (len > MAX) {
            printf("input string too long\n");
            exit(1);
          }
          lastdot=i;
          continue;
        }
      }
      if (lastdot<slen-1) len++;
      if (len > MAX) {
        printf("input string too long\n");
        exit(1);
      }
      body = new long[len];
      lastdot=j=0;
      for(i=1; i<=slen; i++) {
        if (i==slen) {
          if (lastdot+1==i) break;              // ".123.345."
          body[j]=myatoi(s,lastdot+1,i);        // ".123.345.23"
          if (body[j]>=len) {
            printf("input string not over integer alphabet\n");
            exit(1);
          }
          j++;
          break;
        }
        if (s[i]=='.') {
          body[j]=myatoi(s,lastdot+1,i);
          if (body[j]>=len) {
            printf("input string not over integer alphabet\n");
            exit(1);
          }
          if (j==slen-1) break;
          j++;
            lastdot=i;
        }
      }
      return;
    }

    // case normal notation
    for(len=0; len <= MAX && s[len]!='\0'; len++) {
      if (s[len]<'0' || s[len]>'9') {
        printf("input string: incorrect alphabet\n");
        exit(1);
      }
    }//endfor
    if (len == MAX) {
      if (s[len]!='\0') {
        printf("input string too long\n");
        exit(1);
      }
    }
    for(i=0; i<len; i++) {
      if (long(s[i]-'0')>= len) {
        printf("input string not over integer alphabet\n");
        exit(1);
      }
    }
    body=new long[len];
    for(i=0; s[i]!=0; i++)
      body[i]=long(s[i]-'0');
  }

public:
  Lstring(char* s) {
    parse_string(s);
  }

  Lstring(long len1) {
    long i;
    if (len1 >= MAX) {
      printf("requested length of Lstring is too big\n");
      exit(1);
    }
    len=len1;
    body=new long[len];
    for(i=0; i<len; i++)
      body[i]=0;
  }
  Lstring(Lstring& s1) {  // copy constructor
    long i;
    len=s1.len;
    body=new long[len];
    for(i=0; i<len; i++)
      body[i]=s1.body[i];
  }
  ~Lstring() { if (body) delete[] body; }
  Lstring& operator= (Lstring& s1) {   // assignment
    long i;
    if (body) delete[] body;
    len=s1.len;
    body = new long[len];
    for(i=0; i<len; i++)
      body[i]=s1.body[i];
    return *this;
  }
  Lstring& operator= (const char* s) {   // assignment
    if (body) delete[] body;
    body=0;
    len=0;
    parse_string(s);
    return *this;
  }
  long& operator[] (long i) {  // indexing
    if (i >= len) {
      printf("index out of range\n");
      exit(1);
    }
    return body[i];
  }
  bool operator== (Lstring& s1) {
    long i;
    if (len != s1.len) return false;
    for(i=0; i<len; i++)
      if (body[i] != s1.body[i]) return false;
    return true;
  }
  bool operator!= (Lstring& s1) {
    long i;
    if (len != s1.len) return true;
    for(i=0; i<len; i++)
      if (body[i] != s1.body[i]) return true;
    return false;
  }
  bool operator< (Lstring& s1) {
    long i, len1;
    len1=min(len,s1.len);
    for(i=0; i<len1; i++) {
      if (body[i]<s1.body[i])
        return true;
      else if (body[i]>s1.body[i])
        return false;
      else
        continue;
    }
    // so this is a prefix of s1 or s1 is a prefix of this
    if (len < len1) // this is a proper prefix of s1
      return true;
    else if (len > len1) // s1 is a proper prefix of this
      return false;
    else                // this=s1
      return false;
  }
  bool operator<= (Lstring& s1) {
    long i, len1;
    len1=min(len,s1.len);
    for(i=0; i<len1; i++) {
      if (body[i]<s1.body[i])
        return true;
      else if (body[i]>s1.body[i])
        return false;
      else
        continue;
    }
    // so this is a prefix of s1 or s1 is a prefix of this
    if (len < len1) // this is a proper prefix of s1
      return true;
    else if (len > len1) // s1 is a proper prefix of this
      return false;
    else                // this=s1
      return true;
  }
  bool operator> (Lstring& s1) {
    long i, len1;
    len1=min(len,s1.len);
    for(i=0; i<len1; i++) {
      if (body[i]<s1.body[i])
        return false;
      else if (body[i]>s1.body[i])
        return true;
      else
        continue;
    }
    // so this is a prefix of s1 or s1 is a prefix of this
    if (len < len1) // this is a proper prefix of s1
      return false;
    else if (len > len1) // s1 is a proper prefix of this
      return true;
    else                // this=s1
      return false;
  }
  bool operator>= (Lstring& s1) {
    long i, len1;
    len1=min(len,s1.len);
    for(i=0; i<len1; i++) {
      if (body[i]<s1.body[i])
        return false;
      else if (body[i]>s1.body[i])
        return true;
      else
        continue;
    }
    // so this is a prefix of s1 or s1 is a prefix of this
    if (len < len1) // this is a proper prefix of s1
      return false;
    else if (len > len1) // s1 is a proper prefix of this
      return true;
    else                // this=s1
      return true;
  }
  Lstring& operator+ (Lstring& s1) {  // concatenation
    long i, j;
    i = MAX-len;
    if (i<s1.len) {
      printf("concatenation: resulting string length overflow\n");
      exit(1);
    }
    Lstring* x=new Lstring(len+s1.len);
    for(i=0; i<len; i++)
      x->body[i]=body[i];
    for(j=0; j<s1.len; j++)
      x->body[i+j]=s1.body[j];
    return *x;
  }
  bool ispref(Lstring& s1) { // this is a prefix of s1
    long i;
    if (s1.len < len) return false;
    for(i=0; i<len; i++)
      if (body[i] != s1.body[i]) return false;
    return true;
  }
  bool isppref(Lstring& s1) { // this is a proper prefix of s1
    long i;
    if (s1.len <= len) return false;
    for(i=0; i<len; i++)
      if (body[i] != s1.body[i]) return false;
    return true;
  }
  bool issuff(Lstring& s1) { // this is a suffix of s1
    long i;
    if (s1.len < len) return false;
    for(i=len-1; i>=0; i--)
      if (body[i] != s1.body[i]) return false;
    return true;
  }
  bool ispsuff(Lstring& s1) { // this is a proper suffix of s1
    long i;
    if (s1.len <= len) return false;
    for(i=len-1; i>=0; i++)
      if (body[i] != s1.body[i]) return false;
    return true;
  }
  void show(FILE* fp=stdout) {
    long i;
    for(i=0; i<len; i++)
      fprintf(fp,".%ld",body[i]);
  }
  void show_part(long size,FILE* fp=stdout) {
    long i;
    for(i=0; i<size && i<len; i++)
      fprintf(fp,".%ld",body[i]);
    if (len > size) fprintf(fp," ...");
  }
  void shownl(FILE* fp=stdout) {
    show(fp);
    fprintf(fp,"\n");
  }
  void shownl_part(long size,FILE* fp=stdout) {
    show_part(size,fp);
    fprintf(fp,"\n");
  }
  long getLen() { return len; }
  // generate random Lstring of length len
  static Lstring& genrand(long len) {
    Lstring* r;
    r = new Lstring(len);
    srand((unsigned int)time(NULL));
    for(long i=0; i<len; i++)
      (*r)[i]=long(rand() % len);
    return *r;
  }
};

#endif // end _Lstring_hpp
