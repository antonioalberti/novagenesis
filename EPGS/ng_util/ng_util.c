/*
	NovaGenesis

	Name:		EPGS novagenesis utilities
	Object:		ng_util
	File:		ng_util.c
	Author:		Vâner José Magalhães
	Date:		05/2021
	Version:	0.1

  	Copyright (C) 2021  Vâner José Magalhães

    This work is available under the GNU General Public License (See COPYING.txt).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "ng_util.h"
#include "../epgs_wrapper.h"

char *strtok_r (
	char *str,
	const char *delim,
	char **nextp)
{
  char *ret;

  if (str == NULL)
	{
	  str = *nextp;
	}

  str += ng_strspn (str, delim);

  if (*str == '\0')
	{
	  return NULL;
	}

  ret = str;

  str += ng_strcspn (str, delim);

  if (*str)
	{
	  *str++ = '\0';
	}

  *nextp = str;

  return ret;
}


