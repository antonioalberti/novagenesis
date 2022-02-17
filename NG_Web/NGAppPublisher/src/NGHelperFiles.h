/*
 * NGHelperFiles.h
 *
 *  Created on: 04/10/2015
 *      Author: danf
 */

#ifndef NGHELPERFILES_H_
#define NGHELPERFILES_H_

#include <string>
#include <vector>

#define SUFFIX_WORDLIST		"_wordlist"

class NGHelperFiles {

private:
	std::string filename;
	std::vector<std::string> arrWordlist;
	int index;
public:
	NGHelperFiles(std::string path,std::string file);
	std::string getFilename();
	bool hasWord();
	std::string getWord();
	virtual ~NGHelperFiles();
};

#endif /* NGHELPERFILES_H_ */
