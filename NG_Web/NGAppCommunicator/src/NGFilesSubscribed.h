/*
 * NGFilesSubscribed.h
 *
 *  Created on: 10/10/2015
 *      Author: danf
 */

#ifndef NGSUBSCRIBEDFILES_H_
#define NGSUBSCRIBEDFILES_H_

#include <string>
#include <vector>

class NGFilesSubscribed {

private:
	NGFilesSubscribed();
	virtual ~NGFilesSubscribed();

private:
	static NGFilesSubscribed* _Instance;
	std::vector<std::string>	arrFiles;

public:
	static NGFilesSubscribed* getInstance();
	bool hasFile(std::string hash);
	void getListToSubscribe(
			std::vector<std::string> *arrHashFiles,
			std::vector<std::string> *arrListToSubscribe
	);
	void addFilesSubscribed(std::vector<std::string> arrFilesSubscribed);
};

#endif /* NGSUBSCRIBEDFILES_H_ */
