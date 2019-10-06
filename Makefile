#NAME: Yining Wang
#EMAIL: wangyining@g.ucla.edu
#ID: 504983099

.SILENT:

default:
	gcc -o  simpsh -g -Wall -Wextra simpsh.c

dist: default
	tar -cvzf lab1-504983099.tar.gz simpsh.c README Makefile report.pdf

clean:
	rm simpsh lab1-504983099.tar.gz
	
check: default
	
	touch test1 test2 test3 test4 test5 test6 test7;
	echo "test1" > test1;
	echo "test2" > test2;
	echo "test3" > test3;
	echo "test4" > test4;
	echo "test5" > test5;
	
	##see if it works normally with --rdonly
	echo "start test1"; \
	./simpsh --rdonly test1; \
	if [ $$? -eq 0 ] ; then \
		echo "test 1 is passed"; \
	else \
		echo "test 1 failed"; \
	fi; \
	
	# --wronly
	echo "start test 2"; \
	./simpsh --rdonly test1 --wronly test2 --wronly test3; \
	if [ $$? -eq 0 ] ; then \
		echo "test 2 is passed"; \
	else \
		echo "test 2 failed"; \
	fi; \
	
	# --command
	echo "start test3"; \
	./simpsh --rdonly test1 --creat --wronly test8 --wronly test3 --command 0 1 2 cat; \
	diff test1 test8; \
	if [ $$? -eq 0 ] ; then \
		echo "test 3 is passed"; \
	else \
		echo "test 3 failed"; \
	fi; \
	
	# --profile
	echo "start test 4"; \
	./simpsh --profile --rdonly test1 >test3; \
	if [ $$? -eq 0 ] && grep -q "0" test3 ; then \
		echo "passed test4"; \
	else \
		echo "test 4 is failed"; \
	fi; 
    