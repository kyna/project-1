//
// a template for receiving face tracking osc messages from
// Kyle McDonald's FaceOSC https://github.com/kylemcdonald/ofxFaceTracker
//
// 2012 Dan Wilcox danomatika.com
// for the IACD Spring 2012 class at the CMU School of Art
//
// adapted from from Greg Borenstein's 2011 example
// http://www.gregborenstein.com/
// https://gist.github.com/1603230
//
import oscP5.*;
OscP5 oscP5;

int found;
int speed;
int width = 1100;
int height = 700;
int x, y;
int score = 0;

PImage lpic;
PImage fpic;
PImage spic;
Bug lonely;
Bug friendly;
Bug sassy;
boolean arrived = false;

ArrayList splatters;
PImage l1, l2, l3, f1, f2, f3, s1, s2, s3;

// pose
float poseScale;
PVector posePosition;
PVector poseOrientation;

// gesture
float mouthHeight;
float mouthWidth;

void setup() {
  size(width, height);
  frameRate(30);

  smooth();

  oscP5 = new OscP5(this, 8338);
  oscP5.plug(this, "found", "/found");
  oscP5.plug(this, "poseScale", "/pose/scale");
  oscP5.plug(this, "posePosition", "/pose/position");
  oscP5.plug(this, "poseOrientation", "/pose/orientation");
  oscP5.plug(this, "mouthWidthReceived", "/gesture/mouth/width");
  oscP5.plug(this, "mouthHeightReceived", "/gesture/mouth/height");

  poseOrientation = new PVector();
  posePosition = new PVector();

  speed = 150;

  lpic = loadImage("lonely.png");
  fpic = loadImage("friendly.png");
  spic = loadImage("sassy.png");

  splatters = new ArrayList();

  l1 = loadImage("l1.png");
  l2 = loadImage("l2.png");
  l3 = loadImage("l3.png");
  f1 = loadImage("f1.png");
  f2 = loadImage("f2.png");
  f3 = loadImage("f3.png");
  s1 = loadImage("s1.png");
  s2 = loadImage("s2.png");
  s3 = loadImage("s3.png");

  lonely = new Bug(new PVector(width/2, height/2), 1.5, 0.03, lpic);
  friendly = new Bug(new PVector(0, 0), 1.75, 0.04, fpic);
  sassy = new Bug(new PVector(-100, 0), 2.25, 0.05, spic);
}

void draw() {  
  background(0);
  noStroke();

  if (found <= 0) {
    fill(255);
    text("Searching for face...", 50, 50);
  }
  if (found > 0) {

    posePosition.x += speed * (0.3*poseOrientation.y);
    posePosition.y += speed * (0.3*poseOrientation.x);
    println("pose.x " + posePosition.x);
    println("pose.y " + posePosition.y);
    posePosition.x = constrain(posePosition.x, 0, width);
    posePosition.y = constrain(posePosition.y, 0, height);

    pushMatrix();

    translate(posePosition.x, posePosition.y);
    scale(poseScale);
    fill(175, 0, 0);
    ellipse(-50, 0, 20, 20);
    ellipse(50, 0, 20, 20);
    if (mouthHeight < 2) arc(0, 20, mouthWidth*5, mouthHeight*2, 0, PI);
    else { 
      arc(0, 20, mouthWidth*5, mouthHeight*10, 0, PI);
      //teeth
      fill(0);
      for (int i = -24; i <= 24; i += 6) {
        rect(i, 18, 2, 10, 1);
      }
    }
    popMatrix();

    fill(255, 255, 100);
    lonely.wander();
    lonely.run();
    fill(100, 255, 255);
    friendly.wander();
    friendly.run();
    fill(255, 100, 255);
    sassy.wander();
    sassy.run();

    if (mouthHeight > 2) {
      eaten(lonely, mouthWidth, mouthHeight, posePosition);
      eaten(friendly, mouthWidth, mouthHeight, posePosition);
      eaten(sassy, mouthWidth, mouthHeight, posePosition);
    }

    for (int j = splatters.size()-1; j >= 0; j--) {
      Splatter s = (Splatter) splatters.get(j);
      if (s.life <= 0) splatters.remove(j);
      s.run();
    }
    fill(255);
    text(score, 50, 50);
  }
}

void eaten(Bug bug, float mouthWidth, float mouthHeight, PVector posePosition) {
  if ((posePosition.x-(mouthWidth*4)-20 <= bug.loc.x) && 
    (bug.loc.x <= (3*(mouthWidth*2)+posePosition.x)+10)) {
    if ((posePosition.y+75 <= bug.loc.y) &&
      (bug.loc.y <= posePosition.y+(mouthHeight*20)+75)) {
      Splatter temp;
      PVector tempLoc = new PVector(bug.loc.x, bug.loc.y);  

      bug.loc.x = random(-100, 0);
      bug.loc.y = random(-100, 0);
      score++;

      if (bug.bug == lpic) {
        temp = new Splatter(tempLoc, (loadImage("l" + (int)random(1, 3) + ".png")));
        splatters.add(temp);
      }
      else if (bug.bug == fpic) {
        temp = new Splatter(tempLoc, (loadImage("f" + (int)random(1, 3) + ".png")));
        splatters.add(temp);
      }
      else if (bug.bug == spic) {
        temp = new Splatter(tempLoc, (loadImage("s" + (int)random(1, 3) + ".png")));
        splatters.add(temp);
      }
    }
  }
}

class Splatter {
  PVector loc;
  int life;
  PImage splat;

  Splatter(PVector l, PImage s) {
    loc = l;
    life = 255;
    splat = s;
  }

  void run() {
    pushMatrix();
    translate(loc.x, loc.y);
    tint(255, 255, 255, life);
    image(splat, 0, 0);
    if (life > 0) life--;
    popMatrix();
  }
}

class Bug {
  PVector loc;
  PVector vel;
  PVector acc;
  float r;
  float wandertheta;
  float maxForce;
  float maxSpeed;
  boolean dead;
  PImage bug;

  Bug(PVector l, float ms, float mf, PImage b) {
    acc = new PVector(0, 0);
    vel = new PVector(0, 0);
    loc = l.get();
    r = 3.0;
    wandertheta = 0.0;
    maxSpeed = ms;
    maxForce = mf;
    dead = false;
    bug = b;
  }

  void run() {
    update();
    borders();
    render();
  }

  void update() {
    vel.add(acc);
    vel.limit(maxSpeed);
    loc.add(vel);
    acc.mult(0);
  }

  void wander() {
    float wanderR = 1.0;
    float wanderD = 100.0;
    float change = 0.25;
    wandertheta += random(-change, change);

    //calc new target to steer towards
    PVector circleloc = vel.get();
    circleloc.normalize();
    circleloc.mult(wanderD);
    circleloc.add(loc);

    PVector circleOffset = new PVector(wanderR*cos(wandertheta), wanderR*sin(wandertheta));
    PVector target = PVector.add(circleloc, circleOffset);
    acc.add(steer(target, false)); //steer towards target
  }

  PVector steer(PVector target, boolean slowdown) {
    PVector steer;
    PVector desired = PVector.sub(target, loc); //vector pointing to target from loc
    float d = desired.mag(); //distance from target is magnitude of vector
    //if distance is greater than 0, calc steering, else return zero vector
    if (d > 0) {
      if ((arrived == false) && (x>0) && (y>0) && (d < 20)) arrived = true;
      desired.normalize();
      if ((slowdown)&&(d < 100.0f)) desired.mult(maxSpeed*(d/100.0f)); //damping
      else desired.mult(maxSpeed);

      steer = PVector.sub(desired, vel);
      steer.limit(maxForce); //don't surpass maxForce for smoothness of steering
    }
    else steer = new PVector(0, 0);
    return steer;
  }

  void render() {
    pushMatrix();
    translate(loc.x, loc.y);
    tint(255, 255, 255, 255);
    image(bug, 0, 0);
    popMatrix();
  }

  // Wraparound
  void borders() {
    if (loc.x < -r) loc.x = width+r;
    if (loc.y < -r) loc.y = height+r;
    if (loc.x > width+r) loc.x = -r;
    if (loc.y > height+r) loc.y = -r;
  }
}

// OSC CALLBACK FUNCTIONS

public void found(int i) {
  found = i;
}

public void poseScale(float s) {
  println("scale: " + s);
  poseScale = s;
}

public void poseOrientation(float x, float y, float z) {
  poseOrientation.set(x, y, z);
}

public void mouthWidthReceived(float w) {
  mouthWidth = w;
}

public void mouthHeightReceived(float h) {
  mouthHeight = h;
}

// all other OSC messages end up here
void oscEvent(OscMessage m) {
  if (m.isPlugged() == false) {
  }
}

void keyPressed() {
  if ((key=='q') || (key=='Q')) exit();
}

