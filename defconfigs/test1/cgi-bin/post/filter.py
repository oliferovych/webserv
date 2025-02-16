#!/usr/bin/env python3
import cgi
import os
import collections
from PIL import Image
import io

UPLOAD_DIR = os.environ.get('UPLOAD_DIR')
if not UPLOAD_DIR:
    raise EnvironmentError("UPLOAD_DIR environment variable is not set")

def save_uploaded_file(file_item, filename):
    """Save the uploaded file to the uploads directory."""
    file_path = os.path.join(UPLOAD_DIR, filename)
    with open(file_path, "wb") as f:
        f.write(file_item.file.read())
    return file_path

def generate_histogram(text):
    """Generate a character frequency histogram from text."""
    char_count = collections.Counter(text)
    histogram = [f"{char}: {'█' * count} {count}" for char, count in sorted(char_count.items()) if char.isprintable() and not char.isspace()]
    return histogram

def process_text_file(file_content, filename):
    """Generate a histogram from in-memory text content and return HTML output."""
    text_content = file_content.decode("utf-8")
    histogram = generate_histogram(text_content)

    result = f"<h1>Character Frequency Histogram</h1><h3>File: {filename}</h3>"
    if histogram:
        result += "<pre>" + "\n".join(histogram) + "</pre>"
    else:
        result += "<p>No printable characters found in the file.</p>"

    return result

def process_image_file(file_content, filename):
    """Convert an in-memory image to grayscale and return HTML output."""
    image = Image.open(io.BytesIO(file_content))  # Load image directly from memory
    grayscale_image = image.convert("L")

    filtered_filename = "grayscale_" + filename
    filtered_path = os.path.join(UPLOAD_DIR, filtered_filename)
    grayscale_image.save(filtered_path)  # Save for display

    return f'<h1>Grayscale Image</h1><p>Original: {filename}</p><p>Filtered Image:<br><img src="{filtered_path}" alt="Grayscale Image"></p>'

def main():
    print("Content-type: text/html\n")
    print("<html><head><title>File Processing</title></head><body>")

    form = cgi.FieldStorage()
    file_item = form["file"]

    if file_item.filename:
        filename = os.path.basename(file_item.filename.lower())  # Prevent directory traversal
        file_ext = os.path.splitext(filename)[1]

        file_content = file_item.file.read()  # Read file into memory

        # Save the file first
        file_path = save_uploaded_file(file_item, filename)

        # Process based on file type using in-memory content
        if file_ext in {".txt", ".csv", ".log"}:
            print(process_text_file(file_content, filename))
        elif file_ext in {".jpg", ".jpeg", ".png"}:
            print(process_image_file(file_content, filename))
        else:
            print(f"<p>Unsupported file type: {file_ext}. Please upload a text or image file.</p>")
    else:
        print("<p>No file uploaded.</p>")

    print("</body></html>")

if __name__ == "__main__":
    main()
