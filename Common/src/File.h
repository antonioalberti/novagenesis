/*
	NovaGenesis

	Name:		Operating system file handler
	Object:		File
	File:		File.h
	Author:		Antonio Marcos Alberti
	Date:		05/2021
	Version:	0.1

  	Copyright (C) 2021  Antonio Marcos Alberti

    This work is available under the GNU Lesser General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

class File : public fstream {
 public:

  // Informacoes a respeito do arquivo associado
  string Name;
  string Path;
  string Option;
  string Description;

  File ();

  File (const File &F);

  File &operator= (const File &F);

  // Seta a descricao sobre o arquivo
  void SetName (string Name_)
  { Name = Name_; }
  void SetPath (string Path_)
  { Path = Path_; }
  void SetOption (string Option_)
  { Option = Option_; }
  void SetDescription (string Description_)
  { Description = Description_; }

  // Retorna as informacoes a respeito do arquivo associado
  void GetName (string &Name_) const
  { Name_ = Name; }
  void GetPath (string &Path_) const
  { Path_ = Path; }
  void GetOption (string &Option_) const
  { Option_ = Option; }
  void GetDescription (string &Description_) const
  { Description_ = Description; }
  string GetName () const
  { return Name; }
  string GetPath () const
  { return Path; }
  string GetOption () const
  { return Option; }
  string GetDescription () const
  { return Description; }

  // Abre um arquivo para escrita com opcoes definidas por option
  int OpenOutputFile (string Name_, string Path_, string Option_);

  // Reabre um arquivo para escrita. Continua escrevendo.
  int OpenOutputFile ();

  // Reabre um arquivo para escrita. Inicia escrita do zero.
  int OverwriteOutputFile ();

  // Abre um arquivo para leitura com opcoes definidas por option
  int OpenInputFile (string Name_, string Path_, string Option_);

  // Reabre um arquivo para leitura
  int OpenInputFile ();

  // Fecha arquivo
  int CloseFile ();

  // Verifica se o arquivo esta aberto
  int FileIsOpen ();

  // Verifica se houve alguma falha apos alguma operacao com o arquivo
  int CheckForFileFail ();

  // Verifica se chegou o fim de um arquivo
  int CheckForEndOfFile ();
};

#endif
