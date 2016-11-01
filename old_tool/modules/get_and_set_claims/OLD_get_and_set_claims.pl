#!/usr/bin/env perl
#---------------------------------------------------------------
# get_and_set_claims.pl module: Gathering claims in the file.csv
# and insert the claims with assert in the code source.
# Author: Herbert O. Rocha E-mail: herberthb12@gmail.com
# Version: 2 - Year: 2012
# License: GPL V3
#---------------------------------------------------------------

#dir resutl_claims
#$dir_result_claims = "result_claims/".$ARGV[0];
$dir_result_claims = $ARGV[0];

#Set flag to asserts
$flag_assert = 0;

#Path where the source code it is
$dir_c_code = $ARGV[1];

#checking if there is claims to apply
$if_blank=`cat $dir_result_claims | wc -l`;

if($if_blank == 0){	
	print 1;
	exit;
}


#Reading the file.csv with gathered data from results of the claims
open(ENTRADA , "<$dir_result_claims") or die "Could not open for reading the file.csv: $!";
		
while (<ENTRADA>) { # assigning to the variable $_ one line per time
	push(@LinhasFile_abs,$_);
}

close ENTRADA;


# Getting the name of the C file and reading the C source code
#already pre-processing
$name_c_code="";
@linhas_c_code="";
if($ARGV[0] =~ m/(^.[^.]*)/){
	$name_c_code=$1;
	
	#PATH of the C source code	
	$path_c_code = $dir_c_code;
		
	open(C_CODE , "<$path_c_code") or die "Could not open for reading the file.c: $!";
		
	while (<C_CODE>) { 
		#checking if there is the library <assert.h>		
		if($_ =~ m/(<assert.h>)/){
			#flag to set that already there is the library <assert.h>
			$flag_assert = 1;
		}
		if($_ =~ m/(<stdio.h>)/){
			#flag to set that already there is the library <stdio.h>
			$flag_stdio = 1;
		}
		push(@linhas_c_code,$_);
	}
	close C_CODE;	 	
}	

# Generating the new source C code, applying the CUnit model and insert the asserts
$new_name_code_path = $ARGV[2];

#*** Open file ***
open(NEW_FILEC , ">$new_name_code_path") or die "Could not open for reading the new new_file.c: $!";
$size_new_file_inst = @linhas_c_code;

# Reading the vector with the lines from file.abs that has the properties
# gathering from claims

# Auxiliary list adopting to receives the Nº CLAIMS checked that will be insert in the code
@rec_ns_claims_verified=();
# Auxiliary list to comments
@rec_claims_Coments_verified=();
# Auxiliary list to property
@rec_claims_properties_verified=();
# Auxiliary list to line number of the claim
@rec_claims_line_n_verified=();

$size_line_file_abs = @LinhasFile_abs;

for ($i=0; $i <= $size_line_file_abs; $i++) {	
	@rec_each_line = split(/;/,$LinhasFile_abs[$i]);
	
	# Checking if there is directives to apply boolean expressions in the claims		
	if($rec_each_line[-1] =~ m/(.*FALSE.*|.*TRUE.*)/g){
		# checking is in the source code
		$get_flag_assert=$1;
		if($get_flag_assert =~ m/(.*FALSE.*)/){			
			#print "assert(0)";
			# assigning to correct varibles or lists			
			push(@rec_claims_line_n_verified,$rec_each_line[0]);
			push(@rec_ns_claims_verified,$rec_each_line[1]);
			push(@rec_claims_Coments_verified,$rec_each_line[2]);
			push(@rec_claims_properties_verified,"0");					
		}else{
			# assigning to correct varibles or lists			
			push(@rec_claims_line_n_verified,$rec_each_line[0]);
			push(@rec_ns_claims_verified,$rec_each_line[1]);
			push(@rec_claims_Coments_verified,$rec_each_line[2]);
			push(@rec_claims_properties_verified,"1");
		}		
	}
	# Checking properties that "still" do not applied is this FORTES versions
	# so jumping those	
	elsif($rec_each_line[-1] =~ m/(!tmp\$.|SAME-OBJECT|INVALID-POINTER|POINTER_OFFSET)/){		
		next;		
	}
	else{
		# valid properties to apply in assertion
		# assigning to correct varibles or lists			
		push(@rec_claims_line_n_verified,$rec_each_line[0]);
		push(@rec_ns_claims_verified,$rec_each_line[1]);
		push(@rec_claims_Coments_verified,$rec_each_line[2]);
		push(@rec_claims_properties_verified,$rec_each_line[3]);	
		
		
	}
	
}

# Handling source code to insert the assertions
# Run the source code to count and to read the lines

# Counter for sum to control the lines increment
#i=0 -> Sn=(i=0+1)+(i=1+1)+(i=2+1), always an increment more for analised line
$sn_i=0;

#-------------------------------------------------------
#checking BASIC LIBRARIES TO RUN CODE

#checking if there is the library <assert.h>		
#if($flag_assert != 1){	
	#**********************************************************
	# Includes for FORTES
	#**********************************************************
	print NEW_FILEC "#include \"CUnit/Basic.h\" //-> by FORTES \n";					
	$sn_i=$sn_i+1;	
	# Now already there is an assertion
	#$flag_assert = 1;
	
#}

if($flag_stdio != 1){	
	#**********************************************************
	# Includes for FORTES
	#**********************************************************
	print NEW_FILEC "#include <stdio.h> //-> by FORTES \n";					
	# Now already there is an assertion
	$flag_stdio = 1;
	$sn_i=$sn_i-1;	
}

#-------------------------------------------------------


# Counter to list de line number
$cont_line=0;
$flag_func_cunit=0;	
for($cont=0;$cont<$size_new_file_inst; $cont++){				
			
	# Checking if the line was identified in the ESBMC verification and
	# is in the ".abs" file
	# the less 1 in ($num_line_claim-1) is beacause the vector to start in 0
	# i.e., 1-1=0			
	if($cont == ($rec_claims_line_n_verified[$cont_line]-1)+$sn_i){		
				
		# writing an assertion and the comments in the source code		
		chomp($rec_claims_properties_verified[$cont_line]);
		chomp($rec_ns_claims_verified[$cont_line]);
		
		# <to_do> apply the resume comments
		chomp($rec_claims_Coments_verified[$cont_line]);
		
		# remove the blank spaces
		$rec_claims_properties_verified[$cont_line]=trim($rec_claims_properties_verified[$cont_line]);
		
		#print NEW_FILEC "CU_ASSERT($rec_claims_properties_verified[$cont_line]); //-> $rec_ns_claims_verified[$cont_line]::$rec_claims_line_n_verified[$cont_line]->by FORTES \n";		
		print NEW_FILEC "CU_ASSERT($rec_claims_properties_verified[$cont_line]); //-> $rec_ns_claims_verified[$cont_line] from ESBMC -> by FORTES \n";		
				
		$cont_line = $cont_line+1;
		while($cont == ($rec_claims_line_n_verified[$cont_line]-1)+$sn_i){
			
			chomp($rec_claims_properties_verified[$cont_line]);
			chomp($rec_ns_claims_verified[$cont_line]);
					
			chomp($rec_claims_Coments_verified[$cont_line]);
					
			$rec_claims_properties_verified[$cont_line]=trim($rec_claims_properties_verified[$cont_line]);
					
			#print NEW_FILEC "CU_ASSERT($rec_claims_properties_verified[$cont_line]); //-> $rec_ns_claims_verified[$cont_line]::$rec_claims_line_n_verified[$cont_line]->by FORTES \n";			
			print NEW_FILEC "CU_ASSERT($rec_claims_properties_verified[$cont_line]); //-> $rec_ns_claims_verified[$cont_line] from ESBMC -> by FORTES \n";			
			
			$cont_line = $cont_line+1;			
			
		}		
		
	}
		
	
	#**********************************************************
	# Functions for CUnit with global variables - FORTES
	#**********************************************************
	#Checking the flag, comments and includes after to insert the functions
	#Here I believe that code has at least one include in the header file
	if($flag_func_cunit == 0 && !($linhas_c_code[$cont] =~ /(^\/|^#|^ |^\t|^$)/)){		
		
		print NEW_FILEC "/************************** FORTES ***************************/ \n
		/* Pointer to the file used by the tests. */
		static FILE* temp_file = NULL;
		/* The suite initialization function.
		* Opens the temporary file used by the tests.
		* Returns zero on success, non-zero otherwise.
		*/
		int init_suite1(void) {
			if (NULL == (temp_file = fopen(\"temp.txt\", \"w+\"))) {
				return -1;
			} else {
				return 0;
			}
		}

		/* The suite cleanup function.
		* Closes the temporary file used by the tests.
		* Returns zero on success, non-zero otherwise.
		*/
		int clean_suite1(void) {
			if (0 != fclose(temp_file)) {
				return -1;
			} else {
				temp_file = NULL;
				return 0;
			}
		}

		int argc_my;
		char **argv_my; 
		/************************** FORTES ***************************/
		/********Space of C code with test cases <- FORTES************/
		\n";
		
		#stop flag
		$flag_func_cunit=1;		
	}
	
	#**********************************************************
	# Update name main functions for testclaim on CUnit - FORTES
	#**********************************************************
	if( $linhas_c_code[$cont] =~ /main.*\(/ ){
		if(!($linhas_c_code[$cont] =~ /{/)){			
			$inter_cont=$cont;
			while(!($linhas_c_code[$inter_cont] =~ /{/)){
				#The old main functions has been commented here
				chomp($linhas_c_code[$inter_cont]);
				print NEW_FILEC "//".$linhas_c_code[$inter_cont]."-> by FORTES \n";
				$inter_cont=$inter_cont+1;
			}
			print NEW_FILEC "void testClaims(void){";
			$cont = $inter_cont+1;
		}
	}
	print NEW_FILEC $linhas_c_code[$cont];
			
}

#***************************************************************
# End test functions and insert the new main for CUnit - FORTES
#***************************************************************
print NEW_FILEC "/********* End of space of C code with test cases *********/ \n";
print NEW_FILEC "/* The main() function for setting up and running the tests.
* Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main(int argc, char *argv[3]) 
{
    CU_pSuite pSuite = NULL;
    argc_my = argc;		
	argv_my = argv;
	

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry */
    pSuite = CU_add_suite(\"log\", init_suite1, clean_suite1);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
    if ((NULL == CU_add_test(pSuite, \"testClaims\", testClaims))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
";

#*** Close the file ***
close(NEW_FILEC);

# The function that remove the blank spaces
sub trim {
  my $string = shift;
  for ($string) {
    s/^\s+//;
    s/\s+$//;
  }
  return $string;
}


