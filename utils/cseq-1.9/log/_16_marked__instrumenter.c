
# 1 "<previous_module>"
unsigned int __cs_active_thread[THREADS + 1] = {1};

# 2 "<previous_module>"
unsigned int __cs_pc[THREADS + 1];

# 3 "<previous_module>"
unsigned int __cs_pc_cs[THREADS + 1];

# 4 "<previous_module>"
unsigned int __cs_thread_index;

# 5 "<previous_module>"
unsigned int __cs_last_thread;

# 6 "<previous_module>"
unsigned int __cs_thread_lines[] = {6, 3, 3, 3};

# 8 "<previous_module>"
void *__cs_safe_malloc(unsigned int __cs_size)
{
          
# 9 "<previous_module>"
          void *__cs_ptr = (malloc(__cs_size));
          
# 10 "<previous_module>"
          KLEE_assume(__cs_ptr);
          return (__cs_ptr);
}


# 13 "<previous_module>"
void __cs_init_scalar(void *__cs_var, unsigned int __cs_size)
{
          
# 14 "<previous_module>"
          if (__cs_size == (sizeof(int)))
                    
# 15 "<previous_module>"
          * ((int *) __cs_var) = (KLEE_nondet_int());
          else

# 16 "<previous_module>"
                    {
                    
# 17 "<previous_module>"
                    __cs_var = (malloc(__cs_size));
          }

}


# 21 "<previous_module>"
void __CSEQ_message(char *__cs_message)
{
          ;
}


# 23 "<previous_module>"
typedef int cspthread_t;

# 25 "<previous_module>"
void *__cs_threadargs[THREADS + 1];

# 27 "<previous_module>"
typedef int cspthread_key_t;

# 29 "<previous_module>"
cspthread_key_t __cs_keys[1][THREADS + 2];

# 30 "<previous_module>"
void (*__cs_key_destructor[1])(void *);

# 32 "<previous_module>"
int pthread_create(cspthread_t *__cs_new_thread_id, void *__cs_attr, void *(*__cs_func)(void *), void *__cs_arg, int __cs_threadID)
{
          
# 33 "<previous_module>"
          if (__cs_threadID > THREADS)
                    return (0);

          
# 34 "<previous_module>"
          * __cs_new_thread_id = (__cs_threadID);
          
# 35 "<previous_module>"
          __cs_active_thread[__cs_threadID] = (1);
          
# 36 "<previous_module>"
          __cs_threadargs[__cs_threadID] = (__cs_arg);
          
# 37 "<previous_module>"
          __CSEQ_message("thread spawned");
          
# 38 "<previous_module>"
          return (0);
}


# 41 "<previous_module>"
int pthread_join(cspthread_t __cs_id, void **__cs_value_ptr)
{
          
# 42 "<previous_module>"
          KLEE_assume(__cs_pc[__cs_id] == __cs_thread_lines[__cs_id]);
          
# 43 "<previous_module>"
          return (0);
}


# 46 "<previous_module>"
int pthread_exit(void *__cs_value_ptr)
{
          
# 47 "<previous_module>"
          if ((__cs_key_destructor[0] != 0) && (__cs_keys[0][__cs_thread_index + 1] != 0))
          {
                    
# 48 "<previous_module>"
                    __cs_key_destructor[0](__cs_keys[0][__cs_thread_index + 1]);
          }

}


# 52 "<previous_module>"
int pthread_self(void)
{
          return (__cs_thread_index + 1);
}


# 54 "<previous_module>"
typedef int cspthread_mutex_t;

# 56 "<previous_module>"
int pthread_mutex_init(cspthread_mutex_t *__cs_m, int __cs_val)
{
          
# 57 "<previous_module>"
          * __cs_m = (- 1);
          
# 58 "<previous_module>"
          return (0);
}


# 61 "<previous_module>"
int pthread_mutex_destroy(cspthread_mutex_t *__cs_mutex_to_destroy)
{
          
# 62 "<previous_module>"
          * __cs_mutex_to_destroy = (- 2);
          
# 63 "<previous_module>"
          __CSEQ_message("lock destroyed");
          
# 64 "<previous_module>"
          return (0);
}


# 67 "<previous_module>"
int pthread_mutex_lock(cspthread_mutex_t *__cs_mutex_to_lock)
{
          
# 68 "<previous_module>"
          KLEE_assume((* __cs_mutex_to_lock) == (- 1));
          
# 69 "<previous_module>"
          * __cs_mutex_to_lock = (__cs_thread_index + 1);
          
# 70 "<previous_module>"
          __CSEQ_message("lock acquired");
          
# 71 "<previous_module>"
          return (0);
}


# 74 "<previous_module>"
int pthread_mutex_unlock(cspthread_mutex_t *__cs_mutex_to_unlock)
{
          
# 75 "<previous_module>"
          KLEE_assume((* __cs_mutex_to_unlock) == (__cs_thread_index + 1));
          
# 76 "<previous_module>"
          * __cs_mutex_to_unlock = (- 1);
          
# 77 "<previous_module>"
          __CSEQ_message("lock released");
          
# 78 "<previous_module>"
          return (0);
}


# 81 "<previous_module>"
typedef int cspthread_cond_t;

# 83 "<previous_module>"
int pthread_cond_init(cspthread_cond_t *__cs_cond_to_init, void *__cs_attr)
{
          
# 84 "<previous_module>"
          * __cs_cond_to_init = (- 1);
          
# 85 "<previous_module>"
          return (0);
}


# 88 "<previous_module>"
int pthread_cond_destroy(cspthread_cond_t *__cs_cond_to_destroy)
{
          
# 89 "<previous_module>"
          * __cs_cond_to_destroy = (- 2);
          
# 90 "<previous_module>"
          return (0);
}


# 93 "<previous_module>"
int pthread_cond_wait_1(cspthread_cond_t *__cs_cond_to_wait_for, cspthread_mutex_t *__cs_m)
{
          
# 94 "<previous_module>"
          pthread_mutex_unlock(__cs_m);
          
# 95 "<previous_module>"
          return (0);
}


# 98 "<previous_module>"
int pthread_cond_wait_2(cspthread_cond_t *__cs_cond_to_wait_for, cspthread_mutex_t *__cs_m)
{
          
# 99 "<previous_module>"
          KLEE_assume((* __cs_cond_to_wait_for) == 1);
          
# 100 "<previous_module>"
          pthread_mutex_lock(__cs_m);
          
# 101 "<previous_module>"
          return (0);
}


# 104 "<previous_module>"
int pthread_cond_signal(cspthread_cond_t *__cs_cond_to_signal)
{
          
# 105 "<previous_module>"
          * __cs_cond_to_signal = (1);
          
# 106 "<previous_module>"
          __CSEQ_message("conditional variable signal");
          
# 107 "<previous_module>"
          return (0);
}


# 110 "<previous_module>"
int pthread_cond_broadcast(cspthread_cond_t *__cs_cond_to_broadcast)
{
          
# 111 "<previous_module>"
          * __cs_cond_to_broadcast = (1);
          
# 112 "<previous_module>"
          __CSEQ_message("conditional variable broadcast");
          
# 113 "<previous_module>"
          return (0);
}


# 119 "<previous_module>"
typedef struct cspthread_barrier_t
{
          
# 117 "<previous_module>"
          unsigned int init;
          
# 118 "<previous_module>"
          unsigned int current;
} cspthread_barrier_t;

# 121 "<previous_module>"
int pthread_barrier_init(cspthread_barrier_t *__cs_barrier_to_init, void *__cs_attr, unsigned int count)
{
          
# 122 "<previous_module>"
          __cs_barrier_to_init->current = (count);
          
# 123 "<previous_module>"
          __cs_barrier_to_init->init = (count);
          
# 124 "<previous_module>"
          return (0);
}


# 127 "<previous_module>"
int pthread_barrier_destroy(cspthread_barrier_t *__cs_barrier_to_destroy)
{
          
# 128 "<previous_module>"
          __cs_barrier_to_destroy->init = (- 1);
          
# 129 "<previous_module>"
          __cs_barrier_to_destroy->current = (- 1);
          
# 130 "<previous_module>"
          return (0);
}


# 134 "<previous_module>"
int pthread_barrier_wait_1(cspthread_barrier_t *__cs_barrier_to_wait)
{
          
# 135 "<previous_module>"
          __cs_barrier_to_wait->current--;
          
# 136 "<previous_module>"
          return (0);
}


# 139 "<previous_module>"
int pthread_barrier_wait_2(cspthread_barrier_t *__cs_barrier_to_wait)
{
          
# 140 "<previous_module>"
          KLEE_assume(__cs_barrier_to_wait->current == 0);
          
# 141 "<previous_module>"
          __cs_barrier_to_wait->current = (__cs_barrier_to_wait->init);
          
# 142 "<previous_module>"
          return (0);
}


# 145 "<previous_module>"
int pthread_key_create(cspthread_key_t *key, void (*destructor)(void *))
{
          
# 146 "<previous_module>"
          static int currentkey = (0);
          
# 147 "<previous_module>"
          __cs_key_destructor[0] = (destructor);
          
# 148 "<previous_module>"
          * key = (currentkey++);
          
# 149 "<previous_module>"
          return (0);
}


# 152 "<previous_module>"
int pthread_setspecific(cspthread_key_t key, const void *value)
{
          
# 153 "<previous_module>"
          __cs_keys[key][__cs_thread_index + 1] = (value);
          
# 154 "<previous_module>"
          return (0);
}


# 157 "<previous_module>"
void *pthread_getspecific(cspthread_key_t key)
{
          
# 158 "<previous_module>"
          return (__cs_keys[key][__cs_thread_index + 1]);
}


# 162 "<previous_module>"
void __CSEQ_noop(void)
{
}


# 165 "<previous_module>"
void __VERIFIER_error();

# 167 "<previous_module>"
typedef int _____STARTSTRIPPINGFROMHERE_____;

# 169 "<previous_module>"
typedef int __cs_barrier_t;

# 171 "<previous_module>"
typedef int __cs_barrierattr_t;

# 173 "<previous_module>"
typedef int __cs_attr_t;

# 175 "<previous_module>"
typedef int __cs_cond_t;

# 177 "<previous_module>"
typedef int __cs_condattr_t;

# 179 "<previous_module>"
typedef int __cs_key_t;

# 181 "<previous_module>"
typedef int __cs_mutex_t;

# 183 "<previous_module>"
typedef int __cs_mutexattr_t;

# 185 "<previous_module>"
typedef int __cs_once_t;

# 187 "<previous_module>"
typedef int __cs_rwlock_t;

# 189 "<previous_module>"
typedef int __cs_rwlockattr_t;

# 191 "<previous_module>"
typedef int __cs_t;

# 193 "<previous_module>"
typedef int size_t;

# 195 "<previous_module>"
typedef int __builtin_va_list;

# 197 "<previous_module>"
typedef int __gnuc_va_list;

# 199 "<previous_module>"
typedef int __int8_t;

# 201 "<previous_module>"
typedef int __uint8_t;

# 203 "<previous_module>"
typedef int __int16_t;

# 205 "<previous_module>"
typedef int __uint16_t;

# 207 "<previous_module>"
typedef int __int_least16_t;

# 209 "<previous_module>"
typedef int __uint_least16_t;

# 211 "<previous_module>"
typedef int __int32_t;

# 213 "<previous_module>"
typedef int __uint32_t;

# 215 "<previous_module>"
typedef int __int64_t;

# 217 "<previous_module>"
typedef int __uint64_t;

# 219 "<previous_module>"
typedef int __int_least32_t;

# 221 "<previous_module>"
typedef int __uint_least32_t;

# 223 "<previous_module>"
typedef int __s8;

# 225 "<previous_module>"
typedef int __u8;

# 227 "<previous_module>"
typedef int __s16;

# 229 "<previous_module>"
typedef int __u16;

# 231 "<previous_module>"
typedef int __s32;

# 233 "<previous_module>"
typedef int __u32;

# 235 "<previous_module>"
typedef int __s64;

# 237 "<previous_module>"
typedef int __u64;

# 239 "<previous_module>"
typedef int _LOCK_T;

# 241 "<previous_module>"
typedef int _LOCK_RECURSIVE_T;

# 243 "<previous_module>"
typedef int _off_t;

# 245 "<previous_module>"
typedef int __dev_t;

# 247 "<previous_module>"
typedef int __uid_t;

# 249 "<previous_module>"
typedef int __gid_t;

# 251 "<previous_module>"
typedef int _off64_t;

# 253 "<previous_module>"
typedef int _fpos_t;

# 255 "<previous_module>"
typedef int _ssize_t;

# 257 "<previous_module>"
typedef int wint_t;

# 259 "<previous_module>"
typedef int _mbstate_t;

# 261 "<previous_module>"
typedef int _flock_t;

# 263 "<previous_module>"
typedef int _iconv_t;

# 265 "<previous_module>"
typedef int __ULong;

# 267 "<previous_module>"
typedef int __FILE;

# 269 "<previous_module>"
typedef int ptrdiff_t;

# 271 "<previous_module>"
typedef int wchar_t;

# 273 "<previous_module>"
typedef int __off_t;

# 275 "<previous_module>"
typedef int __pid_t;

# 277 "<previous_module>"
typedef int __loff_t;

# 279 "<previous_module>"
typedef int u_char;

# 281 "<previous_module>"
typedef int u_short;

# 283 "<previous_module>"
typedef int u_int;

# 285 "<previous_module>"
typedef int u_long;

# 287 "<previous_module>"
typedef int ushort;

# 289 "<previous_module>"
typedef int uint;

# 291 "<previous_module>"
typedef int clock_t;

# 293 "<previous_module>"
typedef int time_t;

# 295 "<previous_module>"
typedef int daddr_t;

# 297 "<previous_module>"
typedef int caddr_t;

# 299 "<previous_module>"
typedef int ino_t;

# 301 "<previous_module>"
typedef int off_t;

# 303 "<previous_module>"
typedef int dev_t;

# 305 "<previous_module>"
typedef int uid_t;

# 307 "<previous_module>"
typedef int gid_t;

# 309 "<previous_module>"
typedef int pid_t;

# 311 "<previous_module>"
typedef int key_t;

# 313 "<previous_module>"
typedef int ssize_t;

# 315 "<previous_module>"
typedef int mode_t;

# 317 "<previous_module>"
typedef int nlink_t;

# 319 "<previous_module>"
typedef int fd_mask;

# 321 "<previous_module>"
typedef int _types_fd_set;

# 323 "<previous_module>"
typedef int fd_set;

# 325 "<previous_module>"
typedef int clockid_t;

# 327 "<previous_module>"
typedef int timer_t;

# 329 "<previous_module>"
typedef int useconds_t;

# 331 "<previous_module>"
typedef int suseconds_t;

# 333 "<previous_module>"
typedef int FILE;

# 335 "<previous_module>"
typedef int fpos_t;

# 337 "<previous_module>"
typedef int cookie_read_function_t;

# 339 "<previous_module>"
typedef int cookie_write_function_t;

# 341 "<previous_module>"
typedef int cookie_seek_function_t;

# 343 "<previous_module>"
typedef int cookie_close_function_t;

# 345 "<previous_module>"
typedef int cookie_io_functions_t;

# 347 "<previous_module>"
typedef int div_t;

# 349 "<previous_module>"
typedef int ldiv_t;

# 351 "<previous_module>"
typedef int lldiv_t;

# 353 "<previous_module>"
typedef int sigset_t;

# 355 "<previous_module>"
typedef int __sigset_t;

# 357 "<previous_module>"
typedef int _sig_func_ptr;

# 359 "<previous_module>"
typedef int sig_atomic_t;

# 361 "<previous_module>"
typedef int __tzrule_type;

# 363 "<previous_module>"
typedef int __tzinfo_type;

# 365 "<previous_module>"
typedef int mbstate_t;

# 367 "<previous_module>"
typedef int sem_t;

# 369 "<previous_module>"
typedef int pthread_t;

# 371 "<previous_module>"
typedef int pthread_attr_t;

# 373 "<previous_module>"
typedef int pthread_mutex_t;

# 375 "<previous_module>"
typedef int pthread_mutexattr_t;

# 377 "<previous_module>"
typedef int pthread_cond_t;

# 379 "<previous_module>"
typedef int pthread_condattr_t;

# 381 "<previous_module>"
typedef int pthread_key_t;

# 383 "<previous_module>"
typedef int pthread_once_t;

# 385 "<previous_module>"
typedef int pthread_rwlock_t;

# 387 "<previous_module>"
typedef int pthread_rwlockattr_t;

# 389 "<previous_module>"
typedef int pthread_spinlock_t;

# 391 "<previous_module>"
typedef int pthread_barrier_t;

# 393 "<previous_module>"
typedef int pthread_barrierattr_t;

# 395 "<previous_module>"
typedef int jmp_buf;

# 397 "<previous_module>"
typedef int rlim_t;

# 399 "<previous_module>"
typedef int sa_family_t;

# 401 "<previous_module>"
typedef int sigjmp_buf;

# 403 "<previous_module>"
typedef int stack_t;

# 405 "<previous_module>"
typedef int siginfo_t;

# 407 "<previous_module>"
typedef int z_stream;

# 409 "<previous_module>"
typedef int int8_t;

# 411 "<previous_module>"
typedef int uint8_t;

# 413 "<previous_module>"
typedef int int16_t;

# 415 "<previous_module>"
typedef int uint16_t;

# 417 "<previous_module>"
typedef int int32_t;

# 419 "<previous_module>"
typedef int uint32_t;

# 421 "<previous_module>"
typedef int int64_t;

# 423 "<previous_module>"
typedef int uint64_t;

# 425 "<previous_module>"
typedef int int_least8_t;

# 427 "<previous_module>"
typedef int uint_least8_t;

# 429 "<previous_module>"
typedef int int_least16_t;

# 431 "<previous_module>"
typedef int uint_least16_t;

# 433 "<previous_module>"
typedef int int_least32_t;

# 435 "<previous_module>"
typedef int uint_least32_t;

# 437 "<previous_module>"
typedef int int_least64_t;

# 439 "<previous_module>"
typedef int uint_least64_t;

# 441 "<previous_module>"
typedef int int_fast8_t;

# 443 "<previous_module>"
typedef int uint_fast8_t;

# 445 "<previous_module>"
typedef int int_fast16_t;

# 447 "<previous_module>"
typedef int uint_fast16_t;

# 449 "<previous_module>"
typedef int int_fast32_t;

# 451 "<previous_module>"
typedef int uint_fast32_t;

# 453 "<previous_module>"
typedef int int_fast64_t;

# 455 "<previous_module>"
typedef int uint_fast64_t;

# 457 "<previous_module>"
typedef int intptr_t;

# 459 "<previous_module>"
typedef int uintptr_t;

# 461 "<previous_module>"
typedef int intmax_t;

# 463 "<previous_module>"
typedef int uintmax_t;

# 465 "<previous_module>"
typedef _Bool bool;

# 467 "<previous_module>"
typedef void BZFILE;

# 469 "<previous_module>"
typedef int va_list;

# 471 "<previous_module>"
typedef int loff_t;

# 473 "<previous_module>"
typedef int _____STOPSTRIPPINGFROMHERE_____;

# 475 "<previous_module>"
cspthread_mutex_t m;

# 477 "<previous_module>"
int data = (0);

# 479 "<previous_module>"
void *thread1_0(void *__cs_param_thread1_arg)

# 481 "<previous_module>"
{
          
# 482 "<previous_module>"
          IF(1,0,tthread1_0_1)__CSEQ_removeindent;
          
# 484 "<previous_module>"
          pthread_mutex_lock(& m);
          
# 485 "<previous_module>"
          tthread1_0_1: IF(1,1,tthread1_0_2)__CSEQ_removeindent;
          
# 487 "<previous_module>"
          data++;
          
# 488 "<previous_module>"
          tthread1_0_2: IF(1,2,tthread1_0_3)__CSEQ_removeindent;
          
# 490 "<previous_module>"
          pthread_mutex_unlock(& m);
          
# 491 "<previous_module>"
          __exit_thread1:
          KLEE_assume(__cs_pc_cs[1] >= 3);

          
# 492 "<previous_module>"
          ;
          ;
          
# 494 "<previous_module>"
          tthread1_0_3: __CSEQ_removeindent;
          
# 496 "<previous_module>"
          pthread_exit(0);
}


# 502 "<previous_module>"
void *thread2_0(void *__cs_param_thread2_arg)

# 504 "<previous_module>"
{
          
# 505 "<previous_module>"
          IF(2,0,tthread2_0_1)__CSEQ_removeindent;
          
# 507 "<previous_module>"
          pthread_mutex_lock(& m);
          
# 508 "<previous_module>"
          tthread2_0_1: IF(2,1,tthread2_0_2)__CSEQ_removeindent;
          
# 510 "<previous_module>"
          data += (2);
          
# 511 "<previous_module>"
          tthread2_0_2: IF(2,2,tthread2_0_3)__CSEQ_removeindent;
          
# 513 "<previous_module>"
          pthread_mutex_unlock(& m);
          
# 514 "<previous_module>"
          __exit_thread2:
          KLEE_assume(__cs_pc_cs[2] >= 3);

          
# 515 "<previous_module>"
          ;
          ;
          
# 517 "<previous_module>"
          tthread2_0_3: __CSEQ_removeindent;
          
# 519 "<previous_module>"
          pthread_exit(0);
}


# 525 "<previous_module>"
void *thread3_0(void *__cs_param_thread3_arg)

# 527 "<previous_module>"
{
          
# 528 "<previous_module>"
          IF(3,0,tthread3_0_1)__CSEQ_removeindent;
          
# 530 "<previous_module>"
          pthread_mutex_lock(& m);
          
# 532 "<previous_module>"
          static _Bool __cs_local_thread3___cs_tmp_if_cond_0;
          
# 533 "<previous_module>"
          tthread3_0_1: IF(3,1,tthread3_0_2)__CSEQ_removeindent;
          
# 535 "<previous_module>"
          __cs_local_thread3___cs_tmp_if_cond_0 = (data >= 3);
          
# 537 "<previous_module>"
          if (__cs_local_thread3___cs_tmp_if_cond_0)

# 539 "<previous_module>"
                    {
                    
# 541 "<previous_module>"
                    KLEE_assert(0);
                    
# 543 "<previous_module>"
                    ;
                    ;
          }

          
# 545 "<previous_module>"
          ;
          
# 546 "<previous_module>"
          tthread3_0_2: IF(3,2,tthread3_0_3)__CSEQ_removeindent;
          
# 548 "<previous_module>"
          pthread_mutex_unlock(& m);
          
# 549 "<previous_module>"
          __exit_thread3:
          KLEE_assume(__cs_pc_cs[3] >= 3);

          
# 550 "<previous_module>"
          ;
          ;
          
# 552 "<previous_module>"
          tthread3_0_3: __CSEQ_removeindent;
          
# 554 "<previous_module>"
          pthread_exit(0);
}


# 560 "<previous_module>"
int main_thread(void)

# 561 "<previous_module>"
{
          
# 562 "<previous_module>"
          IF(0,0,tmain_1)__CSEQ_removeindent;
          
# 564 "<previous_module>"
          pthread_mutex_init(& m, 0);
          
# 566 "<previous_module>"
          static cspthread_t __cs_local_main_t1;
          __cs_init_scalar(& __cs_local_main_t1, sizeof(cspthread_t));
          
# 568 "<previous_module>"
          static cspthread_t __cs_local_main_t2;
          __cs_init_scalar(& __cs_local_main_t2, sizeof(cspthread_t));
          
# 570 "<previous_module>"
          static cspthread_t __cs_local_main_t3;
          __cs_init_scalar(& __cs_local_main_t3, sizeof(cspthread_t));
          
# 572 "<previous_module>"
          pthread_create(& __cs_local_main_t1, 0, thread1_0, 0, 1);
          
# 573 "<previous_module>"
          tmain_1: IF(0,1,tmain_2)__CSEQ_removeindent;
          
# 575 "<previous_module>"
          pthread_create(& __cs_local_main_t2, 0, thread2_0, 0, 2);
          
# 576 "<previous_module>"
          tmain_2: IF(0,2,tmain_3)__CSEQ_removeindent;
          
# 578 "<previous_module>"
          pthread_create(& __cs_local_main_t3, 0, thread3_0, 0, 3);
          
# 579 "<previous_module>"
          tmain_3: IF(0,3,tmain_4)__CSEQ_removeindent;
          
# 581 "<previous_module>"
          pthread_join(__cs_local_main_t1, 0);
          
# 582 "<previous_module>"
          tmain_4: IF(0,4,tmain_5)__CSEQ_removeindent;
          
# 584 "<previous_module>"
          pthread_join(__cs_local_main_t2, 0);
          
# 585 "<previous_module>"
          tmain_5: IF(0,5,tmain_6)__CSEQ_removeindent;
          
# 587 "<previous_module>"
          pthread_join(__cs_local_main_t3, 0);
          
# 589 "<previous_module>"
          goto __exit_main;
          ;
          
# 590 "<previous_module>"
          __exit_main:
          KLEE_assume(__cs_pc_cs[0] >= 6);

          
# 591 "<previous_module>"
          ;
          ;
          
# 593 "<previous_module>"
          tmain_6: __CSEQ_removeindent;
          
# 595 "<previous_module>"
          pthread_exit(0);
}


# 600 "<previous_module>"
int main(void)
{
          
# 601 "<previous_module>"
          /* round  0 */__CSEQ_removeindent;
          
# 602 "<previous_module>"
              /* main */__CSEQ_removeindent;
          
# 603 "<previous_module>"
          __cs_thread_index = (0);
          
# 604 "<previous_module>"
          unsigned int __cs_tmp_t0_r0;
          
# 605 "<previous_module>"
          __cs_pc_cs[0] = (__cs_tmp_t0_r0);
          
# 606 "<previous_module>"
          KLEE_assume(__cs_pc_cs[0] > 0);
          
# 607 "<previous_module>"
          KLEE_assume(__cs_pc_cs[0] <= 6);
          
# 608 "<previous_module>"
          main_thread();
          
# 609 "<previous_module>"
          __cs_pc[0] = (__cs_pc_cs[0]);
          
# 611 "<previous_module>"
              /* thread1_0 */__CSEQ_removeindent;
          
# 612 "<previous_module>"
          unsigned int __cs_tmp_t1_r0;
          
# 613 "<previous_module>"
          if (__cs_active_thread[1])
          {
                    
# 614 "<previous_module>"
                    __cs_thread_index = (1);
                    
# 615 "<previous_module>"
                    __cs_pc_cs[1] = (__cs_tmp_t1_r0);
                    
# 616 "<previous_module>"
                    KLEE_assume(__cs_pc_cs[1] <= 3);
                    
# 617 "<previous_module>"
                    thread1_0(__cs_threadargs[1]);
                    
# 618 "<previous_module>"
                    __cs_pc[1] = (__cs_pc_cs[1]);
          }

          
# 621 "<previous_module>"
              /* thread2_0 */__CSEQ_removeindent;
          
# 622 "<previous_module>"
          unsigned int __cs_tmp_t2_r0;
          
# 623 "<previous_module>"
          if (__cs_active_thread[2])
          {
                    
# 624 "<previous_module>"
                    __cs_thread_index = (2);
                    
# 625 "<previous_module>"
                    __cs_pc_cs[2] = (__cs_tmp_t2_r0);
                    
# 626 "<previous_module>"
                    KLEE_assume(__cs_pc_cs[2] <= 3);
                    
# 627 "<previous_module>"
                    thread2_0(__cs_threadargs[2]);
                    
# 628 "<previous_module>"
                    __cs_pc[2] = (__cs_pc_cs[2]);
          }

          
# 631 "<previous_module>"
              /* thread3_0 */__CSEQ_removeindent;
          
# 632 "<previous_module>"
          unsigned int __cs_tmp_t3_r0;
          
# 633 "<previous_module>"
          if (__cs_active_thread[3])
          {
                    
# 634 "<previous_module>"
                    __cs_thread_index = (3);
                    
# 635 "<previous_module>"
                    __cs_pc_cs[3] = (__cs_tmp_t3_r0);
                    
# 636 "<previous_module>"
                    KLEE_assume(__cs_pc_cs[3] <= 3);
                    
# 637 "<previous_module>"
                    thread3_0(__cs_threadargs[3]);
                    
# 638 "<previous_module>"
                    __cs_pc[3] = (__cs_pc_cs[3]);
          }

          
# 641 "<previous_module>"
          unsigned int __cs_tmp_t0_r1;
          
# 642 "<previous_module>"
          if (__cs_active_thread[0])
          {
                    
# 643 "<previous_module>"
                    __cs_thread_index = (0);
                    
# 644 "<previous_module>"
                    __cs_pc_cs[0] = (__cs_tmp_t0_r1);
                    
# 645 "<previous_module>"
                    KLEE_assume(__cs_pc_cs[0] >= __cs_pc[0]);
                    
# 646 "<previous_module>"
                    KLEE_assume(__cs_pc_cs[0] <= 6);
                    
# 647 "<previous_module>"
                    main_thread();
          }

          
# 650 "<previous_module>"
          return (0);
}

