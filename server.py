from flask import Flask, request, render_template
app = Flask(__name__)

@app.route("/script", methods=['POST'])
def script():
    input_string = request.form['data']

    """
    Your script code here
    """

    return "backend response"

@app.route('/')
def static_page():
    return render_template('index.html')

if __name__ == "__main__":
    app.run()