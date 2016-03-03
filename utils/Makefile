
CFLAGS = -Wno-write-strings 


all: comp-binary-results utils-tester 


comp-binary-results: comp-binary-results.cpp
	g++ -o comp-binary-results comp-binary-results.cpp -lquadmath 


utils-tester: utils-tester.cpp
	g++ $(CFLAGS) -o utils-tester utils-tester.cpp -lquadmath 


clean:
	-rm *~
	-rm *.o
	-rm comp-binary-results
	-rm utils-tester 
