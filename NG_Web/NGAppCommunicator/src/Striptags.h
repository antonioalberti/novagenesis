/*
 * Striptags.h
 *
 *  Created on: 29/03/2015
 *      Author: danf
 */

#ifndef STRIPTAGS_H_
#define STRIPTAGS_H_

#include <vector>
#include <string>

using namespace std;

#define PHP_STRIP "/home/danf/test/regex/strip.php "
#define BUFF_SIZE 1035

class Striptags {
public:
	Striptags();
	virtual ~Striptags();
	bool makeStriptags(string html_file);
	string getWord(unsigned i);
	unsigned getSize();
private:
	vector<string> *arrWords;
};

#endif /* STRIPTAGS_H_ */
