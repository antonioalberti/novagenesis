/*
	NovaGenesis

	Name:		EPGS novagenesis json
	Object:		ng_json
	File:		ng_json.c
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

#include "ng_json.h"
#include "ng_util.h"
#include "../epgs_wrapper.h"

struct _ng_json {
  int json_data_count;
  int json_all_data_size;
  char **json_data;
};

struct _ng_json *ng_json_create (const char *json)
{
  struct _ng_json *p_json = (struct _ng_json *)ng_malloc (sizeof (struct _ng_json) * 1);
  char *end_str;

  p_json->json_data_count = 0;
  p_json->json_all_data_size = 0;
  p_json->json_data = (char **)ng_malloc (sizeof (char **) * 1);

  if (json)
	{
	  int jsonLen = ng_strlen (json);
	  char *aux_token = (char *)ng_calloc (sizeof (char) * 1, jsonLen - 1);
	  ng_strncpy (aux_token, &json[1], jsonLen - 2);

	  char *token = strtok_r (aux_token, ",", &end_str);

	  while (token != NULL)
		{
		  ng_json_add_element (p_json, token);
		  token = strtok_r (NULL, ",", &end_str);
		}
	  if (aux_token != NULL)
		{
		  ng_free (aux_token);
		}
	}

  return p_json;
}

void ng_json_destroy (struct _ng_json *ngJSON)
{
  int i = 0;
  for (i = 0; i < ngJSON->json_data_count; i++)
	{
	  if (ngJSON->json_data[i])
		{
		  ng_free (ngJSON->json_data[i]);
		}
	}
  if (ngJSON->json_data)
	{
	  ng_free (ngJSON->json_data);
	}

  if (ngJSON)
	{
	  ng_free (ngJSON);
	}

}

void ng_json_add_int (struct _ng_json *ngJSON, const char *name, int value)
{

  char c_value[10];
  ng_sprintf (c_value, "%d", value);

  int element_size = ng_strlen (name) * sizeof (char) + ng_strlen (c_value) * sizeof (char) + 3;  // "":


  ngJSON->json_data = (char **)ng_realloc (ngJSON->json_data, (ngJSON->json_data_count + 1) * sizeof (char *));
  ngJSON->json_data[ngJSON->json_data_count] = (char *)ng_malloc (element_size + 1);
  ng_sprintf (ngJSON->json_data[ngJSON->json_data_count], "\"%s\":%s", name, c_value);

  ngJSON->json_all_data_size += element_size;
  ngJSON->json_data_count++;

}

void ng_json_add_string (struct _ng_json *ngJSON, const char *name, const char *value)
{

  int element_size = ng_strlen (name) * sizeof (char) + ng_strlen (value) * sizeof (char) + 5; // "":""

  ngJSON->json_data = (char **)ng_realloc (ngJSON->json_data, (ngJSON->json_data_count + 1) * sizeof (char *));
  ngJSON->json_data[ngJSON->json_data_count] = (char *)ng_malloc (element_size + 1);
  ng_sprintf (ngJSON->json_data[ngJSON->json_data_count], "\"%s\":\"%s\"", name, value);

  ngJSON->json_all_data_size += element_size;
  ngJSON->json_data_count++;

}

void ng_json_add_element (struct _ng_json *ngJSON, const char *element)
{

  int element_size = ng_strlen (element) * sizeof (char); // "":""

  ngJSON->json_data = (char **)ng_realloc (ngJSON->json_data, (ngJSON->json_data_count + 1) * sizeof (char *));
  ngJSON->json_data[ngJSON->json_data_count] = (char *)ng_malloc (element_size + 1);
  ng_sprintf (ngJSON->json_data[ngJSON->json_data_count], "%s", element);

  ngJSON->json_all_data_size += element_size;
  ngJSON->json_data_count++;

}

void ng_json_getJSon (NgJSon *ngJSON, char **jsonStr)
{
  char *ng_json_result = (char *)ng_calloc ((ngJSON->json_all_data_size + 2), sizeof (char) * 1);

  ng_strcpy (ng_json_result, "{");

  int i = 0;
  for (i = 0; i < ngJSON->json_data_count; i++)
	{
	  ng_strcat (ng_json_result, ngJSON->json_data[i]);

	  if (i < ngJSON->json_data_count - 1)
		{
		  ng_strcat (ng_json_result, ",");
		}
	  else
		{
		  ng_strcat (ng_json_result, "}");
		}
	}
  *jsonStr = ng_json_result;
}

void ng_json_get_string (struct _ng_json *ngJSON, const char *name, char **valueStr)
{

  char *value = NULL;
  char *end_str;

  char *aux_name = (char *)ng_malloc (sizeof (char) * (ng_strlen (name) + 3));
  ng_strcpy (aux_name, "\"");
  ng_strcat (aux_name, name);
  ng_strcat (aux_name, "\"");

  int i = 0;
  for (i = 0; i < ngJSON->json_data_count; i++)
	{

	  char *aux_token = (char *)ng_malloc (sizeof (char) * (ng_strlen (ngJSON->json_data[i]) + 1));
	  ng_strcpy (aux_token, ngJSON->json_data[i]);

	  char *token = strtok_r (aux_token, ":", &end_str);

	  if (token)
		{
		  int comp = ng_strcmp (aux_name, token);

		  if (comp == 0)
			{
			  token = strtok_r (NULL, ":", &end_str);
			  int len = ng_strlen (token);
			  value = (char *)ng_calloc (sizeof (char) * 1, len - 1);
			  ng_strncpy (value, &token[1], len - 2);
			}
		}
	  ng_free (aux_token);
	}

  ng_free (aux_name);
  aux_name = NULL;

  *valueStr = value;
}

int ng_json_get_int (struct _ng_json *ngJSON, const char *name)
{
  int value = 0;
  char *end_str;

  char *aux_name = (char *)ng_malloc (sizeof (char) * (ng_strlen (name) + 3));
  ng_strcpy (aux_name, "\"");
  ng_strcat (aux_name, name);
  ng_strcat (aux_name, "\"");

  int i = 0;
  for (i = 0; i < ngJSON->json_data_count; i++)
	{

	  char *aux_token = (char *)ng_malloc (sizeof (char) * (ng_strlen (ngJSON->json_data[i]) + 1));
	  ng_strcpy (aux_token, ngJSON->json_data[i]);

	  char *token = strtok_r (aux_token, ":", &end_str);

	  if (token)
		{
		  if (ng_strcmp (aux_name, token) == 0)
			{
			  token = strtok_r (NULL, ":", &end_str);
			  value = ng_atoi (token);
			}
		}
	  ng_free (aux_token);
	}
  ng_free (aux_name);

  return value;
}
