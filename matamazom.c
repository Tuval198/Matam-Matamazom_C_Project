#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "amount_set.h"
#include "list.h"
#include "product.h"
#include "matamazom.h"

struct Matamazom_t{
    AmountSet products_set; //amount set of products
    List orders;
    //TODO : check if need anything else and update create and destroy
};

Matamazom matamazomCreate(){
    Matamazom new_matamazom = malloc(sizeof(*new_matamazom));
    if(new_matamazom == NULL){
        return NULL;
    }

    new_matamazom->products_set = asCreate(productCopy,
                     productDestroy, productCompare);
    if(new_matamazom->products_set == NULL){
        free(new_matamazom);
        return NULL;
    }

    new_matamazom->orders = listCreate(orderCopy,orderDestroy);
    if(new_matamazom->orders == NULL){
        asDestroy(new_matamazom->products_set);
        free(new_matamazom);
        return NULL;
    }

    return new_matamazom;
}

void matamazomDestroy(Matamazom matamazom){
    if(matamazom == NULL){
        return;
    }
    asDestroy(matamazom->products_set);
    listDestroy(matamazom->orders);
    free(matamazom);
}
//MatamazomResult mtmNewProduct(Matamazom matamazom, const unsigned int id,
//                                const char *name, const double amount,
//                                const MatamazomAmountType amountType,
//                                const MtmProductData customData,
//                                MtmCopyData copyData,MtmFreeData freeData,
//                                MtmGetProductPrice prodPrice){
//    productCreate(/*TODO*/);
//
//
//
//}
//
//atamazomResult mtmClearProduct(Matamazom matamazom, const unsigned int id){
//
//    /*TODO*/
//}