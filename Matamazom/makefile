
CC = gcc
OBJS_MATAMAZOM = tests/matamazom_main.o tests/matamazom_tests.o matamazom.o amount_set.o matamazom_print.o order.o product.o 
EXEC_MATAMAZOM = matamazom
OBJS_AMOUNT_SET = tests/amount_set_main.o amount_set.o tests/amount_set_tests.o
EXEC_AMOUNT_SET = amount_set
COMP_FLAG = -std=c99 -Wall -Werror -pedantic-errors -DNDEBUG

$(EXEC_MATAMAZOM) : $(OBJS_MATAMAZOM)
	$(CC) matamazom_main.o matamazom_tests.o matamazom.o amount_set.o matamazom_print.o order.o product.o -o $@ -L. -lm -lmtm

$(EXEC_AMOUNT_SET) : $(OBJS_AMOUNT_SET)
	$(CC) amount_set_main.o amount_set.o amount_set_tests.o -o $@
	
matamazom.o: matamazom.c amount_set.h list.h product.h matamazom.h \
 order.h matamazom_print.h
	$(CC) $(COMP_FLAG) -c $*.c 

tests/matamazom_tests.o: tests/matamazom_tests.c tests/matamazom_tests.h \
 tests/../matamazom.h tests/test_utilities.h
	$(CC) $(COMP_FLAG) -c $*.c
	
matamazom_print.o: matamazom_print.c matamazom_print.h
	$(CC) $(COMP_FLAG) -c $*.c 
	
order.o: order.c product.h order.h amount_set.h
	$(CC) $(COMP_FLAG) -c $*.c

product.o: product.c product.h
	$(CC) $(COMP_FLAG) -c $*.c

tests/matamazom_main.o: tests/matamazom_main.c tests/matamazom_tests.h \
 tests/test_utilities.h
	$(CC) $(COMP_FLAG) -c $*.c
	
amount_set.o: amount_set.c amount_set.h
	$(CC) $(COMP_FLAG) -c $*.c
	
tests/amount_set_main.o: tests/amount_set_main.c tests/amount_set_tests.h tests/test_utilities.h
	$(CC) $(COMP_FLAG) -c $*.c
	
tests/amount_set_tests.o: tests/amount_set_tests.c tests/../amount_set.h tests/amount_set_tests.h tests/test_utilities.h
	$(CC) $(COMP_FLAG) -c $*.c

clean:	
	rm -f *.o

 
