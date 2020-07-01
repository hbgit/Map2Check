int main(void)
{
	int k=1;
	int i=1;
	int j=0;
	int n = __VERIFIER_nondet_int();
	
	while(i<n) {
		j=0;
		while(j<i) {
			k += (i-j);
			j++;
		}
		i++;
	}
	__VERIFIER_assert(k==0);
}
