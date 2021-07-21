/*
 * NGFilesSubscribed.cpp
 *
 *  Created on: 10/10/2015
 *      Author: danf
 */

#include "NGFilesSubscribed.h"
#include <dirent.h>
#include <string.h>
#include <iostream>

NGFilesSubscribed* NGFilesSubscribed::_Instance = NULL;

NGFilesSubscribed::NGFilesSubscribed()
{
}

NGFilesSubscribed::~NGFilesSubscribed() {
	delete _Instance;
}

NGFilesSubscribed*
NGFilesSubscribed::getInstance(){
	if( _Instance == NULL ){
		_Instance = new NGFilesSubscribed();
	}
	return _Instance;
}

bool NGFilesSubscribed::hasFile(std::string hash) {
	for (unsigned int i = 0; i < arrFiles.size(); ++i) {
		if( arrFiles.at(i) == hash ){
			return true;
		}
	}
	return false;
}

void NGFilesSubscribed::getListToSubscribe(
		std::vector<std::string>* arrHashFiles,
		std::vector<std::string>* arrListToSubscribe) {
	for (unsigned int i = 0; i < arrHashFiles->size(); ++i) {
		/* Search for "arrHashFiles->at(i)" */
		if( !hasFile(arrHashFiles->at(i)) ){
			arrListToSubscribe->push_back(arrHashFiles->at(i));
		}
	}
}

void NGFilesSubscribed::addFilesSubscribed(
		std::vector<std::string> arrFilesSubscribed) {
	for (unsigned int i = 0; i < arrFilesSubscribed.size(); ++i) {
		/* Search for "arrHashFiles->at(i)" */
		if( !hasFile(arrFilesSubscribed.at(i)) ){
			arrFiles.push_back(arrFilesSubscribed.at(i));
		}
	}
}
