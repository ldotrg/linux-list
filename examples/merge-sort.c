#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"

#include "common.h"

static uint16_t values[256];

static void entry_newline(int idx)
{
    if (idx % 16 == 0)
        printf("\n");
}

static void list_split(struct list_head *head,
                       struct list_head *left,
                       struct list_head *right)
{
    struct list_head *fast;
    struct list_head *slow;
    struct listitem *item = NULL;
    if (list_empty(head) || list_is_singular(head))
        return;

    fast = head->next;
    slow = head->next;
    while ((fast->next->next != head) && (fast->next != head)) {
        slow = slow->next;
        fast = fast->next->next;
    }
    /* Cut From middle point to tail node*/
    list_cut_position(right, slow, head->prev);
    list_cut_position(left, head, slow);
}

static void list_merge(struct list_head *head,
                       struct list_head *left,
                       struct list_head *right)
{
    struct listitem *item_l = NULL;
    struct listitem *item_r = NULL;
    while (1) {
        if (list_empty(left)) {
            list_splice_tail(right, head);
            break;
        }
        if (list_empty(right)) {
            list_splice_tail(left, head);
            break;
        }
        item_l = list_first_entry(left, struct listitem, list);
        item_r = list_first_entry(right, struct listitem, list);
        if (item_l->i < item_r->i) {
            list_move_tail(left->next, head);
        } else {
            list_move_tail(right->next, head);
        }

    }
}


static void list_merge_sort(struct list_head *head)
{
    struct list_head list_left, list_right;
    struct listitem *item = NULL, *is = NULL;

    if (list_empty(head) || list_is_singular(head))
        return;

    INIT_LIST_HEAD(&list_left);
    INIT_LIST_HEAD(&list_right);
    list_split(head, &list_left, &list_right);
    list_merge_sort(&list_left);
    list_merge_sort(&list_right);
    list_merge(head, &list_left, &list_right);
}

int main(void)
{
    struct list_head testlist;
    struct listitem *item, *is = NULL;
    size_t i;

    random_shuffle_array(values, (uint16_t) ARRAY_SIZE(values));

    INIT_LIST_HEAD(&testlist);

    assert(list_empty(&testlist));
    printf("Before:");
    for (i = 0; i < ARRAY_SIZE(values); i++) {
        item = (struct listitem *) malloc(sizeof(*item));
        assert(item);
        item->i = values[i];
        entry_newline(i);
        printf("%4d ", item->i);
        list_add_tail(&item->list, &testlist);
    }

    assert(!list_empty(&testlist));

    qsort(values, ARRAY_SIZE(values), sizeof(values[0]), cmpint);
    list_merge_sort(&testlist);

    i = 0;
    printf("\n\nAfter:");
    list_for_each_entry_safe (item, is, &testlist, list) {
        assert(item->i == values[i]);
        entry_newline(i);
        printf("%4d ", item->i);
        list_del(&item->list);
        free(item);
        i++;
    }
    printf("\n");

    assert(i == ARRAY_SIZE(values));
    assert(list_empty(&testlist));

    return 0;
}
