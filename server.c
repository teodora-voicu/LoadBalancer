/* Copyright 2023 Teodora Voicu */

#include <stdlib.h>
#include <string.h>
#include "hashtable2.h"
#include "server.h"
#include <stdio.h>

#define HMAXX 10

server_memory *init_server_memory()
{
	server_memory *new_server_memory = malloc(sizeof(server_memory));
	hashtable_t *htserv = ht_create(HMAXX,
	   hash_function_servers, compare_function_strings);
	new_server_memory->htserver = htserv;
	new_server_memory->keys_no = 0;
	new_server_memory->keys_size = 0;
	new_server_memory->keys = NULL;
	return new_server_memory;
}

void set_server_id(server_memory *server, int server_id)
{
	server->server_id = server_id;
}

void server_store(server_memory *server, char *key, char *value)
{
	if (!ht_has_key(server->htserver, key))
	{
		ht_put(server->htserver, key, strlen(key) + 1, value, strlen(value) + 1);
		char **new_keys = malloc((server->keys_no + 1) * sizeof(*new_keys));
		if (new_keys == NULL)
		{
			return;
		}

		for (int i = 0; i < server->keys_no; i++)
		{
			new_keys[i] = malloc((strlen(server->keys[i]) + 1) * sizeof(char));
			if (new_keys[i] == NULL)
			{
				return;
			}
			strcpy(new_keys[i], server->keys[i]);
		}

		new_keys[server->keys_no] = malloc((strlen(key) + 1) * sizeof(char));
		if (new_keys[server->keys_no] == NULL)
		{
			return;
		}
		strcpy(new_keys[server->keys_no], key);

		for (int i = 0; i < server->keys_no; i++)
		{
			free(server->keys[i]);
		}
		free(server->keys);

		server->keys = new_keys;
		server->keys_no++;
	}
}

char *server_retrieve(server_memory *server, char *key)
{
	return (char *)ht_get(server->htserver, key);
}

void server_remove(server_memory *server, char *key)
{
	if (ht_has_key(server->htserver, key))
	{
		ht_remove_entry(server->htserver, key);
		char **new_keys = malloc((server->keys_no - 1) * sizeof(*new_keys));
		if (new_keys == NULL)
			return;
		int j = 0;
		for (int i = 0; i < server->keys_no; i++)
		{
			if (strcmp(key, server->keys[i]) != 0)
			{
				new_keys[j] = malloc((strlen(server->keys[i]) + 1) * sizeof(char));
				if (new_keys[j] == NULL)
					return;
				strcpy(new_keys[j++], server->keys[i]);
			}
		}

		for (int i = 0; i < server->keys_no; i++)
			free(server->keys[i]);
		free(server->keys);
		server->keys = new_keys;
		server->keys_no--;
	}
}

void free_server_memory(server_memory *server)
{
	ht_free(server->htserver);

	for (int i = 0; i < server->keys_no; i++)
	{
		if (server->keys[i] != NULL)
			free(server->keys[i]);
	}
	free(server->keys);

	free(server);
}
