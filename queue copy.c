#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* private function */
void _node_swap(struct list_head *node1, struct list_head *node2);
void _node_quick_sort(struct list_head *upper_node, struct list_head *lower_node,struct list_head *head);
/* private function */



/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *n = malloc(sizeof(struct list_head));
    if (!n) return NULL;
    INIT_LIST_HEAD(n);
    return n;
}

/* Free all storage used by queue */
void q_free(struct list_head *l) {
    if (!l) return;
    struct list_head *entry, *safe;
    list_for_each_safe(entry, safe, l) {
        element_t *ele = container_of(entry, element_t, list);
        free(ele->value);
        free(ele);
    };
    free(l);
}

/* Insert an element at head of queue */
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

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) return false;
    element_t *ele = malloc(sizeof(element_t));
    if (!ele) return false;
    ele->value = strdup(s);
    INIT_LIST_HEAD(&ele->list);
    list_add_tail(&ele->list,head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // printf("%s\n", sp);
    // sp used to store the remove target
    if (!head) return NULL;
    element_t *ele = list_first_entry(head, element_t, list);
    list_del(head->next);
    strncpy(sp, ele->value, bufsize);
    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    // printf("%s\n", sp);
    // sp used to store the remove target
    if (!head) return NULL;
    element_t *ele = list_last_entry(head, element_t, list);
    list_del(head->prev);
    strncpy(sp, ele->value, bufsize);
    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head) return 0;

    int len = 0;
    struct list_head *lh;
    list_for_each(lh, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    // find middle
    int nums = q_size(head)/2;
    
    struct list_head *node, *safe;
    list_for_each_safe(node, safe, head) {
        if (nums<=0) break;
        nums--;
    };
    element_t *ele = container_of(node, element_t, list);
    free(ele->value);
    list_del(node);
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
    // // sorted solution // time: O(n)
    // if (!head) return true;
    // struct list_head *curr, *next;
    // curr = head->next;
    // next = curr->next;
    // while (curr!=next && next!=head) {
    //     element_t *curr_ele = list_entry(curr, element_t, list);
    //     element_t *next_ele = list_entry(next, element_t, list);
    //     if (strcmp(curr_ele->value, next_ele->value)==0) {
    //         while (strcmp(curr_ele->value, next_ele->value)==0) {
    //             list_del(next);
    //             next = curr->next;
    //             free(next_ele->value);  // free string space
    //             free(next_ele);         // free element node
    //             next_ele = list_entry(next, element_t, list);
    //             if (next==head) break;
    //         }
    //         list_del(curr);
    //         free(curr_ele->value);
    //         free(curr_ele);
    //         curr_ele = list_entry(curr, element_t, list);
    //         curr = next;
    //         next = curr->next;
    //     } else {
    //         curr = next;
    //         next = curr->next;
    //     }
    // }
    // return true;

    // unsorted solution
    // without hashtable solution, need to check every element time(nlogn)
    if (!head) return true;
    struct list_head *curr, *next;
    curr = head->next;
    next = curr->next;
    while (curr!=head) {
        element_t *curr_ele = list_entry(curr, element_t, list);
        bool is_dup = false;
        while (next!=head) {
            element_t *next_ele = list_entry(next, element_t, list);
            if (strcmp(curr_ele->value, next_ele->value)==0) {
                is_dup=true;
                list_del(next);
                free(next_ele->value);
                free(next_ele);
                next = curr->next;
            } else {
                next = next->next;
            };
        }
        if (is_dup) {
            list_del(curr);
            free(curr_ele->value);
            free(curr_ele);
        }
        curr = next;
        next = curr->next;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head)) return;
    struct list_head *curr = head->next;
    struct list_head *next = curr->next;
    while (curr!=head && next!=head) {
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
void q_reverse(struct list_head *head) {
    // step1: [curr, n1, n2, n3 ...]
    // step2: [n1, curr, n2, n3 ...]
    // step3: [n2, n1, curr, n3 ...]
    // stepn: [... curr->head]
    if (!head || list_empty(head)) return;
    struct list_head *curr = head->next;
    while (curr->next!=head) {
        list_move(curr->next, head);
    };
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {
    // if (list_is_singular(head)) return;
    // // bubble sort
    // bool is_term = false;
    // struct list_head *term = head->prev;
    // struct list_head *curr = head->next;
    // struct list_head *next = curr->next;
    // struct list_head *safe = curr->next;

    // // find the smallest
    // element_t *curr_ele = list_entry(curr, element_t, list);
    // while (next!=head) {
    //     if (next==term) is_term = true;
    //     element_t *next_ele = list_entry(next, element_t, list);
    //     if (strcmp(next_ele->value, curr_ele->value)<0) {
    //         safe = next->next;
    //         list_move(next, head);
    //         curr = next;
    //         curr_ele = list_entry(curr, element_t, list);
    //         next = safe;
    //     } else {
    //         next = next->next;
    //     }
    //     if (is_term) break;
    // }

    // // ascending sort
    // is_term = false;
    // next = curr->next;
    // while (curr!=head) {
    //     print_list(head);
    //     if (next==term) is_term = true;
    //     element_t *next_ele = list_entry(next, element_t, list);
    //     printf("curr value: %s\n", curr_ele->value);
    //     printf("next value: %s\n", next_ele->value);
    //     if (strcmp(next_ele->value, curr_ele->value)==0) {
    //         curr = curr->next;
    //         curr_ele = list_entry(curr, element_t, list);
    //         is_term=false;
    //         term = head->prev;
    //     } else {
    //         //large case
    //         list_move_tail(next, head);
    //     }
    //     next = curr->next;
    //     next_ele = list_entry(next, element_t, list);
    //     if (is_term) {
    //         curr = curr->next;
    //         curr_ele = list_entry(curr, element_t, list);
    //         term = head->prev;
    //     }
    // }

    // Quick sort
    _node_quick_sort(head->next, head->prev, head);
}


/* private function */
void _node_quick_sort(struct list_head *upper_node, struct list_head *lower_node,struct list_head *head) {
    if (upper_node==lower_node || upper_node==head || lower_node==head) return;
    struct list_head *top = upper_node->prev;
    struct list_head *bot = lower_node->next;

    struct list_head *last = lower_node;
    struct list_head *pivot = upper_node;
    struct list_head *curr = pivot->next;
    element_t *curr_ele = list_entry(curr, element_t, list);
    element_t *pivot_ele = list_entry(pivot, element_t, list);

    // finally curr point the first element at larger size
    while (curr!=last) {
        if (strcmp(pivot_ele->value, curr_ele->value)>=0) {
            curr = curr->next;
        } else {
            _node_swap(curr, last);
            if (curr!=last) {
                struct list_head *safe = curr->prev;
                curr = last;
                last = safe;
            }
        }
        curr_ele = list_entry(curr, element_t, list);
    }
    if (strcmp(pivot_ele->value, curr_ele->value)>0) {
        _node_swap(pivot, curr);
    } else {
        if (strcmp(pivot_ele->value, list_entry(curr->prev, element_t, list)->value)>0) {
            _node_swap(pivot, curr->prev);
        }
    }
    if (top->next!=pivot) {
        _node_quick_sort(top->next, pivot->prev, head);
    }
    if (bot->prev!=pivot) {
        _node_quick_sort(pivot->next, bot->prev, head);
    }
}

void _node_swap(struct list_head *node1, struct list_head *node2) {
    if (node1->next!=node2 && node2->next!=node1) {
        // a node1 b node2 c
        struct list_head *safe;
        node1->prev->next = node2;
        node2->next->prev = node1;

        node1->next->prev = node2;
        node2->prev->next = node1;

        safe = node2->prev;
        node2->prev = node1->prev;
        node1->prev = safe;

        safe = node2->next;
        node2->next = node1->next;
        node1->next = safe;
    } else {
        // head node1 node2
        node1->prev->next = node2;
        node2->next->prev = node1;

        node1->next = node2->next;
        node2->prev = node1->prev;
        node1->prev = node2;
        node2->next = node1;
    }
}