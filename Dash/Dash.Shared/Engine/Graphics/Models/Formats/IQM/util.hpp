#ifndef __UTIL_H__
#define __UTIL_H__

#ifndef NULL
#define NULL 0
#endif

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef signed long long int llong;
typedef unsigned long long int ullong;

template<class T> static inline T Radians(T x) { return (x * M_PI) / 180; }
template<class T> static inline T Degrees(T x) { return (x * 180) / M_PI; }

template<class T>
static inline T Clamp(T val, T minval, T maxval)
{
    return max(minval, min(val, maxval));
}

static inline bool IsLittlEendian() { union { int i; uchar b[sizeof(int)]; } conv; conv.i = 1; return conv.b[0] != 0; }

template<class T> inline T EndianSwap(T n) { union { T t; uint i; } conv; conv.t = n; conv.i = _byteswap_ulong(conv.i); return conv.t; }
template<> inline ushort EndianSwap<ushort>(ushort n) { return _byteswap_ushort(n); }
template<> inline short EndianSwap<short>(short n) { return _byteswap_ushort(n); }
template<> inline uint EndianSwap<uint>(uint n) { return _byteswap_ulong(n); }
template<> inline int EndianSwap<int>(int n) { return _byteswap_ulong(n); }
template<class T> inline void EndianSwap(T *buf, int len) { for (T *end = &buf[len]; buf < end; buf++) *buf = EndianSwap(*buf); }
template<class T> inline T LilSwap(T n) { return IsLittlEendian() ? n : EndianSwap(n); }
template<class T> inline void LilSwap(T *buf, int len) { if (!IsLittlEendian()) EndianSwap(buf, len); }
template<class T> inline T BigSwap(T n) { return IsLittlEendian() ? EndianSwap(n) : n; }
template<class T> inline void BigSwap(T *buf, int len) { if (IsLittlEendian()) EndianSwap(buf, len); }

template<class T> T GetVal(FILE *f) { T n; return fread(&n, 1, sizeof(n), f) == sizeof(n) ? n : 0; }
template<class T> T GetLil(FILE *f) { return lilswap(getval<T>(f)); }
template<class T> T GetBig(FILE *f) { return bigswap(getval<T>(f)); }

#endif

