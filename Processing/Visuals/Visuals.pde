import punktiert.math.Vec; //<>//
import punktiert.physics.*;
import processing.opengl.*;
import apsync.*;
import processing.serial.*; 
import oscP5.*;
import netP5.*;
import spout.*;

OscP5 oscP5;
NetAddress pD;
VPhysics physics;  
VParticle center;
VParticleGroup repelers;
BAttraction attr;
BVortex vortex;
AP_Sync streamer;
Spout spout;

ArrayList<VParticle> repelersList = new ArrayList<VParticle>();
int amount = 1000, data[] = new int [8];
float time = millis();
float repelersPos [][] = new float [8][2];
public int data1, data2, data3, data4, data5, data6, data7, data0;


public void setup() {
  size(1360, 960, OPENGL);
  //fullScreen(OPENGL, 2);
  noStroke();
  fill(255, 255);
  repelersSetup();
  spout = new Spout(this);
  spout.createSender("Spout Processing");
  streamer = new AP_Sync(this, "COM8", 9600);
  oscP5 = new OscP5(this, 7200);
  pD = new NetAddress("127.0.0.1", 7200);

  physics = new VPhysics(width, height);
  physics.setfriction(0.1);

  center = new VParticle(width/2, height/2).lock();
  physics.addParticle(center);

  attr = new BAttraction(new Vec(width/2, height/2), 150, -1);
  physics.addBehavior(attr);

  vortex = new BVortex(new Vec(width/2, height/2), 300, 0.1);

  for (int i = 0; i < amount; i++) {

    float rad = random(3, 10);
    Vec pos = new Vec(random(width), random(height));
    VParticle particle = new VParticle(pos, 5, rad);
    particle.addBehavior(new BCollision(0));
    physics.addParticle(particle);
  }

  for (int i = 0; i < physics.particles.size(); i++) {
    VParticle p = physics.particles.get(i);
    physics.addSpring(new VSpringRange(p, center, 150, 250, 0.0003));
  }

  for (int i = 0; i < 8; i++) {  
    Vec pos = new Vec(repelersPos[i][0], repelersPos[i][1]);
    VParticle particle = new VParticle(pos, 5, 100).lock();
    particle.addBehavior(new BCollision(0.5));
    repelersList.add(particle);
    physics.addParticle(particle);
  }
  repelers = new VParticleGroup(repelersList);
  physics.addGroup(repelers);
}

public void draw() {
  background(0);
  dataRead();
  repelers.update();
  physics.update();
  for (int i = 0; i < 8; i++) {
    VParticle p = repelers.getParticle(i);
    if (data[i] < 100 && data[i] > 0) {
      p.radius = 50;
    } else if (data[i] >= 100) {
      p.radius = 100;
    } else {
      p.radius = 1;
    }
    //p.radius = map(data[i],0,255,0,100);
  }
  for (int i = 8; i < physics.particles.size(); i++) {
    VParticle p = physics.particles.get(i);
    vortex.apply(p);
    if (p.getRadius() < 25) {
      fill(255);
      ellipse(p.x, p.y, p.getRadius(), p.getRadius());
    }
  }
  gradientEllipse(width/2, height/2, 180, 180, 255, 0, 100);
  fill(0);
  ellipseMode(CENTER);
  ellipse(width/2, height/2, 200, 200);
  spout.sendTexture();
}
