#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "queue.h"
#include "list.h"
#include <string.h>

bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) return false;
    element_t *ele = malloc(sizeof(element_t));
    if (!ele) return false;
    ele->value = strdup(s);
    INIT_LIST_HEAD(&ele->list);
    list_add(&ele->list,head);
    return true;
}

int main() {
    struct list_head *que = q_new();
    q_insert_head(que, "a");
    q_insert_head(que, "b");
    q_insert_head(que, "c");
    q_insert_head(que, "c");
    q_insert_head(que, "d");

    // int cnt =0;
    // int big_list_size = 80;

    // struct list_head *ori = head;
    // struct list_head *cur = head->next;
    // while (ori != cur && cnt < lcnt) {
    //     element_t *e = list_entry(cur, element_t, list);
    //     if (cnt < big_list_size)
    //         report_noreturn(vlevel, cnt == 0 ? "%s" : " %s", e->value);
    //     cnt++;
    //     cur = cur->next;
    // }

    return 0;
}