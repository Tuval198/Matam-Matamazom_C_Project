#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include "amount_set.h"
#include "list.h"
#include "product.h"
#include "matamazom.h"
#include "order.h"
#include "matamazom_print.h"

#define MAX_INT_DIFFRENCE 0.001
#define Half 0.5
#define FIRST_ID 1

struct Matamazom_t{
    AmountSet products_set; //amount set of products
    List orders;
    unsigned int next_order_id;
    //TODO : check if need anything else and update create and destroy
};

Matamazom matamazomCreate(){
    Matamazom new_matamazom = malloc(sizeof(*new_matamazom));
    if(new_matamazom == NULL){
        return NULL;
    }

    new_matamazom->products_set = asCreate((CopyASElement) productCopy,
                              (FreeASElement) productDestroy, (CompareASElements) productCompare);
    if(new_matamazom->products_set == NULL){
        free(new_matamazom);
        return NULL;
    }

    new_matamazom->orders = listCreate((CopyListElement) orderCopy,(FreeListElement) orderDestroy);
    if(new_matamazom->orders == NULL){
        asDestroy(new_matamazom->products_set);
        free(new_matamazom);
        return NULL;
    }
    new_matamazom->next_order_id = FIRST_ID;
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

static bool isInt(const double amount){
    //returns true if number is int (with MAX_INT_DIFFRENCE mistake)
    double temp = floor(amount);
    if(amount - temp <= MAX_INT_DIFFRENCE ||
       temp + 1 - amount <= MAX_INT_DIFFRENCE){
        return true;
    }
    return false;
}

static bool isHalfInt(const double amount){
    //returns true if number is int or half int (with MAX_INT_DIFFRENCE mistake)
    if(isInt(amount) || isInt(amount + Half)){
        return true;
    }
    return false;
}
static bool amountTypeCorrect(MatamazomAmountType type ,const double amount){
    /*help function - gets amount and amount type and returns if the amount is
    from that type. does not handle NULL or wrong arguments */
    if(type == MATAMAZOM_INTEGER_AMOUNT){
        return (isInt(amount));
    }
    if(type == MATAMAZOM_HALF_INTEGER_AMOUNT){
        return (isHalfInt(amount));
    }
    if(type == MATAMAZOM_ANY_AMOUNT)
    {
        return true;
    }
    return false; //amount type does not exist
}

MatamazomResult mtmNewProduct(Matamazom matamazom, const unsigned int id,
                                const char *name, const double amount,
                                const MatamazomAmountType amountType,
                                const MtmProductData customData,
                                MtmCopyData copyData,MtmFreeData freeData,
                                MtmGetProductPrice prodPrice){
    if(matamazom == NULL || name == NULL || customData == NULL ||
        copyData == NULL || freeData == NULL || prodPrice == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    if( !((*name >= 'a' && *name <= 'z') || (*name >= 'A' && *name <= 'Z')
             || (*name >= '0' && *name <= '9'))  || (strlen(name) == 0) ){
        return MATAMAZOM_INVALID_NAME;
    }
    if( (amount <0) || !amountTypeCorrect(amountType,amount)){
        return MATAMAZOM_INVALID_AMOUNT;
    }

    Product product = productCreate(id,name,amountType,customData, copyData,
                                    freeData,prodPrice);
    if(product == NULL){
        return MATAMAZOM_OUT_OF_MEMORY;
    }
    AmountSetResult res = asRegister(matamazom->products_set, product);
    asChangeAmount(matamazom->products_set ,product ,amount );
    //now product is copied to the amount set, we should destroy our copy
    productDestroy(product);

    if(res == AS_ITEM_ALREADY_EXISTS){
        return MATAMAZOM_PRODUCT_ALREADY_EXIST;
    }
    else if (res == AS_SUCCESS){
        return MATAMAZOM_SUCCESS;
    }
    return MATAMAZOM_OUT_OF_MEMORY;
}

MatamazomResult mtmChangeProductAmount(Matamazom matamazom,
                                    const unsigned int id, const double amount){
    if(matamazom == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }

    AS_FOREACH(Product,current_product ,matamazom->products_set){
        if(current_product->id == id){
            if(! amountTypeCorrect(current_product->amount_type, amount)){
                return MATAMAZOM_INVALID_AMOUNT;
            }

            AmountSetResult res = asChangeAmount(matamazom->products_set,
                                     current_product, amount);
            assert(res != AS_NULL_ARGUMENT); //shouldn't happen

            if(res == AS_INSUFFICIENT_AMOUNT){
                return MATAMAZOM_INSUFFICIENT_AMOUNT;
            }
            return MATAMAZOM_SUCCESS;
        }
    }
    return MATAMAZOM_PRODUCT_NOT_EXIST;

}

MatamazomResult mtmClearProduct(Matamazom matamazom, const unsigned int id){

    if(matamazom == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }

    AS_FOREACH(Product, current_product, matamazom->products_set) {
        if (current_product->id == id) {
            //deleting product from all orders :
            LIST_FOREACH(Order, current_order, matamazom->orders) {
                if (asContains(current_order->products_busket, current_product)) {
                    asDelete(current_order->products_busket, current_product);
                }
            }

            //deleting product from matamazom :
            #ifndef NDEBUG
            AmountSetResult res =
            #endif
            asDelete(matamazom->products_set, current_product);
            assert(res != AS_ITEM_DOES_NOT_EXIST || res != AS_NULL_ARGUMENT);
            return MATAMAZOM_SUCCESS;
        }
    }
    //we only get here if product is not found in AS_FOREACH
    return MATAMAZOM_PRODUCT_NOT_EXIST;
}

unsigned int mtmCreateNewOrder(Matamazom matamazom){
    if(matamazom == NULL){
        return 0;
    }
    Order new_order = orderCreate(matamazom->next_order_id, (CopyASElement) productCopy,
                                    (FreeASElement) productDestroy, (CompareASElements) productCompare);
    if(new_order == NULL){
        return 0;
    }


    ListResult res = listInsertLast(matamazom->orders, new_order);
    //new_order is copied to the list, now we have to destroy it
    orderDestroy(new_order);
    if(res != LIST_SUCCESS){
        return 0;
    }
    matamazom->next_order_id ++;
    return matamazom->next_order_id - 1 ;
    //we return the old "next_order_id" (current id)
}

MatamazomResult mtmChangeProductAmountInOrder(Matamazom matamazom,
                                                const unsigned int orderId,
                                                const unsigned int productId,
                                                  const double amount){
    if(matamazom == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    //check if contains list ?
    Product product_to_add;
    bool product_in_matamazom = false;

    AS_FOREACH(Product, current_product, matamazom->products_set){
        if(current_product->id == productId){
            product_to_add = current_product;
            product_in_matamazom = true;
            if(!amountTypeCorrect(current_product->amount_type, amount)){
                return MATAMAZOM_INVALID_AMOUNT;
            }
        }
    }
    if(!product_in_matamazom){
        return MATAMAZOM_PRODUCT_NOT_EXIST;
    }

    LIST_FOREACH(Order, current_order, matamazom->orders){
        if(current_order->id == orderId){
            OrderResult res =
            orderChangeAmount(current_order, productId, amount, product_to_add);
            if(res == ORDER_OUT_OF_MEMORY){
                return MATAMAZOM_OUT_OF_MEMORY;
            }
            return MATAMAZOM_SUCCESS;
        }
    }
    return MATAMAZOM_ORDER_NOT_EXIST;
}

MatamazomResult mtmCancelOrder(Matamazom matamazom, const unsigned int orderId){
    if(matamazom == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    #ifndef NDEBUG
    ListResult res = LIST_SUCCESS;
    #endif
    LIST_FOREACH(Order, current_order, matamazom->orders) {
        if (current_order->id == orderId) {
            #ifndef NDEBUG
            res =
            #endif
            listRemoveCurrent(matamazom->orders);
            assert(res == LIST_SUCCESS);
            return MATAMAZOM_SUCCESS;
        }
    }
    //if we are here we've been throught all orders and didnt find the id
    return MATAMAZOM_ORDER_NOT_EXIST;
}

static Order orderFind(Matamazom matamazom, const unsigned int orderId){
    //return pointer to the order with orderId or NULL if not exists
    LIST_FOREACH(Order, current_order, matamazom->orders) {
        if (current_order->id == orderId) {
            return current_order;
        }
    }
    return NULL;
}

MatamazomResult mtmShipOrder(Matamazom matamazom, const unsigned int orderId){
    if(matamazom == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    //finding the order:
    Order order_to_ship = orderFind(matamazom, orderId);
    if(order_to_ship == NULL){
        return MATAMAZOM_ORDER_NOT_EXIST;
    }

    double temp_amount_in_order, temp_amount_in_warhouse;
    /*this loop only makes sure that order is legal*/
    AS_FOREACH(Product, current_product, order_to_ship->products_busket){
        asGetAmount(order_to_ship->products_busket,
                            current_product , &temp_amount_in_order);
       asGetAmount(matamazom->products_set, current_product,
                         &temp_amount_in_warhouse);
       if(temp_amount_in_order > temp_amount_in_warhouse){
           return MATAMAZOM_INSUFFICIENT_AMOUNT;
       }
    }//if we got here order is legal

    double profit_addition;
    //this ships the order assuming its legal
    AS_FOREACH(Product, current_product_order, order_to_ship->products_busket){
        asGetAmount(order_to_ship->products_busket,
                          current_product_order, &temp_amount_in_order);

        /****start of updating profit: ****/
        profit_addition = current_product_order->prod_price
                (current_product_order->product_data, temp_amount_in_order);
        //finding the product in matamazom:
        AS_FOREACH(Product, current_product_mtm, matamazom->products_set){
            if(productCompare(current_product_mtm, current_product_order) == 0){
                current_product_mtm->profits += profit_addition;
            }
        }
        /****end of updating profit****/

        /*changing product amount: */
        temp_amount_in_order = -temp_amount_in_order;
        //we want to decrease the amount from warhouse

        mtmChangeProductAmount(matamazom,
                     current_product_order->id, temp_amount_in_order);

    }
    mtmCancelOrder(matamazom, orderId);
    return MATAMAZOM_SUCCESS;
}

static void printProductsSet(AmountSet amount_set, FILE *output){
    /*this help function gets amount set and prints to file all
     * the products (without heading)*/
    double amount, price;
    AS_FOREACH(Product, current_product, amount_set){
        asGetAmount(amount_set, current_product, &amount);
        price = current_product->prod_price(current_product->product_data, 1);

        mtmPrintProductDetails(current_product->name, current_product ->id,
                               amount,price , output);
    }
    return;
}

MatamazomResult mtmPrintInventory(Matamazom matamazom, FILE *output){
    if(matamazom == NULL || output == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    fprintf(output, "Inventory Status:\n");
    printProductsSet(matamazom->products_set, output);
    return MATAMAZOM_SUCCESS;
}

MatamazomResult mtmPrintOrder(Matamazom matamazom, const unsigned int orderId,
                                            FILE *output){
    if(matamazom == NULL || output == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    Order order_ptr = orderFind(matamazom, orderId);
    if(order_ptr == NULL){
        return MATAMAZOM_ORDER_NOT_EXIST;
    }
    mtmPrintOrderHeading(orderId, output);

    double total_price = 0 , amount = 0 , price = 0;
    AS_FOREACH(Product, current_product, order_ptr->products_busket){
        asGetAmount(order_ptr->products_busket,
                                current_product, &amount);
        price = current_product->prod_price(current_product->product_data,
                                                                 amount);
        // we can't use printProductsSet because it prints price for 1 product
        mtmPrintProductDetails(current_product->name, current_product ->id,
                               amount, price , output);
        total_price += current_product->prod_price(current_product->product_data
                                                     , amount);
    }
    mtmPrintOrderSummary(total_price, output);
    return MATAMAZOM_SUCCESS;
}

MatamazomResult mtmPrintBestSelling(Matamazom matamazom, FILE *output){
    if(matamazom == NULL || output == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }
    fprintf(output, "Best Selling Product:\n");
    double max_profit = 0;
    Product best_selling = NULL; //pointer to best selling product
    AS_FOREACH(Product, current_product, matamazom->products_set){
        if(current_product->profits > max_profit){
            max_profit = current_product->profits;
            best_selling = current_product;
        }
    }

    if(max_profit == 0){
        fprintf(output, "none\n");
    }
    else {
        mtmPrintIncomeLine(best_selling->name, best_selling->id, max_profit,
                                                                      output);
    }
    return MATAMAZOM_SUCCESS;
}

MatamazomResult mtmPrintFiltered(Matamazom matamazom,
                            MtmFilterProduct customFilter, FILE *output){
    if(matamazom == NULL || customFilter == NULL || output == NULL){
        return MATAMAZOM_NULL_ARGUMENT;
    }

    double amount, price;
    AS_FOREACH(Product, current_product, matamazom->products_set){
        asGetAmount(matamazom->products_set, current_product, &amount);
        price = current_product->prod_price(current_product->product_data, 1);

        if( customFilter(current_product->id, current_product->name, amount,
                            current_product->product_data) ){
            mtmPrintProductDetails(current_product->name, current_product->id,
                                      amount, price, output);
        }
    }//AS_FOREACH
    return MATAMAZOM_SUCCESS;
}


