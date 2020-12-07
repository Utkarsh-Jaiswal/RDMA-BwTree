Acknowledgement
==============

bplus-tree
https://github.com/sayef/bplus-tree


Compile and Run
===============
| Command | Description |
|---------|-------------|
|make | Compiles and links|
|make clean | removes binaries|


Usage
===============
On Purdue bigdata servers{bigdata1.cs.purdue.edu, bigdata2.cs.purdue.edu}

| Command | Description |
|---------|-------------|
|ulimit -l 2000000 | set file size limit|

Run ./server and ./compute_node on different instances of the bigdata servers.
Input domain address or IP of the server in the compute_node