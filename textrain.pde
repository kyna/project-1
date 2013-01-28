import processing.video.*;
Capture cam;

int height = 480;
int width = 640;

int g = 0;
int b = 100;
boolean flipG = false;
boolean flipB = true;

class Letter {
  int y;
  int x;
  int speed;
  char c;

  Letter(char character, int xPos, int yPos) {
    c = character;
    x = xPos;
    y = yPos;
    speed = (int) random(1, 2);
  }
}

ArrayList<Letter> currentLetters;
Letter testLetter = new Letter('a', 50, 50);

void setup() {
  size(width, height);
  background(255);
  frameRate(10);

  textSize(18);

  cam = new Capture(this, width, height);
  cam.start();

  smooth();

  currentLetters = new ArrayList();
  String poem = "anyone lived in a pretty how town (with up so floating many bells down)";

  int xInit = 0;
  int yInit = 0;

  for (int i=0; i < 71; i++) {
    if (i<33) yInit = (int)random(-10, 10);
    else yInit = (int)random(-15, -25);
    currentLetters.add(new Letter(poem.charAt(i), xInit, yInit));
    xInit += (9 + (int)random(-3, 3));
    if (xInit > width) xInit = 9;
  }
}

void draw() {
  cam.read();
  cam.loadPixels();

  pushMatrix();
  translate (width, 0); 
  scale(-1, 1);
  image (cam, 0, 0, width, height);
  popMatrix();

  fill(0, g, 100);

  for (int i=0; i < 71; i++) {
    Letter curr = currentLetters.get(i);

    int index = width*curr.y + (width-curr.x-1);
    index = constrain (index, 0, width*height-1);

    if ((brightness(cam.pixels[index])) > 105) curr.y += curr.speed;
    else {
      while ( (curr.y > 0.0) && ((brightness(cam.pixels[index])) < 95)) {
        curr.y -= .025;
        index = width*curr.y + (width-curr.x-1);
        index = constrain (index, 0, width*height-1);
      }
    }
    text(curr.c, curr.x, curr.y);

    if (curr.y >= height) {
      curr.y = (int)random(-15, 15);
      curr.speed = (int)random(1, 2);
    }
  }

  if(!flipG) {
    if (g<100) g++;
    else flipG=true;
  }
  else if(flipG) {
    if (g>0) g--;
    else flipG=false;
  }

  if(!flipB) {   
    if (b<150) b++;
    else flipB=true;
  }
  else if(flipB) {
    if (b>0) b--;
    else flipB=false;
  }
}

