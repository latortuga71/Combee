import sys
import json
from flask import Flask,request,jsonify

app = Flask(__name__)

@app.route("/",methods = ["POST"])
def post():
    #headers = request.headers
    #agent = headers.get("User-Agent")
    if request.is_json:
        content = request.get_json()
        print(content)
        #print(jsonify({"message":"OK"}),200)
        return jsonify({"message":"OK"},200)
    else:
        return jsonify({"message":"ERROR Invalid Json"})

if __name__ == "__main__":
    app.run(host="127.0.0.1",port=8000,ssl_context='adhoc') # app.run(ssl_context='adhoc')
