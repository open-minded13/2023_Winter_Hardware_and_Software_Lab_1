
var numReg = /[1-9]/g;
var wordReg = /[\u0041-\u005a]+/g;


var str=songs[songName];
var shows;
var words;
var wordsIndex;
var wordsCurrent;
var current;

var songs={
    'star': 
    `1 1 4 4 5 5 4 3 4 3 3 2 2 1 
     C C G G A A G F F E E D D C

     5 5 4 4 3 3 2 5 5 4 4 3 3 2 
     G G F F E E D G G F F E E D

     1 1 4 4 5 5 4 3 4 3 3 2 2
     C C G G A A G F F E E D D C`,
};
function start(){

}

function show_num(obj,num){
    obj.innerHTML=num
    //obj.setAttribute("class","hover")
    obj.style.backgroundColor=rgb(255, 97, 97)
}

function recover(obj){
    obj.innerHTML=""
    //obj.setAttribute("class","hover ")
    obj.style.backgroundColor=rgb(255,255,255)
    
}



