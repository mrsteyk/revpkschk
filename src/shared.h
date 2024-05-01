#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <shellapi.h>
#else
#define __debugbreak() __builtin_trap()
typedef unsigned long long LARGE_INTEGER;
#error TODO
#endif

// rdtsc
#include <intrin.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef uint8_t  u8;
typedef int16_t  i16;
typedef uint16_t u16;
typedef int32_t  i32;
typedef int32_t  b32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;

#define sizeof(N) sizeof(N)

#if defined(_MSC_VER) || (defined(__clang__) && defined(_WIN32))
# pragma section(".rdata$", read)
# define read_only __declspec(allocate(".rdata$"))
#elif (defined(__clang__) && (defined(__gnu_linux__) || defined(__linux__)))
# define read_only __attribute__((section(".rodata")))
#else
// NOTE(rjf): I don't know of a useful way to do this in GCC land.
// __attribute__((section(".rodata"))) looked promising, but it introduces a
// strange warning about malformed section attributes, and it doesn't look
// like writing to that section reliably produces access violations, strangely
// enough. (It does on Clang)
# define read_only
#endif

//~ mrsteyk: prng by Wangyi Fudan

#define RO32( X ) (((X) >> 32) | ((X) << 32))

static inline u64
wyrand(u64 *seed){
    *seed+=0xa0761d6478bd642full;
    u64 see1=*seed^0xe7037ed1a0b428dbull;
    see1*=RO32(see1);
    return(*seed*(RO32(*seed)))^((see1>>32)|(see1<<32));
}

//~ mrsteyk: constants

#define KB(n)  (((u64)(n)) << 10)
#define MB(n)  (((u64)(n)) << 20)
#define GB(n)  (((u64)(n)) << 30)
#define TB(n)  (((u64)(n)) << 40)
#define Thousand(n)   ((n)*1000)
#define Million(n)    ((n)*1000000)
#define Billion(n)    ((n)*1000000000)

static const u32 bitmask1  = 0x00000001;
static const u32 bitmask2  = 0x00000003;
static const u32 bitmask3  = 0x00000007;
static const u32 bitmask4  = 0x0000000f;
static const u32 bitmask5  = 0x0000001f;
static const u32 bitmask6  = 0x0000003f;
static const u32 bitmask7  = 0x0000007f;
static const u32 bitmask8  = 0x000000ff;
static const u32 bitmask9  = 0x000001ff;
static const u32 bitmask10 = 0x000003ff;
static const u32 bitmask11 = 0x000007ff;
static const u32 bitmask12 = 0x00000fff;
static const u32 bitmask13 = 0x00001fff;
static const u32 bitmask14 = 0x00003fff;
static const u32 bitmask15 = 0x00007fff;
static const u32 bitmask16 = 0x0000ffff;
static const u32 bitmask17 = 0x0001ffff;
static const u32 bitmask18 = 0x0003ffff;
static const u32 bitmask19 = 0x0007ffff;
static const u32 bitmask20 = 0x000fffff;
static const u32 bitmask21 = 0x001fffff;
static const u32 bitmask22 = 0x003fffff;
static const u32 bitmask23 = 0x007fffff;
static const u32 bitmask24 = 0x00ffffff;
static const u32 bitmask25 = 0x01ffffff;
static const u32 bitmask26 = 0x03ffffff;
static const u32 bitmask27 = 0x07ffffff;
static const u32 bitmask28 = 0x0fffffff;
static const u32 bitmask29 = 0x1fffffff;
static const u32 bitmask30 = 0x3fffffff;
static const u32 bitmask31 = 0x7fffffff;
static const u32 bitmask32 = 0xffffffff;

static const u64 bitmask33 = 0x00000001ffffffffull;
static const u64 bitmask34 = 0x00000003ffffffffull;
static const u64 bitmask35 = 0x00000007ffffffffull;
static const u64 bitmask36 = 0x0000000fffffffffull;
static const u64 bitmask37 = 0x0000001fffffffffull;
static const u64 bitmask38 = 0x0000003fffffffffull;
static const u64 bitmask39 = 0x0000007fffffffffull;
static const u64 bitmask40 = 0x000000ffffffffffull;
static const u64 bitmask41 = 0x000001ffffffffffull;
static const u64 bitmask42 = 0x000003ffffffffffull;
static const u64 bitmask43 = 0x000007ffffffffffull;
static const u64 bitmask44 = 0x00000fffffffffffull;
static const u64 bitmask45 = 0x00001fffffffffffull;
static const u64 bitmask46 = 0x00003fffffffffffull;
static const u64 bitmask47 = 0x00007fffffffffffull;
static const u64 bitmask48 = 0x0000ffffffffffffull;
static const u64 bitmask49 = 0x0001ffffffffffffull;
static const u64 bitmask50 = 0x0003ffffffffffffull;
static const u64 bitmask51 = 0x0007ffffffffffffull;
static const u64 bitmask52 = 0x000fffffffffffffull;
static const u64 bitmask53 = 0x001fffffffffffffull;
static const u64 bitmask54 = 0x003fffffffffffffull;
static const u64 bitmask55 = 0x007fffffffffffffull;
static const u64 bitmask56 = 0x00ffffffffffffffull;
static const u64 bitmask57 = 0x01ffffffffffffffull;
static const u64 bitmask58 = 0x03ffffffffffffffull;
static const u64 bitmask59 = 0x07ffffffffffffffull;
static const u64 bitmask60 = 0x0fffffffffffffffull;
static const u64 bitmask61 = 0x1fffffffffffffffull;
static const u64 bitmask62 = 0x3fffffffffffffffull;
static const u64 bitmask63 = 0x7fffffffffffffffull;
static const u64 bitmask64 = 0xffffffffffffffffull;

static const u32 bit1  = (1<<0);
static const u32 bit2  = (1<<1);
static const u32 bit3  = (1<<2);
static const u32 bit4  = (1<<3);
static const u32 bit5  = (1<<4);
static const u32 bit6  = (1<<5);
static const u32 bit7  = (1<<6);
static const u32 bit8  = (1<<7);
static const u32 bit9  = (1<<8);
static const u32 bit10 = (1<<9);
static const u32 bit11 = (1<<10);
static const u32 bit12 = (1<<11);
static const u32 bit13 = (1<<12);
static const u32 bit14 = (1<<13);
static const u32 bit15 = (1<<14);
static const u32 bit16 = (1<<15);
static const u32 bit17 = (1<<16);
static const u32 bit18 = (1<<17);
static const u32 bit19 = (1<<18);
static const u32 bit20 = (1<<19);
static const u32 bit21 = (1<<20);
static const u32 bit22 = (1<<21);
static const u32 bit23 = (1<<22);
static const u32 bit24 = (1<<23);
static const u32 bit25 = (1<<24);
static const u32 bit26 = (1<<25);
static const u32 bit27 = (1<<26);
static const u32 bit28 = (1<<27);
static const u32 bit29 = (1<<28);
static const u32 bit30 = (1<<29);
static const u32 bit31 = (1<<30);
static const u32 bit32 = (1<<31);

static const u64 bit33 = (1ull<<32);
static const u64 bit34 = (1ull<<33);
static const u64 bit35 = (1ull<<34);
static const u64 bit36 = (1ull<<35);
static const u64 bit37 = (1ull<<36);
static const u64 bit38 = (1ull<<37);
static const u64 bit39 = (1ull<<38);
static const u64 bit40 = (1ull<<39);
static const u64 bit41 = (1ull<<40);
static const u64 bit42 = (1ull<<41);
static const u64 bit43 = (1ull<<42);
static const u64 bit44 = (1ull<<43);
static const u64 bit45 = (1ull<<44);
static const u64 bit46 = (1ull<<45);
static const u64 bit47 = (1ull<<46);
static const u64 bit48 = (1ull<<47);
static const u64 bit49 = (1ull<<48);
static const u64 bit50 = (1ull<<49);
static const u64 bit51 = (1ull<<50);
static const u64 bit52 = (1ull<<51);
static const u64 bit53 = (1ull<<52);
static const u64 bit54 = (1ull<<53);
static const u64 bit55 = (1ull<<54);
static const u64 bit56 = (1ull<<55);
static const u64 bit57 = (1ull<<56);
static const u64 bit58 = (1ull<<57);
static const u64 bit59 = (1ull<<58);
static const u64 bit60 = (1ull<<59);
static const u64 bit61 = (1ull<<60);
static const u64 bit62 = (1ull<<61);
static const u64 bit63 = (1ull<<62);
static const u64 bit64 = (1ull<<63);


//~ mrsteyk: perf

// NOTE(mrsteyk): took this from my simpler_kyber repo
typedef struct {
    uint64_t tsc;
#if defined(_WIN32)
    LARGE_INTEGER pc;
#endif
} Perf;

typedef struct {
    Perf start;
    Perf end;
} Measurement;

static void
perf_measure(Perf* a) {
    a->tsc = __rdtsc();
#if defined(_WIN32)
    QueryPerformanceCounter(&a->pc);
#endif
}

static void
measurement_print(Measurement a, LARGE_INTEGER PC_FREQ) {
    uint64_t tsc = a.end.tsc - a.start.tsc;
#if defined(_WIN32)
    uint64_t pc = a.end.pc.QuadPart - a.start.pc.QuadPart;
    printf("%.04fms %llucy", pc*1000/(float)PC_FREQ.QuadPart, tsc);
#else
    printf("%llucy", tsc);
#endif
}

//~ mrsteyk: memory

#define AlignPow2(x,b)     (((x) + (b) - 1)&(~((b) - 1)))

#if defined(_WIN32)
//- from win32 layer Ryan "rjf" wrote
static void*
os_reserve(u64 size){
    void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return(result);
}

static b32
os_commit(void *ptr, u64 size){
    b32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
    return(result);
}

static void
os_decommit(void *ptr, u64 size){
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

static void
os_release(void *ptr, u64 size){
    // NOTE(rjf): size not used - not necessary on Windows, but necessary for other OSes.
    VirtualFree(ptr, 0, MEM_RELEASE);
}
#else
#error TODO
#endif

//~ mrsteyk: single arena

typedef struct {
    u64 pos;
    u64 commit;
    u64 reserve;
    u64 align;
} Arena;

#define ARENA_DEFAULT_RESERVE MB(16)
#define ARENA_DEFAULT_COMMIT  KB(64)
#define ARENA_DEFAULT_ALIGN      16

static Arena*
arena_create(u64 init_commit, u64 init_reserve, u64 align) {
    // NOTE(mrsteyk): no 2MB pages support
    const u64 page_size = 4096;
    
    u64 reserve = AlignPow2(init_reserve, page_size);
    u64 commit = AlignPow2(init_commit, page_size);
    
    void* ptr = os_reserve(reserve);
    os_commit(ptr, commit);
    
    Arena* arena = (Arena*)ptr;
    arena->pos = AlignPow2(sizeof(Arena), align);
    arena->commit = commit;
    arena->reserve = reserve;
    arena->align = align;
    
    return(arena);
}

#define ArenaCreateDefault() arena_create(ARENA_DEFAULT_COMMIT, ARENA_DEFAULT_RESERVE, ARENA_DEFAULT_ALIGN)
#define ArenaStartPos(arena) AlignPow2(sizeof(Arena), (arena)->align)

static void*
arena_push_size(Arena* arena, u64 size) {
    u64 pos = arena->align > 1 ? AlignPow2(arena->pos, arena->align) : arena->pos;
    u64 pos_new = pos + size;
    
    if (arena->commit < pos_new) {
        u64 commit_new = AlignPow2(pos_new, ARENA_DEFAULT_COMMIT);
        u64 commit_clamp = (commit_new > arena->reserve) ? arena->reserve : commit_new;
        u64 commit = commit_clamp - arena->commit;
        os_commit((u8*)arena + arena->commit, commit);
        arena->commit = commit_clamp;
    }
    
    void* ptr = 0;
    
    if (arena->commit >= pos_new) {
        ptr = (u8*)arena + pos;
        arena->pos = pos_new;
    }
    
    // TODO(mrsteyk): oops
    if (ptr == 0) __debugbreak();
    
    return(ptr);
}

static void
arena_pop_to(Arena* arena, u64 pos) {
    if (pos < sizeof(Arena))
        arena->pos = sizeof(Arena);
    else
        arena->pos = pos;
}

static void
arena_reset(Arena* arena) {
    arena->pos = AlignPow2(sizeof(Arena), arena->align);
}

static void
arena_put_back(Arena *arena, u64 amt) {
    u64 pos_old = arena->pos;
    u64 pos_new = pos_old;
    if (amt < pos_old) {
        pos_new = pos_old - amt;
    }
    arena_pop_to(arena, pos_new);
}

//~ mrsteyk: @strings

typedef struct {
    u8* ptr;
    u64 size;
} S8;

typedef struct {
    u16* ptr;
    u64 size;
} S16;

static S8
str8(u8* ptr, u64 size) {
    S8 ret;
    ret.ptr = ptr;
    ret.size = size;
    return(ret);
}

static S16
str16(u16* ptr, u64 size) {
    S16 ret;
    ret.ptr = ptr;
    ret.size = size;
    return(ret);
}

#define S8_lit( S ) str8((u8*)(S), sizeof(S) - 1)
#define S16_lit( S ) str16((u16*)(S), (sizeof(S) - 2)/2)

//- rjf: UTF Decoding Types

typedef struct UnicodeDecode UnicodeDecode;
struct UnicodeDecode
{
    u32 inc;
    u32 codepoint;
};

read_only static u8 utf8_class[32] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5,
};

static UnicodeDecode
utf8_decode(u8 *str, u64 max){
    UnicodeDecode result = {1, 0xFFFFFFFF};
    u8 byte = str[0];
    u8 byte_class = utf8_class[byte >> 3];
    switch (byte_class)
    {
        case 1:
        {
            result.codepoint = byte;
        }break;
        case 2:
        {
            if (2 < max)
            {
                u8 cont_byte = str[1];
                if (utf8_class[cont_byte >> 3] == 0)
                {
                    result.codepoint = (byte & bitmask5) << 6;
                    result.codepoint |=  (cont_byte & bitmask6);
                    result.inc = 2;
                }
            }
        }break;
        case 3:
        {
            if (2 < max)
            {
                u8 cont_byte[2] = {str[1], str[2]};
                if (utf8_class[cont_byte[0] >> 3] == 0 &&
                    utf8_class[cont_byte[1] >> 3] == 0)
                {
                    result.codepoint = (byte & bitmask4) << 12;
                    result.codepoint |= ((cont_byte[0] & bitmask6) << 6);
                    result.codepoint |=  (cont_byte[1] & bitmask6);
                    result.inc = 3;
                }
            }
        }break;
        case 4:
        {
            if (3 < max)
            {
                u8 cont_byte[3] = {str[1], str[2], str[3]};
                if (utf8_class[cont_byte[0] >> 3] == 0 &&
                    utf8_class[cont_byte[1] >> 3] == 0 &&
                    utf8_class[cont_byte[2] >> 3] == 0)
                {
                    result.codepoint = (byte & bitmask3) << 18;
                    result.codepoint |= ((cont_byte[0] & bitmask6) << 12);
                    result.codepoint |= ((cont_byte[1] & bitmask6) <<  6);
                    result.codepoint |=  (cont_byte[2] & bitmask6);
                    result.inc = 4;
                }
            }
        }
    }
    return(result);
}

static UnicodeDecode
utf16_decode(u16 *str, u64 max){
    UnicodeDecode result = {1, 0xFFFFFFFF};
    result.codepoint = str[0];
    result.inc = 1;
    if (max > 1 && 0xD800 <= str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] && str[1] < 0xE000){
        result.codepoint = ((str[0] - 0xD800) << 10) | (str[1] - 0xDC00) + 0x10000;
        result.inc = 2;
    }
    return(result);
}

static u32
utf8_encode(u8 *str, u32 codepoint){
    u32 inc = 0;
    if (codepoint <= 0x7F){
        str[0] = (u8)codepoint;
        inc = 1;
    }
    else if (codepoint <= 0x7FF){
        str[0] = (bitmask2 << 6) | ((codepoint >> 6) & bitmask5);
        str[1] = bit8 | (codepoint & bitmask6);
        inc = 2;
    }
    else if (codepoint <= 0xFFFF){
        str[0] = (bitmask3 << 5) | ((codepoint >> 12) & bitmask4);
        str[1] = bit8 | ((codepoint >> 6) & bitmask6);
        str[2] = bit8 | ( codepoint       & bitmask6);
        inc = 3;
    }
    else if (codepoint <= 0x10FFFF){
        str[0] = (bitmask4 << 4) | ((codepoint >> 18) & bitmask3);
        str[1] = bit8 | ((codepoint >> 12) & bitmask6);
        str[2] = bit8 | ((codepoint >>  6) & bitmask6);
        str[3] = bit8 | ( codepoint        & bitmask6);
        inc = 4;
    }
    else{
        str[0] = '?';
        inc = 1;
    }
    return(inc);
}

static u32
utf16_encode(u16 *str, u32 codepoint){
    u32 inc = 1;
    if (codepoint == 0xFFFFFFFF){
        str[0] = (u16)'?';
    }
    else if (codepoint < 0x10000){
        str[0] = (u16)codepoint;
    }
    else{
        u32 v = codepoint - 0x10000;
        //str[0] = safe_cast_u16(0xD800 + (v >> 10));
        str[0] = (u16)(0xD800 + (v >> 10));
        //str[1] = safe_cast_u16(0xDC00 + (v & bitmask10));
        str[1] = (u16)(0xDC00 + (v & bitmask10));
        inc = 2;
    }
    return(inc);
}

static S8
str8_from_16(Arena *arena, S16 in){
    u64 cap = in.size*3;
    //u8 *str = push_array_no_zero(arena, U8, cap + 1);
    u8 *str = (u8*)arena_push_size(arena, cap + 1);
    u16 *ptr = in.ptr;
    u16 *opl = ptr + in.size;
    u64 size = 0;
    UnicodeDecode consume;
    for (; ptr < opl; ptr += consume.inc){
        consume = utf16_decode(ptr, opl - ptr);
        size += utf8_encode(str + size, consume.codepoint);
    }
    str[size] = 0;
    arena_put_back(arena, (cap - size));
    
    S8 ret;
    ret.ptr = str;
    ret.size = size;
    return(ret);
}

static S16
str16_from_8(Arena *arena, S8 in){
    u64 cap = in.size*2;
    //u16 *str = push_array_no_zero(arena, u16, cap + 1);
    u16 *str = (u16*)arena_push_size(arena, sizeof(u16) * (cap + 1));
    u8 *ptr = in.ptr;
    u8 *opl = ptr + in.size;
    u64 size = 0;
    UnicodeDecode consume;
    for (; ptr < opl; ptr += consume.inc){
        consume = utf8_decode(ptr, opl - ptr);
        size += utf16_encode(str + size, consume.codepoint);
    }
    str[size] = 0;
    arena_put_back(arena, (cap - size)*2);
    
    S16 ret;
    ret.ptr = str;
    ret.size = size;
    return(ret);
}

//- mrsteyk: methods

static S16
S16_strrchr(S16 s, u16 c) {
    for (u64 i = s.size - 1; i >= 0 && i < s.size; i--) {
        if (s.ptr[i] == c) {
            S16 ret;
            ret.ptr = s.ptr + i;
            ret.size = s.size - i;
            return(ret);
        }
    }
    
    S16 ret = {0};
    return(ret);
}

static S8
S8_strrchr(S8 s, u8 c) {
    for (u64 i = s.size - 1; i >= 0 && i < s.size; i--) {
        if (s.ptr[i] == c) {
            S8 ret;
            ret.ptr = s.ptr + i;
            ret.size = s.size - i;
            return(ret);
        }
    }
    
    S8 ret = {0};
    return(ret);
}

static int
S8_cmp(S8 a, S8 b) {
    if (a.size != b.size) {
        // TODO(mrsteyk): ?
        return a.size - b.size;
    }
    return memcmp(a.ptr, b.ptr, a.size);
}

static S8
S8_from_c(const char* c) {
    u64 size = strlen(c);
    S8 ret;
    ret.ptr = (u8*)c;
    ret.size = size;
    return(ret);
}

static S16
S16_from_c(const wchar_t* c) {
    u64 size = wcslen (c);
    S16 ret;
    ret.ptr = (u16*)c;
    ret.size = size;
    return(ret);
}
