/*
	NovaGenesis

	Name:		Operating system file handler
	Object:		File
	File:		File.cpp
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
#include "File.h"
#endif

File::File ()
	: fstream ()
{
}

File::File (const File &F)
{
  Name = F.GetName ();
  Path = F.GetPath ();
  Option = F.GetOption ();
  Description = F.GetDescription ();
}

File &File::operator= (const File &F)
{
  Name = F.GetName ();
  Path = F.GetPath ();
  Option = F.GetOption ();
  Description = F.GetDescription ();

  return *this;
}

int File::OpenOutputFile (string Name_, string Path_, string Option_)
{
  if (FileIsOpen () == 1) // se for igual a 1 o arquivo esta aberto
	{
	  return OK;
	}
  else
	{
	  if (Option_ == "BINARY")
		{
		  open ((Path_ + Name_).c_str (), ios::out | ios::binary);

		  if (ios::fail () == 1) // se for igual a 1 houve erro
			{
			  return ERROR;
			}
		  else
			{
			  Name = Name_;
			  Path = Path_;
			  Option = Option_;

			  return OK;
			}
		}

	  if (Option_ == "DEFAULT")
		{
		  open ((Path_ + Name_).c_str (), ios::out);

		  if (ios::fail () == 1) // se for igual a 1 houve erro
			{
			  return ERROR;
			}
		  else
			{
			  Name = Name_;
			  Path = Path_;
			  Option = Option_;

			  return OK;
			}
		}
	  else
		{
		  return ERROR;
		}
	}
}

int File::OpenOutputFile ()
{
  if (FileIsOpen () == 1) // se for igual a 1 o arquivo esta aberto
	{
	  return ERROR;
	}
  else
	{
	  if (Option == "BINARY")
		{
		  open ((Path + Name).c_str (), ios::out | ios::app | ios::binary);

		  if (ios::fail () == 1) // se for igual a 1 houve erro
			{
			  return ERROR;
			}
		  else
			{
			  return OK;
			}
		}

	  if (Option == "DEFAULT")
		{
		  open ((Path + Name).c_str (), ios::out | ios::app);

		  if (ios::fail () == 1) // se for igual a 1 houve erro
			{
			  return ERROR;
			}
		  else
			{
			  return OK;
			}
		}
	  else
		{
		  return ERROR;
		}
	}
}

int File::OverwriteOutputFile ()
{
  if (FileIsOpen () == 1) // se for igual a 1 o arquivo esta aberto
	{
	  return OK;
	}
  else
	{
	  if (Option == "BINARY")
		{
		  open ((Path + Name).c_str (), ios::out | ios::binary);

		  if (ios::fail () == 1) // se for igual a 1 houve erro
			{
			  return ERROR;
			}
		  else
			{
			  return OK;
			}
		}

	  if (Option == "DEFAULT")
		{
		  open ((Path + Name).c_str (), ios::out);

		  if (ios::fail () == 1) // se for igual a 1 houve erro
			{
			  return ERROR;
			}
		  else
			{
			  return OK;
			}
		}
	  else
		{
		  return ERROR;
		}
	}
}

int File::OpenInputFile (string Name_, string Path_, string Option_)
{
  if (FileIsOpen () == 1) // se for igual a 1 o arquivo esta aberto
	{
	  return OK;
	}
  else
	{
	  if (Option_ == "BINARY")
		{
		  open ((Path_ + Name_).c_str (), ios::in | ios::binary | ios::ate);

		  if (ios::fail () == 1) // se for igual a 1 houve erro
			{
			  return ERROR;
			}
		  else
			{
			  Name = Name_;
			  Path = Path_;
			  Option = Option_;

			  return OK;
			}
		}

	  if (Option_ == "DEFAULT")
		{
		  open ((Path_ + Name_).c_str (), ios::in);

		  if (ios::fail () == 1) // se for igual a 1 houve erro
			{
			  return ERROR;
			}
		  else
			{
			  Name = Name_;
			  Path = Path_;
			  Option = Option_;

			  return OK;
			}
		}
	  else
		{
		  return ERROR;
		}
	}
}

int File::OpenInputFile ()
{
  if (FileIsOpen () == 1) // se for igual a 1 o arquivo esta aberto
	{
	  return OK;
	}
  else
	{
	  if (Option == "BINARY")
		{
		  open ((Path + Name).c_str (), ios::in | ios::binary | ios::ate);

		  if (ios::fail () == 1) // se for igual a 1 houve erro
			{
			  return ERROR;
			}
		  else
			{
			  return OK;
			}
		}

	  if (Option == "DEFAULT")
		{
		  open ((Path + Name).c_str (), ios::in);

		  if (ios::fail () == 1) // se for igual a 1 houve erro
			{
			  return ERROR;
			}
		  else
			{
			  return OK;
			}
		}
	  else
		{
		  return ERROR;
		}
	}
}

int File::CloseFile ()
{
  if (FileIsOpen () == 1) // Retorna 1 se o arquivo esta aberto
	{
	  close ();

	  return OK;
	}

  return ERROR;
}

int File::FileIsOpen ()
{
  return rdbuf ()->is_open ();  // Retorna 1 se o arquivo esta aberto
}

int File::CheckForFileFail ()
{
  return ios::fail ();
}

int File::CheckForEndOfFile ()
{
  return ios::eof ();
}
