#include "amount_set_tests.h"
#include "test_utilities.h"
#include <stdlib.h>

bool (*tests[]) (void) = {
                      testCreation,
                      testReading,
                      testInsertion,
                      testDeletion,
                      testIteration2,
                      testCopying,
};

const char* testNames[] = {
                           "testCreation",
                           "testReading",
                           "testInsertion",
                           "testDeletion",
                           "testIteration2",
                           "testCopying",
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
      fprintf(stdout, "Usage: amount_set <test index>\n");
      return 0;
  }

  int test_idx = strtol(argv[1], NULL, 10);
  if (test_idx < 1 || test_idx > 6) {
      fprintf(stderr, "Invalid test index %d\n", test_idx);
      return 0;
  }

  RUN_TEST(tests[test_idx - 1], testNames[test_idx - 1]);
  return 0;
}
