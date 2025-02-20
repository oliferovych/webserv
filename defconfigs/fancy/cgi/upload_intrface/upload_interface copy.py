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
    params = parse.parse_qs(query_string)
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
    file_list = ''.join(f'<li>{file} <a href="/cgi/upload_interface.py?delete={file}">Delete</a></li>' for file in files)
    return f"""
    <html>
    <head>
        <title>File Upload and Delete</title>
        <style>
            body {{
                font-family: 'Arial', sans-serif;
                background-color: #282c34;
                color: #61dafb;
                margin: 0;
                padding: 0;
                display: flex;
                justify-content: center;
                align-items: center;
                height: 100vh;
            }}

            .container {{
                background-color: #20232a;
                padding: 20px;
                border-radius: 10px;
                box-shadow: 0 0 20px rgba(0, 0, 0, 0.5);
                width: 500px;
                text-align: center;
            }}

            .container h1 {{
                margin-bottom: 20px;
                font-size: 4em;
                color: #61dafb;
                animation: glow 1s ease-in-out infinite alternate;
            }}

            @keyframes glow {{
                from {{
                    text-shadow: 0 0 10px #61dafb, 0 0 20px #61dafb, 0 0 30px #61dafb, 0 0 40px #61dafb, 0 0 50px #61dafb, 0 0 60px #61dafb, 0 0 70px #61dafb;
                }}

                to {{
                    text-shadow: 0 0 20px #61dafb, 0 0 30px #61dafb, 0 0 40px #61dafb, 0 0 50px #61dafb, 0 0 60px #61dafb, 0 0 70px #61dafb, 0 0 80px #61dafb;
                }}
            }}

            .upload-btn-wrapper {{
                position: relative;
                overflow: hidden;
                display: inline-block;
            }}

            .btn {{
                border: 2px solid #61dafb;
                color: #61dafb;
                background-color: transparent;
                padding: 10px 20px;
                border-radius: 8px;
                font-size: 16px;
                cursor: pointer;
                transition: all 0.3s ease;
            }}

            .btn:hover {{
                background-color: #61dafb;
                color: #20232a;
            }}

            .upload-btn-wrapper input[type=file] {{
                font-size: 100px;
                position: absolute;
                left: 0;
                top: 0;
                opacity: 0;
            }}

            .file-list {{
                margin-top: 20px;
                text-align: left;
                color: #61dafb;
            }}

            .file-item {{
                display: flex;
                justify-content: space-between;
                align-items: center;
                padding: 10px;
                border-bottom: 1px solid #61dafb;
            }}

            .delete-btn {{
                background-color: #f44336;
                color: white;
                border: none;
                padding: 5px 10px;
                border-radius: 5px;
                cursor: pointer;
                transition: background-color 0.3s ease;
            }}

            .delete-btn:hover {{
                background-color: #d32f2f;
            }}

            .dropdown {{
                margin-top: 10px;
                background-color: #333;
                border-radius: 8px;
                padding: 10px;
                display: none;
                text-align: left;
            }}

            .dropdown h2 {{
                margin: 0;
                font-size: 1.5em;
                color: #61dafb;
            }}

            .dropdown p {{
                margin: 10px 0;
                color: #61dafb;
            }}

            .dropdown-btn {{
                background-color: #61dafb;
                color: #20232a;
                border: none;
                padding: 10px 20px;
                border-radius: 8px;
                cursor: pointer;
                transition: background-color 0.3s ease;
            }}

            .dropdown-btn:hover {{
                background-color: #4a90e2;
            }}

            .upload-zone {{
                border: 2px dashed #61dafb;
                padding: 40px;
                text-align: center;
                margin: 20px 0;
                border-radius: 8px;
                transition: border-color 0.3s ease;
                cursor: pointer;
            }}

            .upload-zone:hover {{
                border-color: #4a90e2;
            }}

            .upload-button {{
                background-color: #61dafb;
                color: #20232a;
                border: none;
                padding: 10px 20px;
                border-radius: 8px;
                cursor: pointer;
                transition: background-color 0.3s ease;
            }}

            .upload-button:hover {{
                background-color: #4a90e2;
            }}

            .file-input {{
                display: none;
            }}

            .delete-input {{
                margin-top: 20px;
                padding: 10px;
                border: 2px solid #61dafb;
                border-radius: 8px;
                background-color: transparent;
                color: #61dafb;
                font-size: 16px;
                width: calc(100% - 24px);
            }}

            .delete-input::placeholder {{
                color: #61dafb;
            }}

            .delete-file-btn {{
                margin-top: 10px;
                background-color: #f44336;
                color: white;
                border: none;
                padding: 10px 20px;
                border-radius: 8px;
                cursor: pointer;
                transition: background-color 0.3s ease;
            }}

            .delete-file-btn:hover {{
                background-color: #d32f2f;
            }}

            .message {{
                margin-top: 20px;
                padding: 10px;
                border-radius: 8px;
                background-color: #333;
                color: #61dafb;
                display: none;
            }}
        </style>
    </head>
    <body>
        <div class="container">
            <h1>Upload Files</h1>
            <div class="upload-zone" id="dropZone">
                <input type="file" id="fileInput" class="file-input">
                <button class="upload-button" onclick="document.getElementById('fileInput').click()">
                    {message}
                </button>
            </div>
            <button class="dropdown-btn" onclick="toggleDropdown()">More Info</button>
            <div class="dropdown" id="dropdown">
                <h2>Instructions</h2>
                <p>Drag and drop files into the upload zone or click the "Choose File" button to select files from your computer.</p>
                <p>You will be able to delete the uploaded files from the server by typing the filename down below and pressing "Delete File".</p>
            </div>
            <input type="text" id="deleteInput" class="delete-input" placeholder="Enter file name to delete">
            <button class="delete-file-btn" id="deleteFileBtn">Delete File</button>
            <div class="message" id="message"></div>
        </div>
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
print("Content-Type: text/html\r\n\r\n")
print(generate_html(message))
