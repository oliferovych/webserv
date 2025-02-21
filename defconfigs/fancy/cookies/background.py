import os
import sys
import json
import random

DB_FILE = os.getenv("SESSION_DB", "sessionsDB.json")

def load_sessions():
    try:
        if os.path.exists(DB_FILE):
            with open(DB_FILE, "r") as file:
                content = file.read().strip()
                if not content:
                    raise ValueError(f"The session database file {DB_FILE} has wrong format (cant be empty).")
                return json.loads(content)
        else:
            raise FileNotFoundError(f"Session database file {DB_FILE} not found.")
    except (IOError, json.JSONDecodeError, ValueError) as e:
        sys.stderr.write(f"Error accessing session database: {str(e)}\n")
        sys.exit(1)  # Stop execution

def save_sessions(sessions):
    try:
        with open(DB_FILE, "w") as file:
            json.dump(sessions, file, indent=4)
    except IOError as e:
        sys.stderr.write(f"Error saving sessions: {str(e)}\n")
        sys.exit(2)

def generate_random_color():
    return "#{:06X}".format(random.randint(0, 0xFFFFFF))

def get_session_color(session_id):
    sessions = load_sessions()

    if session_id in sessions:
        return sessions[session_id]["color"]

    # Generate a new color for new session
    new_color = generate_random_color()
    sessions[session_id] = {"color": new_color}
    save_sessions(sessions)

    return new_color

session_id = os.getenv("SESSION_ID")

if not session_id:
	sys.stderr.write(f"session_ID failed")
	sys.exit(3)


background_color = get_session_color(session_id)

# print("Content-Type: text/plain\r\n\r\n")
# print(DB_FILE)
# print(session_id)
# print(background_color)

print("Content-Type: text/html\r\n\r\n")
print(f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Random Background Color</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            background-color: {background_color};
            transition: background-color 0.5s ease-in-out;
        }}
        .container {{
            text-align: center;
            color: white;
            background-color: rgba(0, 0, 0, 0.2);
            padding: 10px 40px;
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.3);
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
        }}
        h1 {{
            font-size: 2.5rem;
            margin-bottom: 20px;
        }}
        button {{
            background-color: #f44336;
            border: none;
            color: white;
            font-weight: bold;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
            margin-bottom: 20px;
        }}
        button:hover {{
            background-color: #d32f2f;
        }}
    </style>
</head>
<body id="body">
    <div class="container">
        <h1>Cookie</h1>
        <button onclick="deleteSession()">Delete Session</button>
    </div>
    <script>
        function deleteSession() {{
            document.cookie = "session_id=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/";
            document.cookie = 'session_id=reset; path=/';
            location.reload();
        }}
    </script>
</body>
</html>""")
