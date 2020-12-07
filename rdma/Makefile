CXXFLAG = -libverbs -pthread -std=c++17

all: 
	g++ compute_node.cc rdma.cc -o compute_node $(CXXFLAG)
	g++ server.cc rdma.cc -o server $(CXXFLAG)

clean:
	rm -f compute_node server
