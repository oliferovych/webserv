#!/usr/bin/env python3

import os

UPLOAD_DIR = os.environ.get('UPLOAD_DIR', '/default/upload/dir')

# Ensure the upload directory exists
if not os.path.exists(UPLOAD_DIR):
    os.makedirs(UPLOAD_DIR)

# Generate the HTML response
def generate_html():
    files = os.listdir(UPLOAD_DIR)
    file_list = ''.join(f'<li>{file} <a href="/cgi-bin/delete_file.py?delete={file}">Delete</a></li>' for file in files)
    return f"""
    <html>
    <head>
        <title>File Upload and Delete</title>
    </head>
    <body>
        <h3>Upload a file</h3>
        <form action="/cgi-bin/upload_file.py" method="get" enctype="multipart/form-data">
            <input type="file" name="file">
            <input type="submit" value="Upload">
        </form>
        <h3>Delete a file</h3>
        <ul>
            {file_list}
        </ul>
    </body>
    </html>
    """

# Print the HTML response
print("Content-Type: text/html")
print()
print(generate_html())
