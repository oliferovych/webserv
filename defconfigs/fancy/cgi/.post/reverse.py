#!/usr/bin/env python3

import cgi
import os

print("Content-Type: text/html\r\n\r\n")  # Required for CGI output
# Get uploaded file
form = cgi.FieldStorage()
file_item = form["file"]

if file_item.filename:
    # Read and reverse file content
    file_content = file_item.file.read().decode("utf-8")
    reversed_content = file_content[::-1]

    # Print the reversed content in an HTML page
    print(f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Reversed Text</title>
    </head>
    <body>
        <h2>Reversed Content:</h2>
        <pre>{reversed_content}</pre>
        <br>
        <a href="/">Go Back</a>
    </body>
    </html>
    """)

else:
    print("<h2>No file uploaded!</h2>")
