#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
//we need matamazom.h for specific datatypes like MtmProductData
#include "product.h"


Product productCreate(const unsigned int id, const char *name,
                      ProductAmountType amountType,
                      ProductData customData,
                      ProductCopyData copyData,ProductFreeData freeData,
                      GetProductPrice prodPrice){
    if(name == NULL || customData == NULL || copyData == NULL
       || freeData == NULL || prodPrice == NULL){
        return NULL;
    }

    Product temp_product = malloc(sizeof(*temp_product)); //to be returned
    if(temp_product == NULL){
        return NULL;
    }

    temp_product->product_data = copyData(customData);
    if (temp_product->product_data == NULL){
        free(temp_product);
        return NULL;
    }

    temp_product->name = malloc( sizeof(char) * (strlen(name) + 1) ) ;
    if(temp_product->name == NULL){
        freeData(temp_product->product_data);
        free(temp_product);
        return NULL;
    }
    strcpy(temp_product->name, name);

    temp_product->id = id;
    temp_product->amount_type = amountType;
    temp_product->profits = 0 ; //starts without profits

    temp_product->copy_data = copyData;
    temp_product->free_data = freeData;
    temp_product->prod_price = prodPrice;

    return temp_product;
}

void productDestroy(Product product){
    if(product == NULL){
        return;
    }
    product->free_data(product->product_data);
    free(product->name);
    free(product);
}

int productCompare(Product product1, Product product2){
    //products are compared by id (higher id means "bigger" product)
    assert(product1 != NULL && product2 != NULL);
    return (product1->id) - (product2->id);
}

Product productCopy(Product product)
{
    assert(product != NULL);
    Product new_product = productCreate(product->id,product->name,
            product->amount_type, product->product_data, product->copy_data,
            product->free_data, product->prod_price);

    return new_product;
}

double productGetPrice(Product product,const double amount){
    assert(product != NULL);
    return product->prod_price(product->product_data, amount);
}
