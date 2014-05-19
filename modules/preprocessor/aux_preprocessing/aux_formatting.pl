#!/usr/bin/env perl

#processamento auxiliar de texto
#para declaração de variavel e inicialização simulateneo composto
#ex base: unsigned long int id_ch = 0, j, z = func(a,b,c);
#OBS: falta criar validação para quando não houver declaração composta do arquivo
#STATUS: NO OKAY

#validando entrada do arquivo

if($#ARGV != 0 ){
print "Sorry, you need a file.c -> ./aux_formatation <file.c> \n";
exit;
}

$pathfile = $ARGV[0];

#lendo cada arquivo.c já pre-processado
open(ENTRADA , "<$pathfile") or die "Nao foi possivel abrir o arquivo.c para leitura: $!";

while (<ENTRADA>) { # atribui à variável $_ uma linha de cada vez
	push(@LinhasFile,$_);
}
	
close ENTRADA;

#aplicando alterações e reescrevendo arquivo
#abrindo o novo code C para escrever e inserir a assertiva
open(NEW_FILEC , ">$pathfile") or die "Nao foi possivel abrir o novo arquivo.c: $!";

$size_linhas_file = @LinhasFile;

for($i=0;$i < $size_linhas_file; $i++){
	
	#lista para armazenar os resultados de função
	@list_func;
	$cont_func = 0;
	$mont_type = "";
	
	#buscando declaração de variavel e inicialização simulateneo composto
	#o padrão para identificar se é uma declaração multipla se seguir o modelo ex. int a, b;
	#ou seja a declaração do tipo e o ponto e virgula no final.
	
	#OBS: ainda falta tratar para exs. //unsigned long int id_ch = 2,3, j, z = func(y);
	#OBS: tratar para ele pular as linhas que iniciarem comentário
	
	
	#1º identificando se é declarado o tipo
	#aqui supeitamos que é uma variavel
	if($LinhasFile[$i] =~ m/int|char|float|double|unsigned|long|short/){
		
		#verificando se não é expressão tipo for, while, if
		#removendo os espaços da linha para a proxima verificação
		while($LinhasFile[$i] =~ m/([^[:blank:]])/g){
			$rec_string = $rec_string.$1;
		}
			
		#checando o ultimo simbolo no final da linha
		 if($rec_string =~ m/(.$)/){
			
			
		    if($1 eq ";"){
			
			#agora sabemos que é a declaração de uma variavel fora de uma estrutura tipo for, while, if
			
			#verificando se existe mais de 1 variavel sendo declarada
			#contador de variaveis declaradas
			$cont_var_dec = 0;
			while($rec_string =~ m/(,)/g){
				$cont_var_dec++;
			}
			
			#print $cont_var_dec."\n";
			
			
			if($cont_var_dec >= 1){
				#agora sabemos que é uma declaração composta
				#obtendo o tipo das variaveis
				while($LinhasFile[$i] =~ m/(int|char|float|double|unsigned|long|short)/g){
					$mont_type = $mont_type." ".$1;
				}
							
				#verificando se alguma das variaveis e inicializada com uma função ou expressão
				#working with ex. z = func(a,b,c)
			
				#falta tratar para exs. como:
				# -> a = 2(a-b) | a = 2(a-2,3) | a = 2,3 | a={1,2}
			
				if($LinhasFile[$i] =~ /[\(\)]/){
				
					#obter o conteudo o parenteses e seu conteudo
					while($LinhasFile[$i] =~ m/(\(.[^\(\)]*\))/g){
					
						$list_func[$cont_func] = $1;
						$cont_func++;						
					
						#subs
						#variavel de marcação
						$y = " @";
						$LinhasFile[$i] =~ s/\(.[^\(\)]*\)/$y/;																	
							
					}
					@list_split = split(/,/,$LinhasFile[$i]);										
					$rec_mont_line = mont_line(@list_split);
					
					#agora trocando os @s pelos seus respectivos valores
					$cont_subs = 0;
					
					while($rec_mont_line =~ m/(@)/g){
						$rec_mont_line =~ s/@/$list_func[$cont_subs]/;
						$cont_subs++;
					}
					
					print NEW_FILEC $rec_mont_line;
					
					#$LinhasFile[$i] = $rec_mont_line;
					$i++;
					
					@list_split = ();
				
				}else{
					@list_split = split(/,/,$LinhasFile[$i]);										
					print NEW_FILEC mont_line(@list_split);
					#$LinhasFile[$i] = $rec_mont_line;
					$i++;
					@list_split = ();
				
				}	
			}			
			
		}
	  }
	
		
	}
	$rec_string = "";	
	$mont_new_txt = "";	
	print NEW_FILEC $LinhasFile[$i];
}

#*** Close the file ***
close(NEW_FILEC);


#função de montagem da linha com \n
sub mont_line{

@list_split_vars = @_;

#montando as novas linhas e suas respectivas vars
$size_split = @list_split_vars;

for($cont_split = 0; $cont_split < $size_split; $cont_split++){
	
	if($cont_split != 0){
	
		if($list_split_vars[$cont_split] =~ m/(.[^;]+)/g){
			 $list_split_vars[$cont_split] = $mont_type." ".$1.";\n";
		}
		$mont_new_txt = $mont_new_txt." ".$list_split_vars[$cont_split];
		
	}else{
		$list_split_vars[$cont_split] = $list_split_vars[$cont_split]."; \n";
		$mont_new_txt = $mont_new_txt." ".$list_split_vars[$cont_split];
	}
	
	
}
return $mont_new_txt;

}

	

#foreach(@list_func){
#		print $_."\n";
#}




