/*
 * NGFiles.h
 *
 *  Created on: 03/10/2015
 *      Author: danf
 */

#ifndef NGFILES_H_
#define NGFILES_H_

#include <string>
#include <vector>

#define NGFILES_PATH "/home/william/newWorkspace/novagenesis/NG_Web/HtmlToHash/SiteFiles/NGFiles/"

class NGFilesSubscribed {

private:
	NGFilesSubscribed(std::string path);
	virtual ~NGFilesSubscribed();

private:
	static NGFilesSubscribed* _Instance;
	std::vector<std::string>	arrFiles;
	int index;
	std::string path;

public:
	static NGFilesSubscribed* getInstance(std::string path);
	std::string getFile();
	bool next();
};

#endif /* NGFILES_H_ */
