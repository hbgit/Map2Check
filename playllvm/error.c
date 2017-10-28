extern void __VERIFIER_error() __attribute__ ((__noreturn__));

int main(){
	
	int a = 1;
	int b = 3;
	
	if(a > b){
		__VERIFIER_error();
	}else{
		b++;
	}
	
	return 0;
}
