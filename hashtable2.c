/* Copyright 2023 Laborator */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "hashtable2.h"

#define MAX_STRING_SIZE 256
#define HMAX 10

#define DIE(assertion, call_description)                                       \
    do {                                                                       \
        if (assertion) {                                                       \
            fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                 \
            perror(call_description);                                          \
            exit(errno);                                                       \
        }                                                                      \
    } while (0)

linked_list_t *
ll_create(unsigned int data_size)
{
    linked_list_t *ll;

    ll = malloc(sizeof(*ll));
    DIE(ll == NULL, "linked_list malloc");

    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;

    return ll;
}

void ll_add_nth_node(linked_list_t *list, unsigned int n, const void *new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t *new_node;

    if (!list)
    {
        return;
    }

    if (n > list->size)
    {
        n = list->size;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0)
    {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(*new_node));
    DIE(new_node == NULL, "new_node malloc");
    new_node->data = malloc(list->data_size);
    DIE(new_node->data == NULL, "new_node->data malloc");
    memcpy(new_node->data, new_data, list->data_size);

    new_node->next = curr;
    if (prev == NULL)
    {
        list->head = new_node;
    }
    else
    {
        prev->next = new_node;
    }

    list->size++;
}

ll_node_t *
ll_remove_nth_node(linked_list_t *list, unsigned int n)
{
    ll_node_t *prev, *curr;

    if (!list || !list->head)
    {
        return NULL;
    }

    if (n > list->size - 1)
    {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0)
    {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL)
    {
        list->head = curr->next;
    }
    else
    {
        prev->next = curr->next;
    }

    list->size--;

    return curr;
}

unsigned int
ll_get_size(linked_list_t *list)
{
    if (!list)
    {
        return -1;
    }

    return list->size;
}

void ll_free(linked_list_t **pp_list)
{
    ll_node_t *currNode;

    if (!pp_list || !*pp_list)
    {
        return;
    }

    while (ll_get_size(*pp_list) > 0)
    {
        currNode = ll_remove_nth_node(*pp_list, 0);
        free(currNode->data);
        currNode->data = NULL;
        free(currNode);
        currNode = NULL;
    }

    free(*pp_list);
    *pp_list = NULL;
}

void ll_print_int(linked_list_t *list)
{
    ll_node_t *curr;

    if (!list)
    {
        return;
    }

    curr = list->head;
    while (curr != NULL)
    {
        printf("%d ", *((int *)curr->data));
        curr = curr->next;
    }

    printf("\n");
}

void ll_print_string(linked_list_t *list)
{
    ll_node_t *curr;

    if (!list)
    {
        return;
    }

    curr = list->head;
    while (curr != NULL)
    {
        printf("%s ", (char *)curr->data);
        curr = curr->next;
    }

    printf("\n");
}

int compare_function_ints(void *a, void *b)
{
    int int_a = *((int *)a);
    int int_b = *((int *)b);

    if (int_a == int_b)
    {
        return 0;
    }
    else if (int_a < int_b)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

int compare_function_strings(void *a, void *b)
{
    char *str_a = (char *)a;
    char *str_b = (char *)b;
    return strcmp(str_a, str_b);
}

unsigned int
hash_function_int(void *a)
{
    /*
     * Credits: https://stackoverflow.com/a/12996028/7883884
     */
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int
hash_function_string(void *a)
{
    /*
     * Credits: http://www.cse.yorku.ca/~oz/hash.html
     */
    unsigned char *puchar_a = (unsigned char *)a;
    unsigned long hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

    return hash;
}

hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void *),
          int (*compare_function)(void *, void *))
{
    hashtable_t *ht = malloc(sizeof(*ht));
    DIE(ht == NULL, "malloc failed");

    ht->buckets = malloc(hmax * sizeof(*ht->buckets));
    DIE(ht->buckets == NULL, "malloc failed");

    ht->size = 0;

    ht->hmax = hmax;

    ht->hash_function = hash_function_string;
    ht->compare_function = compare_function_strings;

    for (int i = 0; i < hmax; i++)
    {
        ht->buckets[i] = ll_create(sizeof(info));
    }

    return ht;
}

int ht_has_key(hashtable_t *ht, void *key)
{
    int index = ht->hash_function(key) % ht->hmax;

    linked_list_t *bucket = ht->buckets[index];

    ll_node_t *curr = bucket->head;

    for (int i = 0; i < bucket->size; i++)
    {
        if (ht->compare_function(key, ((info *)curr->data)->key) == 0)
            return 1;
        curr = curr->next;
    }

    return 0;
}

void *
ht_get(hashtable_t *ht, void *key)
{
    int index = ht->hash_function(key) % ht->hmax;

    linked_list_t *bucket = ht->buckets[index];

    ll_node_t *curr = bucket->head;

    for (int i = 0; i < bucket->size; i++)
    {
        if (ht->compare_function(key, ((info *)curr->data)->key) == 0)
        {
            return ((info *)curr->data)->value;
        }

        curr = curr->next;
    }

    return NULL;
}

void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
            void *value, unsigned int value_size)
{
    info data;
    int index = ht->hash_function(key) % ht->hmax;
    linked_list_t *bucket = ht->buckets[index];

    int pos = 0;
    ll_node_t *curr = bucket->head;

    while (curr != NULL &&
       ht->compare_function(key, ((info *)curr->data)->key) != 0)
    {
        curr = curr->next;
        pos++;
    }

    if (curr == NULL)
    {
        data.key = malloc(key_size);
        DIE(NULL == data.key, "malloc failed");
        memcpy(data.key, key, key_size);

        data.value = malloc(value_size);
        DIE(NULL == data.value, "malloc failed");
        memcpy(data.value, value, value_size);

        ll_add_nth_node(bucket, pos, &data);
    }
    else
    {
        memcpy(((info *)curr->data)->value, value, value_size);
    }

    ht->size++;
}

void ht_remove_entry(hashtable_t *ht, void *key)
{
    if (ht_has_key(ht, key))
    {
        int index = ht->hash_function(key) % ht->hmax;

        linked_list_t *bucket = ht->buckets[index];

        ll_node_t *curr = bucket->head;

        int pos = 0;

        while (ht->compare_function(key, ((info *)curr->data)->key) != 0
                && curr != NULL)
        {
            curr = curr->next;
            pos++;
        }

        if (curr)
        {
            curr = ll_remove_nth_node(bucket, pos);
            free(((info *)curr->data)->key);
            free(((info *)curr->data)->value);
            free(curr->data);
            free(curr);
            ht->size--;
        }
    }
}

void ht_free(hashtable_t *ht)
{
    ll_node_t *curr;
    for (int i = 0; i < ht->hmax; i++)
    {
        if (ht->buckets[i]->head != NULL)
        {
            curr = ht->buckets[i]->head;
            while (curr != NULL)
            {
                free(((info *)curr->data)->value);
                free(((info *)curr->data)->key);
                curr = curr->next;
            }
        }
        ll_free(&ht->buckets[i]);
    }

    free(ht->buckets);
    free(ht);
}

unsigned int
ht_get_size(hashtable_t *ht)
{
    if (ht == NULL)
        return 0;

    return ht->size;
}

unsigned int
ht_get_hmax(hashtable_t *ht)
{
    if (ht == NULL)
        return 0;

    return ht->hmax;
}
