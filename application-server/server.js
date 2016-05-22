var express = require('express'),
    app = express();

app.use(express.static(__dirname + '/public'));

console.log('Server starting at http://127.0.0.1:8080/\n');
app.listen(8080);
