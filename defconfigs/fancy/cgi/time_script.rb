require 'time'

print "Content-Type: text/html\r\n\r\n"
print "<html>"
print "<head>"
print "<title>Current Time</title>"
print "<style>"
print "body { font-family: Arial, sans-serif; background-color: #f0f0f0; text-align: center; padding-top: 50px; }"
print ".title { font-size: 2em; margin-bottom: 20px; }"
print ".quote-container { font-size: 1.2em; margin-bottom: 20px; }"
print ".time-container { background-color: #fff; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); display: inline-block; }"
print ".time { font-size: 2em; color: #333; }"
print "</style>"
print "</head>"
print "<body>"
print "<div class='time-container'>"
print "<h1>Current Time with Ruby</h1>"
print "<p class='time'>#{Time.now.strftime('%Y-%m-%d %H:%M:%S')}</p>"
print "</div>"
print "</body>"
print "</html>"
