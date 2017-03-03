CFLAGS = -Wall -Werror
DFLAGS = -g
EXECS = stringcompare

all: $(EXECS)

stringcompare: stringcompare.o longest_common_substring.o longest_common_subsequence.o
	gcc stringcompare.o longest_common_substring.o longest_common_subsequence.o -o stringcompare

stringcompare.o: stringcompare.c
	gcc -c stringcompare.c $(CFLAGS) $(DFLAGS)

longest_common_substring.o: longest_common_substring.c
	gcc -c longest_common_substring.c $(CFLAGS) $(DFLAGS)

longest_common_subsequence.o: longest_common_subsequence.c
	gcc -c longest_common_subsequence.c $(CFLAGS) $(DFLAGS)

clean:
	rm -f *.o

pristine: clean
	rm -f $(EXECS)
