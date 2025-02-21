<?php

#!/usr/bin/env php

echo "Content-type: text/html\n\n";
echo "<html>";
echo "<head>";
echo "<title>Current Time</title>";
echo "<style>";
echo "body { font-family: Arial, sans-serif; background-color: #f0f0f0; text-align: center; padding-top: 50px; }";
echo ".time-container { background-color: #fff; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); display: inline-block; }";
echo ".time { font-size: 2em; color: #333; }";
echo "</style>";
echo "</head>";
echo "<body>";
echo "<div class='time-container'>";
echo "<h1>Current Time</h1>";
echo "<p class='time'>" . date("Y-m-d H:i:s") . "</p>";
echo "</div>";
echo "</body>";
echo "</html>";
?>
