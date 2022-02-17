/*
 * NGFiles.cpp
 *
 *  Created on: 03/10/2015
 *      Author: danf
 */

#include "NGFiles.h"
#include <dirent.h>
#include <string.h>
#include <iostream>

NGFilesSubscribed* NGFilesSubscribed::_Instance = NULL;

NGFilesSubscribed::NGFilesSubscribed(std::string path):
	index(-1)
{
	const char* ignore = "_wordlist";
	int lenign = strlen(ignore);
	int lenfile;

	struct dirent *dp;
	DIR * dirp = opendir(path.c_str());
	while ((dp = readdir(dirp)) != NULL){
		lenfile = strlen(dp->d_name);
		if( (lenfile-lenign) > 0 ){
			int pos = lenfile-lenign;
			char* suffix = &dp->d_name[pos];
			if( strcmp(suffix,ignore) != 0){
				arrFiles.push_back(dp->d_name);
			}
		}
	}
	closedir(dirp);
}

NGFilesSubscribed::~NGFilesSubscribed() {
	delete _Instance;
}

NGFilesSubscribed*
NGFilesSubscribed::getInstance(std::string path){
	if( _Instance == NULL ){
		_Instance = new NGFilesSubscribed(path);
	}
	return _Instance;
}

std::string
NGFilesSubscribed::getFile(){
	return arrFiles.at(index);
}

bool
NGFilesSubscribed::next(){
	if( index < (int)arrFiles.size()-1 ){
		index++;
		return true;
	}
	return false;
}

