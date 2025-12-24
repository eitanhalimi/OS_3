CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -pg

all: profile

# Build executables
convex_list: ConvexHull_list.cpp
	$(CXX) $(CXXFLAGS) -o convex_list ConvexHull_list.cpp

convex_deque: ConvexHull_deque.cpp
	$(CXX) $(CXXFLAGS) -o convex_deque ConvexHull_deque.cpp

# Run profiling and generate reports
profile: convex_list convex_deque
	./convex_list < test_input.txt
	gprof convex_list gmon.out > result/gprof_list.txt
	./convex_deque < test_input.txt
	gprof convex_deque gmon.out > result/gprof_deque.txt

clean:
	rm -f convex_list convex_deque gmon.out result/*.txt

#זמנים מדוייקים אפשר לראות בהרצה הבאה 
#time ./convex_deque < test_input.txt
#time ./convex_list < test_input.txt