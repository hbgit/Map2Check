#!/usr/bin/env perl

#-----------------------------------------------------
# usage way: ./get_data_func_location.pl c_file
#-----------------------------------------------------

# Checking input args
$c_file = $ARGV[0];


#--------------------------------------------------------
# Get the name of the functions and the 
# line number where it begin
#--------------------------------------------------------
@functions_name=`ctags --sort=NO -x --c-kinds=f $c_file | grep -o "^[^ ]*"`;

#--------------------------------------------------------
# Get the number of the line where each function begin
#--------------------------------------------------------
$rec_list=`ctags --sort=NO -x --c-kinds=f $c_file | grep -o \"function[ ]*[0-9][^ ]*\"`;
@tmp = split("\n",$rec_list);
@functions_start_line_num=();

foreach(@tmp){
    
    if($_ =~ m/(.[^ ]+$)/){
        $rc_n = $1;
        $rc_n =~ s/[ ]+//g;
        push(@functions_start_line_num,$rc_n)
    }
}


for ($count_f=0; $count_f <= $#functions_name; $count_f++){
    
    chomp($functions_name[$count_f]);
    
    $getEndLineFunction=`awk \'NR > first && /^}$\/ { print NR; exit }\' first=$functions_start_line_num[$count_f] $ARGV[0]`;
    chomp($getEndLineFunction);
    
    print $functions_name[$count_f].";".($functions_start_line_num[$count_f]-1).";".($getEndLineFunction-1)."\n";
    
}

