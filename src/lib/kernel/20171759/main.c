#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include "hash.h"
#include "bitmap.h"
#define SAME_STRING(a, b) strcmp((a), (b)) == 0

char list_name[20][15]; //list의 이름을 모아놓는 변수
char hash_name[20][15]; //hash의 이름을 모아놓는 변수
char bitmap_name[20][15]; //bitmap의 이름을 모아놓는 변수
struct list list_pool[15]; //list의 시작 끝을 모아놓는 변수
struct hash hash_pool[15]; //hash 객체를 모아놓는 변수
struct bitmap* bitmap_pool[15]; //bitmap 객체를 모아놓는 변수
int lists, hashes, bitmaps; //lists : list의 개수, hashes : hash의 개수, bitmaps : bitmap의 개수

/* list_XXX 형태를 갖는 모든 operation을 관할 */
void do_list(char* operation) { //list 처리 함수
    char dsName[20]; //list의 이름
    int listIdx, value; //list의 index

    scanf("%s", dsName);

    for (int i = 0; i < lists; i++) {
        if (SAME_STRING(dsName, list_name[i])) {
            listIdx = i;
            break;
        }
    }

    /* list_XXX의 형태로 모든 operation이 들어오기에, XXX의 가장 첫 글자를 기준으로 분류한다.*/
    if (operation[5] == 'p') { //push or pop
        if (operation[6] == 'u') { //push
            scanf("%d", &value); //list에 넣을 값을 받는다.
            
            struct node* temp = malloc(sizeof(struct node)); //new node
            temp->data = value;

            if (operation[10] == 'b') { //push_back
                list_push_back(&list_pool[listIdx], &(temp->elem));
            }
            else if (operation[10] == 'f') { //push_front
                list_push_front(&list_pool[listIdx], &(temp->elem));
            }
        }
        else if (operation[6] == 'o') { //pop
            if (operation[9] == 'b') { //pop_back
                list_pop_back(&list_pool[listIdx]);
            }
            else if (operation[9] == 'f') { //pop_front
                list_pop_front(&list_pool[listIdx]);
            }
        }
    }
    else if (operation[5] == 'r') { //remove or reverse
        if (operation[7] == 'm') { //remove
            scanf("%d", &value);

            struct list_elem* mov = list_begin(&list_pool[listIdx]); //첫 번째 원소로 간다.
            while (value--) { //입력받은 위치까지 iteration
                mov = list_next(mov);
            }
            list_remove(mov);
        }
        else if (operation[7] == 'v') { //reverse
            list_reverse(&list_pool[listIdx]);
        }
    }
    else if (operation[5] == 's') { //swap, splice, shuffle, sort, size
        if (operation[6] == 'h') { //shuffle
            list_shuffle(&list_pool[listIdx]);
        }
        else if (operation[6] == 'o') { //sort
            list_sort(&list_pool[listIdx], list_compare, NULL);
        }
        else if (operation[6] == 'p') { //splice
            int pos, from, to, targetIdx; //pos 앞에 from부터 to까지를 삽입한다.
            char targetList[20];

            scanf("%d %s %d %d", &pos, targetList, &from, &to);

            for (int i = 0; i < lists; i++) { //targetList를 찾는다.
                if (SAME_STRING(targetList, list_name[i])) {
                    targetIdx = i;
                    break;
                }
            }

            //list iterator
            struct list_elem* _pos = list_begin(&list_pool[listIdx]); //targetPosition
            struct list_elem* _from = list_begin(&list_pool[targetIdx]), *_to = list_begin(&list_pool[targetIdx]);

            for (int i = 0; i < pos; i++) _pos = list_next(_pos);
            for (int i = 0; i < from; i++) _from = list_next(_from);
            for (int i = 0; i < to; i++) _to = list_next(_to);

            list_splice(_pos, _from, _to);
        }
        else if (operation[6] == 'w') { //swap
            int pos1, pos2;
            scanf("%d %d", &pos1, &pos2);

            struct list_elem* _pos1 = list_begin(&list_pool[listIdx]);
            struct list_elem* _pos2 = list_begin(&list_pool[listIdx]);

            for (int i = 0; i < pos1; i++) _pos1 = list_next(_pos1);
            for (int i = 0; i < pos2; i++) _pos2 = list_next(_pos2);

            list_swap(_pos1, _pos2);
        }
        else if (operation[6] == 'i') { //size
            printf("%zu\n", list_size(&list_pool[listIdx]));
        }
    }
    else if (operation[5] == 'u') {
        char duplicates[20]; //duplicate list name
        int dupIdx = -1; //duplicates' index. -1 is a flag of NULL.

        if (getchar() != '\n') { //duplicates가 입력으로 주어지는지 확인한다.
            scanf("%s", duplicates);
            for (int i = 0; i < lists; i++) {
                if (SAME_STRING(duplicates, list_name[i])) {
                    dupIdx = i;
                    break;
                }
            }
        }

        if (dupIdx == -1) list_unique(&list_pool[listIdx], NULL, list_compare, NULL); //duplicates가 주어지지 않은 경우
        else list_unique(&list_pool[listIdx], &list_pool[dupIdx], list_compare, NULL);
    }
    else if (operation[5] == 'f') { //front
        printf("%d\n", list_entry(list_front(&list_pool[listIdx]), struct node, elem)->data);
    }
    else if (operation[5] == 'b') { //back
        printf("%d\n", list_entry(list_back(&list_pool[listIdx]), struct node, elem)->data);
    }
    else if (operation[5] == 'i') { //insert, insert_ordered
        if (strlen(operation) < 15) { //insert
            struct list_elem* mov = list_begin(&list_pool[listIdx]); //list traverse
            struct node* temp = malloc(sizeof(struct node)); //new node
            
            int pos; //Where to insert
            scanf("%d %d", &pos, &value); 
            temp->data = value;

            while (pos--) { //traverse mov to pos
                mov = list_next(mov);
            }

            list_insert(mov, &(temp->elem));
        }  
        else { //insert_ordered
            list_sort(&list_pool[listIdx], list_compare, NULL); //우선 정렬을 진행한다.

            scanf("%d", &value);
            struct node* temp = malloc(sizeof(struct node)); //new node
            temp->data = value;

            list_insert_ordered(&list_pool[listIdx], &(temp->elem), list_compare, NULL);
        }
    }
    else if (operation[5] == 'e') { //empty
        if (list_empty(&list_pool[listIdx])) printf("true\n");
        else printf("false\n");
    }
    else if (operation[5] == 'm') { //max or min
        if (operation[6] == 'a') { //max
            printf("%d\n", list_entry(list_max(&list_pool[listIdx], list_compare, NULL), struct node, elem)->data);
        }
        else if (operation[6] == 'i') { //min
            printf("%d\n", list_entry(list_min(&list_pool[listIdx], list_compare, NULL), struct node, elem)->data);
        }
    }
}

/* hash_XXX 형태를 갖는 모든 operation을 관할 */
void do_hash(char* operation) {
    char dsName[20]; //hash의 이름
    int hashIdx, value; //hash의 index

    scanf("%s", dsName);

    for (int i = 0; i < hashes; i++) {
        if (SAME_STRING(dsName, hash_name[i])) {
            hashIdx = i;
            break;
        }
    }

    if (operation[5] == 'i') { //insert
        scanf("%d", &value); //hash에 넣을 값을 찾는다.
        struct hash_node* temp = malloc(sizeof(struct hash_node));

        temp->data = value;
        hash_insert(&hash_pool[hashIdx], &(temp->elem));
    }
    else if (operation[5] == 'd') { //delete
        scanf("%d", &value); //삭제 할 값을 입력받는다
        struct hash_node temp;
        temp.data = value;

        hash_delete(&hash_pool[hashIdx], &(temp.elem)); //삭제 대상 값이 있는 hash_elem을 전달한다
    }
    else if (operation[5] == 'e') { //empty
        if (hash_empty(&hash_pool[hashIdx])) printf("true\n");
        else printf("false\n");
    }
    else if (operation[5] == 's') { //size
        printf("%zu\n", hash_size(&hash_pool[hashIdx]));
    }
    else if (operation[5] == 'c') { //clear
        hash_clear(&hash_pool[hashIdx], NULL);
    }
    else if (operation[5] == 'f') { //find
        scanf("%d", &value);
        struct hash_node target;
        target.data = value;

        struct hash_elem* temp = hash_find(&hash_pool[hashIdx], &(target.elem));

        if (temp) //찾은 hash_elem이 존재하는 경우에만 print를 한다.
            printf("%d\n", hash_entry(temp, struct hash_node, elem)->data);
    }
    else if (operation[5] == 'r') { //replace
        scanf("%d", &value);
        struct hash_node* temp = malloc(sizeof(struct hash_node));

        temp->data = value;
        hash_replace(&hash_pool[hashIdx], &(temp->elem));
    }
    else if (operation[5] == 'a') {
        char powerOf[10]; //각 원소를 몇 승 할지 받는다
        scanf("%s", powerOf);

        if (powerOf[0] == 's') { //제곱
            hash_apply(&(hash_pool[hashIdx]), hash_action_square);
        }
        else if (powerOf[0] == 't') { //세제곱
            hash_apply(&(hash_pool[hashIdx]), hash_action_triple);
        }
    }
}

/* bitmap_XXX 형태를 갖는 모든 operation을 관할 */
void do_bitmap(char* operation) {
    char dsName[20]; //bitmap의 이름
    int bmIdx, value; //bitmap의 index

    scanf("%s", dsName);

    for (int i = 0; i < bitmaps; i++) {
        if (SAME_STRING(dsName, bitmap_name[i])) {
            bmIdx = i;
            break;
        }
    }

    if (operation[7] == 'a') { //all, any
        size_t startIdx, sz; //startIdx 부터 sz만큼의 bit를 전부 true로 바꾼다
        scanf("%zu %zu", &startIdx, &sz);

        if (operation[8] == 'l') { //all
            if (bitmap_all(bitmap_pool[bmIdx], startIdx, sz)) printf("true\n"); 
            else printf("false\n");
        }
        else if (operation[8] == 'n') { //any
            if (bitmap_any(bitmap_pool[bmIdx], startIdx, sz)) printf("true\n");
            else printf("false\n");
        }
    
    }
    else if (operation[7] == 'c') { //contains, count
        size_t startIdx, sz; //from ,to
        char testValue[8]; //boolean value
        bool value; //testValue into boolean type

        scanf("%zu %zu %s", &startIdx, &sz, testValue);

        value = testValue[0] == 't' ? true : false;

        if (operation[9] == 'n') { //contains
            if (bitmap_contains(bitmap_pool[bmIdx], startIdx, sz, value)) printf("true\n");
            else printf("false\n");
        }
        else if (operation[9] == 'u') { //count
            printf("%zu\n", bitmap_count(bitmap_pool[bmIdx], startIdx, sz, value));
        }
    }
    else if (operation[7] == 'd') { //dump
        bitmap_dump(bitmap_pool[bmIdx]);
    }
    else if (operation[7] == 'e') { //expand
        scanf("%d", &value);
        bitmap_pool[bmIdx] = bitmap_expand(bitmap_pool[bmIdx], value);
    }
    else if (operation[7] == 'f') { //flip
        scanf("%d", &value);
        bitmap_flip(bitmap_pool[bmIdx], value);
    }
    else if (operation[7] == 'm') { //mark
        size_t idx; //where to mark
        scanf("%zu", &idx);

        bitmap_mark(bitmap_pool[bmIdx], idx);
    }
    else if (operation[7] == 'n') { //none
        size_t startIdx, sz; //from ,to
        scanf("%zu %zu", &startIdx, &sz);

        if (bitmap_none(bitmap_pool[bmIdx], startIdx, sz)) printf("true\n");
        else printf("false\n");
    }
    else if (operation[7] == 'r') { //reset
        scanf("%d", &value);
        bitmap_reset(bitmap_pool[bmIdx], value);
    }
    else if (operation[7] == 's') {
        size_t startIdx, sz; //from ,to
        char testValue[8]; //boolean value
        bool value; //testValue into boolean type

        if (operation[8] == 'c') { //scan or scan_and_flip
            scanf("%zu %zu %s", &startIdx, &sz, testValue);
            value = testValue[0] == 't' ? true : false;

            if (strlen(operation) < 15) { //scan
                printf("%zu\n", bitmap_scan(bitmap_pool[bmIdx], startIdx, sz, value));
            }
            else { //scan_and_flip
                printf("%zu\n", bitmap_scan_and_flip(bitmap_pool[bmIdx], startIdx, sz, value));
            }
        }
        else if (operation[8] == 'e') {
            if (strlen(operation) == 10) { //set
                scanf("%zu %s", &startIdx, testValue);
                value = testValue[0] == 't' ? true : false;

                bitmap_set(bitmap_pool[bmIdx], startIdx, value);
            }
            else if (operation[11] == 'a') { //set_all
                scanf("%s", testValue);
                value = testValue[0] == 't' ? true : false;

                bitmap_set_all(bitmap_pool[bmIdx], value);
            }
            else if (operation[11] == 'm') { //set_multiple
                scanf("%zu %zu %s", &startIdx, &sz, testValue);
                value = testValue[0] == 't' ? true : false;

                bitmap_set_multiple(bitmap_pool[bmIdx], startIdx, sz, value);
            }
        }
        else if (operation[8] == 'i') { //size
            printf("%zu\n", bitmap_size(bitmap_pool[bmIdx]));
        }
    }
    else if (operation[7] == 't') { //test
        scanf("%d", &value);
        if (bitmap_test(bitmap_pool[bmIdx], value)) printf("true\n");
        else printf("false\n");
    }
}

int main() {
    char temp[30], dsType[20], dsName[20]; //temp : 다목적, dsType : 객체 종류, dsName : 객체 이름
    
    while (1) {
        scanf("%s", temp);

        if (SAME_STRING(temp, "quit")) break; //끝!

        if (temp[0] == 'l') { //linked list와 관련된 함수일 경우 list 처리 함수를 호출한다.
            do_list(temp);
        }
        else if (temp[0] == 'h') { //hash와 관련된 함수일 경우 hash 처리 함수를 호출한다.
            do_hash(temp); 
        }
        else if (temp[0] == 'b') { //bitmap과 관련된 함수일 경우 bitmap 처리 함수를 호출한다.
            do_bitmap(temp);
        }
        else if (temp[0] == 'c') { //새로운 객체를 만든다.
            scanf("%s %s", dsType, dsName); //어떤 객체 종류인지를 확인한다.

            if (SAME_STRING(dsType, "list")) { //새로운 list를 만드는 경우
                list_init(&list_pool[lists]);
                strcpy(list_name[lists++], dsName);
            }
            else if (SAME_STRING(dsType, "hashtable")) { //새로운 hash를 만드는 경우
                hash_init(&hash_pool[hashes], hash_function, hash_compare, NULL);
                strcpy(hash_name[hashes++], dsName);
            }
            else if (SAME_STRING(dsType, "bitmap")) { //새로운 bitmap를 만드는 경우
                size_t szbitmap; //bitmap size
                scanf("%zu", &szbitmap);

                bitmap_pool[bitmaps] = bitmap_create(szbitmap);
                strcpy(bitmap_name[bitmaps++], dsName);
            }
        }
        else if (temp[0] == 'd') { //dumpdata or delete
            scanf("%s", dsName); //객체 이름부터 일단 받는다.
            for (int i = 0; i < lists; i++) { //list 자료구조에 있는 이름인지 확인한다
                if (SAME_STRING(list_name[i], dsName)) {
                    if (temp[1] == 'u') { //dumpdata list
                        list_print(&list_pool[i]);
                    }
                    else { //delete list
                        list_free(&list_pool[i]);
                    }
                    goto next;
                }
            }
            for (int i = 0; i < hashes; i++) {
                if (SAME_STRING(hash_name[i], dsName)) {
                    if (temp[1] == 'u') { //dumpdata hash
                        if (hash_empty(&hash_pool[i])) continue;
                        hash_apply(&hash_pool[i], hash_action_print);
                        printf("\n");
                    }
                    else { //delete hash
                        hash_destroy(&hash_pool[i], NULL);
                    }
                    goto next;
                }
            }
            for (int i = 0; i < bitmaps; i++) {
                if (SAME_STRING(bitmap_name[i], dsName)) {
                    if (temp[1] == 'u') { //dumpdata bitmap
                        bitmap_print(bitmap_pool[i]);
                    }
                    else { //delete bitmap
                        bitmap_destroy(bitmap_pool[i]);
                    }
                    goto next;
                }
            }
        }
        else {
            printf("Invalid Input has been applied.\n");
            return 1;
        }
        next:; //for dumpdata & delete
    }

    return 0;
}
