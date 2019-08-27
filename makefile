all: serverA.o serverB.o serverC.o aws.o client.o  monitor.o

serverA.o :
	gcc -o serverA serverA.c 

serverB.o :
	gcc -o serverB serverB.c 

serverC.o :
	gcc -o serverC serverC.c 

aws.o:
	gcc -o aws aws.c

client.o :
	gcc -o client client.c 

monitor.o:
	gcc -o monitor monitor.c

serverA: serverA.o
	     ./serverA
serverB: serverB.o
	     ./serverB
serverC: serverC.o
	     ./serverC
aws: aws.o
	   ./aws 25422
client: client.o
	   ./client ee450 25422
monitor: monitor.o
	     ./monitor ee450
clean:
	S(rm) serverA
	S(rm) serverB
	S(rm) serverC
	S(rm) aws
	S(rm) client
	S(rm) monitor
