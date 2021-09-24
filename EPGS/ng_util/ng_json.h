/*
	NovaGenesis

	Name:		EPGS novagenesis json
	Object:		ng_json
	File:		ng_json.h
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

#ifndef _NG_JSON_H
#define _NG_JSON_H

typedef struct _ng_json NgJSon;

extern NgJSon *ng_json_create (const char *json);
extern void ng_json_destroy (NgJSon *ngJSON);
extern void ng_json_add_int (NgJSon *ngJSON, const char *name, int value);
extern void ng_json_add_string (NgJSon *ngJSON, const char *name, const char *value);
extern void ng_json_add_element (NgJSon *ngJSON, const char *element);
extern void ng_json_getJSon (NgJSon *ngJSON, char **jsonStr);
extern void ng_json_get_string (NgJSon *ngJSON, const char *name, char **valueStr);
extern int ng_json_get_int (NgJSon *ngJSON, const char *name);

#endif /* _NG_JSON_H*/
