load(system.exec_dir + '../web/lib/init.js');
load('hmac.js');

function a2h(str) {
    var hex = '';
    str.split('').forEach(function(ch) { hex += parseInt(ascii(ch), 16); });
    return hex;
}

function b2h(str) {
    if (typeof str !== 'string') str += '';
    var ret = '';
    for (var i = 0; i < str.length; i++) {
        var n = ascii(str[i]).toString(16);
        ret += n.length < 2 ? '0' + n : n;
    }
    return ret;
}

var payload = http_request.query.payload[0];

var f = new File(system.ctrl_dir + '../github.txt');
f.open('w');
f.writeln(b2h(hmac_sha1(a2h(settings.github_secret), payload)));
f.writeln(http_request.header['x-hub-signature']);
f.close();