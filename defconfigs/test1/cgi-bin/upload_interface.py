#!/usr/bin/env python3

import cgi
import os
import cgitb

cgitb.enable()  # Enable debugging

UPLOAD_DIR = os.environ.get('UPLOAD_DIR')
if not UPLOAD_DIR:
    raise EnvironmentError("UPLOAD_DIR environment variable is not set")

# Ensure the upload directory exists

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Handle file upload
def handle_upload():
    fileitem = form['file']
    if fileitem.filename:
        filename = os.path.basename(fileitem.filename)
        filepath = os.path.join(UPLOAD_DIR, filename)
        with open(filepath, 'wb') as f:
            f.write(fileitem.file.read())
        return f'The file "{filename}" was uploaded successfully.'
    else:
        return 'No file was uploaded.'

def handle_delete():
    query_string = os.environ.get('QUERY_STRING', '')
    params = urllib.parse.parse_qs(query_string)
    filename = params.get('delete', [None])[0]
    if filename:
        filepath = os.path.join(UPLOAD_DIR, filename)
        if os.path.exists(filepath):
            os.remove(filepath)
            return f'The file "{filename}" was deleted successfully.'
        else:
            return f'The file "{filename}" does not exist.'
    else:
        return 'No file was specified for deletion.'

# Generate the HTML response
def generate_html(message=''):
    files = os.listdir(UPLOAD_DIR)
    file_list = ''.join(f'<li>{file} <a href="/cgi-bin/upload_interface.py?delete={file}">Delete</a></li>' for file in files)
    return f"""
    <html>
    <head>
        <title>File Upload and Delete</title>
    </head>
    <body>
        <h2>{message}</h2>
        <h3>Upload a file</h3>
        <form action="/cgi-bin/" method="post" enctype="multipart/form-data">
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

# Determine the action to take
message = ''
if 'file' in form:
    message = handle_upload()
elif 'delete' in os.environ.get('QUERY_STRING', ''):
    message = handle_delete()

# Print the HTML response
print("Content-Type: text/html\r\n")
print("\r\n")
print(generate_html(message))
