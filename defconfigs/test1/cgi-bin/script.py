#!/usr/bin/env python3

import os
import sys

# Print the content-type header
print("Content-Type: text/html\n")

try:
    # Read the raw POST data from stdin
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    raw_post_data = sys.stdin.read(content_length)

    # Print the raw POST data for debugging to stderr (terminal)
    print(f"Raw POST data:\n{raw_post_data}", file=sys.stderr)

    # Find the boundary
    boundary = raw_post_data.splitlines()[0].strip('--')  # Boundary comes as first line
    
    # Split the raw data using the boundary
    parts = raw_post_data.split(f'--{boundary}')
    
    # Initialize a variable to store the file content
    file_content = None

    for part in parts:
        # Skip the pre/post boundary parts and look for the 'file' part
        if 'Content-Disposition' in part and 'filename' in part:
            # Extract the file content (after the content-type headers)
            content_start = part.find("\r\n\r\n") + 4  # Skip the headers to get to the body
            file_content = part[content_start:]  # File content starts here
            break

    # If a file was uploaded, print its contents
    if file_content:
        print("<html><body>")
        print("<h2>Received File Content:</h2>")
        print(f"<pre>{file_content}</pre>")
        print("</body></html>")
    else:
        print("<html><body>")
        print("<h2>No file content found!</h2>")
        print("</body></html>")

except Exception as e:
    print(f"Error: {str(e)}", file=sys.stderr)
    sys.exit(1)
