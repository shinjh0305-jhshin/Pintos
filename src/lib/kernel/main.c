#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#define SAME_STRING(a, b) strcmp((a), (b)) == 0

char list_name[20][15]; //list의 이름을 모아놓는 변수
struct list list_pool[15]; //list의 시작 끝을 모아놓는 변수
int lists; //lists : list의 개수


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

int main() {
    char temp[20], dsType[20], dsName[20]; //temp : 다목적, dsType : 객체 종류, dsName : 객체 이름
    
    while (1) {
        scanf("%s", temp);

        if (SAME_STRING(temp, "quit")) break; //끝!

        if (temp[0] == 'l') { //linked list와 관련된 함수일 경우 list 처리 함수를 호출한다.
            do_list(temp);
        }
        else if (temp[0] == 'c') { //새로운 객체를 만든다.
            scanf("%s %s", dsType, dsName); //어떤 객체 종류인지를 확인한다.

            if (SAME_STRING(dsType, "list")) { //새로운 list를 만드는 경우
                list_init(&list_pool[lists]);
                strcpy(list_name[lists++], dsName);
            }
        }
        else if (temp[0] == 'd') { //dumpdata or delete
            scanf("%s", dsName); //객체 이름부터 일단 받는다.
            for (int i = 0; i < lists; i++) {
                if (SAME_STRING(list_name[i], dsName)) {
                    if (temp[1] == 'u') { //dumpdata
                        list_print(&list_pool[i]);
                    }
                    else { //delete
                        list_free(&list_pool[i]);
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
