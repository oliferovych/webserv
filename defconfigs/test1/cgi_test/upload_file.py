#!/usr/bin/env python3

import cgi
import os

UPLOAD_DIR = os.environ.get('UPLOAD_DIR', '/default/upload/dir')

# Ensure the upload directory exists
if not os.path.exists(UPLOAD_DIR):
    os.makedirs(UPLOAD_DIR)

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get filename here
fileitem = form['file']

# Check if the file was uploaded
if fileitem.filename:
    # Strip leading path from file name to avoid directory traversal attacks
    filename = os.path.basename(fileitem.filename)
    filepath = os.path.join(UPLOAD_DIR, filename)

    # Open the file for writing in binary mode
    with open(filepath, 'wb') as f:
        f.write(fileitem.file.read())

    message = f'The file "{filename}" was uploaded successfully.'
else:
    message = 'No file was uploaded.'

# Generate the HTML response
print("Content-Type: text/html")
print()
print(f"""
<html>
<head>
    <title>File Upload</title>
</head>
<body>
    <h2>{message}</h2>
    <a href="/cgi-bin/upload_interface.py">Go back</a>
</body>
</html>
""")
