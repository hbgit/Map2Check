#include <assert.h>                     /** by FORTES **/
//#include <stdint.h>                     /** by FORTES **/
//#include <stdio.h>                      /** by FORTES **/
//#include <stdlib.h>                     /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/


typedef int intptr_t;
struct _IO_FILE;
typedef struct _IO_FILE FILE;

extern struct _IO_FILE *stderr;
extern int fprintf (FILE *__restrict __stream,
      const char *__restrict __format, ...);
extern int printf (const char *__restrict __format, ...);


extern void __VERIFIER_error(int linenumber);

typedef long unsigned int   size_t;

typedef int                 wchar_t;

union wait
{
   int w_status;
   struct
   {
      unsigned int __w_termsig  : 7;
      unsigned int __w_coredump : 1;
      unsigned int __w_retcode  : 8;
      unsigned int              : 16;
   }
       __wait_terminated;
   struct
   {
      unsigned int __w_stopval : 8;
      unsigned int __w_stopsig : 8;
      unsigned int             : 16;
   }
       __wait_stopped;
};

typedef union
{
   union wait *__uptr;
   int        *__iptr;
};

typedef struct
{
   int quot;
   int rem;
} div_t;

typedef struct
{
   long int quot;
   long int rem;
} ldiv_t;

typedef struct
{
   long long int quot;
   long long int rem;
} lldiv_t;

extern size_t __ctype_get_mb_cur_max(void);

extern double atof(const char *__nptr);

extern int atoi(const char *__nptr);

extern long int atol(const char *__nptr);

extern long long int atoll(const char *__nptr);

extern double strtod(const char *__nptr, char **__endptr);

extern float strtof(const char *__nptr, char **__endptr);

extern long double strtold(const char *__nptr, char **__endptr);

extern long int strtol(const char *__nptr, char **__endptr, int __base);

extern unsigned long int strtoul(const char *__nptr, char **__endptr, int __base);

extern long long int strtoq(const char *__nptr, char **__endptr, int __base);

extern unsigned long long int strtouq(const char *__nptr, char **__endptr, int __base);

extern long long int strtoll(const char *__nptr, char **__endptr, int __base);

extern unsigned long long int strtoull(const char *__nptr, char **__endptr, int __base);

extern char *l64a(long int __n);

extern long int a64l(const char *__s);

typedef unsigned char        __u_char;

typedef unsigned short int   __u_short;

typedef unsigned int         __u_int;

typedef unsigned long int    __u_long;

typedef signed char          __int8_t;

typedef unsigned char        __uint8_t;

typedef signed short int     __int16_t;

typedef unsigned short int   __uint16_t;

typedef signed int           __int32_t;

typedef unsigned int         __uint32_t;

typedef signed long int      __int64_t;

typedef unsigned long int    __uint64_t;

typedef long int             __quad_t;

typedef unsigned long int    __u_quad_t;

typedef unsigned long int    __dev_t;

typedef unsigned int         __uid_t;

typedef unsigned int         __gid_t;

typedef unsigned long int    __ino_t;

typedef unsigned long int    __ino64_t;

typedef unsigned int         __mode_t;

typedef unsigned long int    __nlink_t;

typedef long int             __off_t;

typedef long int             __off64_t;

typedef int                  __pid_t;

typedef struct
{
   int __val[2];
} __fsid_t;

typedef long int             __clock_t;

typedef unsigned long int    __rlim_t;

typedef unsigned long int    __rlim64_t;

typedef unsigned int         __id_t;

typedef long int             __time_t;

typedef unsigned int         __useconds_t;

typedef long int             __suseconds_t;

typedef int                  __daddr_t;

typedef long int             __swblk_t;

typedef int                  __key_t;

typedef int                  __clockid_t;

typedef void *               __timer_t;

typedef long int             __blksize_t;

typedef long int             __blkcnt_t;

typedef long int             __blkcnt64_t;

typedef unsigned long int    __fsblkcnt_t;

typedef unsigned long int    __fsblkcnt64_t;

typedef unsigned long int    __fsfilcnt_t;

typedef unsigned long int    __fsfilcnt64_t;

typedef long int             __ssize_t;

typedef __off64_t            __loff_t;

typedef __quad_t *           __qaddr_t;

typedef char *               __caddr_t;

typedef long int             __intptr_t;

typedef unsigned int         __socklen_t;

typedef __u_char             u_char;

typedef __u_short            u_short;

typedef __u_int              u_int;

typedef __u_long             u_long;

typedef __quad_t             quad_t;

typedef __u_quad_t           u_quad_t;

typedef __fsid_t             fsid_t;

typedef __loff_t             loff_t;

typedef __ino_t              ino_t;

typedef __dev_t              dev_t;

typedef __gid_t              gid_t;

typedef __mode_t             mode_t;

typedef __nlink_t            nlink_t;

typedef __uid_t              uid_t;

typedef __off_t              off_t;

typedef __pid_t              pid_t;

typedef __id_t               id_t;

typedef __ssize_t            ssize_t;

typedef __daddr_t            daddr_t;

typedef __caddr_t            caddr_t;

typedef __key_t              key_t;

typedef __clock_t            clock_t;

typedef __time_t             time_t;

typedef __clockid_t          clockid_t;

typedef __timer_t            timer_t;

typedef unsigned long int    ulong;

typedef unsigned short int   ushort;

typedef unsigned int         uint;

typedef int                  int8_t;

typedef int                  int16_t;

typedef int                  int32_t;

typedef int                  int64_t;

typedef unsigned int         u_int8_t;

typedef unsigned int         u_int16_t;

typedef unsigned int         u_int32_t;

typedef unsigned int         u_int64_t;

typedef int                  register_t;

typedef int                  __sig_atomic_t;


typedef struct
{
   unsigned long int __val[1024 / (8 * (sizeof(unsigned long int)))];
} __sigset_t;

typedef __sigset_t   sigset_t;

struct timespec
{
   __time_t tv_sec;
   long int tv_nsec;
};

struct timeval
{
   __time_t      tv_sec;
   __suseconds_t tv_usec;
};

typedef __suseconds_t   suseconds_t;

typedef long int        __fd_mask;

typedef struct
{
   __fd_mask __fds_bits[1024 / (8 * ((int)(sizeof(__fd_mask))))];
} fd_set;

typedef __fd_mask   fd_mask;

extern int select(int __nfds, fd_set *__readfds, fd_set *__writefds, fd_set *__exceptfds, struct timeval *__timeout);

extern int pselect(int __nfds, fd_set *__readfds, fd_set *__writefds, fd_set *__exceptfds, const struct timespec *__timeout, const __sigset_t *__sigmask);

extern unsigned int gnu_dev_major(unsigned long long int __dev);

extern unsigned int gnu_dev_minor(unsigned long long int __dev);

extern unsigned long long int gnu_dev_makedev(unsigned int __major, unsigned int __minor);

typedef __blksize_t         blksize_t;

typedef __blkcnt_t          blkcnt_t;

typedef __fsblkcnt_t        fsblkcnt_t;

typedef __fsfilcnt_t        fsfilcnt_t;

typedef unsigned long int   pthread_t;

typedef union
{
   char     __size[56];
   long int __align;
} pthread_attr_t;

typedef struct __pthread_internal_list
{
   struct __pthread_internal_list *__prev;
   struct __pthread_internal_list *__next;
} __pthread_list_t;

typedef union
{
   struct __pthread_mutex_s
   {
      int              __lock;
      unsigned int     __count;
      int              __owner;
      unsigned int     __nusers;
      int              __kind;
      int              __spins;
      __pthread_list_t __list;
   }        __data;
   char     __size[40];
   long int __align;
} pthread_mutex_t;

typedef union
{
   char __size[4];
   int  __align;
} pthread_mutexattr_t;

typedef union
{
   struct
   {
      int                    __lock;
      unsigned int           __futex;
      unsigned long long int __total_seq;
      unsigned long long int __wakeup_seq;
      unsigned long long int __woken_seq;
      void                   *__mutex;
      unsigned int           __nwaiters;
      unsigned int           __broadcast_seq;
   }             __data;
   char          __size[48];
   long long int __align;
} pthread_cond_t;

typedef union
{
   char __size[4];
   int  __align;
} pthread_condattr_t;

typedef unsigned int   pthread_key_t;

typedef int            pthread_once_t;

typedef union
{
   struct
   {
      int               __lock;
      unsigned int      __nr_readers;
      unsigned int      __readers_wakeup;
      unsigned int      __writer_wakeup;
      unsigned int      __nr_readers_queued;
      unsigned int      __nr_writers_queued;
      int               __writer;
      int               __shared;
      unsigned long int __pad1;
      unsigned long int __pad2;
      unsigned int      __flags;
   }        __data;
   char     __size[56];
   long int __align;
} pthread_rwlock_t;

typedef union
{
   char     __size[8];
   long int __align;
} pthread_rwlockattr_t;

typedef volatile int   pthread_spinlock_t;

typedef union
{
   char     __size[32];
   long int __align;
} pthread_barrier_t;

typedef union
{
   char __size[4];
   int  __align;
} pthread_barrierattr_t;

extern long int random(void);

extern void srandom(unsigned int __seed);

extern char *initstate(unsigned int __seed, char *__statebuf, size_t __statelen);

extern char *setstate(char *__statebuf);

struct random_data
{
   int32_t *fptr;
   int32_t *rptr;
   int32_t *state;
   int     rand_type;
   int     rand_deg;
   int     rand_sep;
   int32_t *end_ptr;
};

extern int random_r(struct random_data *__buf, int32_t *__result);

extern int srandom_r(unsigned int __seed, struct random_data *__buf);

extern int initstate_r(unsigned int __seed, char *__statebuf, size_t __statelen, struct random_data *__buf);

extern int setstate_r(char *__statebuf, struct random_data *__buf);

extern int rand(void);

extern void srand(unsigned int __seed);

extern int rand_r(unsigned int *__seed);

extern double drand48(void);

extern double erand48(unsigned short int __xsubi[3]);

extern long int lrand48(void);

extern long int nrand48(unsigned short int __xsubi[3]);

extern long int mrand48(void);

extern long int jrand48(unsigned short int __xsubi[3]);

extern void srand48(long int __seedval);

extern unsigned short int *seed48(unsigned short int __seed16v[3]);

extern void lcong48(unsigned short int __param[7]);

struct drand48_data
{
   unsigned short int     __x[3];
   unsigned short int     __old_x[3];
   unsigned short int     __c;
   unsigned short int     __init;
   unsigned long long int __a;
};

extern int drand48_r(struct drand48_data *__buffer, double *__result);

extern int erand48_r(unsigned short int __xsubi[3], struct drand48_data *__buffer, double *__result);

extern int lrand48_r(struct drand48_data *__buffer, long int *__result);

extern int nrand48_r(unsigned short int __xsubi[3], struct drand48_data *__buffer, long int *__result);

extern int mrand48_r(struct drand48_data *__buffer, long int *__result);

extern int jrand48_r(unsigned short int __xsubi[3], struct drand48_data *__buffer, long int *__result);

extern int srand48_r(long int __seedval, struct drand48_data *__buffer);

extern int seed48_r(unsigned short int __seed16v[3], struct drand48_data *__buffer);

extern int lcong48_r(unsigned short int __param[7], struct drand48_data *__buffer);

extern void *malloc(size_t __size);

extern void *calloc(size_t __nmemb, size_t __size);

extern void *realloc(void *__ptr, size_t __size);

extern void free(void *__ptr);

extern void cfree(void *__ptr);

extern void *alloca(size_t __size);

extern void *valloc(size_t __size);

extern int posix_memalign(void **__memptr, size_t __alignment, size_t __size);

extern void abort(void);

extern int atexit(void (*__func)(void));

extern int on_exit(void (*__func)(int __status, void *__arg), void *__arg);

extern void exit(int __status);

extern void _Exit(int __status);

extern char *getenv(const char *__name);

extern char *__secure_getenv(const char *__name);

extern int putenv(char *__string);

extern int setenv(const char *__name, const char *__value, int __replace);

extern int unsetenv(const char *__name);

extern int clearenv(void);

extern char *mktemp(char *__template);

extern int mkstemp(char *__template);

extern int mkstemps(char *__template, int __suffixlen);

extern char *mkdtemp(char *__template);

extern int system(const char *__command);

extern char *realpath(const char *__name, char *__resolved);

typedef int (*__compar_fn_t)(const void *, const void *);

extern void *bsearch(const void *__key, const void *__base, size_t __nmemb, size_t __size, __compar_fn_t __compar);

extern void qsort(void *__base, size_t __nmemb, size_t __size, __compar_fn_t __compar);

extern int abs(int __x);

extern long int labs(long int __x);

extern long long int llabs(long long int __x);

extern div_t div(int __numer, int __denom);

extern ldiv_t ldiv(long int __numer, long int __denom);

extern lldiv_t lldiv(long long int __numer, long long int __denom);

extern char *ecvt(double __value, int __ndigit, int *__decpt, int *__sign);

extern char *fcvt(double __value, int __ndigit, int *__decpt, int *__sign);

extern char *gcvt(double __value, int __ndigit, char *__buf);

extern char *qecvt(long double __value, int __ndigit, int *__decpt, int *__sign);

extern char *qfcvt(long double __value, int __ndigit, int *__decpt, int *__sign);

extern char *qgcvt(long double __value, int __ndigit, char *__buf);

extern int ecvt_r(double __value, int __ndigit, int *__decpt, int *__sign, char *__buf, size_t __len);

extern int fcvt_r(double __value, int __ndigit, int *__decpt, int *__sign, char *__buf, size_t __len);

extern int qecvt_r(long double __value, int __ndigit, int *__decpt, int *__sign, char *__buf, size_t __len);

extern int qfcvt_r(long double __value, int __ndigit, int *__decpt, int *__sign, char *__buf, size_t __len);

extern int mblen(const char *__s, size_t __n);

extern int mbtowc(wchar_t *__pwc, const char *__s, size_t __n);

extern int wctomb(char *__s, wchar_t __wchar);

extern size_t mbstowcs(wchar_t *__pwcs, const char *__s, size_t __n);

extern size_t wcstombs(char *__s, const wchar_t *__pwcs, size_t __n);

extern int rpmatch(const char *__response);

extern int getsubopt(char **__optionp, char **const __tokens, char **__valuep);

extern int getloadavg(double __loadavg[], int __nelem);



//typedef int intptr_t;
//struct _IO_FILE;
//typedef struct _IO_FILE FILE;

//extern struct _IO_FILE *stderr;
//extern int fprintf (FILE *__restrict __stream,
      //const char *__restrict __format, ...);
//extern int printf (const char *__restrict __format, ...);




int *a;

int *b;

int n;

void foo()
{
   int i;

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(i), (void *)(intptr_t)(NULL), "i", 1, 0, 0, 0, "int", 623); /** by FORTES **/

   for (i = 0; i < n; i++)
   {
// FORTES: Claim 1
      __MAP_FORTES_assert(!((void *)(intptr_t)(a + i) < GET_LAST_ADDR_FROM_MALLOC(list_LOG_mcf, (void *)&(a))) || !((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&a, (void *)(intptr_t)a))), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 625 \n \t Comments:   dereference failure: dynamic object lower bound");
// FORTES: Claim 2
      __MAP_FORTES_assert(!((intptr_t)(a) + i >= ((intptr_t)(a) + (n * sizeof(*a)))) || !((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&a, (void *)(intptr_t)a))), "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 625 \n \t Comments:   dereference failure: dynamic object upper bound");
      a[i]         = -1;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a[i]), (void *)(intptr_t)(a[i]), "a[i]", 1, 0, 0, 0, "int", 625); /** by FORTES **/
   }
   for (i = 0; i < (128 - 1); i++)
   {
// FORTES: Claim 3
      __MAP_FORTES_assert(!((void *)(intptr_t)(b + i) < GET_LAST_ADDR_FROM_MALLOC(list_LOG_mcf, (void *)&(b))) || !((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&b, (void *)(intptr_t)b))), "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 627 \n \t Comments:   dereference failure: dynamic object lower bound");
// FORTES: Claim 4
      __MAP_FORTES_assert(!((intptr_t)(b) + i >= ((intptr_t)(b) + (n * sizeof(*b)))) || !((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&b, (void *)(intptr_t)b))), "[Failed]\n VIOLATED PROPERTY: Claim 4 \n \t Location at original code in line: 627 \n \t Comments:   dereference failure: dynamic object upper bound");
      b[i]         = -1;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(b[i]), (void *)(intptr_t)(b[i]), "b[i]", 1, 0, 0, 0, "int", 627); /** by FORTES **/
   }
}


int main()
{
   MAP_GB_VAR_FORTES(); /** by FORTES **/

   n            = 128;
   a            = malloc(n * (sizeof(*a)));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a), (void *)(intptr_t)(a), "a", 2, 1, 0, 0, "int", 632); /** by FORTES **/

   b            = malloc(n * (sizeof(*b)));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(b), (void *)(intptr_t)(b), "b", 2, 1, 0, 0, "int", 633); /** by FORTES **/

   *(b++)       = 0;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(b), (void *)(intptr_t)(NULL), "b", 2, 0, 0, 0, "int", 634); /** by FORTES **/

   foo();
// FORTES: Claim 5
   __MAP_FORTES_assert(!((void *)(intptr_t)(b - 1) < GET_LAST_ADDR_FROM_MALLOC(list_LOG_mcf, (void *)&(b))) || !((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&b, (void *)(intptr_t)b))), "[Failed]\n VIOLATED PROPERTY: Claim 5 \n \t Location at original code in line: 636 \n \t Comments:   dereference failure: dynamic object lower bound");
// FORTES: Claim 6
   __MAP_FORTES_assert(!((void *)(intptr_t)(b - 1) >= ((intptr_t)(b) + (n * sizeof(*b)))) || !((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&b, (void *)(intptr_t)b))), "[Failed]\n VIOLATED PROPERTY: Claim 6 \n \t Location at original code in line: 636 \n \t Comments:   dereference failure: dynamic object upper bound");
   if (b[-1])
   {
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(a), 637));                                                 /** by FORTES **/
      free(a);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a), (void *)(intptr_t)(a), "a", 2, 0, 1, 0, "int", 637); /** by FORTES **/

// FORTES: Claim 7
      __MAP_FORTES_assert(!((intptr_t)b < 0) || !((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&b, (void *)(intptr_t)b))), "[Failed]\n VIOLATED PROPERTY: Claim 7 \n \t Location at original code in line: 637 \n \t Comments:   dereference failure: dynamic object lower bound");
// FORTES: Claim 8
      __MAP_FORTES_assert(!((intptr_t)b >= ((intptr_t)(b) + (n * sizeof(*b)))) || !((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&b, (void *)(intptr_t)b))), "[Failed]\n VIOLATED PROPERTY: Claim 8 \n \t Location at original code in line: 637 \n \t Comments:   dereference failure: dynamic object upper bound");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(b), 637));                                                 /** by FORTES **/
      free(b);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(b), (void *)(intptr_t)(b), "b", 2, 0, 1, 0, "int", 637); /** by FORTES **/
   }
   else
   {
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(a), 639));                                                 /** by FORTES **/
      free(a);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a), (void *)(intptr_t)(a), "a", 2, 0, 1, 0, "int", 639); /** by FORTES **/

// FORTES: Claim 9
      __MAP_FORTES_assert(!((intptr_t)b < 0) || !((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&b, (void *)(intptr_t)b))), "[Failed]\n VIOLATED PROPERTY: Claim 9 \n \t Location at original code in line: 639 \n \t Comments:   dereference failure: dynamic object lower bound");
// FORTES: Claim 10
      __MAP_FORTES_assert(!((intptr_t)b >= ((intptr_t)(b) + (n * sizeof(*b)))) || !((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&b, (void *)(intptr_t)b))), "[Failed]\n VIOLATED PROPERTY: Claim 10 \n \t Location at original code in line: 639 \n \t Comments:   dereference failure: dynamic object upper bound");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(b), 639));                                                 /** by FORTES **/
      free(b);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(b), (void *)(intptr_t)(b), "b", 2, 0, 1, 0, "int", 639); /** by FORTES **/
   }
   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 640));                                                                   /** by FORTES **/
   return(0);

   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 640)); /** by FORTES **/
}


/** Mapping global variables <- by FORTES**/
MAP_GB_VAR_FORTES()
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(n), (void *)(intptr_t)(NULL), "n", 0, 0, 0, 0, "int", 620); /** by FORTES **/
}
/** End mapping global variables <- by FORTES**/
