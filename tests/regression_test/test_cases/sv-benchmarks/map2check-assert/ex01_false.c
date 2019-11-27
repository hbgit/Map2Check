extern void __VERIFIER_error() __attribute__ ((__noreturn__));

void __VERIFIER_assert(int cond) {
  if (!(cond)) {
    ERROR: __VERIFIER_error();
  }
  return;
}

int main(void)
{
	int i=1;
	int n = __VERIFIER_nondet_int();

	while(i<n) {
		i++;
	}
	__VERIFIER_assert(i>n);
}
