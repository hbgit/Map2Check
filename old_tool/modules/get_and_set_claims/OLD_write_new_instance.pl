#!/usr/bin/env perl
use Switch;

#---------------------------------------------------------------
# Usage way: ./write_new_instance.pl < C_file > < data_functions_location > < csv_map_file.map > <csv_data_claims.csv>
# GOAL: 
#   Write a new instance of the analyzed C program. This way,
#   the tool gathers the claims in the csv_data_claims.csv
#   and insert the claims using asserts in the source, as well as,
#   the function from Map2Check library to tracking the memory and
#   help the tool to validate the asserts in the the execution.

# Author: Herbert O. Rocha 
# E-mail: herberthb12@gmail.com
# Version: 1 - Year: 2014
# License: GPL V3
#---------------------------------------------------------------

#---------------------------------------------------------------
### Gathering the input args

# Validate the args

if($#ARGV < 4 or $#ARGV > 4){
    print("Error. Sorry about that! \n");
    print("Usage way: \n");
    print("    ./write_new_instance.pl < C_file > < data_functions_location > < csv_map_file.map > <csv_data_claims.csv>");
    exit();
}


$c_program_path = $ARGV[0];
$file_data_loc_functions = $ARGV[1];
$file_map = $ARGV[2];
$file_data_claims = $ARGV[3];
$has_claims = $ARGV[4];
#---------------------------------------------------------------


#---------------------------------------------------------------
#
# Reading the source code to identify itens that will not be write again. 
# For this we create a list to save the number of the lines identified. 
# Additionally creating a list with the source code
$num_line = 0;
@DONT_write_line_number = ();
@list_c_program = ();
$flag_assert = 0;
$flag_stdlib = 0;
$flag_stdio = 0;
$flag_stdint = 0;

open(ENTRADA , "<$c_program_path") or die "Error. Could not possible to read the C file program";      
while (<ENTRADA>) {     
    
    if($_ =~ m/(<assert.h>)/){
        #flag to set that already there is the library <assert.h>
        $flag_assert = 1;
        push(@DONT_write_line_number,$num_line);
    }
    
    if($_ =~ m/(<stdio.h>)/){
        #flag to set that already there is the library <stdio.h>
        $flag_stdio = 1;               
        push(@DONT_write_line_number,$num_line);
    }
    
    if($_ =~ m/(<stdlib.h>)/){
        #flag to set that already there is the library <stdlib.h>
        $flag_stdlib = 1;
        push(@DONT_write_line_number,$num_line);
    }
    
    if($_ =~ m/(<stdint.h>)/){
        #flag to set that already there is the library <stdint.h>
        $flag_stdint = 1;
        push(@DONT_write_line_number,$num_line);
    }
    
    push(@list_c_program,$_);    
    $num_line++;
}
close ENTRADA;

#---------------------------------------------------------------


#---------------------------------------------------------------
# Reading and gathering the data LOC functions in the program
open(ENTRADA , "<$file_data_loc_functions") or die "Error. Could not possible to read the location data function file!";        
@name_function=();
@start_line_function=();
@end_line_function=();
while (<ENTRADA>) { 
    #index 0 -> name function; 1 - start line num func; 2 - end line num func
    @tmp=split(/;/,$_); 
    push(@name_function,$tmp[0]);
    push(@start_line_function,$tmp[1]); 
    push(@end_line_function,$tmp[2]);   
}
close ENTRADA;
#---------------------------------------------------------------


#---------------------------------------------------------------
#
# Gathering the data from map file and identify the global variables
# to help in the write process
@save_location_GB_VAR=();
$count=0;

open(ENTRADA , "<$file_map") or die "Error. Could not possible to read the CSV map file";      
@original_line_program=();
@line2map = ();
@var2map=();
@var_points_to2map=();
@id_func2map=();
@is_dynam2map=();
@is_free2map=();
@is_global2map=();

#ID scope
$count_ID = 0;
$save_last_scope = '';
while (<ENTRADA>) { 
    
    if($count > 0){         
        my @tmp=split(/;/,$_);
        push(@original_line_program,$tmp[0]);    
        push(@var2map,$tmp[1]);  
        push(@var_points_to2map, $tmp[2]);  
            
        # Convert name/function scope to
        # integers ID        
        if($tmp[3] eq "Global"){
            # ID = 0
            push(@id_func2map,0);
        }else{
            # ID >= 1
            if($save_last_scope eq $tmp[3]){
                push(@id_func2map,$count_ID);
            }else{
                $count_ID++;
                $save_last_scope = $tmp[3];
                push(@id_func2map,$count_ID);
            }
            
        }
        
        push(@is_dynam2map,$tmp[5]);        
        push(@is_free2map,$tmp[6]);
        
        chomp($tmp[7]);
        # Here we use -1 because the counter of the C program list start from 0, then 0 to up N-1
        push(@line2map,(int($tmp[7])-1));
        
        #Identifying the global variables        
        if($tmp[4] == 1){            
            # $count is the number of the csv row, in this case
            # -1 cuz the first row is the header
            # >>> checking if the line to be mapped is already
            # in a function scope
            #@start_line_function=();
            #@end_line_function=();
            # For each function
            $i = 0;
            $flag_count_funcs = 0;
            while($i <= $#start_line_function){
                if($tmp[7] >= $start_line_function[$i] and $tmp[7] <= $end_line_function[$i]){
                    $flag_count_funcs++;
                }
                $i++;
            }
            
            if($flag_count_funcs == 0){
                push(@save_location_GB_VAR,($count-1));
            }
            
        }
    }    
    
    $count++;
}
close ENTRADA;

#---------------------------------------------------------------

#---------------------------------------------------------------
# Reading the file_data_claims that has the data claims about the program
if($has_claims == 1){
    open(ENTRADA , "<$file_data_claims") or die "Error. Could not open for reading the claims data file!";

    @list_line_num_CL = ();
    @list_id_CL = ();
    @list_comments_CL = ();
    @list_property_CL = ();
    @list_original_line_CL = ();

    while (<ENTRADA>) { # assigning to the variable $_ one line per time
        
        my @tmp=split(/;/,$_);        
        
        push(@list_line_num_CL, (int(trim($tmp[0]))-1));            
        push(@list_id_CL, trim($tmp[1]));         
        push(@list_comments_CL, trim($tmp[2])); 
        
        #print "CL::::::::: ".trim($tmp[3])."\n";
        push(@list_property_CL, trim($tmp[3])); 
        push(@list_original_line_CL, trim($tmp[4]));
        
    }

    close ENTRADA;
}

#---------------------------------------------------------------


#######################################################################
##### => Write the new instance of the analyzed C source code
#######################################################################
$index2map = 0;
$sizeofFunc = $#name_function;
$cont_funcs = 0;
$count_CL = 0;
$tmp_count_line = ();
$sizeofl2map = $#line2map;
$flag_already_line = 0; # this is identify if a line code is mark to map and we already print this line
$flag_check_library = 1;
$flag_main=0;
$flag_CL_added=0;
$flag_add_main_leak_assert = 0;


for ($count_i=0; $count_i <= $#list_c_program; $count_i++){
     
    
    #----------------------------------------------
    #### Write the header of the source code
    #----------------------------------------------
    if($flag_check_library == 1){
        if($flag_assert == 0){         
            print "#include <assert.h> /** by FORTES **/ \n";        
        }     
        if($flag_stdlib == 0){ 
            print "#include <stdlib.h> /** by FORTES **/ \n";        
        }
        if($flag_stdio == 0){ 
            print "#include <stdio.h> /** by FORTES **/ \n";        
        }
        if($flag_stdint == 0){ 
            print "#include <stdint.h> /** by FORTES **/ \n";        
        }   

        print "#include \"check_safety_memory_FORTES.h\" /** by FORTES **/ \n";
        $flag_check_library = 0;
    }
    
    #----------------------------------------------
    #### Starting to write the body of the code
    #----------------------------------------------
    if( grep { $_ eq $count_i} @DONT_write_line_number   ){
        $flag_already_line = 0;     
    }
    
    #----------------------------------------------
    # >>> Identify the scopes of the code    
    #----------------------------------------------
    # Identify if the line is in the functions
    $flag_find_delimiter_func = 0;      
    if ( grep { $_ eq $count_i} @start_line_function ) {
        
        #print "IS A FUNCTION ---------------------------------------- \n";
        
        #print "Writing function: ".$name_function[$cont_funcs]."\n";
        #print "End of this function: ".$end_line_function[$cont_funcs]."\n";
        # Save header line number
        $save_count_code = $count_i;            
                
        #check if the functions is the main function
        #to set the flag to allow write the marks code to global variable       
        if($name_function[$cont_funcs] eq "main" and $flag_main == 0){
            $flag_main=1;
            #print "======================================================> MAIN \n"
        }        
        
        #salva o número das linhas quando a declaração da função for do tipo 2 
        @save_num_fun_M2=(); 
        while($flag_find_delimiter_func == 0){            
            
            if($list_c_program[$count_i] =~ m/^[{]+/){
                $flag_find_delimiter_func = 1;
                                
                # >>> print block delimiter                
                print $list_c_program[$count_i];
                $count_i++;                
                
                #Print the marks to map global variables
                if($flag_main == 1 and (scalar(@save_location_GB_VAR)) >= 1){
                    print "\n";                 
                    print "MAP_GB_VAR_FORTES(); /** by FORTES **/ \n";                                      
                    print "\n";
                }
                        
                    
                # >>> Now print the marks to args function variables, e.g., fun(int a, int b){
                # and func( int a,
                #           int b)
                #     {                 
                if($index2map <= $sizeofl2map and $line2map[$index2map] == $save_count_code){  
                    
                    #print "$index2map <= $sizeofl2map and $line2map[$index2map] == $save_count_code \n";
                    #print "IF - LINE::::::::::::: ".$save_count_code."\n"; 
                    #print "IF - LINE::::::::::::: ".$count_i."\n";
                    
                    while($line2map[$index2map] == $save_count_code){
                        print "list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(".$var2map[$index2map]."),(void *)(intptr_t)(".$var_points_to2map[$index2map]."),".$id_func2map[$index2map].",".$is_dynam2map[$index2map].",".$is_free2map[$index2map].",".$original_line_program[$index2map]."); /** by FORTES **/ \n";                   
                        $index2map++;                        
                    }
                    
                    # In case if the header has multiple var decl
                    $save_count_code++;
                    while($save_count_code <= $count_i and $line2map[$index2map] == $save_count_code){
                        
                        print "list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(".$var2map[$index2map]."),(void *)(intptr_t)(".$var_points_to2map[$index2map]."),".$id_func2map[$index2map].",".$is_dynam2map[$index2map].",".$is_free2map[$index2map].",".$original_line_program[$index2map]."); /** by FORTES **/ \n";                   
                        $index2map++;
                        $save_count_code++;
                        
                    }
                    
                    
                }  
                
                
                              
                #Para quando a declaração das variaveis da função estiver neste modelo func(a,b) int a; int b{
                elsif($index2map <= $sizeofl2map and grep { $_ eq $line2map[$index2map]} @save_num_fun_M2){                  
                    #print "NEW --------------- \n";
                    print "list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(".$var2map[$index2map]."),(void *)(intptr_t)(".$var_points_to2map[$index2map]."),".$id_func2map[$index2map].",".$is_dynam2map[$index2map].",".$is_free2map[$index2map].",".$original_line_program[$index2map]."); /** by FORTES **/ \n"; 
                    $index2map++;
                    
                    $flag_try_kth1=0;
                    while($index2map <= $sizeofl2map and $flag_try_kth1 == 0){
                        
                        if(grep { $_ eq $line2map[$index2map]} @save_num_fun_M2){
                            print "list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(".$var2map[$index2map]."),(void *)(intptr_t)(".$var_points_to2map[$index2map]."),".$id_func2map[$index2map].",".$is_dynam2map[$index2map].",".$is_free2map[$index2map].",".$original_line_program[$index2map]."); /** by FORTES **/ \n";                   
                            $index2map++;
                        }else{
                            $flag_try_kth1=1;
                        }
                        
                    }
                }
                
                #-----------------------------------------------------------------------
                # >>> The body of the function from this point                
                #-----------------------------------------------------------------------
                while($count_i <= $end_line_function[$cont_funcs]){
                    
                    #>>> Insert the assertion with the esbmc claim translated
                    #if($has_claims = 1){
                        
                        #while( $list_line_num_CL[$count_CL] == $count_i ){
                            #print "// FORTES: ".$list_id_CL[$count_CL]."\n";
                            #@tmp_comments = split(/\?/,$list_comments_CL[$count_CL]);
                            #$msg_assert = ", \"[Failed]\\n VIOLATED PROPERTY: ".$list_id_CL[$count_CL]." \\n \\t Location at original code in line: ".$list_original_line_CL[$count_CL]." \\n \\t Comments: ".trim($tmp_comments[2])."\"";                            
                            #print "__MAP_FORTES_assert(".$list_property_CL[$count_CL]." ".$msg_assert."); \n";                        
                            #$count_CL++;                        
                        #}
                        
                    #}
                                        
                    
                    # >>> Insert assertion from Map2Check library related to INVALID FREE
                    ## FOR FUTURE add a Trigger by option                                                                        
                    if($is_free2map[$index2map] == 1 and $line2map[$index2map] == $count_i) {                                                                
                        print "assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(".$var_points_to2map[$index2map]."),".$original_line_program[$index2map].")); /** by FORTES **/\n";                                                                
                    }
                    
                    
                    
                    #if($flag_already_line == 0){                            
                            
                    # WORKING
                    
                    print $list_line_num_CL[$count_CL]." == ".$count_i."\n";
                    
                    while($list_line_num_CL[$count_CL] == $count_i) {
                        #print "out-CLAIM ::: ".$list_id_CL[$count_CL]." \n";
                        print "// FORTES: ".$list_id_CL[$count_CL]."\n";
                        @tmp_comments = split(/\?/,$list_comments_CL[$count_CL]);
                        $msg_assert = ", \"[Failed]\\n VIOLATED PROPERTY: ".$list_id_CL[$count_CL]." \\n \\t Location at original code in line: ".$list_original_line_CL[$count_CL]." \\n \\t Comments: ".trim($tmp_comments[2])."\"";                            
                        print "__MAP_FORTES_assert(".$list_property_CL[$count_CL]." ".$msg_assert."); \n";                        
                        $count_CL++;
                    }
                    
                    
                    # >>> Here we identify a line to map and set the claims 
                    @tmp_lines_CL = () ;
                    @tmp_lines_Code = () ;
                    $flag_while_map = 0; 
                    $tmp_count_CL = $count_CL;
                    
                    push(@tmp_lines_Code, $count_i);
                    while(not($list_c_program[$count_i] =~ m/;$/)){
                        if($list_line_num_CL[$tmp_count_CL] == $count_i) {
                            push(@tmp_lines_CL, $count_i);
                            $tmp_count_CL++;
                        }                                
                        $count_i++; 
                        push(@tmp_lines_Code, $count_i);
                        $flag_while_map = 1;                                                                
                    }
                    
                    if($flag_while_map == 1){
                        foreach(@tmp_lines_CL){
                            while($list_line_num_CL[$count_CL] == $_) {
                                #print "CLAIM ::: ".$list_id_CL[$count_CL]." \n";
                                print "// FORTES: ".$list_id_CL[$count_CL]."\n";
                                @tmp_comments = split(/\?/,$list_comments_CL[$count_CL]);
                                $msg_assert = ", \"[Failed]\\n VIOLATED PROPERTY: ".$list_id_CL[$count_CL]." \\n \\t Location at original code in line: ".$list_original_line_CL[$count_CL]." \\n \\t Comments: ".trim($tmp_comments[2])."\"";                            
                                print "__MAP_FORTES_assert(".$list_property_CL[$count_CL]." ".$msg_assert."); \n";                        
                                $count_CL++;
                            }                                                                        
                        }
                        
                        foreach(@tmp_lines_Code){
                            print "BF:: ".$list_c_program[$_];
                        }
                    }else{
                        print "BF:: ".$list_c_program[$count_i];
                    }
                        
                    #}
                    
                    
                    
                    
                    while($index2map <= $sizeofl2map and $line2map[$index2map] == $count_i){                        
                        
                        print "list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(".$var2map[$index2map]."),(void *)(intptr_t)(".$var_points_to2map[$index2map]."),".$id_func2map[$index2map].",".$is_dynam2map[$index2map].",".$is_free2map[$index2map].",".$original_line_program[$index2map]."); /** by FORTES **/ \n"; 
                        
                        $flag_already_line = 1;
                        $index2map++;
                    }                   
                    
                    
                    if($flag_already_line == 0){
                        
                        #-------------------------------------------------------------------------------
                        # >>> Insert assertion from Map2Check library related to MEMORY LEAK in the other function
                        # except in the "main"
                        #-------------------------------------------------------------------------------                        
                        ## FOR FUTURE add a Trigger by option  
                        if($list_c_program[$count_i] =~ m/\bexit\b|\babort\b/ and $flag_main == 0){
                            if( grep { $_ eq 1 } @is_dynam2map ){
                                print "assert(CHECK_MEMORY_LEAK(list_LOG_mcf, ".$id_func2map[($index2map-1)].", ".$original_line_program[($index2map-1)]." )); /** by FORTES **/ \n";
                            }
                        }
                        
                        #-------------------------------------------------------------------------------
                        # >>> Insert assertion from Map2Check library related to MEMORY LEAK in the main function
                        #-------------------------------------------------------------------------------
                        ## FOR FUTURE add a Trigger by option                                                                        
                        if( $count_i == $end_line_function[$cont_funcs] and $flag_main == 1 and $flag_add_main_leak_assert == 0 ){
                            if( grep { $_ eq 1 } @is_dynam2map ){
                                print "assert(CHECK_MEMORY_LEAK(list_LOG_mcf, ".$id_func2map[($index2map-1)].", ".$original_line_program[($index2map-1)]." )); /** by FORTES **/ \n";
                            }
                            $flag_add_main_leak_assert = 1;                            
                            $flag_main = 0;
                        }
                        elsif( $list_c_program[$count_i] =~ m/\breturn\b|\bexit\b|\babort\b/  and $flag_main == 1){
                            if( grep { $_ eq 1 } @is_dynam2map ){
                                print "assert(CHECK_MEMORY_LEAK(list_LOG_mcf, ".$id_func2map[($index2map-1)].", ".$original_line_program[($index2map-1)]." )); /** by FORTES **/ \n";
                            }
                            $flag_add_main_leak_assert = 1;  
                            $flag_main = 0;                          
                        }
                        
                        #print "B_FUNC: ".$list_c_program[$count_i];
                        #print $list_c_program[$count_i];
                        #$count_i++;
                        
                        
                        
                    }elsif($flag_already_line == 1){
                        # Here we continue to analyze the next lines
                        $count_i++;
                        $flag_already_line = 0;
                    }
                    
                }               
                                   
                            
            }else{
                                
                # save the line if we have this pattern func(a,b) int a; int b{ ???
                push(@save_num_fun_M2, $count_i);
                # >>> Print the header of the function                
                print $list_c_program[$count_i];
                $count_i++;         
                
            }
            
        }
        
        #index to function name, based on sequence top down
        $cont_funcs++;        
        
    }
    else{
        
        ## >>> Global scope of the code                
        while($index2map <= $sizeofl2map and $line2map[$index2map] == $count_i){            
            $flag_already_line = 1;
            $index2map++; 
        }
        
        #Just write the code without modification   
        if($flag_already_line == 0){
            # >>> print global lines that will not tracked
            #print "GB NOT_MP: ".$list_c_program[$count_i];            
            print $list_c_program[$count_i];            
        }else{
            $flag_already_line = 0; 
            #print "GB_MAP: ".$list_c_program[$count_i];
            print $list_c_program[$count_i];
        }
        
    }
    
}


###----------------------------------
# Write in the end of the file
# the function that will map all
# global variables
###----------------------------------

if((scalar(@save_location_GB_VAR)) >= 1){
    print "\n";
    print "/** Mapping global variables <- by FORTES**/ \n";
    print "MAP_GB_VAR_FORTES() \n";
    print "{ \n";
    
        
    foreach(@save_location_GB_VAR){     
        my $tmp_index = $_;     
        #print $tmp_index."\n";
        print "list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(".$var2map[$tmp_index]."),(void *)(intptr_t)(".$var_points_to2map[$tmp_index]."),".$id_func2map[$tmp_index].",".$is_dynam2map[$tmp_index].",".$is_free2map[$tmp_index].",".$original_line_program[$tmp_index]."); /** by FORTES **/ \n";
    }
    
    print "}\n";
    print "/** End mapping global variables <- by FORTES**/ \n";
    print "\n";
}


#######################################################################
##### => Utils functions
#######################################################################
#### Check if a line nu
sub is_a_function{
    my ($num_line) = @_;
    
    if ( grep { $_ eq $num_line} @start_line_function ) {
      return 1;
    }else{
        return 0;
    }
}

### Remove spaces from a string
sub trim{ 
    my $s = shift; 
    $s =~ s/^\s+|\s+$//g; 
    return $s; 
}


