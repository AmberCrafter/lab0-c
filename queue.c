#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

// note.
// 1. memcpy: ref. https://github.com/Nahemah1022/lab0-c/blob/master/queue.c


/* private function */
struct list_head *_node_merge_sort(struct list_head *head,
                                   struct list_head **last);
struct list_head *_node_break_circle_and_remove_head(struct list_head *head);
bool q_shuffle(struct list_head *head);
/* private function */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *n = malloc(sizeof(struct list_head));
    if (!n)
        return NULL;
    INIT_LIST_HEAD(n);
    return n;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    struct list_head *entry, *safe;
    list_for_each_safe (entry, safe, l) {
        element_t *ele = container_of(entry, element_t, list);
        free(ele->value);
        free(ele);
    };
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *ele = malloc(sizeof(element_t));

    if (ele == NULL) {
        return false;
    }
    ele->value = strdup(s);
    if (ele->value == NULL) {
        free(ele);
        return false;
    }
    INIT_LIST_HEAD(&ele->list);
    list_add(&ele->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *ele = malloc(sizeof(element_t));
    if (ele == NULL) {
        return false;
    }
    ele->value = strdup(s);
    if (ele->value == NULL) {
        free(ele);
        return false;
    }
    INIT_LIST_HEAD(&ele->list);
    list_add_tail(&ele->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // sp used to store the remove target
    if (head == NULL || list_empty(head))
        return NULL;
    element_t *ele = list_first_entry(head, element_t, list);
    list_del(head->next);
    if (sp) {
        size_t length = (strlen(ele->value) < bufsize - 1) ? strlen(ele->value)
                                                           : bufsize - 1;
        memcpy(sp, ele->value, length);
        *(sp + length) = '\0';
    }
    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    // sp used to store the remove target
    if (!head || list_empty(head))
        return NULL;
    element_t *ele = list_last_entry(head, element_t, list);
    list_del(head->prev);
    if (sp) {
        size_t length = (strlen(ele->value) < bufsize - 1) ? strlen(ele->value)
                                                           : bufsize - 1;
        memcpy(sp, ele->value, length);
        *(sp + length) = '\0';
    }
    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int len = 0;
    struct list_head *lh;
    list_for_each (lh, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    // find middle
    if (!head || list_empty(head))
        return false;  // failed operator
    int nums = q_size(head) / 2;

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        if (nums <= 0)
            break;
        nums--;
    };
    element_t *ele = container_of(node, element_t, list);
    free(ele->value);
    list_del(node);
    free(ele);
    return true;
}

/* Delete all nodes that have duplicate string */
// delete element step
// 1. remove from linked list
// 2. free char space
// 3. free element space
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    // sorted solution // time: O(n)
    if (!head)
        return true;
    struct list_head *curr, *next;
    curr = head->next;
    next = curr->next;
    while (curr != next && curr != head && next != head) {
        element_t *curr_ele = list_entry(curr, element_t, list);
        element_t *next_ele = list_entry(next, element_t, list);

        if (strcmp(curr_ele->value, next_ele->value) == 0) {
            while (strcmp(curr_ele->value, next_ele->value) == 0) {
                list_del(next);
                next = curr->next;
                free(next_ele->value);  // free string space
                free(next_ele);         // free element node
                next_ele = list_entry(next, element_t, list);
                if (next == head)
                    break;
            }
            list_del(curr);
            free(curr_ele->value);
            free(curr_ele);
            curr_ele = list_entry(curr, element_t, list);
            curr = next;
            next = curr->next;
        } else {
            curr = next;
            next = curr->next;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    struct list_head *curr = head->next;
    struct list_head *next = curr->next;
    while (curr != head && next != head) {
        curr->prev->next = next;
        next->next->prev = curr;
        curr->next = next->next;
        next->prev = curr->prev;

        curr->prev = next;
        next->next = curr;

        curr = curr->next;
        next = curr->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    // step1: [curr, n1, n2, n3 ...]
    // step2: [n1, curr, n2, n3 ...]
    // step3: [n2, n1, curr, n3 ...]
    // stepn: [... curr->head]
    if (!head || list_empty(head))
        return;
    struct list_head *curr = head->next;
    while (curr->next != head) {
        list_move(curr->next, head);
    };
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return;
    // merge sort
    struct list_head *list, *last;
    list = _node_break_circle_and_remove_head(head);
    list = _node_merge_sort(list, &last);
    head->next = list;
    list->prev = head;
    last->next = head;
    head->prev = last;
}

bool q_shuffle(struct list_head *head)
{
    if (list_empty(head))
        return false;
    if (list_is_singular(head))
        return true;

    // set rand seed
    srand(time(NULL));

    struct list_head *curr;
    int length = q_size(head);
    for (int i = 0; i < length; i++) {
        int rnd = (int) ((double) rand() / (RAND_MAX + 1.0) * (length - i));
        curr = head->next;
        for (int j = 0; j < rnd; j++) {
            curr = curr->next;
        }
        list_move_tail(curr, head);
    }

    return true;
}



/* private function */
struct list_head *_node_break_circle_and_remove_head(struct list_head *head)
{
    struct list_head *list = head->next;

    // view: head -> NULL <- list
    head->next = NULL;
    list->prev = NULL;

    // view: NULL <- head -> NULL <- list ->NULL
    head->prev->next = NULL;
    head->prev = NULL;
    return list;
}

struct list_head *_node_merge_sort(struct list_head *list,
                                   struct list_head **last)
{
    if (list == NULL || list->next == NULL)
        return list;
    // list without head
    // view: NULL <- list... ->NULL
    // cutting

    // find mid
    // view:
    // 1 2 3 4 5
    //   ^   ^
    //   s   f
    struct list_head *curr = list, *slow = list, *lhs, *rhs;  // curr as fast
    while (curr->next && curr->next->next) {
        curr = curr->next->next;
        slow = slow->next;
    }

    lhs = list;
    rhs = slow->next;

    // break list
    slow->next = NULL;
    rhs->prev = NULL;

    // divied
    lhs = _node_merge_sort(lhs, last);
    rhs = _node_merge_sort(rhs, last);


    // merge
    // choose smaller one to be start point
    if (strcmp(list_entry(lhs, element_t, list)->value,
               list_entry(rhs, element_t, list)->value) <= 0) {
        curr = lhs;
        lhs = lhs->next;
    } else {
        curr = rhs;
        rhs = rhs->next;
    }

    list = curr;
    while (lhs != NULL && rhs != NULL) {
        if (strcmp(list_entry(lhs, element_t, list)->value,
                   list_entry(rhs, element_t, list)->value) <= 0) {
            curr->next = lhs;
            lhs->prev = curr;
            lhs = lhs->next;
            while (lhs &&
                   strcmp(list_entry(lhs, element_t, list)->value,
                          list_entry(rhs, element_t, list)->value) <= 0) {
                curr = curr->next;
                lhs = lhs->next;
            }
            curr = curr->next;
            curr->next = NULL;
        } else {
            curr->next = rhs;
            rhs->prev = curr;
            rhs = rhs->next;
            while (rhs &&
                   strcmp(list_entry(rhs, element_t, list)->value,
                          list_entry(lhs, element_t, list)->value) <= 0) {
                curr = curr->next;
                rhs = rhs->next;
            }
            curr = curr->next;
            curr->next = NULL;
        }
    }
    if (lhs != NULL) {
        curr->next = lhs;
        lhs->prev = curr;
    } else if (rhs != NULL) {
        curr->next = rhs;
        rhs->prev = curr;
    }
    while (curr->next) {
        curr = curr->next;
    }
    *last = curr;
    return list;
}
