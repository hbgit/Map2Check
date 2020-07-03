#ifndef SMACK_H_
#define SMACK_H_

#define __VERIFIER_assume __SMACK_assume
#define __VERIFIER_assert __SMACK_assert


void __SMACK_assert(int val);
void __SMACK_assume(int val);
int __SMACK_nondet(void);

#endif /*SMACK_H_*/
