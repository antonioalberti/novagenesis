<?php

/* Bin that Get hash from file */
$ngconvert = "/home/william/newWorkspace/novagenesis/NG_Web/NGConverter/Debug/NGConverter";
/* Bin that Get wordlist from file */
$striptags = "/home/william/newWorkspace/novagenesis/NG_Web/StripTags/strip.php";
/* Input */
$htmlpath = "/home/william/newWorkspace/novagenesis/NG_Web/HtmlToHash/SiteFiles/HTMLFiles";
/* Output */
$hashpath = "/home/william/newWorkspace/novagenesis/NG_Web/HtmlToHash/SiteFiles/NGFiles";

function isHtml($folder,$file){
	return (strrpos("$folder/$file",".html") === (strlen("$folder/$file")-5));
}

function getHash($folder,$file){
	global $ngconvert;

	$hash = shell_exec("$ngconvert $folder/$file");
	return str_replace("\n","",$hash);
}

function getTitle($folder,$file){
	$title = shell_exec("grep -m1 -Po '<title>\K.*?(?=</title>)' $folder/$file");
	return str_replace("\n","",$title);
}

function getDescription($folder,$file){
	$description = shell_exec("grep -m1 -Po 'name=\"description\" content=\"\K.*?(?=\")' $folder/$file");
	return str_replace("\n","",$description);
}

function frename($folder,$filefrom,$fileto,$log){
	echo "$log: $folder/$filefrom => $fileto \n";
	shell_exec("mv $folder/$filefrom $folder/$fileto");
}

function buildMetaDescriptor($folder,$file){
	$title = getTitle($folder,$file);
	$description = getDescription($folder,$file);
	$metajson = array(
		'title' => $title,
		'description' => $description,
		'link' => $file
	);
	$json = json_encode($metajson);
	$metafile = "{$file}_metafile";
	shell_exec("echo '$json' > $folder/$metafile");
	return $metafile;
}

function buildWordList($folder,$file){
	global $striptags;

	$wordfile = "{$file}_wordlist";
	shell_exec("$striptags $folder/$file > $folder/$wordfile");
	return $wordfile;
}

function scandirHtmlLast($folder){
	$arrfile = scandir($folder);
	$arrOutput = array();
	foreach( $arrfile as $file ){
		if( is_file("$folder/$file") ){
			if( isHtml($folder,$file) ){
				array_push($arrOutput,$file);			
			} else {
				array_unshift($arrOutput,$file);
			}
		}
	}
	return $arrOutput;
}

function hashAllFiles($folder){
	$arrhash = array();
	/* Get all folders&files */
	$arrfile = scandirHtmlLast($folder);
	foreach($arrfile as $file){
		if( isHtml($folder,$file) ){
			foreach($arrhash as $file1 => $hash ){
				if( is_file("$folder/$hash") ){
					echo "replace: $file1 => $hash at $folder/$file\n";
					shell_exec("sed -i 's/$file1/$hash/g' $folder/*");
				}
			}			
		}
		$hashfile = getHash($folder,$file);
		frename($folder,$file,$hashfile,"index");
		$arrhash[$file] = $hashfile;
		if( isHtml($folder,$file) ){
			echo "building-descriptor: $hashfile($file)\n";
			/* create descriptor */
			$meta = buildMetaDescriptor($folder,$hashfile);
			$hashmeta = getHash($folder,$meta);
			frename($folder,$meta,$hashmeta,"index-meta");
			/* create wordlist */
			$word = buildWordList($folder,$hashfile);
			$hashword = "{$hashmeta}_wordlist";
			frename($folder,$word,$hashword,"index-wordlist");
		}
	}

	echo "moving: $folder/* => $folder/..\n";
	shell_exec("mv $folder/* $folder/..");
	echo "delete: $folder\n";
	shell_exec("rm -r $folder");
}

/* Copy all Input to Output */
shell_exec("cp -r $htmlpath/* $hashpath");

/* Get all folders&files */
$arrfolder = scandir($hashpath);

foreach( $arrfolder as $folder )
{
	if( is_dir("$hashpath/$folder")
		&& $folder != ".."
		&& $folder != ".")
	{
		hashAllFiles("$hashpath/$folder");
	}
}

?>
