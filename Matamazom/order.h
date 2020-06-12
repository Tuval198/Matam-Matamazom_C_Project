#ifndef ORDER_H
#define ORDER_H

#include <stdlib.h>
#include <stdio.h>
#include "amount_set.h"

typedef enum OrderResult_t {
    ORDER_SUCSSES = 0,
    ORDER_OUT_OF_MEMORY,
    ORDER_BAD_AMOUNT
} OrderResult;

/*the struct is in the header file because we dont want to use this data type
 * as an ADT, explanation for this in the dry pdf */
typedef struct order_t{
    unsigned int id;
    AmountSet products_busket;
}*Order;

/* orderCreate : creates a new order and returns pointer to it
 * returns null in case of failure or one of the arguments is null*/
Order orderCreate(unsigned int id, CopyASElement product_copy,
                  FreeASElement product_destroy,
                  CompareASElements product_compare);

/* orderCopy : gets order as argument and returns copy of that order
 * returns NULL if gets NULL*/
Order orderCopy(Order order);

/*orderDestroy : gets order and deallocates it's contents from memory
 * NULL value does nothing*/
void orderDestroy(Order order);

OrderResult orderChangeAmount(Order order, const unsigned int productId,
                        const double amount, Product product);


#endif //ORDER_H
