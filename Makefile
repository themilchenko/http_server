BUILD_DIR := build

.PHONY: clean all

all: clean
	@echo "Building project..."
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ..
	cd $(BUILD_DIR) && make

clean:
	@echo "Cleaning up building artefacts..."
	rm -rf $(BUILD_DIR)

docker: docker_clean docker_build docker_run

docker_build:
	docker build -t http_server:latest .

docker_run:
	docker run --rm -p 80:80 --name http_server_test -t http_server

docker_clean:
	docker rm $(docker ps -a -q)
	docker system prune
