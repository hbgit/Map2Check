extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_unsigned(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern char __VERIFIER_nondet_char(void);
extern unsigned char __VERIFIER_nondet_uchar(void);
extern short __VERIFIER_nondet_short(void);
extern unsigned short __VERIFIER_nondet_ushort(void);
extern long __VERIFIER_nondet_long(void);
extern unsigned long __VERIFIER_nondet_ulong(void);
extern _Bool __VERIFIER_nondet_bool(void);
extern void *__VERIFIER_nondet_pointer(void);
extern char *__VERIFIER_nondet_pchar(void);
extern unsigned long __VERIFIER_nondet_size_t(void);
extern long long __VERIFIER_nondet_loff_t(void);
extern unsigned long __VERIFIER_nondet_sector_t(void);
extern double __VERIFIER_nondet_double(void);
int main(void) {
  int v_int = __VERIFIER_nondet_int();
  unsigned v_unsigned = __VERIFIER_nondet_unsigned();
  unsigned v_uint = __VERIFIER_nondet_uint();
  char v_char = __VERIFIER_nondet_char();
  unsigned char v_uchar = __VERIFIER_nondet_uchar();
  short v_short = __VERIFIER_nondet_short();
  unsigned short v_ushort = __VERIFIER_nondet_ushort();
  long v_long = __VERIFIER_nondet_long();
  unsigned long v_ulong = __VERIFIER_nondet_ulong();
  _Bool v_bool = __VERIFIER_nondet_bool();
  void *v_pointer = __VERIFIER_nondet_pointer();
  char *v_pchar = __VERIFIER_nondet_pchar();
  unsigned long v_size_t = __VERIFIER_nondet_size_t();
  long long v_loff_t = __VERIFIER_nondet_loff_t();
  unsigned long v_sector_t = __VERIFIER_nondet_sector_t();
  double v_double = __VERIFIER_nondet_double();
  return (v_int + (int)v_unsigned + (int)v_uint + v_char + v_uchar + v_short +
          v_ushort + (int)v_long + (int)v_ulong + v_bool + (v_pointer != 0) +
          (v_pchar != 0) + (int)v_size_t + (int)v_loff_t + (int)v_sector_t +
          (int)v_double) & 1;
}
