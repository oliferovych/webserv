#!/usr/bin/env python3

import cgi
import os

UPLOAD_DIR = os.environ.get('UPLOAD_DIR', '/default/upload/dir')

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get filename to delete
filename = form.getvalue('delete')

# Check if the file exists and delete it
if filename:
    filepath = os.path.join(UPLOAD_DIR, filename)
    if os.path.exists(filepath):
        os.remove(filepath)
        message = f'The file "{filename}" was deleted successfully.'
    else:
        message = f'The file "{filename}" does not exist.'
else:
    message = 'No file was specified for deletion.'

# Generate the HTML response
print("Content-Type: text/html")
print()
print(f"""
<html>
<head>
    <title>File Deletion</title>
</head>
<body>
    <h2>{message}</h2>
    <a href="/cgi-bin/upload_interface.py">Go back</a>
</body>
</html>
""")
