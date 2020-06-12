#include "matamazom_tests.h"
#include "test_utilities.h"
#include <stdlib.h>

bool (*tests[]) (void) = {
    testNewProducts,
    testClearProduct,
    testModifyProducts2,
    testModifyOrders2,
    testShipOrder,
    testCancelOrder,
    testAllPrintsNullArgs,
    testPrintInventoryAfterShip,
    testPrint3Orders,
    testPrintBestSelling2,
    testPrintFiltered2,
};

const char* testNames[] = {
    "testNewProducts",
    "testClearProduct",
    "testModifyProducts2",
    "testModifyOrders2",
    "testShipOrder",
    "testCancelOrder",
    "testAllPrintsNullArgs",
    "testPrintInventoryAfterShip",
    "testPrint3Orders",
    "testPrintBestSelling2",
    "testPrintFiltered2",
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
      fprintf(stdout, "Usage: matamazom <test index>\n");
      return 0;
  }

  int test_idx = strtol(argv[1], NULL, 10);
  if (test_idx < 1 || test_idx > 11) {
      fprintf(stderr, "Invalid test index %d\n", test_idx);
      return 0;
  }

  RUN_TEST(tests[test_idx - 1], testNames[test_idx - 1]);
  return 0;
}
