#include "stdafx.h"

#if defined(_EMX) && !defined(_DJGPP)
#include "unios2.cpp"
#endif

bool WideToChar(const wchar *Src,char *Dest,size_t DestSize)
{
  bool RetCode=true;
#ifdef _WIN_32
  if (WideCharToMultiByte(CP_ACP,0,Src,-1,Dest,(int)DestSize,NULL,NULL)==0)
    RetCode=false;
#else
#ifdef _APPLE
  WideToUtf(Src,Dest,DestSize);
#else
#ifdef MBFUNCTIONS

  size_t ResultingSize=wcstombs(Dest,Src,DestSize);
  if (ResultingSize==(size_t)-1)
    RetCode=false;
  if (ResultingSize==0 && *Src!=0)
    RetCode=false;

  if ((!RetCode || *Dest==0 && *Src!=0) && DestSize>NM && strlenw(Src)<NM)
  {
    /* Workaround for strange Linux Unicode functions bug.
       Some of wcstombs and mbstowcs implementations in some situations
       (we are yet to find out what it depends on) can return an empty
       string and success code if buffer size value is too large.
    */
    return(WideToChar(Src,Dest,NM));
  }

#else
  if (UnicodeEnabled())
  {
#if defined(_EMX) && !defined(_DJGPP)
    int len=Min(strlenw(Src)+1,DestSize-1);
    if (uni_fromucs((UniChar*)Src,len,Dest,(size_t*)&DestSize)==-1 ||
        DestSize>len*2)
      RetCode=false;
    Dest[DestSize]=0;
#endif
  }
  else
    for (int I=0;I<DestSize;I++)
    {
      Dest[I]=(char)Src[I];
      if (Src[I]==0)
        break;
    }
#endif
#endif
#endif
  return(RetCode);
}


bool CharToWide(const char *Src,wchar *Dest,size_t DestSize)
{
  bool RetCode=true;
#ifdef _WIN_32
  if (MultiByteToWideChar(CP_ACP,0,Src,-1,Dest,(int)DestSize)==0)
    RetCode=false;
#else
#ifdef _APPLE
  UtfToWide(Src,Dest,DestSize);
#else
#ifdef MBFUNCTIONS

  size_t ResultingSize=mbstowcs(Dest,Src,DestSize);
  if (ResultingSize==(size_t)-1)
    RetCode=false;
  if (ResultingSize==0 && *Src!=0)
    RetCode=false;

  if ((!RetCode || *Dest==0 && *Src!=0) && DestSize>NM && strlen(Src)<NM)
  {
    /* Workaround for strange Linux Unicode functions bug.
       Some of wcstombs and mbstowcs implementations in some situations
       (we are yet to find out what it depends on) can return an empty
       string and success code if buffer size value is too large.
    */
    return(CharToWide(Src,Dest,NM));
  }
#else
  if (UnicodeEnabled())
  {
#if defined(_EMX) && !defined(_DJGPP)
    int len=Min(strlen(Src)+1,DestSize-1);
    if (uni_toucs((char*)Src,len,(UniChar*)Dest,(size_t*)&DestSize)==-1 ||
        DestSize>len)
      DestSize=0;
    RetCode=false;
#endif
  }
  else
    for (int I=0;I<DestSize;I++)
    {
      Dest[I]=(wchar_t)Src[I];
      if (Src[I]==0)
        break;
    }
#endif
#endif
#endif
  return(RetCode);
}


byte* WideToRaw(const wchar *Src,byte *Dest,size_t DestSize)
{
  for (size_t I=0;I<DestSize;I++,Src++)
  {
    Dest[I*2]=(byte)*Src;
    Dest[I*2+1]=(byte)(*Src>>8);
    if (*Src==0)
      break;
  }
  return(Dest);
}


wchar* RawToWide(const byte *Src,wchar *Dest,size_t DestSize)
{
  for (size_t I=0;I<DestSize;I++)
    if ((Dest[I]=Src[I*2]+(Src[I*2+1]<<8))==0)
      break;
  return(Dest);
}


void WideToUtf(const wchar *Src,char *Dest,size_t DestSize)
{
  long dsize=(long)DestSize;
  dsize--;
  while (*Src!=0 && --dsize>=0)
  {
    uint c=*(Src++);
    if (c<0x80)
      *(Dest++)=c;
    else
      if (c<0x800 && --dsize>=0)
      {
        *(Dest++)=(0xc0|(c>>6));
        *(Dest++)=(0x80|(c&0x3f));
      }
      else
        if (c<0x10000 && (dsize-=2)>=0)
        {
          *(Dest++)=(0xe0|(c>>12));
          *(Dest++)=(0x80|((c>>6)&0x3f));
          *(Dest++)=(0x80|(c&0x3f));
        }
        else
          if (c < 0x200000 && (dsize-=3)>=0)
          {
            *(Dest++)=(0xf0|(c>>18));
            *(Dest++)=(0x80|((c>>12)&0x3f));
            *(Dest++)=(0x80|((c>>6)&0x3f));
            *(Dest++)=(0x80|(c&0x3f));
          }
  }
  *Dest=0;
}


void UtfToWide(const char *Src,wchar *Dest,size_t DestSize)
{
  long dsize=(long)DestSize;
  dsize--;
  while (*Src!=0)
  {
    uint c=(byte)*(Src++),d;
    if (c<0x80)
      d=c;
    else
      if ((c>>5)==6)
      {
        if ((*Src&0xc0)!=0x80)
          break;
        d=((c&0x1f)<<6)|(*Src&0x3f);
        Src++;
      }
      else
        if ((c>>4)==14)
        {
          if ((Src[0]&0xc0)!=0x80 || (Src[1]&0xc0)!=0x80)
            break;
          d=((c&0xf)<<12)|((Src[0]&0x3f)<<6)|(Src[1]&0x3f);
          Src+=2;
        }
        else
          if ((c>>3)==30)
          {
            if ((Src[0]&0xc0)!=0x80 || (Src[1]&0xc0)!=0x80 || (Src[2]&0xc0)!=0x80)
              break;
            d=((c&7)<<18)|((Src[0]&0x3f)<<12)|((Src[1]&0x3f)<<6)|(Src[2]&0x3f);
            Src+=3;
          }
          else
            break;
    if (--dsize<0)
      break;
    if (d>0xffff)
    {
      if (--dsize<0 || d>0x10ffff)
        break;
      *(Dest++)=((d-0x10000)>>10)+0xd800;
      *(Dest++)=(d&0x3ff)+0xdc00;
    }
    else
      *(Dest++)=d;
  }
  *Dest=0;
}


bool UnicodeEnabled()
{
#ifdef UNICODE_SUPPORTED
  #ifdef _EMX
    return(uni_ready);
  #else
    return(true);
  #endif
#else
  return(false);
#endif
}


size_t strlenw(const wchar *str)
{
  size_t length=0;
  while (*(str++)!=0)
    length++;
  return(length);
}


wchar* strcpyw(wchar *dest,const wchar *src)
{
  do {
    *(dest++)=*src;
  } while (*(src++)!=0);
  return(dest);
}


wchar* strncpyw(wchar *dest,const wchar *src,size_t n)
{
  do {
    *(dest++)=*src;
  } while (*(src++)!=0 && (int)(--n) > 0);
  return(dest);
}


wchar* strcatw(wchar *dest,const wchar *src)
{
  return(strcpyw(dest+strlenw(dest),src));
}


#ifndef SFX_MODULE
wchar* strncatw(wchar *dest,const wchar *src,size_t n)
{
  dest+=strlenw(dest);
  while (true)
    if ((int)(--n)<0)
    {
      *dest=0;
      break;
    }
    else
      if ((*(dest++)=*(src++))==0)
        break;
  return(dest);
}
#endif


int strcmpw(const wchar *s1,const wchar *s2)
{
  while (*s1==*s2)
  {
    if (*s1==0)
      return(0);
    s1++;
    s2++;
  }
  return(*s1<*s2 ? -1:1);
}


int strncmpw(const wchar *s1,const wchar *s2,size_t n)
{
  while ((int)(n--)>0)
  {
    if (*s1<*s2)
      return(-1);
    if (*s1>*s2)
      return(-1);
    if (*s1==0)
      break;
    s1++;
    s2++;
  }
  return(0);
}


#ifndef SFX_MODULE
int stricmpw(const wchar *s1,const wchar *s2)
{
  char Ansi1[NM*sizeof(wchar)],Ansi2[NM*sizeof(wchar)];
  WideToChar(s1,Ansi1,sizeof(Ansi1));
  WideToChar(s2,Ansi2,sizeof(Ansi2));
  return(stricomp(Ansi1,Ansi2));
}
#endif


#if !defined(SFX_MODULE) && !defined(_WIN_CE)
inline int strnicmpw_w2c(const wchar *s1,const wchar *s2,size_t n)
{
  wchar Wide1[NM*2],Wide2[NM*2];
  strncpyw(Wide1,s1,sizeof(Wide1)/sizeof(Wide1[0])-1);
  strncpyw(Wide2,s2,sizeof(Wide2)/sizeof(Wide2[0])-1);
  Wide1[Min(sizeof(Wide1)/sizeof(Wide1[0])-1,n)]=0;
  Wide2[Min(sizeof(Wide2)/sizeof(Wide2[0])-1,n)]=0;
  char Ansi1[NM*2],Ansi2[NM*2];
  WideToChar(Wide1,Ansi1,sizeof(Ansi1));
  WideToChar(Wide2,Ansi2,sizeof(Ansi2));
  return(stricomp(Ansi1,Ansi2));
}
#endif


#ifndef SFX_MODULE
int strnicmpw(const wchar *s1,const wchar *s2,size_t n)
{
  return(strnicmpw_w2c(s1,s2,n));
}
#endif


wchar* strchrw(const wchar *s,int c)
{
  while (*s)
  {
    if (*s==c)
      return((wchar *)s);
    s++;
  }
  return(NULL);
}


wchar* strrchrw(const wchar *s,int c)
{
  for (int I=(int)(strlenw(s)-1);I>=0;I--)
    if (s[I]==c)
      return((wchar *)(s+I));
  return(NULL);
}


wchar* strpbrkw(const wchar *s1,const wchar *s2)
{
  while (*s1)
  {
    if (strchrw(s2,*s1)!=NULL)
      return((wchar *)s1);
    s1++;
  }
  return(NULL);
}


#ifndef SFX_MODULE
wchar* strlowerw(wchar *Str)
{
  for (wchar *ChPtr=Str;*ChPtr;ChPtr++)
    if (*ChPtr<128)
      *ChPtr=loctolower((byte)*ChPtr);
  return(Str);
}
#endif


#ifndef SFX_MODULE
wchar* strupperw(wchar *Str)
{
  for (wchar *ChPtr=Str;*ChPtr;ChPtr++)
    if (*ChPtr<128)
      *ChPtr=loctoupper((byte)*ChPtr);
  return(Str);
}
#endif


#ifndef SFX_MODULE
wchar* strdupw(const wchar *Str)
{
  if (Str==NULL)
    return(NULL);
  wchar *n=(wchar *)malloc((strlenw(Str)+1)*sizeof(wchar));
  if (n==NULL)
    return(NULL);
  strcpyw(n,Str);
  return(n);
}
#endif


int toupperw(int ch)
{
  return((ch<128) ? loctoupper(ch):ch);
}


int atoiw(const wchar *s)
{
  int n=0;
  while (*s>='0' && *s<='9')
  {
    n=n*10+(*s-'0');
    s++;
  }
  return(n);
}


#ifdef DBCS_SUPPORTED
SupportDBCS gdbcs;

SupportDBCS::SupportDBCS()
{
  Init();
}


void SupportDBCS::Init()
{
  CPINFO CPInfo;
  GetCPInfo(CP_ACP,&CPInfo);
  DBCSMode=CPInfo.MaxCharSize > 1;
  for (uint I=0;I<ASIZE(IsLeadByte);I++)
    IsLeadByte[I]=IsDBCSLeadByte(I)!=0;
}


char* SupportDBCS::charnext(const char *s)
{
  return (char *)(IsLeadByte[(byte)*s] ? s+2:s+1);
}


size_t SupportDBCS::strlend(const char *s)
{
  size_t Length=0;
  while (*s!=0)
  {
    if (IsLeadByte[(byte)*s])
      s+=2;
    else
      s++;
    Length++;
  }
  return(Length);
}


char* SupportDBCS::strchrd(const char *s, int c)
{
  while (*s!=0)
    if (IsLeadByte[(byte)*s])
      s+=2;
    else
      if (*s==c)
        return((char *)s);
      else
        s++;
  return(NULL);
}


void SupportDBCS::copychrd(char *dest,const char *src)
{
  dest[0]=src[0];
  if (IsLeadByte[(byte)src[0]])
    dest[1]=src[1];
}


char* SupportDBCS::strrchrd(const char *s, int c)
{
  const char *found=NULL;
  while (*s!=0)
    if (IsLeadByte[(byte)*s])
      s+=2;
    else
    {
      if (*s==c)
        found=s;
      s++;
    }
  return((char *)found);
}
#endif
