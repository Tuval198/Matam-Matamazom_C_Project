#include "../amount_set.h"
#include "amount_set_tests.h"
#include "test_utilities.h"
#include <stdlib.h>

#define ASSERT_OR_DESTROY(expr) ASSERT_TEST_WITH_FREE((expr), asDestroy(set))

static ASElement copyInt(ASElement number) {
    int *copy = malloc(sizeof(*copy));
    if (copy != NULL) {
        *copy = *(int *)number;
    }
    return copy;
}

static void freeInt(ASElement number) { free(number); }

static int compareInts(ASElement lhs, ASElement rhs) {
    return (*(int *)lhs) - (*(int *)rhs);
}

bool testCreate() {
    AmountSet set = asCreate(copyInt, freeInt, compareInts);
    ASSERT_OR_DESTROY(set);
    asDestroy(set);
    return true;
}

bool testDestroy() {
    AmountSet set = asCreate(copyInt, freeInt, compareInts);
    asDestroy(set);
    /* try to elicit undefined behavior in case asDestroy has bugs */
    AmountSet set2 = asCreate(copyInt, freeInt, compareInts);
    ASSERT_TEST(set2 != NULL);
    asDestroy(set2);
    return true;
}

bool testModify() {
    AmountSet set = asCreate(copyInt, freeInt, compareInts);
    int ids[7] = {0, 4, 2, 5, 7, 3, 1};
    for (int i = 0; i < 7; ++i) {
        ASSERT_OR_DESTROY(asRegister(set, ids + i) == AS_SUCCESS);
    }

    ASSERT_OR_DESTROY(asChangeAmount(set, ids + 2, 10.5) == AS_SUCCESS);
    ASSERT_OR_DESTROY(asChangeAmount(set, ids + 1, 3) == AS_SUCCESS);
    ASSERT_OR_DESTROY(asChangeAmount(set, ids + 1, -17) == AS_INSUFFICIENT_AMOUNT);
    ASSERT_OR_DESTROY(asDelete(set, ids + 3) == AS_SUCCESS);

    asDestroy(set);
    return true;
}

static void addElements(AmountSet set) {
    int ids[7] = {0, 4, 2, 5, 7, 3, 1};
    for (int i = 0; i < 7; ++i) {
        asRegister(set, ids + i);
    }
    asChangeAmount(set, ids + 2, 10.5);
    asChangeAmount(set, ids + 1, 3);
}

bool testModify2() {
    AmountSet set = asCreate(copyInt, freeInt, compareInts);
    addElements(set);
    ASSERT_OR_DESTROY(asClear(set) == AS_SUCCESS);
    int x = 8, y = 3;
    ASSERT_OR_DESTROY(asRegister(set, &x) == AS_SUCCESS);
    ASSERT_OR_DESTROY(asRegister(set, &y) == AS_SUCCESS);
    ASSERT_OR_DESTROY(asClear(set) == AS_SUCCESS);
    asDestroy(set);
    return true;
}

static void destroyAmountSets(AmountSet set1, AmountSet set2) {
    asDestroy(set1);
    asDestroy(set2);
}

bool testCopy() {
    AmountSet set = asCreate(copyInt, freeInt, compareInts);
    addElements(set);
    AmountSet copy = asCopy(set);
    ASSERT_OR_DESTROY(copy != NULL);
    int x = 2;
    ASSERT_TEST_WITH_FREE(asDelete(copy, &x) == AS_SUCCESS, destroyAmountSets(set, copy));
    ASSERT_TEST_WITH_FREE(asContains(set, &x), destroyAmountSets(set, copy));
    asDestroy(copy);
    asDestroy(set);
    return true;
}

bool testGetSize() {
    AmountSet set = asCreate(copyInt, freeInt, compareInts);
    addElements(set);
    ASSERT_OR_DESTROY(asGetSize(set) == 7);
    ASSERT_OR_DESTROY(asGetSize(NULL) == -1);
    asDestroy(set);
    return true;
}

bool testContains() {
    AmountSet set = asCreate(copyInt, freeInt, compareInts);
    addElements(set);
    int x = 3;
    ASSERT_OR_DESTROY(asContains(set, &x));
    x = 30;
    ASSERT_OR_DESTROY(!asContains(set, &x));
    asDestroy(set);
    return true;
}

bool testGetAmount() {
    AmountSet set = asCreate(copyInt, freeInt, compareInts);
    addElements(set);
    double amount = -1.0;
    int x = 2;
    ASSERT_OR_DESTROY(asGetAmount(set, &x, &amount) == AS_SUCCESS);
    ASSERT_OR_DESTROY(-0.001 < amount - 10.5 && amount - 10.5 < 0.001);
    x = 30;
    ASSERT_OR_DESTROY(asGetAmount(set, &x, &amount) == AS_ITEM_DOES_NOT_EXIST);
    asDestroy(set);
    return true;
}

bool testIteration() {
    AmountSet set = asCreate(copyInt, freeInt, compareInts);
    addElements(set);
    int lastId = -1;
    AS_FOREACH(int*, currId, set) {
        ASSERT_OR_DESTROY(lastId < (*currId));
    }
    asDestroy(set);
    return true;
}

#define TOLERANCE (1e-5)

bool isClose(double x, double y, double tol) {
    return (-tol < (x - y) && (x - y) < tol);
}

bool testCreation() {
    AmountSet set = asCreate(copyInt, NULL, compareInts);
    ASSERT_TEST(set == NULL);
    set = asCreate(copyInt, freeInt, compareInts);
    ASSERT_TEST(set != NULL);

    int ids[7] = {0, 4, 2, 5, 7, 3, 1};
    for (int i = 0; i < 7; ++i) {
        ASSERT_OR_DESTROY(asRegister(set, ids + i) == AS_SUCCESS);
    }

    ASSERT_OR_DESTROY(asChangeAmount(set, ids + 2, 10.5) == AS_SUCCESS);
    ASSERT_OR_DESTROY(asChangeAmount(set, ids + 1, 3) == AS_SUCCESS);

    double amount;
    ASSERT_OR_DESTROY(asGetAmount(set, ids + 1, &amount) == AS_SUCCESS);
    ASSERT_OR_DESTROY(isClose(amount, 3, TOLERANCE));

    asDestroy(set);
    return true;
}

bool testReading() {
    AmountSet set = asCreate(copyInt, freeInt, compareInts);
    ASSERT_OR_DESTROY(asGetSize(set) == 0);

    addElements(set);
    double amount = -1.0;
    int x = 2;
    ASSERT_OR_DESTROY(asGetAmount(set, &x, &amount) == AS_SUCCESS);
    ASSERT_OR_DESTROY(isClose(amount, 10.5, TOLERANCE));
    ASSERT_OR_DESTROY(asGetAmount(set, NULL, &amount) == AS_NULL_ARGUMENT);
    ASSERT_OR_DESTROY(asGetAmount(set, &x, NULL) == AS_NULL_ARGUMENT);
    x = 30;
    ASSERT_OR_DESTROY(asGetAmount(set, &x, &amount) == AS_ITEM_DOES_NOT_EXIST);

    ASSERT_OR_DESTROY(asGetSize(set) == 7);
    ASSERT_OR_DESTROY(asGetSize(NULL) == -1);

    x = 2;
    ASSERT_TEST(asContains(NULL, &x) == false);
    ASSERT_TEST(asContains(set, &x) == true);
    x = 30;
    ASSERT_TEST(asContains(set, &x) == false);

    asDestroy(set);
    return true;
}

bool testInsertion() {
    AmountSet set = asCreate(copyInt, freeInt, compareInts);

    ASSERT_OR_DESTROY(asRegister(set, NULL) == AS_NULL_ARGUMENT);
    int ids[7] = {0, 4, 2, 5, 7, 3, 1};
    for (int i = 0; i < 6; ++i) {
        ASSERT_OR_DESTROY(asRegister(set, ids + i) == AS_SUCCESS);
    }
    ASSERT_OR_DESTROY(asRegister(set, ids) == AS_ITEM_ALREADY_EXISTS);
    ASSERT_OR_DESTROY(asRegister(set, ids + 6) == AS_SUCCESS);

    ASSERT_OR_DESTROY(asChangeAmount(set, ids + 2, 10.5) == AS_SUCCESS);
    ASSERT_OR_DESTROY(asChangeAmount(set, ids + 1, 3) == AS_SUCCESS);
    ASSERT_OR_DESTROY(asChangeAmount(set, ids + 1, -4) == AS_INSUFFICIENT_AMOUNT);
    ASSERT_OR_DESTROY(asChangeAmount(set, ids + 1, 2) == AS_SUCCESS);
    double amount = 0;
    ASSERT_OR_DESTROY(asGetAmount(set, ids + 1, &amount) == AS_SUCCESS);
    ASSERT_OR_DESTROY(isClose(amount, 5, TOLERANCE));
    int x = 30;
    ASSERT_OR_DESTROY(asChangeAmount(set, &x, 20) == AS_ITEM_DOES_NOT_EXIST);

    asDestroy(set);
    return true;
}
bool testDeletion() {
    AmountSet set = asCreate(copyInt, freeInt, compareInts);
    int x = 0;
    ASSERT_OR_DESTROY(asDelete(set, NULL) == AS_NULL_ARGUMENT);
    ASSERT_OR_DESTROY(asDelete(set, &x) == AS_ITEM_DOES_NOT_EXIST);
    addElements(set);
    int ids[7] = {0, 1, 2, 3, 4, 5, 7}; // same ids that addElements inserts, but sorted

    x = 30;
    ASSERT_OR_DESTROY(asDelete(set, &x) == AS_ITEM_DOES_NOT_EXIST);
    ASSERT_OR_DESTROY(asDelete(set, ids + 3) == AS_SUCCESS);
    ASSERT_OR_DESTROY(asGetSize(set) == 6);
    ASSERT_OR_DESTROY(asRegister(set, ids + 3) == AS_SUCCESS);

    ASSERT_OR_DESTROY(asClear(set) == AS_SUCCESS);
    ASSERT_OR_DESTROY(asClear(NULL) == AS_NULL_ARGUMENT);
    ASSERT_OR_DESTROY(asGetSize(set) == 0);
    ASSERT_OR_DESTROY(asRegister(set, ids + 3) == AS_SUCCESS);
    ASSERT_OR_DESTROY(asChangeAmount(set, ids + 3, 5) == AS_SUCCESS);

    asDestroy(set);
    return true;
}

bool testIteration2() {
    AmountSet set = asCreate(copyInt, freeInt, compareInts);
    addElements(set);
    int ids[7] = {0, 1, 2, 3, 4, 5, 7}; // same ids that addElements inserts, but sorted

    int i = 0;
    AS_FOREACH(int*, px, set) {
        ASSERT_OR_DESTROY(ids[i] == *px);
        ++i;
    }

    ASSERT_OR_DESTROY(asGetNext(set) == NULL);

    /* test that asGetAmount does not change iterator's state */
    asGetFirst(set);
    i = 7;
    double amount;
    asGetAmount(set, &i, &amount);
    int *pi = asGetNext(set);
    ASSERT_OR_DESTROY(pi != NULL);
    ASSERT_OR_DESTROY(*pi == ids[1]);

    ASSERT_OR_DESTROY(asDelete(set, ids) == AS_SUCCESS);
    int *px = asGetFirst(set);
    ASSERT_OR_DESTROY(px != NULL && *px == ids[1]);

    asDestroy(set);
    return true;
}

typedef struct point_t {
    double x;
    double y;
} *Point;

static ASElement copyPoint(ASElement number) {
    Point copy = malloc(sizeof(*copy));
    if (copy != NULL) {
        *copy = *(Point)number;
    }
    return copy;
}

static void freePoint(ASElement number) {
    free(number);
}

static int compareDoubles(double lhs, double rhs) {
    double dx = lhs - rhs;
    if (dx > TOLERANCE) {
        return 1;
    }
    if (dx < -TOLERANCE) {
        return -1;
    }
    return 0;
}

static int comparePoints(ASElement lhs, ASElement rhs) {
    Point a = lhs;
    Point b = rhs;
    int x_order = compareDoubles(a->x, b->x);
    if (x_order != 0)
        return x_order;
    return compareDoubles(a->y, b->y);
}

static void destroyBoth(AmountSet set1, AmountSet set2) {
    asDestroy(set1);
    asDestroy(set2);
}

bool testCopying() {
    AmountSet set = asCreate(copyPoint, freePoint, comparePoints);
    ASSERT_TEST(set != NULL);

    struct point_t points[] = {{0, 0}, {1, 1}, {0, 1}, {-2, -3}};
    double amounts[] = {1, 2, 3, 4};

    for (int i = 0; i < 4; ++i) {
        ASSERT_OR_DESTROY(asRegister(set, points + i) == AS_SUCCESS);
        ASSERT_OR_DESTROY(asChangeAmount(set, points + i, amounts[i]) == AS_SUCCESS);
    }

    AmountSet copy = asCopy(set);
    ASSERT_TEST_WITH_FREE(copy != NULL, destroyBoth(set, copy));
    ASSERT_TEST_WITH_FREE(asGetSize(copy) == 4, destroyBoth(set, copy));
    double amount = -1;
    ASSERT_TEST_WITH_FREE(asGetAmount(copy, points + 1, &amount) == AS_SUCCESS,
                          destroyBoth(set, copy));
    ASSERT_TEST_WITH_FREE(isClose(amount, 2, TOLERANCE), destroyBoth(set, copy));

    ASSERT_TEST_WITH_FREE(asChangeAmount(set, points + 1, 8) == AS_SUCCESS,
                          destroyBoth(set, copy));
    ASSERT_TEST_WITH_FREE(asGetAmount(copy, points + 1, &amount) == AS_SUCCESS,
                          destroyBoth(set, copy));
    ASSERT_TEST_WITH_FREE(isClose(amount, 2, TOLERANCE), destroyBoth(set, copy));

    ASSERT_TEST_WITH_FREE(asDelete(set, points + 2) == AS_SUCCESS,
                          destroyBoth(set, copy));
    ASSERT_TEST_WITH_FREE(asGetAmount(copy, points + 2, &amount) == AS_SUCCESS,
                          destroyBoth(set, copy));
    ASSERT_TEST_WITH_FREE(isClose(amount, 3, TOLERANCE), destroyBoth(set, copy));

    Point first = asGetFirst(set);
    first->x = -10;
    Point firstCopy = asGetFirst(copy);
    ASSERT_TEST_WITH_FREE(isClose(firstCopy->x, -2, TOLERANCE), destroyBoth(set, copy));

    asDestroy(copy);
    asDestroy(set);
    return true;
}
