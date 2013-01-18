var h = require('./build/Release/erlnode.node');

var hi = new h.ErlNode("e1@localhost", "secret");
console.log(hi.info());
var start = new Date();
console.log(start);
//console.log(hi.send_by_name("elogger", '[~w, ~s]', "self", "NEW_ORDER"));
for(var i=1; i<= 1000; i++) {
  console.log(hi.send_by_name("elogger", '[~w, ~s]', "self", i));
  console.log(hi.receive());
}


console.log((new Date() - start) + 'ms');
console.log('msg count: ' +hi.count());
