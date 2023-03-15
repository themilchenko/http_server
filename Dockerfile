# syntax=docker/dockerfile:1

FROM ubuntu:latest
RUN apt-get -y update && apt-get -y install gcc make cmake
WORKDIR /http_server
COPY / .
RUN make
EXPOSE 80
CMD ["./build/http_server"]
