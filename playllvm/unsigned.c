extern unsigned int __VERIFIER_nondet_uint(void);

main()
{
	unsigned int i = __VERIFIER_nondet_uint();
	__VERIFIER_assume(i>1);
	unsigned int v1 = i;
	__VERIFIER_assume(v1>1);
		
	v1 = v1 + 1U;
	/**if(v1 <= 0)
	{
		__VERIFIER_error();
	}**/
}
