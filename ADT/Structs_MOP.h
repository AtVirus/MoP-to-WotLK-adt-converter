#pragma once

struct MVER
{
	uint32 version;
};

typedef unsigned int uint;
typedef unsigned __int64 ulong;

struct MHDR
{
/// <summary>
        /// flags AND 1 => MFBO included
        /// </summary>
        uint flags;

        /// <summary>
        /// Offset to the MCIN-chunk relative to the beginning of MDHR (normally at 0x14)
        /// </summary>
        uint ofsMcin;
        /// <summary>
        /// Offset to the MTEX-chunk relative to the beginning of MDHR (normally at 0x14)
        /// </summary>
        uint ofsMtex;
        /// <summary>
        /// Offset to the MMDX-chunk relative to the beginning of MDHR (normally at 0x14)
        /// </summary>
        uint ofsMmdx;
        /// <summary>
        /// Offset to the MMID-chunk relative to the beginning of MDHR (normally at 0x14)
        /// </summary>
        uint ofsMmid;
        /// <summary>
        /// Offset to the MWMO-chunk relative to the beginning of MDHR (normally at 0x14)
        /// </summary>
        uint ofsMwmo;
        /// <summary>
        /// Offset to the MWID-chunk relative to the beginning of MDHR (normally at 0x14)
        /// </summary>
        uint ofsMwid;
        /// <summary>
        /// Offset to the MDDF-chunk relative to the beginning of MDHR (normally at 0x14)
        /// </summary>
        uint ofsMddf;
        /// <summary>
        /// Offset to the MODF-chunk relative to the beginning of MDHR (normally at 0x14)
        /// </summary>
        uint ofsModf;
        /// <summary>
        /// Offset to the MFBO-chunk relative to the beginning of MDHR (normally at 0x14)
        /// </summary>
        uint ofsMfbo;
        /// <summary>
        /// Offset to the MH2O-chunk relative to the beginning of MDHR (normally at 0x14)
        /// </summary>
        uint ofsMh2o;
        /// <summary>
        /// Offset to the MTFX-chunk relative to the beginning of MDHR (normally at 0x14)
        /// </summary>
        uint ofsMtfx;
        uint pad, pad2, pad3, pad4;
};

struct MCIN
{
    uint ofsMcnk, size, flags, asyncId;
};

struct MCNK
{
    uint flags, indexX, indexY, nLayers, nDoodadRefs;
    uint ofsHeight, ofsNormal, ofsLayer, ofsRefs, ofsAlpha;
    uint sizeAlpha, ofsShadow, sizeShadow, areaId, nMapObjRefs;
    uint holes;
    ulong lq1, lq2;
    uint predTex, noEffectDoodad, ofsSndEmitter, nSndEmitters;
    uint ofsLiquid, sizeLiqud;
    float posX, posY, posZ;
    uint ofsMCCV, ofsMCLV, unused;
};