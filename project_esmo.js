var oxford = require('project-oxford'),
    client = new oxford.Client('fbbc622b85be4e6797e0c805f18ab207');
var five = require("johnny-five");
var BeagleBone = require('beaglebone-io');
var AIO = require('adafruit-io');

var AIO_KEY = 'e93a27ca383cd5332c39a790b23df1500499f696',
    AIO_USERNAME = 'muonide';
 
var aio = AIO(AIO_USERNAME, AIO_KEY);

var board = new five.Board({ 
  io: new BeagleBone()
});

board.on("ready", function() {
  var anode = new five.Led.RGB({
    pins: {
      red: 6,
      green: 5,
      blue: 9
    },
    isAnode: true
  });

client.emotion.analyzeEmotion({
    path: './myFace.jpg',
}).then(function (response) {
    console.log(response);
    anode.on();

	var highestEmotion;
	var highestScore = 0;
    for (var emotion in response[0].scores) {
        var score = response[0].scores[emotion];
        if (score > highestScore) {
            highestScore = score;
            highestEmotion = emotion;
        }
    }

    highestEmotion.pipe(aio.feeds('emotion'));
    
    if (highestEmotion == 'sadness'){ //check what the response field really
    anode.color("#FFFF00");
	}
	if (highestEmotion == 'happiness') {
 	 anode.color("#0000FF");
	}
	if (highestEmotion == 'neutral'){
		anode.color("#FFFFFF")
	}

});

  this.repl.inject({
    anode: anode
  });

});

