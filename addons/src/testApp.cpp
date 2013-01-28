#include "testApp.h"
#include <gl/glut.h>

float lightPos0[] = {100, 150, -200, 1};

void testApp::setup(){

	ofEnableAlphaBlending();

	// this uses depth information for occlusion
	// rather than always drawing things on top of each other
	glEnable(GL_DEPTH_TEST);

	// the sample simulations are quite small in scale, so
	// we wanna make sure our camera is close enough
	cam.setDistance(50);


	// add the simulations to the vector and initialize the first one
	simulations.push_back(&flocking);
	simulations.push_back(&pathFollowing);
	simulations.push_back(&obstacleAvoidance);
    simulations.push_back(&pursuitAndEvade);

	currentSimulation = NULL;
	setSimulation(0);

	setLight();

	glClearColor(0.3, 0.3, 1.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_CULL_FACE);

    moBlur.setup();
}

void testApp::update(){
	currentSimulation->update();
	moBlur.update();
}

void testApp::draw(){

	ofBackground(0, 0, 0);

    moBlur.begin();
        xPos+=1;
        if(xPos>ofGetWidth()) xPos = 0;
        cam.begin();
		currentSimulation->draw();
        cam.end();
    moBlur.end();

    moBlur.draw();
}

void testApp::keyPressed( int key ){

	if( key == OF_KEY_LEFT ){
		simulationIndex--;
		if(simulationIndex < 0) simulationIndex = simulations.size() - 1;
		setSimulation(simulationIndex);
	}
	else if( key == OF_KEY_RIGHT ){
		simulationIndex++;
		if(simulationIndex > simulations.size() - 1) simulationIndex = 0;
		setSimulation(simulationIndex);
	}
}

void testApp::setLight(){
    float   lightAmbient0[] =   {0.5, 0.5, 0.5, 1.0};
    float   lightDiffuse0[] =   {1.0, 1.0, 1.0, 1.0};
    float   lightSpecular0[] =   {1.0, 1.0, 1.0, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular0);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
}

void testApp::setSimulation( int simulationIndex ){
	// just make sure we are inside out vector
	this->simulationIndex = ofClamp(simulationIndex, 0, simulations.size() - 1);

	// exit the current simulation
	if(currentSimulation) currentSimulation->exit();

	// define the current simulation
	currentSimulation = simulations[simulationIndex];

	// initialize the current simulation
	currentSimulation->setup();
}
