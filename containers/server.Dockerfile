FROM ubuntu:latest
RUN apt-get -y update && apt-get -y install gcc        \
                                            make       \
                                            cmake      \
                                            openssl    \
                                            libssl-dev
WORKDIR /http_server
COPY . .
RUN make
EXPOSE 80
CMD ["./build/http_server", "config/server.conf"]
