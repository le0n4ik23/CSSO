#pragma once

class IVRenderView
{
public:
	// Draw normal brush model.
// If pMaterialOverride is non-null, then all the faces of the bmodel will
// set this material rather than their regular material.
	virtual void			DrawBrushModel(
		CEntity* baseentity,
		void* model,
		const Vector3& origin,
		const Vector3& angles,
		bool bUnused) = 0;

	// Draw brush model that has no origin/angles change ( uses identity transform )
	// FIXME, Material proxy IClientEntity *baseentity is unused right now, use DrawBrushModel for brushes with
	//  proxies for now.
	virtual void			DrawIdentityBrushModel(void* pList, void* model) = 0;

	// Mark this dynamic light as having changed this frame ( so light maps affected will be recomputed )
	virtual void			TouchLight(struct dlight_t* light) = 0;
	// Draw 3D Overlays
	virtual void			Draw3DDebugOverlays(void) = 0;
	// Sets global blending fraction
	virtual void			SetBlend(float blend) = 0;
	virtual float			GetBlend(void) = 0;

	// Sets global color modulation
	virtual void			SetColorModulation(float const* blend) = 0;
	virtual void			GetColorModulation(float* blend) = 0;

	// Wrap entire scene drawing
	virtual void			SceneBegin(void) = 0;
	virtual void			SceneEnd(void) = 0;
};