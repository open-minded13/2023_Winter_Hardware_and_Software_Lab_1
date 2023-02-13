<html>
    <head>
     <script src="test_piano.js"></script>
     <!--  <script>
            buttons.forEach((el,index)=>el.textContent = index+1)
        </script>-->
        <style type="text/css">
 /*html{
    background: #000;
        font-family: 'Noto Serif', serif;
        -webkit-font-smoothing: antialiased;
        text-align: center;
 }*/
 ul.piano{
  /*width:480px;*/
  width: 960px;
  height:360px;
  transform: translate(-50%, -50%);
  position: absolute;
  top: 50%;
  left: 50%;
 }
 li{
  float:left;
  list-style-type: none;
  position: relative;
 }
 li>div{
  height: 360px;
  width: 60px;
  background: rgba(255,255,255,.3);
  border: 1px solid;
  border-color:rgba(0, 0, 0, .8);
  border-bottom-left-radius : 8px;
  border-bottom-right-radius: 8px;
  box-sizing: border-box;
  box-shadow: /*inset  3px 0  10px  #c9c6c6,*/
     inset 5px -8px  0  #00000023;
  text-align: center;
  display:table-cell;
  vertical-align: bottom;
 }
 li:not(:last-child)>div{
  border-right: none;
 }
 li>div:hover{
  background: rgb(255, 97, 97);

 }
 /* Style when the white button is pressed */
 .white_active{
  box-shadow: inset 3px -2px  0  #00000036;
  background: linear-gradient(45deg, rgba(255,255,255,.7), rgba(255,255,255,.5));
 }

 li>p{
  height: 200px;
  width: 40px;
  background-color: #000;
  border-bottom-left-radius : 5.5px;
  border-bottom-right-radius: 5.5px;
  box-shadow: inset 5px -7px 0 #2c2c2c;
  position: absolute;
  top:0;
  left: 40px;
  z-index: 999;
 }
 li>p:hover{
  background: linear-gradient(45deg, #4c4c4c, #444444);
 }
 /* Style when the black button is pressed */
 .black_active{
  box-shadow:inset 3px -5px 0 #2c2c2c;
 }
        </style>
    </head>

<body>
    <ul class="piano">
        <li><div id="C1">C</div><p></p></li>
        <li><div id="D1">D</div><p></p></li>
        <li><div id="E1">E</div></li>
        <li><div id="F1">F</div><p></p></li>
        <li><div id="G1">G</div><p></p></li>
        <li><div id="A1">A</div><p></p></li>
        <li><div id="B1">B</div></li>
        <!--<li><div>B</div></li>-->
        <li><div id="C2">C</div><p></p></li>
        <li><div id="D2">D</div><p></p></li>
        <li><div id="E2">E</div></li>
        <li><div id="F2">F</div><p></p></li>
        <li><div id="G2">G</div><p></p></li>
        <li><div id="A2">A</div><p></p></li>
        <li><div id="B2">B</div></li>
        <li><div id="C3">C</div></li>
        
     </ul>
     <ul >
        <li>C</li>
        <li>D</li>
        <li>E</li>
        <li>F</li>
        <li>G</li>
        <li>A</li>
        <li>B</li>
        <li>C</li>
        <li>D</li>
        <li>E</li>
        <li>F</li>
        <li>G</li>
        <li>A</li>
        <li>B</li>
        <li>C</li>
        
     </ul>
        
    
</body>
</html>




