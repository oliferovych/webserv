#!/bin/bash

# Get the current time
current_time=$(date +"%Y-%m-%d %H:%M:%S")

# Create the HTML content
html_content="
<html>
<head>
	<title>Current Time</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Neon&display=swap');
	    body {
        font-family: 'Arial', sans-serif;
        background-color: #282c34;
        color: #61dafb;
        margin: 0;
        padding: 0;
        display: flex;
        flex-direction: column;
        align-items: center;
        height: 100vh;
        }
        .title, .neon-text {
            margin: 50px 0;
            font-size: 10em;
            color: #61dafb;
            font-family: 'Neon', sans-serif;
            animation: neon 1.5s ease-in-out infinite alternate, flicker 3s infinite;
        }
        h2, h3 {
            color: #61dafb;
            font-family: 'Neon', sans-serif;
            animation: neon 1.5s ease-in-out infinite alternate;
        }
        @keyframes neon {
            from {
                text-shadow: 0 0 5px #61dafb, 0 0 10px #61dafb, 0 0 15px #61dafb, 0 0 20px #61dafb, 0 0 25px #61dafb, 0 0 30px #61dafb, 0 0 35px #61dafb;
            }
            to {
                text-shadow: 0 0 10px #61dafb, 0 0 20px #61dafb, 0 0 30px #61dafb, 0 0 40px #61dafb, 0 0 50px #61dafb, 0 0 60px #61dafb, 0 0 70px #61dafb;
            }
        }
        @keyframes flicker {
            0%, 19%, 21%, 23%, 25%, 54%, 56%, 100% {
                opacity: 1;
            }
            20%, 24%, 55% {
                opacity: 0.5;
            }
            22% {
                opacity: 0.1;
            }
        }
	</style>
</head>
<body>
	<div class='title'>Time</div>
	<p>The current time is: $current_time</p>
</body>
</html>"

# Write the HTML content to a file
echo "$html_content"
