#!/usr/bin/env python3

import cgi
import os

# CGI 프로그램에서 사용할 HTML 헤더 출력

print()

# CGI 매개변수 읽기
form = cgi.FieldStorage()

# 요청 메서드에 따라 요청 처리
if "REQUEST_METHOD" in os.environ and os.environ["REQUEST_METHOD"] == "GET":
    # GET 요청 처리
    print("<h1>GET 요청</h1>")
    if "SERVER_SOFTWARE" in os.environ:
        print("<h2>서버: " + os.environ["SERVER_SOFTWARE"] + "</h2>")
    if "REMOTE_ADDR" in os.environ:
        print("<h3>ADDR: " + os.environ["REMOTE_ADDR"] + "</h3>")
    if "PATH_TRANSLATED" in os.environ:
        print("<h2>패스!!: " + os.environ["PATH_TRANSLATED"] + "</h2>")
    if "SERVER_PROTOCOL" in os.environ:
        print("<h2>패스!!: " + os.environ["SERVER_PROTOCOL"] + "</h2>")
elif "REQUEST_METHOD" in os.environ and os.environ["REQUEST_METHOD"] == "POST":
    # POST 요청 처리
    print("<h1>POST 요청</h1>")
    print("<p>POST 데이터:</p>")
    print("<pre>")
    for key in form.keys():
        print(key + ": " + form.getvalue(key))
    print("</pre>")
else:
    # 잘못된 요청 메서드
    print("<h1>잘못된 요청</h1>")