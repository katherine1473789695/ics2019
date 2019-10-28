#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t i=0;
  while(s[i]!='\0'){
    i++; 
  }
  return i;
}

char *strcpy(char* dst,const char* src) {
  int i=0;
  while(1){
    dst[i]=src[i];
    if(src[i]=='\0')break;
    i++;
  }
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  size_t i;
  for(i=0;i<n&&src[i]!='\0';i++){
    dst[i]=src[i];
  }
  for(;i<n;i++){
    dst[i]='\0';
  }
  return dst;
}

char* strcat(char* dst, const char* src) {
  size_t i;
  size_t len=strlen(dst);
  for(i=0;src[i]!='\0';i++){
    dst[len+i]=src[i];
  }
  dst[len+i]='\0';
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  size_t i;
  for(i=0;s1[1]!='\0'&&s2[i]!='\0';i++){
    if(s1[i]<s2[i])return -1;
    else if(s1[i]>s2[i])return 1;
  }
  if(s1[i]<s2[i])return -1;
  else if(s1[i]>s2[i])return 1;
  return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  return 0;
  size_t i;
  for(i=0;i<n && s1[i]!='\0' && s2[i]!='\0';i++){
    if(s1[i]<s2[i])return -1;
    else if(s1[i]>s2[i])return 1;
  }
  if(i<n){
    if(s1[i]<s2[i])return -1;
    else if(s1[i]<s2[i])return 1;
  }
  return 0;
}

void* memset(void* v,int c,size_t n) {
  char *dst = (char *)v;
  for(;n>0;n--){
    *dst++=c;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  char *dst = (char *)out;
  char *src = (char *)in;
  for(;n>0;n--){
    *dst++=*src++;
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  char *t1=(char *)s1;
  char *t2=(char *)s2;
  size_t i;
  for(i=0;i<n;i++){
    if((*t1)<(*t2))return -1;
    else if((*t1)>(*t2))return 1;
    t1++;
    t2++;
  }
  if(i<n){
    if((*t1)<(*t2))return -1;
    else if((*t1)>(*t2))return 1;
  }
  return 0;
}

#endif
