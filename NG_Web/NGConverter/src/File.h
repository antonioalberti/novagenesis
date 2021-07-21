/*
	Cell Models Set v0.4 - SimATM v1.0 - ATM Network Simulator
	DECOM-FEEC-UNICAMP

	Model:		 File
	Object:		 File
	File:		 file.h
	Author:		 Antonio M. Alberti
	Date:	     08/1999
	Revised in:  11/1999
	Version:     0.3


	TOOL 3
	
	Name:		Block Process
	Object:		Process
	File:		process.h
	Author:		Antonio M. Alberti
	Date:		02/2007
	Version:	0.3

*/

#ifndef _FILE_H
#define _FILE_H

#ifndef _STRING_H
#include <string>
#endif

#ifndef _IOSTREAM_H
#include <iostream>
#endif

#ifndef _FSTREAM_H
#include <fstream>
#endif

#define ERROR 1
#define OK 0

using namespace std;

class File:	public fstream
{
	public:

	// Informacoes a respeito do arquivo associado
	string 		Name;
	string 		Option;
	string	 	Description;

	File();

	File(const File& F);

	File & operator=(const File &F);
   
	// Seta a descricao sobre o arquivo
	void SetName(string Name_){ Name=Name_; }
	void SetOption(string Option_){ Option=Option_; }
	void SetDescription(string Description_){ Description=Description_; }

	// Retorna as informacoes a respeito do arquivo associado
	void GetName(string &Name_) const { Name_=Name; }
	void GetOption(string &Option_) const{ Option_=Option; }
	void GetDescription(string &Description_) const { Description_=Description; }
	string GetName() const { return Name; }
	string GetOption() const { return  Option; }
	string GetDescription() const { return  Description; }

	// Abre um arquivo para escrita com opcoes definidas por option
	int OpenOutputFile(string Name_, string Option_);

	// Reabre um arquivo para escrita. Continua escrevendo. 
	int OpenOutputFile();

	// Reabre um arquivo para escrita. Inicia escrita do zero. 
	int OverwriteOutputFile();

	// Abre um arquivo para leitura com opcoes definidas por option
	int OpenInputFile(string Name_, string Option_);

	// Reabre um arquivo para leitura
	int OpenInputFile();

	// Fecha arquivo
	int CloseFile();
   
	// Verifica se o arquivo esta aberto
	int FileIsOpen();

	// Verifica se houve alguma falha apos alguma operacao com o arquivo
	int CheckForFileFail();

	// Verifica se chegou o fim de um arquivo
	int CheckForEndOfFile();
};

#endif
