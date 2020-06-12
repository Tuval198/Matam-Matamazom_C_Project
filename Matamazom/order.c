#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "product.h"
#include "order.h"
#include "amount_set.h"


Order orderCreate(unsigned int id, CopyASElement product_copy,
                     FreeASElement product_destroy,
                     CompareASElements product_compare){
    if(product_compare == NULL || product_destroy == NULL
        || product_copy == NULL){
        return NULL;
    }
    Order new_order = malloc(sizeof(*new_order));
    if(new_order == NULL){
        return NULL;
    }
    new_order->products_busket =
                        asCreate(product_copy,product_destroy,product_compare);
    if(new_order->products_busket == NULL){
        free(new_order);
        return NULL;
    }

    new_order->id = id;
    return new_order;
}

Order orderCopy(Order order){
    if(order == NULL){
        return NULL;
    }

    Order new_order = malloc(sizeof(*new_order));
    if(new_order == NULL){
        return NULL;
    }

    AmountSet new_busket = asCopy(order->products_busket);
    if(new_busket == NULL){
        free(new_order);
        return NULL;
    }

    new_order->products_busket = new_busket;
    new_order->id = order->id;

    return new_order;
}

void orderDestroy(Order order){
    if(order == NULL){
        return;
    }
    asDestroy(order->products_busket);
    free(order);
}

OrderResult orderChangeAmount(Order order, const unsigned int productId,
                    const double amount, Product product){
    assert(order != NULL);
    AmountSetResult res;
    double new_amount;
    AS_FOREACH(Product, current_product, order->products_busket){
        if(current_product->id == productId){
            res = asChangeAmount(order->products_busket,
                                                    current_product, amount);
            asGetAmount(order->products_busket, current_product, &new_amount);
            if(res == AS_INSUFFICIENT_AMOUNT || new_amount == 0){
                //product has 0 or lower amount so we delete it
                asDelete(order->products_busket, current_product);
            }
            return ORDER_SUCSSES;
            //product already exist in order and we changed/deleted it
        }
    }

    //if we are here product is not in the order and we will add it
    if(amount < 0){
        return ORDER_BAD_AMOUNT;
    }
    res = asRegister(order->products_busket, product);
    assert(res != AS_ITEM_DOES_NOT_EXIST && res != AS_NULL_ARGUMENT);
    if(res == AS_OUT_OF_MEMORY){
        return ORDER_OUT_OF_MEMORY;
    }
    res = asChangeAmount(order->products_busket, product, amount);
    assert(res != AS_INSUFFICIENT_AMOUNT && res != AS_ITEM_DOES_NOT_EXIST
            && res != AS_NULL_ARGUMENT);
    return ORDER_SUCSSES;
}
