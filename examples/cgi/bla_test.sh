#! bin/bash


#http://localhost:82/YoupiBanane/youpi.bla 


# CONTENT_LENGTH=0
# GATEWAY_INTERFACE=CGI/1.1
# HTTP_Accept=*/*
# HTTP_Accept-Encoding=gzip, deflate, br
# HTTP_Connection=keep-alive
# HTTP_Host=localhost:82
# HTTP_Postman-Token=51997ccc-bf24-4695-b142-6e4863aaf7be
# HTTP_User-Agent=PostmanRuntime/7.28.0
# PATH_INFO=/YoupiBanane/youpi.bla
# PATH_TRANSLATED=./www/YoupiBanane/youpi.bla
# QUERY_STRING=
# REMOTE_ADDR=0.0.0.0
# REQUEST_METHOD=GET
# REQUEST_URI=/YoupiBanane/youpi.bla
# SCRIPT_NAME=./tester/cgi_tester
# SERVER_NAME=localhost
# SERVER_PORT=82
# SERVER_PROTOCOL=HTTP/1.1
# SERVER_SOFTWARE=418-IAmATeapot


export CONTENT_LENGTH=4
export GATEWAY_INTERFACE=CGI/1.1
export HTTP_Accept=*/*
export HTTP_Connection=keep-alive
export HTTP_Host=localhost:82
export PATH_INFO=/youpi.bla
export REQUEST_URI=/youpi.bla
export PATH_TRANSLATED=./youpi.bla
export REMOTE_ADDR=0.0.0.0
export REQUEST_METHOD=GET
export SCRIPT_NAME=./cgi_tester
export SERVER_NAME=localhost
export SERVER_PORT=82
export SERVER_PROTOCOL=HTTP/1.1
export SERVER_SOFTWARE=hski

./cgi_tester youpi.bla