void gradientEllipse(float x, float y, float radiusX, float radiusY, int inAlpha, int outAlpha, int circle) {
  noStroke();
  beginShape(TRIANGLE_STRIP);
  for (float theta=0; theta<TWO_PI; theta+=TWO_PI/92) {
    fill(0, inAlpha);
    vertex(x+circle*cos(theta), y+circle*sin(theta));
    fill(0, outAlpha);
    vertex(x+radiusX*cos(theta), y+radiusY*sin(theta));
  }
  endShape();
} 

void dataRead(){
    data[0] = data0;
    OscMessage data0Message = new OscMessage("/data0");
    data0Message.add(data0);
    oscP5.send(data0Message, pD);
    
    data[1] = data1;
    OscMessage data1Message = new OscMessage("/data1");
    data1Message.add(data1);
    oscP5.send(data1Message, pD);
    
    data[2] = data2;
    OscMessage data2Message = new OscMessage("/data2");
    data2Message.add(data2);
    oscP5.send(data2Message, pD);
    
    data[3] = data3;
    OscMessage data3Message = new OscMessage("/data3");
    data3Message.add(data3);
    oscP5.send(data3Message, pD);
    
    data[4] = data4;
    OscMessage data4Message = new OscMessage("/data4");
    data4Message.add(data4);
    oscP5.send(data4Message, pD);
    
    data[5] = data5;
    OscMessage data5Message = new OscMessage("/data5");
    data5Message.add(data5);
    oscP5.send(data5Message, pD);
    
    data[6] = data6;
    OscMessage data6Message = new OscMessage("/data6");
    data6Message.add(data6);
    oscP5.send(data6Message, pD);
    
    data[7] = data7;
    OscMessage data7Message = new OscMessage("/data7");
    data7Message.add(data7);
    oscP5.send(data7Message, pD);
}

void oscEvent(OscMessage theOscMessage) {
  //print("### received an osc message.");
  //print(" addrpattern: "+theOscMessage.addrPattern());
  //println(" typetag: "+theOscMessage.typetag());
}

void repelersSetup() {
  repelersPos[0][0] = width/2;
  repelersPos[0][1] = height/2+180;
  repelersPos[1][0] = width/2+140;
  repelersPos[1][1] = height/2+140;
  repelersPos[2][0] = width/2+180;
  repelersPos[2][1] = height/2;
  repelersPos[3][0] = width/2+140;
  repelersPos[3][1] = height/2-140;
  repelersPos[4][0] = width/2;
  repelersPos[4][1] = height/2-180;
  repelersPos[5][0] = width/2-140;
  repelersPos[5][1] = height/2-140;
  repelersPos[6][0] = width/2-180;
  repelersPos[6][1] = height/2;
  repelersPos[7][0] = width/2-140;
  repelersPos[7][1] = height/2+140;
}
