#!/usr/bin/python
from time import gmtime, strftime

print("Content-Type: text/html\n")
print("<html>")
print("<body>")
print("<h2>Hello World From A Python Cgi !</h2>")
print("</br> time: ")
print(strftime("%Y-%m-%d %H:%M:%S", gmtime()))
print("</html>")
print("</body>")

# print ("Content-Type: text/html")
# print ()
# print ("""
# <html>
# <body>
# <h2>Hello World From A Python Cgi !</h2>
# </body>
# </html>
# """)