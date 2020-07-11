unsigned int __cs_active_thread[THREADS+1]={1};
unsigned int __cs_pc[THREADS+1];
unsigned int __cs_pc_cs[THREADS+1];
unsigned int __cs_thread_index;
unsigned int __cs_last_thread;
unsigned int __cs_thread_lines[] = {6, 3, 3, 3};

void *__cs_safe_malloc(unsigned int __cs_size) {
	void *__cs_ptr = malloc(__cs_size);
	__VERIFIER_assume(__cs_ptr); return __cs_ptr;
}

void __cs_init_scalar(void *__cs_var, unsigned int __cs_size) {
	if (__cs_size == sizeof(int))
		*(int *)__cs_var = __VERIFIER_nondet_int();
	else {
		__cs_var = malloc(__cs_size);
	}
}

void __CSEQ_message(char *__cs_message) { ; }

typedef int cspthread_t;

void *__cs_threadargs[THREADS+1];

typedef int cspthread_key_t;

cspthread_key_t __cs_keys[1][THREADS+2];
void (*__cs_key_destructor[1])(void*);

int pthread_create(cspthread_t *__cs_new_thread_id, void *__cs_attr, void *(*__cs_func)(void*), void *__cs_arg, int __cs_threadID) {
	if (__cs_threadID > THREADS) return 0;
	*__cs_new_thread_id = __cs_threadID;
	__cs_active_thread[__cs_threadID] = 1;
	__cs_threadargs[__cs_threadID] = __cs_arg;
	__CSEQ_message("thread spawned");
	return 0;
}

int pthread_join(cspthread_t __cs_id, void **__cs_value_ptr) {
	__VERIFIER_assume(__cs_pc[__cs_id] == __cs_thread_lines[__cs_id]);
	return 0;
}

int pthread_exit(void *__cs_value_ptr) {
	if (__cs_key_destructor[0]!=0 && __cs_keys[0][__cs_thread_index+1]!=0) {
		__cs_key_destructor[0](__cs_keys[0][__cs_thread_index+1]);
	}
}

int pthread_self(void) { return __cs_thread_index+1; }

typedef int cspthread_mutex_t;

int pthread_mutex_init (cspthread_mutex_t *__cs_m, int __cs_val) {
	*__cs_m = -1;
	return 0;
}

int pthread_mutex_destroy(cspthread_mutex_t *__cs_mutex_to_destroy) {
	*__cs_mutex_to_destroy = -2;
	__CSEQ_message("lock destroyed");
	return 0;
}

int pthread_mutex_lock(cspthread_mutex_t *__cs_mutex_to_lock) {
	__VERIFIER_assume(*__cs_mutex_to_lock==-1);
	*__cs_mutex_to_lock = __cs_thread_index+1;
	__CSEQ_message("lock acquired");
	return 0;
}

int pthread_mutex_unlock(cspthread_mutex_t *__cs_mutex_to_unlock) {
	__VERIFIER_assume(*__cs_mutex_to_unlock==(__cs_thread_index+1));
	*__cs_mutex_to_unlock = -1;
	__CSEQ_message("lock released");
	return 0;
}

typedef int cspthread_cond_t;

int pthread_cond_init(cspthread_cond_t *__cs_cond_to_init, void *__cs_attr) {
	*__cs_cond_to_init = -1;
	return 0;
}

int pthread_cond_destroy(cspthread_cond_t *__cs_cond_to_destroy) {
	*__cs_cond_to_destroy = -2;
	return 0;
}

int pthread_cond_wait_1(cspthread_cond_t *__cs_cond_to_wait_for, cspthread_mutex_t *__cs_m) {
	pthread_mutex_unlock(__cs_m);
	return 0;
}

int pthread_cond_wait_2(cspthread_cond_t *__cs_cond_to_wait_for, cspthread_mutex_t *__cs_m) {
	__VERIFIER_assume(*__cs_cond_to_wait_for == 1);
	pthread_mutex_lock(__cs_m);
	return 0;
}

int pthread_cond_signal(cspthread_cond_t *__cs_cond_to_signal) {
	*__cs_cond_to_signal = 1;
	__CSEQ_message("conditional variable signal");
	return 0;
}

int pthread_cond_broadcast(cspthread_cond_t *__cs_cond_to_broadcast) {
	*__cs_cond_to_broadcast = 1;
	__CSEQ_message("conditional variable broadcast");
	return 0;
}

typedef struct cspthread_barrier_t {
	unsigned int init;
	unsigned int current;
} cspthread_barrier_t;

int pthread_barrier_init(cspthread_barrier_t *__cs_barrier_to_init, void * __cs_attr, unsigned int count){
	__cs_barrier_to_init->current = count;
	__cs_barrier_to_init->init = count;
	return 0;
}

int pthread_barrier_destroy(cspthread_barrier_t *__cs_barrier_to_destroy) {
	__cs_barrier_to_destroy->init = -1;
	__cs_barrier_to_destroy->current = -1;
	return 0;
}


int pthread_barrier_wait_1(cspthread_barrier_t *__cs_barrier_to_wait){
	__cs_barrier_to_wait->current--;
	return 0;
}

int pthread_barrier_wait_2(cspthread_barrier_t *__cs_barrier_to_wait){
	__VERIFIER_assume(__cs_barrier_to_wait->current == 0);
	__cs_barrier_to_wait->current = __cs_barrier_to_wait->init;
	return 0;
}

pthread_key_create(cspthread_key_t *key, void (*destructor)(void*)) {
	static int currentkey = 0;
	__cs_key_destructor[0] = destructor;
	*key = currentkey++;
	return 0;
}

int pthread_setspecific(cspthread_key_t key, const void *value) {
	__cs_keys[key][__cs_thread_index+1] = value;
	return 0;
}

void *pthread_getspecific(cspthread_key_t key) {
	return __cs_keys[key][__cs_thread_index+1];
}


void __CSEQ_noop(void) {};


void __VERIFIER_error();

typedef int _____STARTSTRIPPINGFROMHERE_____;

typedef int __cs_barrier_t;

typedef int __cs_barrierattr_t;

typedef int __cs_attr_t;

typedef int __cs_cond_t;

typedef int __cs_condattr_t;

typedef int __cs_key_t;

typedef int __cs_mutex_t;

typedef int __cs_mutexattr_t;

typedef int __cs_once_t;

typedef int __cs_rwlock_t;

typedef int __cs_rwlockattr_t;

typedef int __cs_t;

typedef int size_t;

typedef int __builtin_va_list;

typedef int __gnuc_va_list;

typedef int __int8_t;

typedef int __uint8_t;

typedef int __int16_t;

typedef int __uint16_t;

typedef int __int_least16_t;

typedef int __uint_least16_t;

typedef int __int32_t;

typedef int __uint32_t;

typedef int __int64_t;

typedef int __uint64_t;

typedef int __int_least32_t;

typedef int __uint_least32_t;

typedef int __s8;

typedef int __u8;

typedef int __s16;

typedef int __u16;

typedef int __s32;

typedef int __u32;

typedef int __s64;

typedef int __u64;

typedef int _LOCK_T;

typedef int _LOCK_RECURSIVE_T;

typedef int _off_t;

typedef int __dev_t;

typedef int __uid_t;

typedef int __gid_t;

typedef int _off64_t;

typedef int _fpos_t;

typedef int _ssize_t;

typedef int wint_t;

typedef int _mbstate_t;

typedef int _flock_t;

typedef int _iconv_t;

typedef int __ULong;

typedef int __FILE;

typedef int ptrdiff_t;

typedef int wchar_t;

typedef int __off_t;

typedef int __pid_t;

typedef int __loff_t;

typedef int u_char;

typedef int u_short;

typedef int u_int;

typedef int u_long;

typedef int ushort;

typedef int uint;

typedef int clock_t;

typedef int time_t;

typedef int daddr_t;

typedef int caddr_t;

typedef int ino_t;

typedef int off_t;

typedef int dev_t;

typedef int uid_t;

typedef int gid_t;

typedef int pid_t;

typedef int key_t;

typedef int ssize_t;

typedef int mode_t;

typedef int nlink_t;

typedef int fd_mask;

typedef int _types_fd_set;

typedef int fd_set;

typedef int clockid_t;

typedef int timer_t;

typedef int useconds_t;

typedef int suseconds_t;

typedef int FILE;

typedef int fpos_t;

typedef int cookie_read_function_t;

typedef int cookie_write_function_t;

typedef int cookie_seek_function_t;

typedef int cookie_close_function_t;

typedef int cookie_io_functions_t;

typedef int div_t;

typedef int ldiv_t;

typedef int lldiv_t;

typedef int sigset_t;

typedef int __sigset_t;

typedef int _sig_func_ptr;

typedef int sig_atomic_t;

typedef int __tzrule_type;

typedef int __tzinfo_type;

typedef int mbstate_t;

typedef int sem_t;

typedef int pthread_t;

typedef int pthread_attr_t;

typedef int pthread_mutex_t;

typedef int pthread_mutexattr_t;

typedef int pthread_cond_t;

typedef int pthread_condattr_t;

typedef int pthread_key_t;

typedef int pthread_once_t;

typedef int pthread_rwlock_t;

typedef int pthread_rwlockattr_t;

typedef int pthread_spinlock_t;

typedef int pthread_barrier_t;

typedef int pthread_barrierattr_t;

typedef int jmp_buf;

typedef int rlim_t;

typedef int sa_family_t;

typedef int sigjmp_buf;

typedef int stack_t;

typedef int siginfo_t;

typedef int z_stream;

typedef int int8_t;

typedef int uint8_t;

typedef int int16_t;

typedef int uint16_t;

typedef int int32_t;

typedef int uint32_t;

typedef int int64_t;

typedef int uint64_t;

typedef int int_least8_t;

typedef int uint_least8_t;

typedef int int_least16_t;

typedef int uint_least16_t;

typedef int int_least32_t;

typedef int uint_least32_t;

typedef int int_least64_t;

typedef int uint_least64_t;

typedef int int_fast8_t;

typedef int uint_fast8_t;

typedef int int_fast16_t;

typedef int uint_fast16_t;

typedef int int_fast32_t;

typedef int uint_fast32_t;

typedef int int_fast64_t;

typedef int uint_fast64_t;

typedef int intptr_t;

typedef int uintptr_t;

typedef int intmax_t;

typedef int uintmax_t;

typedef _Bool bool;

typedef void BZFILE;

typedef int va_list;

typedef int loff_t;

typedef int _____STOPSTRIPPINGFROMHERE_____;

pthread_mutex_t m;

int data = 0;

void *thread1_0(void *__cs_param_thread1_arg)

{
__CSEQ_rawline("IF(1,0,tthread1_0_1)");

     pthread_mutex_lock(& m);
__CSEQ_rawline("tthread1_0_1: IF(1,1,tthread1_0_2)");

     data++;
__CSEQ_rawline("tthread1_0_2: IF(1,2,tthread1_0_3)");

     pthread_mutex_unlock(& m);
     __exit_thread1: __VERIFIER_assume( __cs_pc_cs[1] >= 3 );
;;

__CSEQ_rawline("tthread1_0_3: ");

     pthread_exit(0);
}




void *thread2_0(void *__cs_param_thread2_arg)

{
__CSEQ_rawline("IF(2,0,tthread2_0_1)");

     pthread_mutex_lock(& m);
__CSEQ_rawline("tthread2_0_1: IF(2,1,tthread2_0_2)");

     data += (2);
__CSEQ_rawline("tthread2_0_2: IF(2,2,tthread2_0_3)");

     pthread_mutex_unlock(& m);
     __exit_thread2: __VERIFIER_assume( __cs_pc_cs[2] >= 3 );
;;

__CSEQ_rawline("tthread2_0_3: ");

     pthread_exit(0);
}




void *thread3_0(void *__cs_param_thread3_arg)

{
__CSEQ_rawline("IF(3,0,tthread3_0_1)");

     pthread_mutex_lock(& m);

     static _Bool __cs_local_thread3___cs_tmp_if_cond_0;
__CSEQ_rawline("tthread3_0_1: IF(3,1,tthread3_0_2)");

     __cs_local_thread3___cs_tmp_if_cond_0 = (data >= 3);

     if (__cs_local_thread3___cs_tmp_if_cond_0)

          {

          __VERIFIER_assert(0);

          ;;
     }
     ;
__CSEQ_rawline("tthread3_0_2: IF(3,2,tthread3_0_3)");

     pthread_mutex_unlock(& m);
     __exit_thread3: __VERIFIER_assume( __cs_pc_cs[3] >= 3 );
;;

__CSEQ_rawline("tthread3_0_3: ");

     pthread_exit(0);
}




int main_thread(void)
{
__CSEQ_rawline("IF(0,0,tmain_1)");

     pthread_mutex_init(& m, 0);

     static pthread_t __cs_local_main_t1; __cs_init_scalar(&__cs_local_main_t1, sizeof(cspthread_t));

     static pthread_t __cs_local_main_t2; __cs_init_scalar(&__cs_local_main_t2, sizeof(cspthread_t));

     static pthread_t __cs_local_main_t3; __cs_init_scalar(&__cs_local_main_t3, sizeof(cspthread_t));

     pthread_create(& __cs_local_main_t1, 0, thread1_0, 0, 1);
__CSEQ_rawline("tmain_1: IF(0,1,tmain_2)");

     pthread_create(& __cs_local_main_t2, 0, thread2_0, 0, 2);
__CSEQ_rawline("tmain_2: IF(0,2,tmain_3)");

     pthread_create(& __cs_local_main_t3, 0, thread3_0, 0, 3);
__CSEQ_rawline("tmain_3: IF(0,3,tmain_4)");

     pthread_join(__cs_local_main_t1, 0);
__CSEQ_rawline("tmain_4: IF(0,4,tmain_5)");

     pthread_join(__cs_local_main_t2, 0);
__CSEQ_rawline("tmain_5: IF(0,5,tmain_6)");

     pthread_join(__cs_local_main_t3, 0);

     goto __exit_main;;
     __exit_main: __VERIFIER_assume( __cs_pc_cs[0] >= 6 );
;;

__CSEQ_rawline("tmain_6: ");

     pthread_exit(0);
}



int main(void) {
__CSEQ_rawline("/* round  0 */");
__CSEQ_rawline("    /* main */");
          __cs_thread_index = 0;
          unsigned int __cs_tmp_t0_r0;
          __cs_pc_cs[0] = __cs_tmp_t0_r0;
          __VERIFIER_assume(__cs_pc_cs[0] > 0);
          __VERIFIER_assume(__cs_pc_cs[0] <= 6);
          main_thread();
          __cs_pc[0] = __cs_pc_cs[0];

__CSEQ_rawline("    /* thread1_0 */");
          unsigned int __cs_tmp_t1_r0;
          if (__cs_active_thread[1]) {
             __cs_thread_index = 1;
             __cs_pc_cs[1] = __cs_tmp_t1_r0;
             __VERIFIER_assume(__cs_pc_cs[1] <= 3);
             thread1_0(__cs_threadargs[1]);
             __cs_pc[1] = __cs_pc_cs[1];
          }

__CSEQ_rawline("    /* thread2_0 */");
          unsigned int __cs_tmp_t2_r0;
          if (__cs_active_thread[2]) {
             __cs_thread_index = 2;
             __cs_pc_cs[2] = __cs_tmp_t2_r0;
             __VERIFIER_assume(__cs_pc_cs[2] <= 3);
             thread2_0(__cs_threadargs[2]);
             __cs_pc[2] = __cs_pc_cs[2];
          }

__CSEQ_rawline("    /* thread3_0 */");
          unsigned int __cs_tmp_t3_r0;
          if (__cs_active_thread[3]) {
             __cs_thread_index = 3;
             __cs_pc_cs[3] = __cs_tmp_t3_r0;
             __VERIFIER_assume(__cs_pc_cs[3] <= 3);
             thread3_0(__cs_threadargs[3]);
             __cs_pc[3] = __cs_pc_cs[3];
          }

          unsigned int __cs_tmp_t0_r1;
          if (__cs_active_thread[0]) {
             __cs_thread_index = 0;
             __cs_pc_cs[0] = __cs_tmp_t0_r1;
             __VERIFIER_assume(__cs_pc_cs[0] >= __cs_pc[0]);
             __VERIFIER_assume(__cs_pc_cs[0] <= 6);
             main_thread();
          }

   return 0;
}

