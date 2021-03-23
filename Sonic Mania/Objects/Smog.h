#ifndef OBJ_SMOG_H
#define OBJ_SMOG_H

#include "../SonicMania.h"

// Object Class
typedef struct {
	RSDK_OBJECT
} ObjectSmog;

// Entity Class
typedef struct {
	RSDK_ENTITY
} EntitySmog;

// Object Struct
extern ObjectSmog *Smog;

// Standard Entity Events
void Smog_Update();
void Smog_LateUpdate();
void Smog_StaticUpdate();
void Smog_Draw();
void Smog_Create(void* data);
void Smog_StageLoad();
void Smog_EditorDraw();
void Smog_EditorLoad();
void Smog_Serialize();

// Extra Entity Functions


#endif //!OBJ_SMOG_H