unsigned int __cs_active_thread[THREADS+1]={1};
unsigned int __cs_pc[THREADS+1];
unsigned int __cs_pc_cs[THREADS+1];
unsigned int __cs_thread_index;
unsigned int __cs_last_thread;
unsigned int __cs_thread_lines[] = {<insert-threadsizes-here>};

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
	__VERIFIER_assertext(*__cs_mutex_to_destroy!=0,"attempt to destroy an uninitialized mutex");
	__VERIFIER_assertext(*__cs_mutex_to_destroy!=-2,"attempt to destroy a previously destroyed mutex");
	__VERIFIER_assertext(*__cs_mutex_to_destroy==-1,"attempt to destroy a locked mutex");
	*__cs_mutex_to_destroy = -2;
	__CSEQ_message("lock destroyed");
	return 0;
}

int pthread_mutex_lock(cspthread_mutex_t *__cs_mutex_to_lock) {
	__VERIFIER_assertext(*__cs_mutex_to_lock!=0,"attempt to lock an uninitialized mutex");
	__VERIFIER_assertext(*__cs_mutex_to_lock!=-2,"attempt to lock a destroyed mutex");
	__VERIFIER_assume(*__cs_mutex_to_lock==-1);
	*__cs_mutex_to_lock = __cs_thread_index+1;
	__CSEQ_message("lock acquired");
	return 0;
}

int pthread_mutex_unlock(cspthread_mutex_t *__cs_mutex_to_unlock) {
	__VERIFIER_assertext(*__cs_mutex_to_unlock!=0,"attempt to unlock an uninitialized mutex");
	__VERIFIER_assertext(*__cs_mutex_to_unlock!=-2,"attempt to unlock a destroyed mutex");
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
	__VERIFIER_assertext(*__cs_cond_to_wait_for!=0,"attempt to use an uninitialized conditional variable");
	__VERIFIER_assertext(*__cs_cond_to_wait_for!=-2,"attempt to use a destroyed conditional variable");
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
	__VERIFIER_assertext(count > 0, "count must be greater than 0");
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
	__VERIFIER_assertext(__cs_barrier_to_wait->init > 0, "attempt to use an uninitialized barrier variable");
	__cs_barrier_to_wait->current--;
	return 0;
}

int pthread_barrier_wait_2(cspthread_barrier_t *__cs_barrier_to_wait){
	__VERIFIER_assertext(__cs_barrier_to_wait->init > 0, "attempt to use an uninitialized barrier variable");
	__VERIFIER_assume(__cs_barrier_to_wait->current == 0);
	__cs_barrier_to_wait->current = __cs_barrier_to_wait->init;
	return 0;
}

pthread_key_create(cspthread_key_t *key, void (*destructor)(void*)) {
	static int currentkey = 0;
	__VERIFIER_assertext(currentkey < 1, "multiple thread-specific keys not supported");
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


typedef int __cs_attr_t;

struct cssched_param {
   int sched_priority;
};

int   __cs_attr_init(__cs_attr_t *) { return 0;}
int   __cs_attr_destroy(__cs_attr_t *) { return 0;}
int   __cs_attr_getdetachstate(const __cs_attr_t *, int * s) { return 0;}
int   __cs_attr_getguardsize(const __cs_attr_t *, unsigned int * s) { return 0;}
int   __cs_attr_getinheritsched(const __cs_attr_t *, int * s) { return 0;}
int   __cs_attr_getschedparam(const __cs_attr_t *, struct sched_param * s) { return 0;}
int   __cs_attr_getschedpolicy(const __cs_attr_t *, int * s) { return 0;}
int   __cs_attr_getscope(const __cs_attr_t *, int * s) { return 0;}
int   __cs_attr_getstackaddr(const __cs_attr_t *, void ** s) { return 0;}
int   __cs_attr_getstacksize(const __cs_attr_t *, unsigned int *s) { return 0;}
int   __cs_attr_setdetachstate(__cs_attr_t *, int s) { return 0;}
int   __cs_attr_setguardsize(__cs_attr_t *, unsigned int s) { return 0;}
int   __cs_attr_setinheritsched(__cs_attr_t *, int s) { return 0;}
int   __cs_attr_setschedparam(__cs_attr_t *, const struct sched_param * s) { return 0;}
int   __cs_attr_setschedpolicy(__cs_attr_t *, int s) { return 0;}
int   __cs_attr_setscope(__cs_attr_t *, int s) { return 0;}
int   __cs_attr_setstackaddr(__cs_attr_t *, void * s) { return 0;}
int   __cs_attr_setstacksize(__cs_attr_t *, unsigned int s) { return 0;}

void __CSEQ_noop(void) {};

