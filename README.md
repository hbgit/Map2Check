<h1>Map2Check</h1>
<h3>Memory Management Test Cases Generation of C Programs</h3>

================ 

<b>Goal:</b> 
<p align="justify">
     Map2Check is a tool of automatically generating and checking memory management tests cases in C programs. 
     The generation of the tests cases are based on assertions (safety properties) from the code instructions, adopting the 
     LLVM framework and KLEE tool to generate input values to the test cases generated by Map2Check. 
     Map2Check checks memory safety properties, such as: invalid deference, invalid free, and memory leaks.
</p>

Documentation available at https://map2check.github.io

================

- Requirements for using the tool<br>
To use this tool is necessary a Linux OS system. Our binary tool is avaliable at https://github.com/hbgit/Map2Check/releases
 
================


<b>How to build Map2Check?</b>

<p align="justify">
You can build Map2Check using our dockerfile, more details at https://map2check.github.io/docker.html
</p>


<b>Install Map2Check</b>

<p align="justify">
In order to install Map2Check on your PC, you should download and save the map2check.zip file on your disk from https://github.com/hbgit/Map2Check/releases. 
After that, you should type the following command:
</p>

> $ unzip map2check.zip
> $ cd release

or from GitHub repository

> $ git clone https://github.com/hbgit/Map2Check.git

<b>Testing tool</b>

<p align="justify">
Map2Check can be invoked through a standard command-line interface. Map2Check should be called 
in the installation directory as follows:  
</p>

> $ ./map2check sample/sv-comp/960521-1_false-valid-free.c

For help and others options: 

> $ ./map2check -h

===========================

<b> Authors </b>

Maintainers:
  Herbert O. Rocha (since 2014), Federal University of Roraima, Brazil <br>
  Rafael Menezes   (since 2016), Federal University of Roraima, Brazil <br>

Questions and bug reports:  
  E-mail: map2check.tool@gmail.com

          .-.          
          /v\
         // \\    > L I N U X - GPL <
        /(   )\
         ^^-^^
         



