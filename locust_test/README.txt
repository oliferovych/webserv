Command to run locust:

locust -f locust_test/locustfile.py --host=http://localhost

--host=<hostname>   -----   hostname and the brackets must be replacesd with a link in format "http://" + link

locust then hosts a server of its own on port 8089(at lest for linux)
by going to localhost:8089 you can access the website with the tester
