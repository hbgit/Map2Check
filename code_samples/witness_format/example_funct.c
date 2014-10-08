extern void __VERIFIER_error(void);                                             

void foo(){
	float de = 0.0;
}

int failure() {
	int c;
	int r;
	foo();    
    return 1;
}

int main() {
    int a = 1;
    if(failure()){    
		int k;
		__VERIFIER_error();
	}
}  
