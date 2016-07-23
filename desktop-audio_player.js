var AIO = require('adafruit-io');

var AIO_KEY = 'xxxxxxxxxxx',
    AIO_USERNAME = 'muonide';

 var Player = require('player');
 
// create player instance 
var player = new Player('./xxx.mp3');
 
aio.feeds('emotion').pipe(emotion);
// play now and callback when playend 
if (emotion == 'sad'){
player.play(function(err, player){
  console.log('playend!');
})
}