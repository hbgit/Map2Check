main()
{
            int i = 0;
                    int s = 0;        
                            int Z = __VERIFIER_nondet_int();
                                    __VERIFIER_assume(Z >= 0);
                                            
                                            while(i < Z)
                                                        {
                                                                            s = i + 1;
                                                                                            i++;
                                                                                                    }
                                                    __VERIFIER_assert(s == i);
}
