/*
 * NGHelperFiles.cpp
 *
 *  Created on: 04/10/2015
 *      Author: danf
 */

#include "NGHelperFiles.h"

#include <iostream>

#include <fstream>      // std::ifstream

NGHelperFiles::NGHelperFiles(std::string path,std::string file):
	filename(file),
	index(-1)
{
	std::ifstream ifs ((path+"/"+filename+SUFFIX_WORDLIST).c_str(), std::ifstream::in);
	if( ifs.is_open() ){
		arrWordlist.clear();
		std::string word;
		while(!ifs.eof()){
			ifs >> word;
			arrWordlist.push_back(word);
		}
	}
	ifs.close();
}

NGHelperFiles::~NGHelperFiles()
{

}

std::string
NGHelperFiles::getFilename()
{
	return filename;
}

bool
NGHelperFiles::hasWord()
{
	if( index < (int)arrWordlist.size()-1 ){
		index++;
		return true;
	}
	return false;
}

std::string
NGHelperFiles::getWord()
{
	return arrWordlist.at(index);
}


