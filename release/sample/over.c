extern void __VERIFIER_error(void);
extern int __VERIFIER_nondet_int(void);

main()
{
    int a = __VERIFIER_nondet_int();
    __VERIFIER_assume(a>0);
    int b = __VERIFIER_nondet_int();
    __VERIFIER_assume(b>0);

    if(a)
    {
        int c = a +b;
    }else{
        int c = b;
    }   

}
