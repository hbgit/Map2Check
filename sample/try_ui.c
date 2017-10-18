main()
{
    unsigned a = __VERIFIER_nondet_uint();
    unsigned b = __VERIFIER_nondet_uint();

    __VERIFIER_assume(a<b);
    //__VERIFIER_assume(b>0);

    if(1 > 3)
    {
        __VERIFIER_error();
    }else if(a<b){
		a = a+b;
	}
}
