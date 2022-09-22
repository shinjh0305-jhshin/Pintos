#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include "hash.h"
#define SAME_STRING(a, b) strcmp((a), (b)) == 0

char list_name[20][15]; //list의 이름을 모아놓는 변수
char hash_name[20][15]; //hash의 이름을 모아놓는 변수
struct list list_pool[15]; //list의 시작 끝을 모아놓는 변수
struct hash hash_pool[15];
int lists, hashes; //lists : list의 개수, hashes : hash의 개수

/* list_XXX 형태를 갖는 모든 operation을 관할*/
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
    else if (operation[5] == 's') { //swap, splice, shuffle, sort
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

int main() {
    char temp[20], dsType[20], dsName[20]; //temp : 다목적, dsType : 객체 종류, dsName : 객체 이름
    
    while (1) {
        scanf("%s", temp);

        if (SAME_STRING(temp, "quit")) break; //끝!

        if (temp[0] == 'l') { //linked list와 관련된 함수일 경우 list 처리 함수를 호출한다.
            do_list(temp);
        }
        else if(temp[0] == 'h') { //hash와 관련된 함수일 경우 list 처리 함수를 호출한다.
            do_hash(temp); 
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
                }
            }

        }
        else {
            printf("Invalid Input has been applied.\n");
            return 1;
        }
    }

    return 0;
}
