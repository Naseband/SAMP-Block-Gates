#if defined __BGATES_INCLUDED__
#endinput
#endif
#define __BGATES_INCLUDED__

// ---------------------------------------------------------------------

/*

	Block Gates include by NaS.

	Creates a wall of objects that opens up and closes like a gate.
	The way of the movement can be described by single steps in either front, right or up direction relative to the objects.
	Block gates can be textured and created out of any model.
	They can also be generated at ANY rotation.

	The gates will open when any player enters the area created at its origin. It will automatically close if no players are in the area.


	Functions:

	IsValidBlockGate(id)
	CreateBlockGate(modelid, Float:modelsize, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, width = 8, height = 4, Float:speed = 1.0, Float:areasize = 10.0, worldid = 0, interiorid = 0, playerid = -1, Float:streamdistance = 250.0, Float:drawdistance = 250.0, Float:ob_rx = FLOAT_INFINITY, Float:ob_ry = FLOAT_INFINITY, Float:ob_rz = FLOAT_INFINITY, Float:ob_side_off_mul = 0.5)
	DestroyBlockGate(id)

	AddBlockGateAnimStep(id, pattern, num_right, num_front, num_up, extra_delay = 200, Float:speed_mul = 1.0)
	SetBlockGateAnimMode(id, mode)

	SetBlockGateObjectMaterial(id, materialindex, modelid_a, const modelname_a[], const texturename_a[], color_a = 0, modelid_b = -1, const modelname_b[] = "", const texturename_b[] = "", color_b = 0)
	
	OpenBlockGate(id, delay = 0)
	CloseBlockGate(id, delay = 0)

	GetBlockGateRightVector(id, &Float:vx, &Float:vy, &Float:vz)
	GetBlockGateFrontVector(id, &Float:vx, &Float:vy, &Float:vz)
	GetBlockGateUpVector(id, &Float:vx, &Float:vy, &Float:vz)


	Dependencies:

	- streamer plugin by Incognito
	- rotations.inc 1.2.0 by Nero_3D

*/

// ---------------------------------------------------------------------

#if !defined MAX_BLOCK_GATES
#define MAX_BLOCK_GATES					25
#endif

#if !defined MAX_BLOCK_GATE_OBJECTS
#define MAX_BLOCK_GATE_OBJECTS			100
#endif

#if !defined MAX_BLOCK_GATE_ANIM_STEPS
#define MAX_BLOCK_GATE_ANIM_STEPS		10
#endif

// ---------------------------------------------------------------------

enum (+=1)
{
	BGATE_STATE_CLOSED,
	BGATE_STATE_OPEN,

	BGATE_STATE_OPENING, // Only used internally
	BGATE_STATE_CLOSING
};

enum (+=1)
{
	BGATE_AMODE_ORDERED, // Opens and closes in order
	BGATE_AMODE_REVERSE, // Opens and closes in opposite order
	BGATE_AMODE_RANDOM // Opens and closes in a random order
};

// ---------------------------------------------------------------------

enum E_BLOCK_GATE
{
	bool:bgateUsed,
	bgateNumX,
	bgateNumZ,
	bgateState,
	bgateNextState,
	Float:bgateSpeed,

	bgateModelID,
	Float:bgateModelSize,

	Float:bgateX,
	Float:bgateY,
	Float:bgateZ,
	Float:bgateRX,
	Float:bgateRY,
	Float:bgateRZ,

	Float:bgateVecFrontX,
	Float:bgateVecFrontY,
	Float:bgateVecFrontZ,

	Float:bgateVecRightX,
	Float:bgateVecRightY,
	Float:bgateVecRightZ,

	Float:bgateVecUpX,
	Float:bgateVecUpY,
	Float:bgateVecUpZ,

	bgateAreaID,

	bgateObjectIDs[MAX_BLOCK_GATE_OBJECTS],
	bgateNumObjects,

	bgateAnimStepPattern[MAX_BLOCK_GATE_ANIM_STEPS],
	bgateAnimStepNumRight[MAX_BLOCK_GATE_ANIM_STEPS],
	bgateAnimStepNumFront[MAX_BLOCK_GATE_ANIM_STEPS],
	bgateAnimStepNumUp[MAX_BLOCK_GATE_ANIM_STEPS],
	bgateAnimStepDelay[MAX_BLOCK_GATE_ANIM_STEPS],
	Float:bgateAnimStepSpeedMul[MAX_BLOCK_GATE_ANIM_STEPS],

	bgateNumAnimSteps,
	bgateAnimMode,
	bgateNumUsages
};
new gBlockGateData[MAX_BLOCK_GATES][E_BLOCK_GATE];

// ---------------------------------------------------------------------

#define BGV_F_X(%1) gBlockGateData[%1][bgateVecFrontX]
#define BGV_F_Y(%1) gBlockGateData[%1][bgateVecFrontY]
#define BGV_F_Z(%1) gBlockGateData[%1][bgateVecFrontZ]

#define BGV_R_X(%1) gBlockGateData[%1][bgateVecRightX]
#define BGV_R_Y(%1) gBlockGateData[%1][bgateVecRightY]
#define BGV_R_Z(%1) gBlockGateData[%1][bgateVecRightZ]

#define BGV_U_X(%1) gBlockGateData[%1][bgateVecUpX]
#define BGV_U_Y(%1) gBlockGateData[%1][bgateVecUpY]
#define BGV_U_Z(%1) gBlockGateData[%1][bgateVecUpZ]

forward MoveBlockGate(id, step);

// ---------------------------------------------------------------------

#if !defined FILTERSCRIPT

	public OnGameModeExit()
	{
		for(new i = 0; i < MAX_BLOCK_GATES; i ++) DestroyBlockGate(i);

		#if defined BGATE_OnGameModeExit
			return BGATE_OnGameModeExit();
		#else
			return 1;
		#endif
	}
	#if defined _ALS_OnGameModeExit
		#undef OnGameModeExit
	#else
		#define _ALS_OnGameModeExit
	#endif

	#define OnGameModeExit BGATE_OnGameModeExit
	#if defined BGATE_OnGameModeExit
		forward BGATE_OnGameModeExit();
	#endif

#endif

// ---------------------------------------

#if defined FILTERSCRIPT

	public OnFilterScriptExit()
	{
		for(new i = 0; i < MAX_BLOCK_GATES; i ++) DestroyBlockGate(i);

		#if defined BGATE_OnFilterScriptExit
			return BGATE_OnFilterScriptExit();
		#else
			return 1;
		#endif
	}
	#if defined _ALS_OnFilterScriptExit
		#undef OnFilterScriptExit
	#else
		#define _ALS_OnFilterScriptExit
	#endif

	#define OnFilterScriptExit BGATE_OnFilterScriptExit
	#if defined BGATE_OnFilterScriptExit
		forward BGATE_OnFilterScriptExit();
	#endif

#endif

// ---------------------------------------

public OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA areaid)
{
	new id = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID);

	if(IsValidBlockGate(id) && gBlockGateData[id][bgateAreaID] == areaid)
	{
		OpenBlockGate(id);

		return 1;
	}

	#if defined BGATE_OnPlayerEnterDynamicArea
		return BGATE_OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA areaid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerEnterDynamicArea
	#undef OnPlayerEnterDynamicArea
#else
	#define _ALS_OnPlayerEnterDynamicArea
#endif

#define OnPlayerEnterDynamicArea BGATE_OnPlayerEnterDynamicArea
#if defined BGATE_OnPlayerEnterDynamicArea
	forward BGATE_OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA areaid);
#endif

// ---------------------------------------

public OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA areaid)
{
	new id = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID);

	if(IsValidBlockGate(id) && gBlockGateData[id][bgateAreaID] == areaid && !IsAnyPlayerInDynamicArea(areaid))
	{
		CloseBlockGate(id);

		return 1;
	}

	#if defined BGATE_OnPlayerLeaveDynamicArea
		return BGATE_OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA areaid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerLeaveDynamicArea
	#undef OnPlayerLeaveDynamicArea
#else
	#define _ALS_OnPlayerLeaveDynamicArea
#endif

#define OnPlayerLeaveDynamicArea BGATE_OnPlayerLeaveDynamicArea
#if defined BGATE_OnPlayerLeaveDynamicArea
	forward BGATE_OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA areaid);
#endif

// ---------------------------------------------------------------------

stock IsValidBlockGate(id)
{
	if(id < 0 || id >= MAX_BLOCK_GATES || !gBlockGateData[id][bgateUsed]) return 0;

	return 1;
}

stock CreateBlockGate(modelid, Float:modelsize, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, width = 8, height = 4, Float:speed = 1.0, Float:areasize = 10.0, worldid = 0, interiorid = 0, playerid = -1, Float:streamdistance = 250.0, Float:drawdistance = 250.0, Float:ob_rx = FLOAT_INFINITY, Float:ob_ry = FLOAT_INFINITY, Float:ob_rz = FLOAT_INFINITY, Float:ob_side_off_mul = 1.0)
{
	if(width < 1 || height < 1 || width * height > MAX_BLOCK_GATE_OBJECTS) return -1;

	new id = -1;

	for(new i = 0; i < MAX_BLOCK_GATES; i ++) if(!gBlockGateData[i][bgateUsed])
	{
		id = i;
		break;
	}

	if(id == -1) return -1;

	gBlockGateData[id][bgateUsed] = true;
	gBlockGateData[id][bgateState] = BGATE_STATE_CLOSED;
	gBlockGateData[id][bgateNextState] = BGATE_STATE_CLOSED;
	gBlockGateData[id][bgateNumX] = width;
	gBlockGateData[id][bgateNumZ] = height;
	gBlockGateData[id][bgateSpeed] = speed;

	gBlockGateData[id][bgateModelID] = modelid;
	gBlockGateData[id][bgateModelSize] = modelsize;

	gBlockGateData[id][bgateX] = x;
	gBlockGateData[id][bgateY] = y;
	gBlockGateData[id][bgateZ] = z;

	gBlockGateData[id][bgateRX] = rx;
	gBlockGateData[id][bgateRY] = ry;
	gBlockGateData[id][bgateRZ] = rz;

	gBlockGateData[id][bgateNumAnimSteps] = 0;
	gBlockGateData[id][bgateAnimMode] = BGATE_AMODE_ORDERED;
	gBlockGateData[id][bgateNumUsages] = 0;

	if(areasize > 0.0)
	{
		gBlockGateData[id][bgateAreaID] = CreateDynamicSphere(x, y, z, areasize, worldid, interiorid, playerid);
		Streamer_SetIntData(STREAMER_TYPE_AREA, gBlockGateData[id][bgateAreaID], E_STREAMER_EXTRA_ID, id);
	}
	else
	{
		gBlockGateData[id][bgateAreaID] = -1;
	}

	new Float:matrix[4][4];
	GetRotationMatrixFromEuler(matrix, rx, ry, rz, euler_samp);

	// Reset vectors to 0, MatrixRotate doesn't do that

	gBlockGateData[id][bgateVecFrontX] = 0.0;
	gBlockGateData[id][bgateVecFrontY] = 0.0;
	gBlockGateData[id][bgateVecFrontZ] = 0.0;
	gBlockGateData[id][bgateVecRightX] = 0.0;
	gBlockGateData[id][bgateVecRightY] = 0.0;
	gBlockGateData[id][bgateVecRightZ] = 0.0;
	gBlockGateData[id][bgateVecUpX] = 0.0;
	gBlockGateData[id][bgateVecUpY] = 0.0;
	gBlockGateData[id][bgateVecUpZ] = 0.0;

	// Get the vectors for left/right, front/back and up/down directions, easier and faster for later use. Block Gates can be created at any rotation.

	MatrixRotate(matrix, 0.0, modelsize, 0.0, 0.0, gBlockGateData[id][bgateVecFrontX], gBlockGateData[id][bgateVecFrontY], gBlockGateData[id][bgateVecFrontZ]);
	MatrixRotate(matrix, modelsize, 0.0, 0.0, 0.0, gBlockGateData[id][bgateVecRightX], gBlockGateData[id][bgateVecRightY], gBlockGateData[id][bgateVecRightZ]);
	MatrixRotate(matrix, 0.0, 0.0, modelsize, 0.0, gBlockGateData[id][bgateVecUpX], gBlockGateData[id][bgateVecUpY], gBlockGateData[id][bgateVecUpZ]);

	// Check object rot

	if(ob_rx == FLOAT_INFINITY) ob_rx = rx;
	if(ob_ry == FLOAT_INFINITY) ob_ry = ry;
	if(ob_rz == FLOAT_INFINITY) ob_rz = rz;

	// Create all objects.

	new Float:off_x = (BGV_R_X(id) * (width - ob_side_off_mul)) / -2.0, // The center of the gate should be where specified.
		Float:off_y = (BGV_R_Y(id) * (width - ob_side_off_mul)) / -2.0,
		Float:off_z = (BGV_R_Z(id) * (width - ob_side_off_mul)) / -2.0,
		num;

	for(new ob_x = 0; ob_x < width; ob_x ++)
	{
		for(new ob_z = 0; ob_z < height; ob_z ++)
		{
			new objectid = CreateDynamicObject(modelid, x + BGV_R_X(id) * ob_x + BGV_U_X(id) * ob_z + off_x, y + BGV_R_Y(id) * ob_x + BGV_U_Y(id) * ob_z + off_y, z + BGV_R_Z(id) * ob_x + BGV_U_Z(id) * ob_z + off_z, ob_rx, ob_ry, ob_rz, worldid, interiorid, playerid, streamdistance, drawdistance);

			gBlockGateData[id][bgateObjectIDs][num ++] = objectid;
		}
	}

	gBlockGateData[id][bgateNumObjects] = num;

	return id;
}

stock DestroyBlockGate(id)
{
	if(!IsValidBlockGate(id)) return 0;

	gBlockGateData[id][bgateUsed] = false;

	for(new i = 0; i < gBlockGateData[id][bgateNumObjects]; i ++)
	{
		DestroyDynamicObject(gBlockGateData[id][bgateObjectIDs][i]);
	}

	return 1;
}

stock AddBlockGateAnimStep(id, pattern, num_right, num_front, num_up, extra_delay = 100, Float:speed_mul = 1.0)
{
	new slot;

	if((slot = gBlockGateData[id][bgateNumAnimSteps]) == MAX_BLOCK_GATE_ANIM_STEPS) return 0;

	gBlockGateData[id][bgateAnimStepPattern][slot] = pattern;
	gBlockGateData[id][bgateAnimStepNumRight][slot] = num_right;
	gBlockGateData[id][bgateAnimStepNumFront][slot] = num_front;
	gBlockGateData[id][bgateAnimStepNumUp][slot] = num_up;
	gBlockGateData[id][bgateAnimStepDelay][slot] = extra_delay;
	
	gBlockGateData[id][bgateAnimStepSpeedMul][slot] = speed_mul;

	gBlockGateData[id][bgateNumAnimSteps] ++;

	return 1;
}

stock SetBlockGateAnimMode(id, mode)
{
	gBlockGateData[id][bgateAnimMode] = mode;

	return 1;
}

stock SetBlockGateObjectMaterial(id, materialindex, modelid_a, const modelname_a[], const texturename_a[], color_a = 0, modelid_b = -1, const modelname_b[] = "", const texturename_b[] = "", color_b = 0)
{
	for(new ob_x = 0, num; ob_x < gBlockGateData[id][bgateNumX]; ob_x ++)
	{
		for(new ob_z = 0; ob_z < gBlockGateData[id][bgateNumZ]; ob_z ++)
		{
			new objectid = gBlockGateData[id][bgateObjectIDs][num ++];

			if(modelid_b == -1 || ((ob_z % 2) != (ob_x % 2))) SetDynamicObjectMaterial(objectid, materialindex, modelid_a, modelname_a, texturename_a, color_a);
			else SetDynamicObjectMaterial(objectid, materialindex, modelid_b, modelname_b, texturename_b, color_b);
		}
	}

	return 1;
}

stock OpenBlockGate(id, delay = 0)
{
	gBlockGateData[id][bgateNextState] = BGATE_STATE_OPEN;

	if(gBlockGateData[id][bgateState] == BGATE_STATE_CLOSED)
	{
		gBlockGateData[id][bgateState] = BGATE_STATE_OPENING;

		if(delay) SetTimerEx("MoveBlockGate", delay, 0, "ii", id, 0);
		else MoveBlockGate(id, 0);
	}

	return 1;
}

stock CloseBlockGate(id, delay = 0)
{
	gBlockGateData[id][bgateNextState] = BGATE_STATE_CLOSED;

	if(gBlockGateData[id][bgateState] == BGATE_STATE_OPEN)
	{
		gBlockGateData[id][bgateState] = BGATE_STATE_CLOSING;

		if(delay) SetTimerEx("MoveBlockGate", delay, 0, "ii", id, 0);
		else MoveBlockGate(id, 0);
	}

	return 1;
}

stock GetBlockGateFrontVector(id, &Float:vx, &Float:vy, &Float:vz)
{
	vx = gBlockGateData[id][bgateVecFrontX];
	vy = gBlockGateData[id][bgateVecFrontY];
	vz = gBlockGateData[id][bgateVecFrontZ];
}

stock GetBlockGateRightVector(id, &Float:vx, &Float:vy, &Float:vz)
{
	vx = gBlockGateData[id][bgateVecRightX];
	vy = gBlockGateData[id][bgateVecRightY];
	vz = gBlockGateData[id][bgateVecRightZ];
}

stock GetBlockGateUpVector(id, &Float:vx, &Float:vy, &Float:vz)
{
	vx = gBlockGateData[id][bgateVecUpX];
	vy = gBlockGateData[id][bgateVecUpY];
	vz = gBlockGateData[id][bgateVecUpZ];
}

// ---------------------------------------------------------------------

public MoveBlockGate(id, step)
{
	if(!IsValidBlockGate(id)) return 0;

	new num_steps = gBlockGateData[id][bgateNumAnimSteps],
		bgstate = gBlockGateData[id][bgateState];

	if(step == num_steps) // Reached final step (also the case if there are no steps)
	{
		if(bgstate == BGATE_STATE_OPENING)
		{
			gBlockGateData[id][bgateState] = BGATE_STATE_OPEN;

			if(gBlockGateData[id][bgateNextState] == BGATE_STATE_CLOSED)
			{
				CloseBlockGate(id, 2000);
			}
		}
		else if(bgstate == BGATE_STATE_CLOSING)
		{
			gBlockGateData[id][bgateState] = BGATE_STATE_CLOSED;

			if(gBlockGateData[id][bgateNextState] == BGATE_STATE_OPEN)
			{
				OpenBlockGate(id, 2000);
			}
		}

		return 1;
	}
	else if(step == 0) // On the first step, we must apply the animation mode
	{
		if(num_steps > 1) // Only reorder if there's more than one step
		{
			switch(gBlockGateData[id][bgateAnimMode])
			{
				case BGATE_AMODE_RANDOM:
				{
					for(new i = 0; i < num_steps * 3; i ++) // Randomize the anim array
					{
						new r1, r2;

						do
						{
							r1 = random(num_steps);
							r2 = random(num_steps);
						}
						while(r1 == r2);

						new tmp = gBlockGateData[id][bgateAnimStepPattern][r1]; // Pattern
						gBlockGateData[id][bgateAnimStepPattern][r1] = gBlockGateData[id][bgateAnimStepPattern][r2];
						gBlockGateData[id][bgateAnimStepPattern][r2] = tmp;

						tmp = gBlockGateData[id][bgateAnimStepNumRight][r1]; // Right
						gBlockGateData[id][bgateAnimStepNumRight][r1] = gBlockGateData[id][bgateAnimStepNumRight][r2];
						gBlockGateData[id][bgateAnimStepNumRight][r2] = tmp;

						tmp = gBlockGateData[id][bgateAnimStepNumFront][r1]; // Front
						gBlockGateData[id][bgateAnimStepNumFront][r1] = gBlockGateData[id][bgateAnimStepNumFront][r2];
						gBlockGateData[id][bgateAnimStepNumFront][r2] = tmp;

						tmp = gBlockGateData[id][bgateAnimStepNumUp][r1]; // Up
						gBlockGateData[id][bgateAnimStepNumUp][r1] = gBlockGateData[id][bgateAnimStepNumUp][r2];
						gBlockGateData[id][bgateAnimStepNumUp][r2] = tmp;

						tmp = gBlockGateData[id][bgateAnimStepDelay][r1]; // Delay
						gBlockGateData[id][bgateAnimStepDelay][r1] = gBlockGateData[id][bgateAnimStepDelay][r2];
						gBlockGateData[id][bgateAnimStepDelay][r2] = tmp;

						tmp = _:gBlockGateData[id][bgateAnimStepSpeedMul][r1]; // Speed
						gBlockGateData[id][bgateAnimStepSpeedMul][r1] = gBlockGateData[id][bgateAnimStepSpeedMul][r2];
						gBlockGateData[id][bgateAnimStepSpeedMul][r2] = Float:tmp;
					}
				}

				case BGATE_AMODE_REVERSE:
				{
					if(gBlockGateData[id][bgateNumUsages]) // Don't invert on the very first movement (since opening should always happen in the original order)
					{
						for(new i = 0; i < num_steps / 2; i ++) // Invert the anim array
						{
							new r1 = num_steps - i - 1,
								r2 = i;

							new tmp = gBlockGateData[id][bgateAnimStepPattern][r1]; // Pattern
							gBlockGateData[id][bgateAnimStepPattern][r1] = gBlockGateData[id][bgateAnimStepPattern][r2];
							gBlockGateData[id][bgateAnimStepPattern][r2] = tmp;

							tmp = gBlockGateData[id][bgateAnimStepNumRight][r1]; // Right
							gBlockGateData[id][bgateAnimStepNumRight][r1] = gBlockGateData[id][bgateAnimStepNumRight][r2];
							gBlockGateData[id][bgateAnimStepNumRight][r2] = tmp;

							tmp = gBlockGateData[id][bgateAnimStepNumFront][r1]; // Front
							gBlockGateData[id][bgateAnimStepNumFront][r1] = gBlockGateData[id][bgateAnimStepNumFront][r2];
							gBlockGateData[id][bgateAnimStepNumFront][r2] = tmp;

							tmp = gBlockGateData[id][bgateAnimStepNumUp][r1]; // Up
							gBlockGateData[id][bgateAnimStepNumUp][r1] = gBlockGateData[id][bgateAnimStepNumUp][r2];
							gBlockGateData[id][bgateAnimStepNumUp][r2] = tmp;

							tmp = gBlockGateData[id][bgateAnimStepDelay][r1]; // Delay
							gBlockGateData[id][bgateAnimStepDelay][r1] = gBlockGateData[id][bgateAnimStepDelay][r2];
							gBlockGateData[id][bgateAnimStepDelay][r2] = tmp;

							tmp = _:gBlockGateData[id][bgateAnimStepSpeedMul][r1]; // Speed
							gBlockGateData[id][bgateAnimStepSpeedMul][r1] = gBlockGateData[id][bgateAnimStepSpeedMul][r2];
							gBlockGateData[id][bgateAnimStepSpeedMul][r2] = Float:tmp;
						}
					}
				}
			}
		}

		gBlockGateData[id][bgateNumUsages] ++;
	}

	new Float:speed = gBlockGateData[id][bgateSpeed] * gBlockGateData[id][bgateAnimStepSpeedMul][step],
		num,
		pattern = gBlockGateData[id][bgateAnimStepPattern][step], 
		num_right = gBlockGateData[id][bgateAnimStepNumRight][step],
		num_front = gBlockGateData[id][bgateAnimStepNumFront][step],
		num_up = gBlockGateData[id][bgateAnimStepNumUp][step],
		width = gBlockGateData[id][bgateNumX],
		height = gBlockGateData[id][bgateNumZ];

	if(bgstate == BGATE_STATE_CLOSING) // When closing, all movements must happen in the opposite direction, thus we negate the vectors
	{
		num_right = -num_right;
		num_front = -num_front;
		num_up = -num_up;
	}

	for(new ob_x = 0; ob_x < width ; ob_x ++)
	{
		new bool:right_side = (ob_x < width / 2),
			dx = (right_side ? ob_x + 1 : ob_x);

		for(new ob_z = 0; ob_z < height; ob_z ++)
		{
			new num_front_f = num_front,
				num_right_f = num_right,
				num_up_f = num_up;

			switch(pattern) // Just some random alterations of the movement
			{
				case 1: 
				{
					if((ob_z % 3 == 0) || (ob_z % 3 == 1)) // Moves every first and second of three rows a bit more than the others
					{
						num_front_f *= 2;
					}

					if(dx % 2 == 0) // Up vector gets increased or zero'd for every second object on the X axis
					{
						num_up_f *= 2;
					}
					else num_up_f = 0;
				}
				case 2:
				{
					if((dx % 2) == (ob_z % 2)) // Idk, looks cool
					{
						num_front_f *= 2;
						num_up_f *= 2;
					}
				}
			}

			new Float:off_x = BGV_R_X(id) * (right_side ? num_right_f : -num_right_f) + BGV_F_X(id) * num_front_f + BGV_U_X(id) * num_up_f, // Calculate the offset for this object
				Float:off_y = BGV_R_Y(id) * (right_side ? num_right_f : -num_right_f) + BGV_F_Y(id) * num_front_f + BGV_U_Y(id) * num_up_f,
				Float:off_z = BGV_R_Z(id) * (right_side ? num_right_f : -num_right_f) + BGV_F_Z(id) * num_front_f + BGV_U_Z(id) * num_up_f,
				Float:x, Float:y, Float:z;

			GetDynamicObjectPos(gBlockGateData[id][bgateObjectIDs][num], x, y, z);

			MoveDynamicObject(gBlockGateData[id][bgateObjectIDs][num], x + off_x, y + off_y, z + off_z, VectorSize(off_x, off_y, off_z) * speed);

			num ++;
		}
	}

	// Set a timer for the next step, the time is based on the speed and the current step's delay (objects always move 1 second divided by the speed)

	SetTimerEx("MoveBlockGate", floatround(1000.0 / speed) + gBlockGateData[id][bgateAnimStepDelay][step], 0, "ii", id, step + 1);

	return 1;
}

// ---------------------------------------------------------------------
