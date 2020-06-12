

#include <stdlib.h>
#include <stdio.h>

#include "amount_set.h"

#define SET_IS_NULL -1

/*element list contains first "FAKE" element when set is created*/
typedef struct element_list{
    ASElement element;
    double amount;
    struct element_list* next;
}*ElementList;

struct AmountSet_t{
    ElementList element_list; //point to first node (which is fake)
    ElementList iterator; //not dependent pointer to element inside the list
    /* TODO : Check if we need anything else and add it at creation function*/

    CopyASElement element_copy;
    FreeASElement element_free;
    CompareASElements element_compare;
};

AmountSet asCreate(CopyASElement copyElement,
                   FreeASElement freeElement,
                   CompareASElements compareElements){
    if(copyElement == NULL || freeElement == NULL || compareElements == NULL){
        return NULL;
    }

    AmountSet set = malloc(sizeof(*set));
    if(set == NULL){
        return NULL;
    }

    //creating "FAKE" node in elements list :
    ElementList first_node = malloc(sizeof(*first_node));
    if(first_node == NULL){

        free(set);
        return NULL;
    }
    first_node->amount = 0;
    first_node->element = NULL;
    first_node->next = NULL;
    set->iterator = first_node->next; //iterator starts at the first "element"
    set->element_list = first_node;
    //functions
    set->element_copy = copyElement;
    set ->element_free = freeElement;
    set->element_compare = compareElements;

    return set;
}

static void DeleteFirstElement(AmountSet set){
    //help function, clears the first element
    if( set->element_list->next == NULL){
        return;
    }
    ElementList to_delete = set->element_list->next;
    set->element_list->next = to_delete->next;

    set->element_free(to_delete->element);
    free(to_delete);

}

AmountSetResult asClear(AmountSet set){
    if(set == NULL){
        return AS_NULL_ARGUMENT ;
    }

    while(set->element_list->next != NULL)
    {//will finish when there is no element except the "fake" one
        DeleteFirstElement(set);
    }
    set->iterator = set->element_list ->next; //for safety (iterator shouldnt
                                        // be used after clear function)
    return AS_SUCCESS;
}

void asDestroy(AmountSet set){
    if(set == NULL){
        return;
    }
    asClear(set);
    free(set->element_list); //free the "fake" first node
    free(set);
}

ASElement asGetFirst(AmountSet set) {
    if(set == NULL){
        return NULL;
    }
    if(set->element_list->next == NULL){
        return NULL;
    }
    set->iterator = set->element_list->next;

    return set->iterator->element;
}

ASElement asGetNext(AmountSet set){
    if(set == NULL || set->iterator == NULL){
        //in our code iterators always points element node or null
        return NULL;
    }
    set->iterator = set->iterator->next;
    if(set->iterator == NULL){
    	return NULL;
    }

    return set ->iterator->element;
}

int asGetSize(AmountSet set){
    if(set == NULL){
        return SET_IS_NULL;
    }
    ElementList ptr = set->element_list->next; //cant use iterator
    int counter = 0;
    while(ptr != NULL)
    {
        counter ++;
        ptr = ptr->next;
    }

    return counter;
}

static ElementList FindElement(AmountSet set, ASElement element){
    /*help function to find given element and return pointer to the list node
    that contains that element or "NULL" if it is not in the set*/
    if(set == NULL || element == NULL){
        return NULL;
    }

    ElementList ptr = set->element_list->next;
    while(ptr != NULL) {
        if (set->element_compare(ptr->element, element) == 0) {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

bool asContains(AmountSet set, ASElement element){
    if(set == NULL || element == NULL){
        return false;
    }
    ElementList ptr = FindElement(set,element);
    //the FindElement funtion returns "NULL" if the item is not in the set
    return (ptr != NULL);
}

AmountSetResult asGetAmount(AmountSet set
                             , ASElement element,double *outAmount){
    if(set == NULL || element == NULL || outAmount == NULL){
        return AS_NULL_ARGUMENT;
    }

    ElementList current_element = FindElement(set,element);
    if(current_element == NULL){ //the item is not in the list
        return AS_ITEM_DOES_NOT_EXIST;
    }
     *outAmount = current_element->amount;

     return AS_SUCCESS;
}

AmountSetResult asChangeAmount(AmountSet set,
                                ASElement element, const double amount){
    if(set == NULL || element == NULL){
        return AS_NULL_ARGUMENT;
    }

    ElementList  current_element = FindElement(set, element);
    if(current_element == NULL){
        return AS_ITEM_DOES_NOT_EXIST;
    }
    if((current_element->amount + amount) < 0 ){
        return AS_INSUFFICIENT_AMOUNT;
    }
    current_element->amount += amount;
    return AS_SUCCESS;

}

AmountSetResult asRegister(AmountSet set, ASElement element){
    if(set == NULL || element == NULL){
        return AS_NULL_ARGUMENT;
    }
    if(asContains(set, element)){
        return AS_ITEM_ALREADY_EXISTS;
    }

    ElementList current_element = set->element_list->next;
    //points to first elements node
    ElementList previous_element = set->element_list;
    //points to first "FAKE" node
    while(current_element != NULL &&
            set->element_compare(element, current_element->element) > 0){
                         //element is bigger then current_element
        previous_element = current_element ;
        current_element = current_element->next;
    }

    ElementList new_node = malloc(sizeof(*new_node));
    if(new_node == NULL){

        return AS_OUT_OF_MEMORY;
    }
    new_node->amount = 0;
    new_node->element = set->element_copy(element);
    new_node->next = current_element; //insert before bigger one

    previous_element->next = new_node;

    return AS_SUCCESS;
}

AmountSetResult asDelete(AmountSet set, ASElement element){
    if(set == NULL || element == NULL)
    {
        return AS_NULL_ARGUMENT;
    }
    if(!asContains(set,element)){
        return AS_ITEM_DOES_NOT_EXIST;
    }

    ElementList current = set->element_list->next;
    //points to first elements node
    ElementList previous = set->element_list;
    //points to first "FAKE" node
    while((set->element_compare(element,current->element)) != 0){
        previous = current;
        current = current->next;
    }
    previous->next = current->next;

    //memory deallocate:
    set->element_free(current->element);
    free(current);

    return AS_SUCCESS;
}

AmountSet asCopy(AmountSet set){
    if(set == NULL){
        return NULL;
    }
    AmountSet new_set = asCreate(set->element_copy,set->element_free,
                                                  set->element_compare);

    if(asGetSize(set) == 0){//if the set is empty we are done
        return new_set;
    }

    AmountSetResult result = asRegister(new_set,asGetFirst(set));
    //copy element and set amount = 0
    if(result == AS_NULL_ARGUMENT || result == AS_ITEM_ALREADY_EXISTS
        || result == AS_OUT_OF_MEMORY){
        asDestroy(new_set);
        return NULL;
    }
    asChangeAmount(new_set, set->iterator->element, set->iterator->amount); //copy amount

    while(set->iterator->next != NULL){
        result = asRegister(new_set,asGetNext(set));
        //move iterator and then copy element
        if(result == AS_NULL_ARGUMENT || result == AS_ITEM_ALREADY_EXISTS
           || result == AS_OUT_OF_MEMORY){
            asDestroy(new_set);
            return NULL;
        }
        asChangeAmount(new_set, set->iterator->element, set->iterator->amount);
    }

    return new_set;
}
