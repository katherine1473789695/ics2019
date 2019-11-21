#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  va_list args;
  char str[256];
  int length;
  va_start(args,fmt);
  length=vsprintf(str,fmt,args);
  va_end(args);
  for(int i=0;str[i]!='\0';i++){
    _putc(str[i]);
  }
  return length;
}



int vsprintf(char *out, const char *fmt, va_list ap) {
  int length;
  const char* a;
  char *str=out;
  for(;*fmt;++fmt){
    if(*fmt!='%'){
      *str++=*fmt;
      continue;
    }
    ++fmt;
    switch(*fmt){
      case 's':{
        a=va_arg(ap,char*);
        length=strlen(a);
        for(int i=0;i<length;i++){
          *str++=*a++;
        }
        continue;
      }
      case 'd':{
        int num=va_arg(ap,int);
        int i=0;
        char nums[20];
        if(num==0)nums[0]='0';
        else{
          if(num<0){
            *str++='-';
            num=(-num);
          }
          while(num!=0){
            nums[i++]=num%10+'0';
            num/=10;
          }
        }
        for(int j=i-1;j>=0;j--){
          *str++=nums[j];
        }
        break;
      }
      case 'c':{
        int num=va_arg(ap,int);
        *str++=(char)num;
        break;
      }
      case 'p':
      case 'x':{
        uint32_t num = va_arg(ap,uint32_t);
        int i=0;
        char nums[20];
        if(num==0)nums[0]='0';
        else{
          while(num!=0){
            uint32_t x=num%16;
            if(x<10)nums[i++]=x+'0';
            else nums[i++]=x-10+'a';
            num/=16;
          }
        }
        *str++='0';
        *str++='x';
        for(int j=i-1;j>=0;j--){
          *str++=nums[j];
        }
        break;
      }
    
      default:break;
    }
  }
  *str='\0';
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  int length;
  va_start(args,fmt);
  length=vsprintf(out,fmt,args);
  va_end(args);
  return length;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
