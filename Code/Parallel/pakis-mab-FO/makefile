SRCS = $(shell find . -name "*.cpp") 

OBJS = $(addsuffix .o, $(basename $(SRCS)))

EXEC = pakissat

LIBS = -lkissat -Lkissat_mab/build/ \
       -lpthread -lz -lm -static

CXXFLAGS = -Ikissat_mab -std=c++11 -O3

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(EXEC)