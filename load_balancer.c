/* Copyright 2023 Voicu Teodora-Andreea */

#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "load_balancer.h"
#include "hashtable2.h"
#include <stdio.h>

unsigned int hash_function_servers(void *a)
{
    unsigned int uint_a = *((unsigned int *)a);
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a)
{
    unsigned char *puchar_a = (unsigned char *)a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

load_balancer *init_load_balancer()
{
    load_balancer *main1 = malloc(sizeof(load_balancer));
    main1->server_tags = malloc(sizeof(int));
    main1->servers = NULL;
    main1->servers_no = 0;

    return main1;
}

void servers_sort(load_balancer *main)
{
    int i, j;
    for (i = 0; i < main->servers_no * 3 - 1; i++)
        for (j = 0; j < main->servers_no * 3 - i - 1; j++)
        {
            if (hash_function_servers(&main->server_tags[j]) >
                hash_function_servers(&main->server_tags[j + 1]))
            {
                int temp = main->server_tags[j];
                main->server_tags[j] = main->server_tags[j + 1];
                main->server_tags[j + 1] = temp;
            }
        }
}

server_memory *get_server_by_id(load_balancer *main, int server_id)
{
    for (int i = 0; i < main->servers_no; i++)
    {
        if (main->servers[i]->server_id == server_id)
        {
            return main->servers[i];
        }
    }
    return NULL;
}
int retrieve_server_store_id(load_balancer *main, char *key)
{
    int id;
    if (hash_function_key(key) >
      hash_function_servers(&main->server_tags[main->servers_no * 3 - 1]))
    {
        id = main->server_tags[0] % 100000;
    }
    else
    {
        for (int i = 0; i < main->servers_no * 3; i++)
        {
            if (hash_function_key(key) <=
               hash_function_servers(&main->server_tags[i]))
            {
                id = main->server_tags[i] % 100000;
                break;
            }
        }
    }
    return id;
}

void rebalance_objects(load_balancer *main, int server_id)
{
    server_memory *new_server = get_server_by_id(main, server_id);
    for (int i = 0; i < main->servers_no * 3; i++)
    {
        if (main->server_tags[i] % 100000 == server_id)
        {
            if (i < main->servers_no * 3 - 1)
            {
                int next_server_index = main->server_tags[i + 1] % 100000;
                if (next_server_index != server_id)
                {
                    server_memory *next_server =
                      get_server_by_id(main, next_server_index);
                    int size = next_server->keys_no;
                    char **removed_keys = malloc((size) * sizeof(char *));
                    for (int j = 0; j < next_server->keys_no; j++)
                    {
                        removed_keys[j] = NULL;
                        if (retrieve_server_store_id(main, next_server->keys[j])
                          == server_id && next_server->server_id != server_id)
                        {
                            server_store(new_server, next_server->keys[j],
                             server_retrieve(next_server,
                              next_server->keys[j]));
                            removed_keys[j] =
                               malloc((strlen(next_server->keys[j]) + 1)
                                 * sizeof(char));
                            strcpy(removed_keys[j], next_server->keys[j]);
                        }
                    }
                    for (int j = 0; j < size; j++)
                    {
                        if (removed_keys[j] != NULL)
                        {
                            server_remove(next_server, removed_keys[j]);
                        }
                        free(removed_keys[j]);
                    }

                    free(removed_keys);
                }
            }
            else if (i == main->servers_no * 3 - 1)
            {
                server_memory *next_server;
                if (main->server_tags[0] % 100000 != server_id)
                {
                    next_server =
                    get_server_by_id(main, main->server_tags[0] % 100000);
                }
                else if (main->server_tags[1] % 100000 != server_id)
                {
                    next_server =
                      get_server_by_id(main, main->server_tags[1] % 100000);
                }
                else
                {
                    next_server =
                      get_server_by_id(main, main->server_tags[2] % 100000);
                }
                int size = next_server->keys_no;
                char **removed_keys = malloc((size) * sizeof(*removed_keys));
                for (int j = 0; j < next_server->keys_no; j++)
                {
                    removed_keys[j] = NULL;
                    if (retrieve_server_store_id(main, next_server->keys[j])
                        == server_id && next_server->server_id != server_id)
                    {
                        server_store(new_server, next_server->keys[j],
                           server_retrieve(next_server, next_server->keys[j]));
                        removed_keys[j] =
                           malloc((strlen(next_server->keys[j]) + 1)*
                              sizeof(char));
                        strcpy(removed_keys[j], next_server->keys[j]);
                    }
                }
                for (int j = 0; j < size; j++)
                {
                    if (removed_keys[j] != NULL)
                    {
                        server_remove(next_server, removed_keys[j]);
                    }
                    free(removed_keys[j]);
                }

                free(removed_keys);
            }
        }
    }
}

void loader_add_server(load_balancer *main, int server_id)
{
    int *new_server_tags =
      malloc((main->servers_no + 1) * 3 * sizeof(unsigned int));
    for (int i = 0; i < 3 * (main->servers_no); i++)
    {
        memcpy(&new_server_tags[i],
           &main->server_tags[i], sizeof(main->server_tags[i]));
    }
    int no = main->servers_no * 3;
    new_server_tags[no++] = server_id;
    for (int i = 1; i < 3; i++)
    {
        new_server_tags[no++] = i * 100000 + server_id;
    }
    free(main->server_tags);
    main->server_tags = new_server_tags;

    server_memory **new_servers =
       malloc((main->servers_no + 1) * sizeof(server_memory *));
    for (int i = 0; i < main->servers_no; i++)
    {
        new_servers[i] = main->servers[i];
    }

    server_memory *new_server_memory = init_server_memory();
    set_server_id(new_server_memory, server_id);
    new_servers[main->servers_no] = new_server_memory;
    free(main->servers);
    main->servers = new_servers;
    main->servers_no++;
    servers_sort(main);
    rebalance_objects(main, server_id);
}
void loader_remove_server(load_balancer *main, int server_id)
{
    server_memory *server = get_server_by_id(main, server_id);
    int *new_server_tags = malloc((main->servers_no - 1) * 3 * sizeof(int));
    int k = 0, q = server->keys_no;
    char **keys = malloc((server->keys_no) * sizeof(*keys));
    char **values = malloc((server->keys_no) * sizeof(*values));

    for (int i = 0; i < server->keys_no; i++)
    {
        keys[i] = malloc((strlen(server->keys[i]) + 1) * sizeof(char));
        strcpy(keys[i], server->keys[i]);
        char *value = server_retrieve(server, server->keys[i]);
        values[i] = malloc((strlen(value) + 1) * sizeof(char));
        strcpy(values[i], value);
    }

    for (int i = 0; i < main->servers_no * 3; i++)
    {
        if (main->server_tags[i] % 100000 != server_id)
        {
            memcpy(&new_server_tags[k++],
              &main->server_tags[i], sizeof(main->server_tags[i]));
        }
    }

    int j = 0;
    server_memory **new_servers =
        malloc((main->servers_no - 1) * sizeof(server_memory *));
    for (int i = 0; i < main->servers_no; i++)
    {
        if (main->servers[i]->server_id != server_id)
        {
            new_servers[j++] = main->servers[i];
        }
    }
    free(main->server_tags);
    main->server_tags = new_server_tags;
    free_server_memory(server);
    free(main->servers);
    main->servers = new_servers;
    main->servers_no--;
    for (int i = 0; i < q; i++)
    {
        server_store(get_server_by_id(main,
          retrieve_server_store_id(main, keys[i])), keys[i], values[i]);
        free(keys[i]);
        free(values[i]);
    }

    free(keys);
    free(values);
}

void loader_store(load_balancer *main, char *key, char *value, int *server_id)
{
    int id = retrieve_server_store_id(main, key);
    memcpy(server_id, &id, sizeof(id));
    server_store(get_server_by_id(main, id), key, value);
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id)
{
    int id = retrieve_server_store_id(main, key);
    memcpy(server_id, &id, sizeof(id));
    return server_retrieve(get_server_by_id(main, id), key);
}

void free_load_balancer(load_balancer *main)
{
    for (int i = 0; i < main->servers_no; i++)
        free_server_memory(main->servers[i]);
    free(main->servers);
    free(main->server_tags);
    free(main);
}
