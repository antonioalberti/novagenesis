#!/usr/bin/php
<?php

$fstr = $argv[1];

// ->> Ignora tamanho igual ou menor que $len
$len = 2;

// ->> Ignora (Não Exibe) as palavras inseridas
$ignore = array(
// "em","por","que",
"-","_","–"
);

// ->> Adiciona a ocorrência caso encontre
$accept = array(
// "dia","ano"
);

// ->> Remove todos os caracteres do texto
$remove = array("\n",".",",",";",'"',"'",")","(","]","[","{","}");

if(file_exists($fstr)) {
  ob_start();
  include($fstr);
  $content = ob_get_contents();
  ob_end_clean();
  $text = preg_replace('/(?:<|&lt;)\/?([a-zA-Z]+) *[^<\/]*?(?:>|&gt;)/', '', $content);
  $arr = array("\n",".",",",".",";");
  $text = str_replace($remove," ",$text);
  $arrWords = explode(" ",$text);
  foreach( $arrWords as $word ){
    
	if( !empty($word) ){
		$lword = trim(strtolower($word));
		if( ( strlen($lword) > $len && !in_array($lword,$ignore) ) || in_array($lword,$accept) )
			echo "$lword\n";
	}
  }
}

?>
