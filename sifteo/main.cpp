/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "assets.gen.h"
using namespace Sifteo;

unsigned north = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::BLUE;
unsigned south = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::RED;
unsigned west = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::ORANGE;
unsigned east = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::GREEN;
unsigned shake = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::BLACK;
unsigned touch = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::PURPLE;
unsigned start = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::GRAY;

int steps [] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int index = 0;
unsigned turn = 00;
int end = 0;
int allowed = 1;
int playerAt = 0;

static Metadata M = Metadata()
    .title("Sensors SDK Example")
    .package("com.sifteo.sdk.sensors", "1.1")
    .icon(Icon)
    .cubeRange(0, CUBE_ALLOCATION);

static VideoBuffer vid[CUBE_ALLOCATION];
static TiltShakeRecognizer motion[CUBE_ALLOCATION];

class SensorListener {
public:
    struct Counter {
        unsigned touch;
        unsigned neighborAdd;
        unsigned neighborRemove;
    } counters[CUBE_ALLOCATION];

    void install()
    {
        Events::neighborAdd.set(&SensorListener::onNeighborAdd, this);
        Events::neighborRemove.set(&SensorListener::onNeighborRemove, this);
        //Events::cubeAccelChange.set(&SensorListener::onAccelChange, this);
        Events::cubeTouch.set(&SensorListener::onTouch, this);
        Events::cubeConnect.set(&SensorListener::onConnect, this);

        // Handle already-connected cubes
        for (CubeID cube : CubeSet::connected())
            onConnect(cube);
    }

private:
    void onConnect(unsigned id)
    {
        CubeID cube(id);
        uint64_t hwid = cube.hwID();

        bzero(counters[id]);
        LOG("Cube %d connected\n", id);

        vid[id].initMode(BG0_ROM);
        vid[id].attach(id);
        motion[id].attach(id);

		vid[cube].bg0rom.fill(vec(0,0), vec(16,16), start);

        String<128> str;

		if(cube == 2) str << "MIDDLEMAN\n";
		else if(cube == 0)	str << "Player 1\n";
		else if(cube == 1) str << "Player 2\n";
        vid[cube].bg0rom.text(vec(4,8), str);
    }

	void conclusion(int win)
	{
		vid[00].bg0rom.fill(vec(0,0), vec(16,16), start);
		String<128> str;
		if(win == 0) str << "DRAW\n";
		else{
			if(turn == 00) str << "   P1 LOSES  \n";
			else str << "   P2 LOSES  \n";
		}
		end = 1;
		str << "     END     \n\nTAP TO RESTART";
		vid[2].bg0rom.text(vec(1,7),str);
	}

	void checkIn()
	{
		if(playerAt >= 20) conclusion(0);
		if(playerAt == allowed){
			allowed++;
			if(turn == 00) turn = 01;
			else turn = 00;
			playerAt = 0;
		}
	}

    void onTouch(unsigned id)
    {
		if((end == 0)&&(id == turn)){
			CubeID cube(id);
			counters[id].touch++;
			LOG("Touch event on cube #%d, state=%d\n", id, cube.isTouching());

			vid[id].bg0rom.fill(vec(0,0), vec(16,16), touch);

			String<128> str;
			str << "TOUCH\n";
			vid[cube].bg0rom.text(vec(5,8),str);

			if(cube.isTouching() == 1){
				if(steps[playerAt] == 0) steps[playerAt] = 1;
				else if(steps[playerAt] != 1){
					//LOSER
					conclusion(1);
				}

				index++;
				playerAt++;
				LOG("allowed is %d\n", allowed);
				LOG("playerAt is %d\n", playerAt);
				checkIn();
			}
		}
		if((end == 1)&&(id == 02)){
			end = 0;
			onConnect(00);
			onConnect(01);
			onConnect(02);
			index = 0;
			playerAt = 0;
			allowed = 1;
			for(int i=0; i<=20; i++) steps[i] = 0;
			turn = 0;
		}
    }

    void onAccelChange(unsigned id)
    {
		if((end == 0)&&(id == turn)){
			CubeID cube(id);
			auto accel = cube.accel();

	        unsigned changeFlags = motion[id].update();
		    if (changeFlags) {
				// Tilt/shake changed
	            LOG("Tilt/shake changed, flags=%08x\n", changeFlags);

				if((changeFlags > 80)&&(changeFlags < 300)){
					vid[id].bg0rom.fill(vec(0,0), vec(16,16), shake);

					String<128> str;
					str << "SHAKE\n";
					vid[cube].bg0rom.text(vec(5,8),str);

					if(steps[playerAt] == 0) steps[playerAt] = 2;
					else if(steps[playerAt] != 2){
						//LOSER
						conclusion(1);
					}
					index++;
					playerAt++;
					LOG("allowed is %d\n", allowed);
					LOG("playerAt is %d\n", playerAt);
					checkIn();
				}
				auto tilt = motion[id].tilt;
			}
		}
    }

    void onNeighborRemove(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
		if(end == 0){
			LOG("Neighbor Remove: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);

	        if (firstID < arraysize(counters)) {
		        counters[firstID].neighborRemove++;
	        }
		    if (secondID < arraysize(counters)) {
			    counters[secondID].neighborRemove++;
		    }
		}
    }

    void onNeighborAdd(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
		unsigned side;
		unsigned id;
			if(firstID == 02){
				side = secondSide;
				id = secondID;
			}
			else if (secondID == 02){
				side = firstSide;
				id = firstID;
			}

		if(end == 0){
	        LOG("Neighbor Add: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);

	        if (firstID < arraysize(counters)) {

				if(((firstID == 02) || (secondID == 02))&&(id == turn)){
					if(side == 0){
						vid[id].bg0rom.fill(vec(0,0), vec(16,16), north);
						vid[id].bg0rom.text(vec(5,8),"NORTH");
						if(steps[playerAt] == 0) steps[playerAt] = 5;
						else if(steps[playerAt] != 5){
							//LOSER
							conclusion(1);
						}
					}
					else if(side == 1){
						vid[id].bg0rom.fill(vec(0,0), vec(16,16), east);
						vid[id].bg0rom.text(vec(5,8),"EAST");
						if(steps[playerAt] == 0) steps[playerAt] = 6;
						else if(steps[playerAt] != 6){
							//LOSER
							conclusion(1);
						}
					}
					else if(side == 2){
						vid[id].bg0rom.fill(vec(0,0), vec(16,16), south);
						vid[id].bg0rom.text(vec(5,8),"SOUTH");
						if(steps[playerAt] == 0) steps[playerAt] = 3;
						else if(steps[playerAt] != 3){
							//LOSER
							conclusion(1);
						}
					}
					else if(side == 3){ 
						vid[id].bg0rom.fill(vec(0,0), vec(16,16), west);
						vid[id].bg0rom.text(vec(5,8),"WEST");
						if(steps[playerAt] == 0) steps[playerAt] = 4;
						else if(steps[playerAt] != 4){
							//LOSER
							conclusion(1);
						}
					}
					index++;
					playerAt++;
					LOG("allowed is %d\n", allowed);
					LOG("playerAt is %d\n", playerAt);
					checkIn();
				}
			}
		}
    }
};


void main()
{
    static SensorListener sensors;

    sensors.install();

    // We're entirely event-driven. Everything is
    // updated by SensorListener's event callbacks.
    while (1)
        System::paint();
}
