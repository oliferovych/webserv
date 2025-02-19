#!/usr/bin/env python3
import cgi
import os
import collections

def generate_histogram(text):
    text = text.lower()
    char_count = collections.Counter(text)
    
    max_count = max(char_count.values(), default=1)

    max_bar_length = 400
    min_bar_length = 1

    if max_count < 10:
        max_bar_length = max_count
    
    histogram = [
        f"{char.upper()}: {'█' * max(min_bar_length, int(count / max_count * max_bar_length))} {count}"
        for char, count in sorted(char_count.items(), key=lambda item: (-item[1], item[0]))
        if char.isprintable() and not char.isspace()
    ]

    return histogram



def process_text_file(file_content, filename):
    text_content = file_content.decode("utf-8")
    histogram = generate_histogram(text_content)

    result = f"<h1>Character Frequency Histogram</h1><h3>File: {filename}</h3>"
    if histogram:
        result += "<pre>" + "\n".join(histogram) + "</pre>"
    else:
        result += "<p>No printable characters found in the file.</p>"

    return result

def main():
    print("Content-type: text/html\r\n\r\n")
    print("<html><head><title>File Processing</title></head><body>")


    form = cgi.FieldStorage()
    file_item = form["file"]

    if file_item.filename:
        filename = os.path.basename(file_item.filename.lower())
        file_ext = os.path.splitext(filename)[1]

        file_content = file_item.file.read()

        if file_ext in {".txt", ".csv", ".log", ".html"}:
            print(process_text_file(file_content, filename))
        else:
            print(f"<p>Unsupported file type: {file_ext}. Please upload a text file.</p>")
    else:
        print("<p>No file uploaded.</p>")

    print('''
    <button onclick="window.location.reload();">Back</button>
    ''')
    print("</body></html>")



if __name__ == "__main__":
    main()
