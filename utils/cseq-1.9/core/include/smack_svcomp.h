typedef int s8;
typedef int u8;
typedef int s16;
typedef int u16;
typedef int s32;
typedef int u32;

typedef struct {
    int init;
    int lock;
} spinlock_t;

void spin_lock_init(spinlock_t *lock)
{
  lock->init = 1;
  lock->lock = 0;
}

void spin_lock(spinlock_t *lock)
{
  pthread_t tid = pthread_self();

  __VERIFIER_atomic_begin();
  __VERIFIER_assume(lock->lock == 0);
  lock->lock = tid;
  __VERIFIER_atomic_end();
}

void spin_lock_irqsave(spinlock_t *lock, unsigned long value)
{
  pthread_t tid = pthread_self();

  __VERIFIER_atomic_begin();
  __VERIFIER_assume(lock->lock == 0);
  lock->lock = tid;
  __VERIFIER_atomic_end();
}

void spin_lock_irq(spinlock_t *lock)
{
  pthread_t tid = pthread_self();

  __VERIFIER_atomic_begin();
  __VERIFIER_assume(lock->lock == 0);
  lock->lock = tid;
  __VERIFIER_atomic_end();
}

void spin_lock_bh(spinlock_t *lock)
{
  pthread_t tid = pthread_self();

  __VERIFIER_atomic_begin();
  __VERIFIER_assume(lock->lock == 0);
  lock->lock = tid;
  __VERIFIER_atomic_end();
}

void spin_unlock(spinlock_t *lock)
{
  // pthread_t tid = pthread_self();

  __VERIFIER_atomic_begin();
  lock->lock = 0;
  __VERIFIER_atomic_end();
}

void spin_unlock_irqrestore(spinlock_t *lock, unsigned long value)
{
  // pthread_t tid = pthread_self();

  __VERIFIER_atomic_begin();
  lock->lock = 0;
  __VERIFIER_atomic_end();
}

void spin_unlock_irq(spinlock_t *lock)
{
  // pthread_t tid = pthread_self();

  __VERIFIER_atomic_begin();
  lock->lock = 0;
  __VERIFIER_atomic_end();
}

void spin_unlock_bh(spinlock_t *lock)
{
  // pthread_t tid = pthread_self();

  __VERIFIER_atomic_begin();
  lock->lock = 0;
  __VERIFIER_atomic_end();
}


struct mutex {
    int init;
    int locked;
};

void mutex_init(struct mutex *lock)
{
  lock->locked = 0;
  lock->init = 1;
}

void mutex_lock(struct mutex *lock)
{
  pthread_t tid = pthread_self();

  __VERIFIER_atomic_begin();
  __VERIFIER_assume(lock->locked == 0);
  lock->locked = tid;
  __VERIFIER_atomic_end();
}

bool mutex_lock_interruptible(struct mutex *lock)
{
  bool ret = __VERIFIER_nondet_bool();
  if(!ret) {
    pthread_t tid = pthread_self();

    __VERIFIER_atomic_begin();
    __VERIFIER_assume(lock->locked == 0);
    lock->locked = tid;
    __VERIFIER_atomic_end();
  }
  return ret;
}

void mutex_unlock(struct mutex *lock)
{
  __VERIFIER_atomic_begin();
  lock->locked = 0;
  __VERIFIER_atomic_end();
}

typedef struct poll_table_struct {
    int something;
} poll_table;

struct file_operations {
    struct module *owner;
    loff_t (*llseek) (struct file *, loff_t, int);
    ssize_t (*read) (struct file *, char *, size_t, loff_t *);
    ssize_t (*write) (struct file *, const char *, size_t, loff_t *);
    long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
    int (*open) (struct inode *, struct file *);
};

struct miscdevice {
    int minor;
    const char *name;
    struct file_operations *fops;
};

struct file {
    loff_t f_pos;
    void *private_data;
};

struct inode {
    int unused;
};

struct pci_dev {
    int unused;
};

struct platform_device {
    int dev;
};

struct vm_area_struct {
    int unused;
};

struct cx_dev {
    int unused;
};

struct nsc_gpio_ops {
    struct module* owner;
    u32 (*gpio_config) (unsigned iminor, u32 mask, u32 bits);
    void (*gpio_dump) (struct nsc_gpio_ops *amp, unsigned iminor);
    int (*gpio_get) (unsigned iminor);
    void (*gpio_set) (unsigned iminor, int state);
    void (*gpio_change) (unsigned iminor);
    int (*gpio_current) (unsigned iminor);
    struct device* dev;
};

extern ssize_t nsc_gpio_write(struct file *file, const char *data, size_t len, loff_t *ppos);
extern ssize_t nsc_gpio_read(struct file *file, char *buf, size_t len, loff_t *ppos);
extern void nsc_gpio_dump(struct nsc_gpio_ops *amp, unsigned index);
extern loff_t no_llseek(struct file *file, loff_t offset, int origin);
