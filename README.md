# Web server

> Architecture: fork per request (blocking).

## Stress testing

### On my own machine and in docker container

I use Apache benchmarking tool to count performance. I use this command to give all RPS:

`ab -n 50000 -c 100 http://localhost:8081/httptest/wikipedia_russia.html`

for my machine and

`ab -n 10000 -c 100 http://localhost:80/httptest/wikipedia_russia.html`

for docker.

There is a table which show dependence of used CPUs of requests per second

| CPUs         | RPS on my machine | RPS in Docker container |
|--------------|-------------------|-------------------------|
| 1            | 1743.84           | 878.23                  |
| 2            | 2031.55           | 872.82                  |
| 3            | 2072.09           | 903.86                  |
| 4            | 2116.24           | 1024.65                 |
| 5            | 2113.45           | 1041.20                 |
| 6            | 2088.72           | 1020.67                 |
| 7            | 2089.40           | 1035.54                 |
| 8            | 2038.89           | 1005.36                 |

### Comparison with nginx

I will test it on my machine by following command:

`ab -n 10000 -c 100 http://localhost:80/httptest/wikipedia_russia.html`

Also I set 4 CPUs for both servers. So there are the results:

> `My machine:` 1786.04 rps
>
> `Nginx:` 3244.02 rps
>
> `The result:` 1.81 times nginx server is more than my.
