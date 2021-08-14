#include "SonicMania.h"

ObjectLRZ1Setup *LRZ1Setup;

void LRZ1Setup_Update(void) {}

void LRZ1Setup_LateUpdate(void) {}

void LRZ1Setup_StaticUpdate(void)
{
    LRZ1Setup->palTimer += 24;
    if (LRZ1Setup->palTimer > 255) {
        LRZ1Setup->palTimer -= 256;
        RSDK.RotatePalette(1, 208, 211, true);
        RSDK.RotatePalette(2, 208, 211, true);
        RSDK.RotatePalette(3, 208, 211, true);
        RSDK.RotatePalette(4, 208, 211, true);
    }

    if (LRZ1Setup->fadeTimer <= 0) {
        int cos   = RSDK.Cos1024(2 * (Zone->timer & 0x1FF));
        int blend = cos >> 3;
        if (cos >> 3 >= 0)
            RSDK.SetLimitedFade(5, 1, 4, cos >> 3, 208, 211);
        else
            RSDK.SetLimitedFade(5, 1, 3, -blend, 208, 211);
        RSDK.RotatePalette(3u, 208u, 211, true);
        RSDK.RotatePalette(4u, 208u, 211, true);
        if (blend >= 0)
            RSDK.SetLimitedFade(6, 2, 4, blend, 208, 211);
        else
            RSDK.SetLimitedFade(6, 2, 3, -blend, 208, 211);
        RSDK.RotatePalette(3, 208, 211, false);
        RSDK.RotatePalette(4, 208, 211, false);
        RSDK.SetLimitedFade(0, 5, 6, LRZ1Setup->palTimer, 208, 211);
    }
    else {
        if (LRZ1Setup->fadeTimer == 1) {
            RSDK.CopyPalette(0, 208, 1, 208, 5);
        }
        RSDK.SetLimitedFade(0, 1, 7, LRZ1Setup->fadeTimer, 208, 212);

        if (LRZ1Setup->fadeTimer < 256)
            LRZ1Setup->fadeTimer += 2;
    }

    RSDK.SetLimitedFade(0, 1, 2, (RSDK.Sin256(2 * Zone->timer) >> 1) + 128, 140, 141);
    RSDK.SetLimitedFade(0, 1, 2, (RSDK.Sin256(2 * Zone->timer + 128) >> 1) + 128, 142, 143);
    RSDK.SetLimitedFade(0, 1, 2, (RSDK.Sin256(Zone->timer) >> 1) + 128, 156, 158);

    foreach_active(Player, player)
    {
        if (player->onGround) {
            Hitbox *hitbox = Player_GetHitbox(player);

            byte behaviour = 0;
            int tileInfo   = 0;
            LRZ2Setup_GetTileInfo(&tileInfo, player->collisionPlane, player->position.x, (hitbox->bottom << 16) + player->position.y,
                                  player->moveOffset.x, player->moveOffset.y, &behaviour);
            if (behaviour == 1 && player->shield != SHIELD_FIRE && !LRZ1Setup->fadeTimer) {
                Player_CheckHitFlip(player);
            }
        }
    }

#if RETRO_USE_PLUS
    if (!(Zone->timer & 3)) {
        ++LRZ1Setup->bg1->deformationOffset;
        ++LRZ1Setup->bg2->deformationOffset;
    }
    if (!(Zone->timer & 1)) {
        ++LRZ1Setup->fgLow->deformationOffset;
        ++LRZ1Setup->fgHigh->deformationOffset;
    }
#endif
}

void LRZ1Setup_Draw(void) {}

void LRZ1Setup_Create(void *data) {}

void LRZ1Setup_StageLoad(void)
{
    LRZ1Setup->fadeTimer = 0;
    LRZ1Setup->bg1       = RSDK.GetSceneLayer(RSDK.GetSceneLayerID("Background 1"));
    LRZ1Setup->bg2       = RSDK.GetSceneLayer(RSDK.GetSceneLayerID("Background 2"));
    LRZ1Setup->fgLow     = RSDK.GetSceneLayer(Zone->fgLow);
    LRZ1Setup->fgHigh    = RSDK.GetSceneLayer(Zone->fgHigh);
    RSDK.SetDrawLayerProperties(0, false, LRZ1Setup_DrawLayerCB);
    Animals->animalTypes[0] = ANIMAL_FLICKY;
#if RETRO_USE_PLUS
    Animals->animalTypes[1] = ANIMAL_CUCKY;
#else
    Animals->animalTypes[1] = ANIMAL_RICKY;
#endif

    if (isMainGameMode() && PlayerHelpers_CheckAct1())
        Zone->stageFinishCallback = LRZ1Setup_StageFinishCB;

#if RETRO_USE_PLUS
    if (RSDK_sceneInfo->filter & FILTER_ENCORE) {
        RSDK.LoadPalette(0, "EncoreLRZ1.act", 255);

        for (int i = 0; i < 0x400; ++i) LRZ1Setup->fgLow->deformationData[i] = LRZ1Setup->deformFG[i & 0x1F];
        for (int i = 0; i < 0x400; ++i) LRZ1Setup->fgHigh->deformationData[i] = LRZ1Setup->deformFG[i & 0x1F];
        for (int i = 0; i < 0x400; ++i) LRZ1Setup->bg1->deformationData[i] = LRZ1Setup->deformBG[i & 0x1F];
        for (int i = 0; i < 0x400; ++i) LRZ1Setup->bg2->deformationData[i] = LRZ1Setup->deformBG[i & 0x1F];
    }
    if (!isMainGameMode() || !globals->enableIntro || PlayerHelpers_CheckStageReload()) {
        Zone->screenBoundsL1[0] = 2732;
        Zone->screenBoundsL1[1] = 2732;
        Zone->screenBoundsL1[2] = 2732;
        Zone->screenBoundsL1[3] = 2732;
        foreach_all(LRZ1Outro, outro) { destroyEntity(outro); }
    }
#else
    Zone->screenBoundsL1[0] = 648;
    Zone->screenBoundsL1[1] = 648;
    Zone->screenBoundsL1[2] = 648;
    Zone->screenBoundsL1[3] = 648;
#endif
}

void LRZ1Setup_StageFinishCB(void) { CREATE_ENTITY(LRZ1Outro, NULL, 0, 0); }

void LRZ1Setup_DrawLayerCB(void)
{
    int scroll                = maxVal(0x800000 - 8 * RSDK_screens->position.y * RSDK_screens->position.y, 0);
    LRZ1Setup->bg1->scrollPos = scroll;
    LRZ1Setup->bg2->scrollPos = scroll;
}

void LRZ1Setup_EditorDraw(void) {}

void LRZ1Setup_EditorLoad(void) {}

void LRZ1Setup_Serialize(void) {}
