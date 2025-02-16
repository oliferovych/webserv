#!/bin/bash

printf "Content-Type: text/html\r\n"
printf "\r\n"

# Get the current time
current_time=$(date +"%Y-%m-%d %H:%M:%S")

# Create the HTML content
html_content="
<html>
<head>
	<title>Current Time</title>
</head>
<body>
	<h1>Current Time</h1>
	<p>The current time is: $current_time</p>
</body>
</html>"

# Write the HTML content to a file
printf "$html_content"