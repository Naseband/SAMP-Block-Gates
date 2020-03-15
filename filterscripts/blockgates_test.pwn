#if 1
#pragma option -r
#pragma option -d3
#endif

#include <a_samp>
#include <streamer>
#include <rotations_old>
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

	// anim - anim type (0 or 1)
	// model, modelsize - model id and size of the obj
	// speed, amode - gate speed and anim mode (0 = ordered, 1 = reverse, 2 = random)

	new anim, model, Float:modelsize, Float:speed, amode;

	sscanf(params, "I(0)I(19789)F(1.0)F(0.3)I(0)", anim, model, modelsize, speed, amode);

	if(speed < 0.01) speed = 0.01;

	// Create a block gate at the current position and facing angle

	new id = CreateBlockGate(model, modelsize, x, y, z - 0.5, 0.0, 0.0, a, 10, 10, BGATE_STATE_CLOSED, speed);

	// Apply some textures

	SetBlockGateObjectMaterial(id, 0, 14668, "711c", "bwtilebroth", 0xFFFFFFFF, 14668, "711c", "bwtilebroth", 0xFFAAAAAA);

	// Add all the animation steps, the side vectors are the most important (you can see 4 times -1 on the right vector, that means it will move 4 steps in total - the other axes are for "decoration")

	if(anim)
	{
		AddBlockGateAnimStep(id, 1, -1, 0, 0);
		AddBlockGateAnimStep(id, 1, 0, -1, 0);
		AddBlockGateAnimStep(id, 1, 0, 0, 1);
		AddBlockGateAnimStep(id, 2, 0, -1, 0);
		AddBlockGateAnimStep(id, 1, 0, 0, -1);
		AddBlockGateAnimStep(id, 1, 0, -1, 0);
		AddBlockGateAnimStep(id, 1, -2, 0, 1);
		AddBlockGateAnimStep(id, 2, 1, 0, 0);
		AddBlockGateAnimStep(id, 1, 2, 0, -1);
	}
	else
	{
		AddBlockGateAnimStep(id, 1, -1, -1, 1);
		AddBlockGateAnimStep(id, 2, 1, -1, 0);
		AddBlockGateAnimStep(id, 2, -1, -1, 0);
		AddBlockGateAnimStep(id, 1, 1, -1, 0);
	}

	// Apply the anim mode specified by the player

	SetBlockGateAnimMode(id, amode);

	// Send a message

	new text[89];
	format(text, sizeof(text), "Block Gate created (Anim %d, ID %d, Model %d, Size %.1f, Speed %.1f, Mode %d)", anim, id, model, modelsize, speed, amode);
	SendClientMessage(playerid, -1, text);

	return 1;
}
