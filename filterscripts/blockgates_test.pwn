/*
Test script for blockgates.inc

Adds a command to create a block gate at the current location.
*/

#include <a_samp>
#include <streamer>
#include <rotations_old> // Ver 1.2.0
#include <zcmd>
#include <sscanf2>
#include <blockgates>

public OnPlayerCommandText(playerid, cmdtext[])
{
	return 0;
}

CMD:bg(playerid, const params[])
{
	// Get position and facing angle of the player

	new Float:x, Float:y, Float:z, Float:a;

	GetPlayerPos(playerid, x, y, z);
	GetPlayerFacingAngle(playerid, a);

	// Some options for the player to choose from

	// anim - anim type (0 or 1) - these are just some random ones I made for showcasing.
	// model, modelsize - model id and size of the obj model (x, y, z relative to gate)
	// speed, amode - gate speed and anim mode (0 = ordered, 1 = reverse, 2 = random)

	new anim, model, Float:modelsize_x, Float:modelsize_y, Float:modelsize_z, Float:speed, amode;

	sscanf(params, "I(0)I(19789)F(1.0)F(1.0)F(1.0)F(0.3)I(0)", anim, model, modelsize_x, modelsize_y, modelsize_z, speed, amode);

	if(speed < 0.01) speed = 0.01;

	// Create a block gate at the current position and facing angle

	new id = CreateBlockGate(model, modelsize_x, modelsize_y, modelsize_z, x, y, z - 0.5, 0.0, 0.0, a, 10, 10, speed);

	// Apply some textures (checkerboard) - you can specify two textures for them to be alternated between.

	SetBlockGateObjectMaterial(id, 0, 14668, "711c", "bwtilebroth", 0xFFFFFFFF, 14668, "711c", "bwtilebroth", 0xFFAAAAAA);

	// Add all the animation steps, the side vectors are the most important (you can see 4 times -1 on the right vector, that means it will move 4 steps in total - the other axes are for decoration)

	if(anim)
	{
		AddBlockGateAnimStep(id, 1,  0, -1,  0); // back
		AddBlockGateAnimStep(id, 1, -1,  0,  0); // side
		AddBlockGateAnimStep(id, 1,  0,  0,  1); // up
		AddBlockGateAnimStep(id, 2, -1,  0,  0); // side
		AddBlockGateAnimStep(id, 1,  0,  0, -1); // down
		AddBlockGateAnimStep(id, 1, -1,  0,  0); // side
		AddBlockGateAnimStep(id, 1,  0, -2,  1); // back 2x
		AddBlockGateAnimStep(id, 2,  0,  1,  0); // front
		AddBlockGateAnimStep(id, 1,  0,  2, -1); // down
	}
	else
	{
		AddBlockGateAnimStep(id, 1, -1, -1,  1,  80, 1.0); // back, side, up
		AddBlockGateAnimStep(id, 2, -1,  1,  0, 120, 0.8); // front, side
		AddBlockGateAnimStep(id, 2, -1, -1,  0, 200, 0.55); // back, side
		AddBlockGateAnimStep(id, 1, -1,  1,  0, 350, 0.3); // front, side
	}

	// Apply the anim mode specified by the player

	SetBlockGateAnimMode(id, amode);

	// Send a message

	new text[89];
	format(text, sizeof(text), "Block Gate created (Anim %d, ID %d, Model %d, Speed %.1f, Mode %d)", anim, id, model, speed, amode);
	SendClientMessage(playerid, -1, text);

	return 1;
}
