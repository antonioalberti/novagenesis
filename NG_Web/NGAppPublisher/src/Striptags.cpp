/*
 * Striptags.cpp
 *
 *  Created on: 29/03/2015
 *      Author: danf
 */

#include "Striptags.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Striptags::Striptags()
	:arrWords(NULL)
{

}

Striptags::~Striptags(){
	delete arrWords;
	arrWords = NULL;
}

bool Striptags::makeStriptags(string html_file){
	char path[BUFF_SIZE];
	string exec_strip(PHP_STRIP);
	exec_strip += html_file;
	FILE *fp = popen(exec_strip.c_str(), "r");
	if (fp == NULL) {
		cout<<"Failed to run command\n";
		return false;
	}
	if( arrWords != NULL ){
		delete arrWords;
		arrWords = NULL;
	}
	arrWords = new vector<string>;
	/* Read the output a line at a time - output it. */
	while (fgets(path, sizeof(path) - 1, fp) != NULL) {
		int l = strlen(path);
		path[l-1] = '\0';
		arrWords->push_back(path);
	}

	/* close */
	pclose(fp);

	return true;
}

unsigned Striptags::getSize(){
	return arrWords != NULL ? arrWords->size() : 0;
}

string Striptags::getWord(unsigned i){
	if( arrWords != NULL && i < arrWords->size() ){
		return arrWords->at(i);
	} else {
		return "";
	}
}


