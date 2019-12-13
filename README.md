Full Name: Zhenduo Hou
USC ID:8203461175

In this project, I have implemented one client and three different servers. And I implemented routing function and end-to-end delay computing function in two backend servers. The client will establish a TCP socket connected to AWS server and send a query to AWS server with map ID, initial node and file size. The aws will received the query in UDP and deliver the map id and initial node to backend server A. After the server A get the information, it will find the shortest path depending on its map stored in its database and send back the result to aws.  The aws will forward the path, file_size, transmission speed and propagation speed to server B over UDP. Server b will compute the end-to-end delay and send the results back to aws. At last, the aws will send the path and delay information to client.

All messages send and received are in char format.

Most of socket codes are copied from Beej\'92s.

Makefile:
Makefile is the file with all the functions that executes and run all my code files. You need to run my codes through these make calls:
		make all
		make ServerA
		make ServerB
		make AWS
		./Client <mapID> <NODE> <File Size> (eg: ./Client A 1 654.32)


The whole codes run perfectly on my VisualBox (ubuntu 16.04) and my macbook without errors. Hopefully, you can have the exactly correct outputs on your machine.

