FROM ubuntu:24.04

RUN apt-get update &&  apt-get install -y gcc curl

WORKDIR /app
COPY app/main.c .
RUN gcc -O2 -o server main.c

CMD ["./server"]
