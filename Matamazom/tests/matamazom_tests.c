#include "matamazom_tests.h"
#include "../matamazom.h"
#include "test_utilities.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

// OUTPUT_DIR should be relative to the directory from which the exectuable
// 'matamazon' will be ran.
#define OUTPUT_DIR "./tests/"
#define INVENTORY_OUT_FILE OUTPUT_DIR "printed_inventory.txt"
#define INVENTORY_TEST_FILE OUTPUT_DIR "expected_inventory.txt"
#define INVENTORY_SHIP_OUT_FILE OUTPUT_DIR "printed_inventory_ship.txt"
#define INVENTORY_SHIP_TEST_FILE OUTPUT_DIR "expected_inventory_ship.txt"
#define ORDER_OUT_FILE OUTPUT_DIR "printed_order.txt"
#define ORDER_TEST_FILE OUTPUT_DIR "expected_order.txt"
#define ORDER1_OUT_FILE OUTPUT_DIR "printed_order1.txt"
#define ORDER1_TEST_FILE OUTPUT_DIR "expected_order1.txt"
#define ORDER2_OUT_FILE OUTPUT_DIR "printed_order2.txt"
#define ORDER2_TEST_FILE OUTPUT_DIR "expected_order2.txt"
#define ORDER3_OUT_FILE OUTPUT_DIR "printed_order3.txt"
#define ORDER3_TEST_FILE OUTPUT_DIR "expected_order3.txt"
#define BEST_SELLING_OUT_FILE OUTPUT_DIR "printed_best_selling.txt"
#define BEST_SELLING_TEST_FILE OUTPUT_DIR "expected_best_selling.txt"
#define BEST_SELLING2_OUT_FILE OUTPUT_DIR "printed_best_selling2.txt"
#define BEST_SELLING2_TEST_FILE OUTPUT_DIR "expected_best_selling2.txt"
#define NO_SELLING_OUT_FILE OUTPUT_DIR "printed_no_selling.txt"
#define NO_SELLING_TEST_FILE OUTPUT_DIR "expected_no_selling.txt"
#define FILTERED_OUT_FILE OUTPUT_DIR "printed_filtered.txt"
#define FILTERED_TEST_FILE OUTPUT_DIR "expected_filtered.txt"
#define FILTERED_OUT_FILE_2 OUTPUT_DIR "printed_filtered_2.txt"
#define FILTERED_TEST_FILE_2 OUTPUT_DIR "expected_filtered_2.txt"

#define ASSERT_OR_DESTROY(expr) ASSERT_TEST_WITH_FREE((expr), matamazomDestroy(mtm))

bool testCreate() {
    Matamazom mtm = matamazomCreate();
    ASSERT_TEST(mtm != NULL);
    matamazomDestroy(mtm);
    return true;
}

bool testDestroy() {
    Matamazom mtm = matamazomCreate();
    matamazomDestroy(mtm);
    /* try to elicit undefined behavior in case matamazomDestroy has bugs */
    Matamazom mtm2 = matamazomCreate();
    ASSERT_TEST(mtm2 != NULL);
    matamazomDestroy(mtm2);
    return true;
}

/// FINAL TESTS PRODUCT TYPE & FUNCTIONS ///
typedef struct customData_t{
    double  basePrice;
    double  discountPrecentage;
    double  amountForPrecentageDiscount;
    double  minAmount;
}CustomData, *pCustomData;

//interface function for product with price percentage
static double getPricePercentage(MtmProductData data, double amount){
    if(!data || amount < 0){
        return -1;
    }
    pCustomData prod = (pCustomData)data;
    double initialPrice = amount * prod->basePrice;
    double finalPrice = initialPrice;
    if(amount >= prod->amountForPrecentageDiscount){
        finalPrice = initialPrice * prod->discountPrecentage;
    }
    return finalPrice;
}

static double getPriceRegular(MtmProductData data, double amount){
    if(!data || amount < 0){
        return -1;
    }
    pCustomData prod = (pCustomData)data;
    double finalPrice = amount * prod->basePrice;
    return finalPrice;    
}

//interface function copy the custom data
static MtmProductData copyCustomData(MtmProductData data){
    if(!data){
        return NULL;
    }
    pCustomData srcData = (pCustomData)data;
    pCustomData dstData = malloc(sizeof(*dstData));
    if(!dstData){
        return NULL;
    }
    *dstData = *srcData;//field wise copy of natives
    return dstData;
}

//interface function for freeing the custom data
static void freeCustomData(MtmProductData data){
    if(!data){
        return;
    }
    pCustomData srcData = (pCustomData)data;
    free(srcData);
}

//filter function
bool isAmountLowerThen(const unsigned int id, const char *name,
                               const double amount, MtmProductData customData){
    //input check
    if(amount < 0 || !customData){
        return false;
    }
    pCustomData prod = (pCustomData)customData;
    if(amount < prod->minAmount){
        return true;
    }
    else{
        return false;
    }
}

/// END OF FINAL TESTS PRODUCT TYPE & FUNCTIONS ///

static MtmProductData copyDouble(MtmProductData number) {
    double *copy = malloc(sizeof(*copy));
    if (copy) {
        *copy = *(double*)number;
    }
    return copy;
}

static void freeDouble(MtmProductData number) {
    free(number);
}

static double simplePrice(MtmProductData basePrice, const double amount) {
    return (*(double*)basePrice) * amount;
}

static double buy10Get10ForFree(MtmProductData basePrice, const double amount) {
    double realAmount = amount;
    if (amount >= 20) {
        realAmount -= 10;
    } else if (amount >= 10) {
        realAmount = 10;
    }
    return simplePrice(basePrice, realAmount);
}

bool testModifyProducts() {
    Matamazom mtm = matamazomCreate();
    double basePrice = 8.9;
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmNewProduct(mtm, 4, "Tomato", 2019.11, MATAMAZOM_ANY_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));
    basePrice = 5.8;
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmNewProduct(mtm, 6, "Onion", 1789.75, MATAMAZOM_ANY_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, buy10Get10ForFree));
    basePrice = 2000;
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmNewProduct(mtm, 10, "Television", 15, MATAMAZOM_INTEGER_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));
    basePrice = 5000;
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmNewProduct(mtm, 11, "Television", 4, MATAMAZOM_INTEGER_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));

    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == mtmClearProduct(mtm, 6));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == mtmChangeProductAmount(mtm, 4, 30.5));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == mtmChangeProductAmount(mtm, 4, -12.85));
    ASSERT_OR_DESTROY(MATAMAZOM_INVALID_AMOUNT == mtmChangeProductAmount(mtm, 10, 2.25));
    ASSERT_OR_DESTROY(MATAMAZOM_INSUFFICIENT_AMOUNT == mtmChangeProductAmount(mtm, 10, -1000));

    basePrice = 18.5;
    ASSERT_OR_DESTROY(MATAMAZOM_INVALID_AMOUNT ==
                      mtmNewProduct(mtm, 7, "Watermelon", 24.54,
                                    MATAMAZOM_HALF_INTEGER_AMOUNT, &basePrice, copyDouble,
                                    freeDouble, simplePrice));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmNewProduct(mtm, 7, "Watermelon", 24.5,
                                    MATAMAZOM_HALF_INTEGER_AMOUNT,
                                    &basePrice, copyDouble,
                                    freeDouble, simplePrice));

    matamazomDestroy(mtm);
    return true;
}

static void makeInventory(Matamazom mtm) {
    double basePrice = 8.9;
    mtmNewProduct(mtm, 4, "Tomato", 2019.11, MATAMAZOM_ANY_AMOUNT, &basePrice, copyDouble,
                  freeDouble, simplePrice);
    basePrice = 5.8;
    mtmNewProduct(mtm, 6, "Onion", 1789.75, MATAMAZOM_ANY_AMOUNT, &basePrice, copyDouble,
                  freeDouble, buy10Get10ForFree);
    basePrice = 2000;
    mtmNewProduct(mtm, 10, "Television", 15, MATAMAZOM_INTEGER_AMOUNT, &basePrice,
                  copyDouble, freeDouble, simplePrice);
    basePrice = 5000;
    mtmNewProduct(mtm, 11, "Smart TV", 4, MATAMAZOM_INTEGER_AMOUNT, &basePrice,
                  copyDouble, freeDouble, simplePrice);
    basePrice = 18.5;
    mtmNewProduct(mtm, 7, "Watermelon", 24.5, MATAMAZOM_HALF_INTEGER_AMOUNT, &basePrice,
                  copyDouble, freeDouble, simplePrice);
}

bool testModifyOrders() {
    Matamazom mtm = matamazomCreate();
    makeInventory(mtm);

    unsigned int order1 = mtmCreateNewOrder(mtm);
    ASSERT_OR_DESTROY(order1 > 0);
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmChangeProductAmountInOrder(mtm, order1, 11, 1.0));
    ASSERT_OR_DESTROY(MATAMAZOM_PRODUCT_NOT_EXIST ==
                      mtmChangeProductAmountInOrder(mtm, order1, 15, 1.0));
    ASSERT_OR_DESTROY(MATAMAZOM_INVALID_AMOUNT ==
                      mtmChangeProductAmountInOrder(mtm, order1, 11, 1.2));

    unsigned int order2 = mtmCreateNewOrder(mtm);
    ASSERT_OR_DESTROY(order2 > 0);
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmChangeProductAmountInOrder(mtm, order2, 6, 10.25));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmChangeProductAmountInOrder(mtm, order2, 7, 1.5));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == mtmCancelOrder(mtm, order1));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmChangeProductAmountInOrder(mtm, order2, 10, 2.0));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == mtmShipOrder(mtm, order2));

    matamazomDestroy(mtm);
    return true;
}

static bool fileEqual(FILE *file1, FILE *file2) {
    int c1, c2;
    do {
        c1 = fgetc(file1);
        c2 = fgetc(file2);
    } while (c1 != EOF && c2 != EOF && c1 == c2);
    return (c1 == EOF && c2 == EOF);
}

static bool wholeFileEqual(const char *filename1, const char *filename2) {
    FILE *file1 = fopen(filename1, "r");
    FILE *file2 = fopen(filename2, "r");
    assert(file1);
    assert(file2);
    bool result = fileEqual(file1, file2);
    fclose(file1);
    fclose(file2);
    return result;
}

bool areOrdersIdentical(FILE* expected, FILE* printed){
    assert(expected);
    assert(printed);
    //make sure same line number
    char expectedLine[256];
    char printedLine[256];
    int lineCountExpected = 0;
    int lineCountPrinted = 0;
    while(fgets(printedLine, sizeof(printedLine), printed)){
        lineCountPrinted++;
    }
    while(fgets(expectedLine, sizeof(expectedLine), expected)){
        lineCountExpected++;
    }
    if(lineCountExpected == lineCountPrinted){
        fseek(printed, 0, SEEK_SET );
        fseek(expected, 0, SEEK_SET );
        //skip first line
        fgets(expectedLine, sizeof(expectedLine), expected);
    }
    else{
        return false;
    }
    bool foundLine = false;
    memset(expectedLine, 0, sizeof(expectedLine));
    memset(printedLine, 0, sizeof(printedLine));
    while (fgets(expectedLine, sizeof(expectedLine), expected)) {
        fseek(printed, 0, SEEK_SET );
        foundLine = false;
        while (fgets(printedLine, sizeof(printedLine), printed)) {
            if(strcmp(expectedLine, printedLine) == 0){
                foundLine = true;
                break;
            }
            memset(printedLine, 0, sizeof(printedLine));
        }//end of inner while
        if(foundLine == false){
            return false;
        }
        else{
            foundLine = false;
            memset(expectedLine, 0, sizeof(expectedLine));
            memset(printedLine, 0, sizeof(printedLine)); 
        }  
    }//end of outer while
    return true;
}

bool testPrintInventory() {
    Matamazom mtm = matamazomCreate();
    makeInventory(mtm);
    FILE *outputFile = fopen(INVENTORY_OUT_FILE, "w");
    assert(outputFile);
    ASSERT_OR_DESTROY(mtmPrintInventory(mtm, outputFile) == MATAMAZOM_SUCCESS);
    fclose(outputFile);
    ASSERT_OR_DESTROY(wholeFileEqual(INVENTORY_TEST_FILE, INVENTORY_OUT_FILE));
    matamazomDestroy(mtm);
    return true;
}

static void makeInventory2(Matamazom mtm) {
    double basePrice = 8.9;
    mtmNewProduct(mtm, 4, "Tomato", 2019.11, MATAMAZOM_ANY_AMOUNT, &basePrice, copyDouble,
                  freeDouble, simplePrice);
    CustomData onionData = {5.8, 10, 2, 0.5};
    mtmNewProduct(mtm, 6, "Onion", 1789.75, MATAMAZOM_ANY_AMOUNT,
                    &onionData, copyCustomData, freeCustomData,
                    getPricePercentage);
    basePrice = 2000;
    mtmNewProduct(mtm, 10, "Television", 15, MATAMAZOM_INTEGER_AMOUNT, &basePrice,
                  copyDouble, freeDouble, simplePrice);
    CustomData tvData = {5000, 0, 0, 2};//no discount. same data different function
    mtmNewProduct(mtm, 11, "Smart TV", 4, MATAMAZOM_INTEGER_AMOUNT,
                                    &tvData, copyCustomData, freeCustomData,
                                     getPriceRegular);
    basePrice = 18.5;
    mtmNewProduct(mtm, 7, "Watermelon", 24.5, MATAMAZOM_HALF_INTEGER_AMOUNT, &basePrice,
                  copyDouble, freeDouble, simplePrice);
}

static void makeInventory3(Matamazom mtm) {
    CustomData tomatoData = {8.9, 0, 0, 2000};
    mtmNewProduct(mtm, 4, "Tomato", 2019.11, MATAMAZOM_ANY_AMOUNT, &tomatoData,
                 copyCustomData,freeCustomData, getPriceRegular);

    CustomData onionData = {5.8, 10, 2, 1000};
    mtmNewProduct(mtm, 6, "Onion", 1789.75, MATAMAZOM_ANY_AMOUNT,
                    &onionData, copyCustomData, freeCustomData,
                    getPricePercentage);
    CustomData tvData = {5000, 0, 0, 2};//no discount. same data different function
    mtmNewProduct(mtm, 11, "Smart TV", 4, MATAMAZOM_INTEGER_AMOUNT,
                                    &tvData, copyCustomData, freeCustomData,
                                     getPriceRegular);
    CustomData waterMelonData = {18.5, 10, 2, 24};
    mtmNewProduct(mtm, 7, "Watermelon", 24.5, MATAMAZOM_HALF_INTEGER_AMOUNT,
                        &waterMelonData, copyCustomData, freeCustomData,
                        getPricePercentage);
    CustomData butterData = {7.9, 0, 0, 30};//no butter in the warehouse
    mtmNewProduct(mtm, 20, "Butter", 0, MATAMAZOM_INTEGER_AMOUNT,
                                    &butterData, copyCustomData, freeCustomData,
                                     getPriceRegular);
}

static unsigned int makeOrderForFilter(Matamazom mtm) {
    makeInventory3(mtm);
    unsigned int id = mtmCreateNewOrder(mtm);
    mtmChangeProductAmountInOrder(mtm, id, 4, 59.11);//after ship 1960kg < 2000
    mtmChangeProductAmountInOrder(mtm, id, 6, 15);//not under min
    mtmChangeProductAmountInOrder(mtm, id, 11, 3);// after ship 3 < 2
    return id;
}

static unsigned int makeOrder(Matamazom mtm) {
    makeInventory(mtm);
    unsigned int id = mtmCreateNewOrder(mtm);
    mtmChangeProductAmountInOrder(mtm, id, 6, 10.25);
    mtmChangeProductAmountInOrder(mtm, id, 10, 2);
    mtmChangeProductAmountInOrder(mtm, id, 7, 1.5);
    return id;
}

static unsigned int makeOrderInsufficient(Matamazom mtm) {
    makeInventory2(mtm);
    unsigned int id = mtmCreateNewOrder(mtm);
    mtmChangeProductAmountInOrder(mtm, id, 6, 10.25);
    mtmChangeProductAmountInOrder(mtm, id, 10, 2);
    //make inventory provide only 24.5 kg of watermelon
    mtmChangeProductAmountInOrder(mtm, id, 7, 25);//25>24.5
    return id;
}

static unsigned int makeOrder2(Matamazom mtm) {
    //assuming inventory exist
    unsigned int id = mtmCreateNewOrder(mtm);
    mtmChangeProductAmountInOrder(mtm, id, 6, 10.5);
    mtmChangeProductAmountInOrder(mtm, id, 10, 3);
    mtmChangeProductAmountInOrder(mtm, id, 7, 19);
    return id;
}

static unsigned int makeOrder3(Matamazom mtm) {
    //assuming inventory exist
    unsigned int id = mtmCreateNewOrder(mtm);
    mtmChangeProductAmountInOrder(mtm, id, 4, 2.5);
    mtmChangeProductAmountInOrder(mtm, id, 11, 6);
    return id;
}

bool testPrintOrder() {
    Matamazom mtm = matamazomCreate();
    unsigned int orderId = makeOrder(mtm);
    FILE *outputFile = fopen(ORDER_OUT_FILE, "w");
    assert(outputFile);
    ASSERT_OR_DESTROY(mtmPrintOrder(mtm, orderId, outputFile) == MATAMAZOM_SUCCESS);
    fclose(outputFile);
    FILE *expected = fopen(ORDER_TEST_FILE, "r");
    FILE *printed = fopen(ORDER_OUT_FILE, "r");
    assert(expected);
    assert(printed);

    /* test that prefix and printed orderId is correct */
    const char *expectedPrefix = "Order ";
    int size = strlen(expectedPrefix);
    for (int i = 0; i < size; ++i) {
        ASSERT_OR_DESTROY(fgetc(printed) == expectedPrefix[i]);
    }
    unsigned int printedId;
    fscanf(printed, "%u", &printedId);
    ASSERT_OR_DESTROY(orderId == printedId);

    /* skip the rest of the header line */
    while (fgetc(printed) != '\n');
    while (fgetc(expected) != '\n');

    ASSERT_OR_DESTROY(fileEqual(expected, printed));
    fclose(expected);
    fclose(printed);
    matamazomDestroy(mtm);
    return true;
}

bool testPrintBestSelling() {
    Matamazom mtm = matamazomCreate();
    makeInventory(mtm);

    FILE *outputFile = fopen(NO_SELLING_OUT_FILE, "w");
    assert(outputFile);
    ASSERT_OR_DESTROY(mtmPrintBestSelling(mtm, outputFile) == MATAMAZOM_SUCCESS);
    fclose(outputFile);
    ASSERT_OR_DESTROY(wholeFileEqual(NO_SELLING_TEST_FILE, NO_SELLING_OUT_FILE));

    unsigned int order = mtmCreateNewOrder(mtm);
    mtmChangeProductAmountInOrder(mtm, order, 10, 3.0);
    mtmShipOrder(mtm, order);

    order = mtmCreateNewOrder(mtm);
    mtmChangeProductAmountInOrder(mtm, order, 6, 10.25);
    mtmChangeProductAmountInOrder(mtm, order, 7, 1.5);
    mtmChangeProductAmountInOrder(mtm, order, 11, 1.0);
    mtmShipOrder(mtm, order);

    outputFile = fopen(BEST_SELLING_OUT_FILE, "w");
    assert(outputFile);
    ASSERT_OR_DESTROY(mtmPrintBestSelling(mtm, outputFile) == MATAMAZOM_SUCCESS);
    fclose(outputFile);
    ASSERT_OR_DESTROY(wholeFileEqual(BEST_SELLING_TEST_FILE, BEST_SELLING_OUT_FILE));

    matamazomDestroy(mtm);
    return true;
}

static bool isAmountLessThan10(const unsigned int id, const char *name,
                               const double amount, MtmProductData customData) {
    return amount < 10;
}

bool testPrintFiltered() {
    Matamazom mtm = matamazomCreate();
    makeInventory(mtm);
    FILE *outputFile = fopen(FILTERED_OUT_FILE, "w");
    assert(outputFile);
    ASSERT_OR_DESTROY(mtmPrintFiltered(mtm, isAmountLessThan10, outputFile) == MATAMAZOM_SUCCESS);
    fclose(outputFile);
    ASSERT_OR_DESTROY(wholeFileEqual(FILTERED_TEST_FILE, FILTERED_OUT_FILE));
    matamazomDestroy(mtm);
    return true;
}

bool testNewProducts() {
    Matamazom mtm = matamazomCreate();
    double basePrice = 8.9;
    //check MATAMAZOM_NULL_ARGUMENT
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT ==
                      mtmNewProduct(NULL, 4, "Tomato", 2019.11, MATAMAZOM_ANY_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT ==
                      mtmNewProduct(mtm, 4, NULL, 2019.11, MATAMAZOM_ANY_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT ==
                      mtmNewProduct(mtm, 4, "Tomato", 2019.11, MATAMAZOM_ANY_AMOUNT,
                                    NULL, copyDouble, freeDouble, simplePrice));
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT ==
                      mtmNewProduct(mtm, 4, "Tomato", 2019.11, MATAMAZOM_ANY_AMOUNT,
                                    &basePrice, NULL, freeDouble, simplePrice));
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT ==
                      mtmNewProduct(mtm, 4, "Tomato", 2019.11, MATAMAZOM_ANY_AMOUNT,
                                    &basePrice, copyDouble, NULL, simplePrice));
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT ==
                      mtmNewProduct(mtm, 4, "Tomato", 2019.11, MATAMAZOM_ANY_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, NULL));                                     
    
    //check MATAMAZOM_INVALID_NAME
    ASSERT_OR_DESTROY(MATAMAZOM_INVALID_NAME ==
                      mtmNewProduct(mtm, 4, "", 2019.11, MATAMAZOM_ANY_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));
    ASSERT_OR_DESTROY(MATAMAZOM_INVALID_NAME ==
                      mtmNewProduct(mtm, 4, "~tomato", 2019.11, MATAMAZOM_ANY_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));

    //check MATAMAZOM_INVALID_AMOUNT
    basePrice = 2000;
    ASSERT_OR_DESTROY(MATAMAZOM_INVALID_AMOUNT ==
                      mtmNewProduct(mtm, 10, "Television", 15.5, MATAMAZOM_INTEGER_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));
    ASSERT_OR_DESTROY(MATAMAZOM_INVALID_AMOUNT ==
                      mtmNewProduct(mtm, 10, "Television", -1, MATAMAZOM_INTEGER_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));
    
    //check MATAMAZOM_PRODUCT_ALREADY_EXIST & MATAMAZOM_SUCCESS
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmNewProduct(mtm, 4, "9tomato", 2019.11, MATAMAZOM_ANY_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmNewProduct(mtm, 10, "Television", 0, MATAMAZOM_INTEGER_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));
    ASSERT_OR_DESTROY(MATAMAZOM_PRODUCT_ALREADY_EXIST ==
                      mtmNewProduct(mtm, 4, "tomato", 2019.11, MATAMAZOM_ANY_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));
    ASSERT_OR_DESTROY(MATAMAZOM_PRODUCT_ALREADY_EXIST ==
                      mtmNewProduct(mtm, 10, "Television", 15, MATAMAZOM_INTEGER_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));
    
    matamazomDestroy(mtm);
    return true;
}

//test ability to manage different products types in the same warehouse
bool testModifyProducts2() {
    Matamazom mtm = matamazomCreate();
    double basePrice = 8.9;
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmNewProduct(mtm, 4, "Tomato", 2019.11, MATAMAZOM_ANY_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));
    // basePrice = 5.8;
    //CustomData = 5.8 nis/kg, 10% off for amount bigger then 2 kg, min amount
    //                   in mtm 0.5(for filtering)
    CustomData onionData = {5.8, 10, 2, 0.5};
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmNewProduct(mtm, 6, "Onion", 1789.75, MATAMAZOM_ANY_AMOUNT,
                                    &onionData, copyCustomData, freeCustomData,
                                     getPricePercentage));
    basePrice = 2000;
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmNewProduct(mtm, 10, "Television", 15, MATAMAZOM_INTEGER_AMOUNT,
                                    &basePrice, copyDouble, freeDouble, simplePrice));
    // basePrice = 5000;
    CustomData tvData = {5000, 0, 0, 2};//no discount. same data different functions
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmNewProduct(mtm, 11, "Television", 4, MATAMAZOM_INTEGER_AMOUNT,
                                    &tvData, copyCustomData, freeCustomData,
                                     getPriceRegular));

    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS             == mtmClearProduct(mtm, 6));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS             == mtmChangeProductAmount(mtm, 4, 30.5));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS             == mtmChangeProductAmount(mtm, 4, -12.85));
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT       == mtmChangeProductAmount(NULL, 10, -1000));
    ASSERT_OR_DESTROY(MATAMAZOM_PRODUCT_NOT_EXIST   == mtmChangeProductAmount(mtm, 28, -1000));
    ASSERT_OR_DESTROY(MATAMAZOM_INVALID_AMOUNT      == mtmChangeProductAmount(mtm, 10, 2.25));
    ASSERT_OR_DESTROY(MATAMAZOM_INSUFFICIENT_AMOUNT == mtmChangeProductAmount(mtm, 10, -1000));
    
    

    // basePrice = 18.5;
    CustomData watermelonData = {18.5, 20, 6, 1};
    ASSERT_OR_DESTROY(MATAMAZOM_INVALID_AMOUNT ==
                      mtmNewProduct(mtm, 7, "Watermelon", 24.54,
                                    MATAMAZOM_HALF_INTEGER_AMOUNT, &watermelonData,
                                     copyCustomData,freeCustomData, getPricePercentage));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                      mtmNewProduct(mtm, 7, "Watermelon", 24.5,
                                    MATAMAZOM_HALF_INTEGER_AMOUNT,
                                    &watermelonData, copyCustomData,
                                    freeCustomData, getPricePercentage));

    matamazomDestroy(mtm);
    return true;
}

bool testClearProduct(){
    Matamazom mtm = matamazomCreate();
    makeInventory2(mtm);
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT       == mtmClearProduct(NULL, 6));
    ASSERT_OR_DESTROY(MATAMAZOM_PRODUCT_NOT_EXIST   == mtmClearProduct(mtm, 1));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS             == mtmClearProduct(mtm, 6));
    ASSERT_OR_DESTROY(MATAMAZOM_PRODUCT_NOT_EXIST   == mtmClearProduct(mtm, 6));
    matamazomDestroy(mtm);
    return true;
}

bool testModifyOrders2() {
    Matamazom mtm = matamazomCreate();
    makeInventory2(mtm);

    unsigned int orderInvallid = mtmCreateNewOrder(NULL);
    ASSERT_OR_DESTROY(orderInvallid == 0);
    unsigned int order1 = mtmCreateNewOrder(mtm);
    ASSERT_OR_DESTROY(order1 > 0);
    //check correct error code return order
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==//try to add negative amount for non existing product(in order)
                      mtmChangeProductAmountInOrder(mtm, order1, 11, -1.0));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==//since the product wasn't added we can now add it
                      mtmChangeProductAmountInOrder(mtm, order1, 11, 1.0));
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT ==
                      mtmChangeProductAmountInOrder(NULL, order1+1, 15, 1.2));
    ASSERT_OR_DESTROY(MATAMAZOM_ORDER_NOT_EXIST ==
                      mtmChangeProductAmountInOrder(mtm, order1+1, 15, 1.2));
    ASSERT_OR_DESTROY(MATAMAZOM_PRODUCT_NOT_EXIST ==
                      mtmChangeProductAmountInOrder(mtm, order1, 15, 1.2));
    ASSERT_OR_DESTROY(MATAMAZOM_INVALID_AMOUNT ==
                      mtmChangeProductAmountInOrder(mtm, order1, 11, 1.2));

    matamazomDestroy(mtm);
    return true;
}

bool testShipOrder(){
    Matamazom mtm = matamazomCreate();
    unsigned int orderNum1 = makeOrderInsufficient(mtm);
    unsigned int orderNum2 = makeOrder(mtm);
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT ==
                         mtmShipOrder(NULL, orderNum1 + orderNum2));
    ASSERT_OR_DESTROY(MATAMAZOM_ORDER_NOT_EXIST ==
                         mtmShipOrder(mtm, orderNum1 + orderNum2));
    ASSERT_OR_DESTROY(MATAMAZOM_INSUFFICIENT_AMOUNT ==
                         mtmShipOrder(mtm, orderNum1));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                         mtmShipOrder(mtm, orderNum2));
    
    matamazomDestroy(mtm);
    return true;
}

bool testCancelOrder(){
    Matamazom mtm = matamazomCreate();
    unsigned int orderNum = makeOrder(mtm);
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT ==
                         mtmShipOrder(NULL, orderNum + 1));
    ASSERT_OR_DESTROY(MATAMAZOM_ORDER_NOT_EXIST ==
                         mtmShipOrder(mtm, orderNum + 1));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS ==
                         mtmShipOrder(mtm, orderNum));
    ASSERT_OR_DESTROY(MATAMAZOM_ORDER_NOT_EXIST ==
                         mtmShipOrder(mtm, orderNum));

    matamazomDestroy(mtm);
    return true;
}

bool testAllPrintsNullArgs(){
    Matamazom mtm = matamazomCreate();
    unsigned int orderNum = makeOrder(mtm);
    FILE *outputFile = fopen(INVENTORY_OUT_FILE, "w");
    assert(outputFile);
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT ==
                                mtmPrintInventory(mtm, NULL));
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT ==
                                mtmPrintInventory(NULL, outputFile));
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT ==
                                mtmPrintOrder(NULL, orderNum+1,outputFile));
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT ==
                                mtmPrintOrder(mtm, orderNum+1,NULL));
    ASSERT_OR_DESTROY(MATAMAZOM_ORDER_NOT_EXIST == 
                                mtmPrintOrder(mtm, orderNum+1,outputFile));
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT == 
                                mtmPrintBestSelling(NULL, outputFile));
    ASSERT_OR_DESTROY(MATAMAZOM_NULL_ARGUMENT == 
                                mtmPrintBestSelling(mtm, NULL));

    fclose(outputFile);
    matamazomDestroy(mtm);
    return true;
}

bool testPrintInventoryAfterShip(){
    Matamazom mtm = matamazomCreate();
    unsigned int orderNum = makeOrder(mtm);
    mtmShipOrder(mtm, orderNum);
    FILE *outputFile = fopen(INVENTORY_SHIP_OUT_FILE, "w");
    assert(outputFile);
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == mtmPrintInventory(mtm, outputFile));
    fclose(outputFile);
    ASSERT_OR_DESTROY(wholeFileEqual(INVENTORY_SHIP_TEST_FILE, INVENTORY_SHIP_OUT_FILE));
    matamazomDestroy(mtm);
    return true;
}

//helper function
static bool checkPrintedIds(FILE* p1, FILE* p2, FILE* p3, unsigned int id1,
        unsigned int id2, unsigned int id3, Matamazom mtm){
    const char *expectedPrefix = "Order ";
    int size = strlen(expectedPrefix);
    for (int i = 0; i < size; ++i) {
        ASSERT_OR_DESTROY(fgetc(p1) == expectedPrefix[i]);
        ASSERT_OR_DESTROY(fgetc(p2) == expectedPrefix[i]);
        ASSERT_OR_DESTROY(fgetc(p3) == expectedPrefix[i]);
    }
    unsigned int printedId1;
    unsigned int printedId2;
    unsigned int printedId3;
    fscanf(p1, "%u", &printedId1);
    fscanf(p2, "%u", &printedId2);
    fscanf(p3, "%u", &printedId3);
    ASSERT_OR_DESTROY(id1 == printedId1);
    ASSERT_OR_DESTROY(id2 == printedId2);
    ASSERT_OR_DESTROY(id3 == printedId3);
    return true;
}

bool testPrint3Orders() {
    Matamazom mtm = matamazomCreate();
    //order 1
    unsigned int orderId1 = makeOrder(mtm);
    FILE *outputFile1 = fopen(ORDER1_OUT_FILE, "w");
    assert(outputFile1);
    mtmChangeProductAmountInOrder(mtm, orderId1, 6, -10.25);//onion should be gone
    //order 2
    unsigned int orderId2 = makeOrder2(mtm);
    FILE *outputFile2 = fopen(ORDER2_OUT_FILE, "w");
    assert(outputFile2);
    mtmChangeProductAmountInOrder(mtm, orderId2, 10, -2.5);//nothing should happen
    //order 3
    unsigned int orderId3 = makeOrder3(mtm);
    FILE *outputFile3 = fopen(ORDER3_OUT_FILE, "w");
    assert(outputFile3);
    mtmChangeProductAmountInOrder(mtm, orderId3, 11, 4);//add smart tv

    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == mtmPrintOrder(mtm, orderId1, outputFile1));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == mtmPrintOrder(mtm, orderId2, outputFile2));
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == mtmPrintOrder(mtm, orderId3, outputFile3));
    fclose(outputFile1);
    fclose(outputFile2);
    fclose(outputFile3);
    FILE *printed1 = fopen(ORDER1_OUT_FILE, "r");
    FILE *printed2 = fopen(ORDER2_OUT_FILE, "r");
    FILE *printed3 = fopen(ORDER3_OUT_FILE, "r");
    assert(printed1 && printed1 && printed1);
    
    FILE *expected1 = fopen(ORDER1_TEST_FILE, "r");
    FILE *expected2 = fopen(ORDER2_TEST_FILE, "r");
    FILE *expected3 = fopen(ORDER3_TEST_FILE, "r");
    assert(expected1 && expected2 && expected3);

    bool areIdsOK = checkPrintedIds(printed1, printed2, printed3,
                                     orderId1, orderId2, orderId3, mtm);
    ASSERT_OR_DESTROY(areIdsOK == true);

    ASSERT_OR_DESTROY(areOrdersIdentical(expected1, printed1));
    ASSERT_OR_DESTROY(areOrdersIdentical(expected2, printed2));
    ASSERT_OR_DESTROY(areOrdersIdentical(expected3, printed3));

    fclose(expected1);
    fclose(expected2);
    fclose(expected3);
    fclose(printed1);
    fclose(printed2);
    fclose(printed3);
    matamazomDestroy(mtm);
    return true;
}

bool testPrintBestSelling2() {
    Matamazom mtm = matamazomCreate();
    makeInventory(mtm);
    unsigned int orderId = mtmCreateNewOrder(mtm);
    mtmChangeProductAmountInOrder(mtm, orderId, 10, 3.0);
    mtmChangeProductAmountInOrder(mtm, orderId, 6, 10.25);
    mtmShipOrder(mtm, orderId);//best selling tv with 2000 * 3 = 6000

    orderId = mtmCreateNewOrder(mtm);
    mtmChangeProductAmountInOrder(mtm, orderId, 11, 2.0);
    mtmShipOrder(mtm, orderId);// best sellling should be smart tv w/5000*2=10k

    FILE* outputFile = fopen(BEST_SELLING2_OUT_FILE, "w");
    assert(outputFile);
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == mtmPrintBestSelling(mtm, outputFile));
    fclose(outputFile);
    outputFile = fopen(BEST_SELLING2_OUT_FILE, "a");
    assert(outputFile);
    mtmClearProduct(mtm, 11);//best selling should be tv again
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == mtmPrintBestSelling(mtm, outputFile));
    mtmClearProduct(mtm, 10);//best selling should be now onion
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == mtmPrintBestSelling(mtm, outputFile));
    mtmClearProduct(mtm, 6);//best seller should be now "none"
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == mtmPrintBestSelling(mtm, outputFile));
    fclose(outputFile);
    ASSERT_OR_DESTROY(wholeFileEqual(BEST_SELLING2_TEST_FILE, BEST_SELLING2_OUT_FILE));

    matamazomDestroy(mtm);
    return true;
}

bool testPrintFiltered2() {
    Matamazom mtm = matamazomCreate();
    unsigned int ordID = makeOrderForFilter(mtm);
    FILE *outputFile = fopen(FILTERED_OUT_FILE_2, "w");
    assert(outputFile);
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == 
                    mtmPrintFiltered(mtm, isAmountLowerThen, outputFile));
    fclose(outputFile);
    mtmChangeProductAmount(mtm, 20, 50);//add butter
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == mtmShipOrder(mtm, ordID));
    outputFile = fopen(FILTERED_OUT_FILE_2, "a");
    assert(outputFile);
    ASSERT_OR_DESTROY(MATAMAZOM_SUCCESS == 
                    mtmPrintFiltered(mtm, isAmountLowerThen, outputFile));
    //now the filter should be updated
    fclose(outputFile);
    ASSERT_OR_DESTROY(wholeFileEqual(FILTERED_TEST_FILE_2, FILTERED_OUT_FILE_2));
    matamazomDestroy(mtm);
    return true;
}
