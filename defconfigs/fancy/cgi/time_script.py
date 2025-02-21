import datetime

#!/usr/bin/env python3

print("<html>")
print("<head>")
print("<title>Current Time</title>")
print("<style>")
print("body { font-family: Arial, sans-serif; background-color: #f0f0f0; text-align: center; padding-top: 50px; }")
print(".time-container { background-color: #fff; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); display: inline-block; }")
print(".time { font-size: 2em; color: #333; }")
print("</style>")
print("</head>")
print("<body>")
print("<div class='time-container'>")
print("<h1>Current Time with Python</h1>")
print("<p class='time'>{}</p>".format(datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")))
print("</div>")
print("</body>")
print("</html>")
