#include <stdio.h>
#include <stdbool.h>

#ifndef PRODUCT_H
#define PRODUCT_H

typedef struct product_t{
    unsigned int id;
    MatamazomAmountType amount_type;
    double profits; //saves the profits from that product
    /*TODO add more if needed and update createProduct and destroy */

    //pointers:
    char *name; //important : should copy the string, not the pointer
    MtmProductData product_data;//pointer to specific and generic data

    //function pointers :
    MtmCopyData copy_data;
    MtmFreeData free_data;
    MtmGetProductPrice prod_price;
} *Product;

/* productCreate : creates a new product and returns pointer to it
 * returns null in case of failure or one of the arguments is null
 * products profit will be zero*/
Product productCreate(const unsigned int id, const char *name,
                      const MatamazomAmountType amountType,
                      const MtmProductData customData,
                      MtmCopyData copyData,MtmFreeData freeData,
                      MtmGetProductPrice prodPrice);

/* productDestroy : free product and all its contents from memory
 * NULL value is allowed (does nothing)*/
void productDestroy(Product product);

/* productCompare : products are compared by id.
 * returns 0 if they are identical
 * returns positive number if the first products id is greater
 * returns negative number if the second products id is greater
 * can't get null product*/
int productCompare(Product product1, Product product2);

/* productCopy : gets product as argument and returns copy of that product
 * can't get NULL*/
Product productCopy(Product product);

/* productGetPrice : gets a pointer to product and amount
 * returns type for that amount of product
 * can't get null product*/
double productGetPrice(Product product,const double amount);

#endif //PRODUCT_H
