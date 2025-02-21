#!/bin/bash

HOST=${1:-"localhost"}
PORT=${2:-"8080"}
boundary="----WebKitFormBoundary73Gc5lQ10uYDdw6i"

(
echo -e "POST / HTTP/1.1\r
Host: ${HOST}:${PORT}\r
Transfer-Encoding: chunked\r
Connection: keep-alive\r
Content-Type: multipart/form-data; boundary=${boundary}\r
\r
68\r
------WebKitFormBoundary4XjZxow7uBuxLpLr\r
Content-Disposition: form-data; name=\"file\"; filename=\"text.txt\"\r
\r
18\r
Content-Type: text/plain\r
\r
\r
17\r
hallo mein name ist tom\r
\r
2A\r
------WebKitFormBoundary4XjZxow7uBuxLpLr--\r
\r
0\r
\r"
) | nc ${HOST} ${PORT}