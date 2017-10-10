-- Globals
global F_GetMatArray
global F_GetSceneMaterials
global a
global F_NameCompareCaseInsensitive
global F_Lowercase
global F_TrimDups
global list_mat_array
global F_Clamper
global full_vrstr

-- Variables
vray_cur = vray as string
newVRay = false
fRender = false
Brazil = false
conv_mat = #()
cur_proc2_text1 ="There are no specified materials for "
cur_proc2_text2 =" Material(s) is "
max_value = 245

-- Functions
fn GetRenderers = 
	(
		rendererlist = rendererClass.classes
		for i = 1 to rendererlist.count do
			(
			if (findstring (rendererlist[i] as string) "finalRender_stage_1") != undefined then fRender=true
			if (findstring (rendererlist[i] as string) "Brazil_r_s_V1_2") != undefined then Brazil=true
			)
		-- check V-Ray version
		global vrayver
		local first_num
		fltr_vray_str = filterString vray_cur "_"

		if fltr_vray_str.count==5 then 
			(
			vrayver=fltr_vray_str[4] as integer
			first_num = fltr_vray_str[3] as integer
			full_vrstr="V-Ray Advanced "+fltr_vray_str[3]+"."+fltr_vray_str[4]+"."+fltr_vray_str[5]
			)
		else if fltr_vray_str.count==6 then
			(
			vrayver=(fltr_vray_str[5] + "0") as integer
			first_num = fltr_vray_str[4] as integer
			full_vrstr="V-Ray Advanced "+fltr_vray_str[4]+"."+fltr_vray_str[5]+" "+fltr_vray_str[6]
			)

		if (vrayver>=46) or (first_num>1) then 
			(
			newVRay=true
			)
		else 
			(
			newVRay=false
			)
	)
	
fn F_ListMat = 
	(
		F_GetSceneMaterials()
		global sc_mat=a.count
		list_mat_array=#()
		for m in a do
			(
			cls_mat=classof m
			if (cls_mat==Standardmaterial) then cls_mat="Standard"
			if (cls_mat==RaytraceMaterial) then cls_mat="Raytrace"
			if (cls_mat==Multimaterial) then cls_mat="Multi/Sub-Object"
			cls_str = m.name + ">(" + cls_mat as string + ")"
			append list_mat_array cls_str
			)
	)

fn F_GetSceneMaterials = 
	(
	a = #()
	for i in $* do
		(
		if i.material != undefined then
			(
			if (finditem a i.material) == 0 then append a i.material
			)
		)
	qsort a F_NameCompareCaseInsensitive
	return a
	)

fn F_Lowercase instring =
	(
	upper="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	lower="abcdefghijklmnopqrstuvwxyz" 
	outstring=copy instring 
	for i = 1 to outstring.count do 
		(
		j=findString upper outstring[i]
		if (j != undefined) do outstring[i]=lower[j]
		)
	outstring
	)

fn F_NameCompareCaseInsensitive s1 s2 = 
	(
	if (F_Lowercase s1.name) < (F_Lowercase s2.name) then return -1
	else if (F_Lowercase s1.name) > (F_Lowercase s2.name) then return 1
	else return 0
	)
	
fn F_TrimDups a = 
	(
	for i in a.count to 1 by -1 do 
		(
		idx = findItem a a[i]
		if (idx != 0) AND (idx != i) do deleteItem a i
		)
	a
	)

fn F_GetMatArray = 
	(
	mat_array = F_GetSceneMaterials()
	F_TrimDups mat_array
	return mat_array
	)

--------------------------
-- From Standard
--------------------------

fn std_refl x = (
		srefl=(x/100*255) as integer
		std_refl_col=color srefl srefl srefl
		return std_refl_col
		)

fn convertFrom_Standard origMtl = (
--	if origMtl.shaderType!=6 then
		(
		if origMtl.shaderType!=6 and (origMtl.selfIllumAmount==100 and origMtl.Diffuse.v==255) then
			(
			if newVRay==true then
				(
				local r=VRayLightMtl()
				r.name=origMtl.name
				r.color=origMtl.Diffuse
				r.twoSided=origMtl.twoSided
				if origMtl.selfIllumMap!=undefined then r.texmap=origMtl.selfIllumMap
				r.texmap_on=origMtl.selfillumMapEnable
				return r
				)
			)
		else local r=VRayMtl()
		)
	r.name=origMtl.name
	
	r.effectsChannel=origMtl.effectsChannel
	
	r.option_doubleSided = origMtl.twoSided
	
	-- Shader if possible
	if origMtl.shaderType == 5 then 
	(
	r.brdf_type = 0
	)
	if origMtl.shaderType == 1 then 
	(
	r.brdf_type = 1
	)
	if origMtl.shaderType == 2 then 
	(
	r.brdf_type = 2
	)
	
	-- diffuse
	OrigRGB_value=origMtl.diffuse.v
	if OrigRGB_value==255 then
		(
		d_color=origMtl.diffuse
		d_color.v=max_value
		r.diffuse=d_color
		)
	else r.diffuse=origMtl.diffuse
	if origMtl.diffuseMap!=undefined then
		(
		r.texmap_diffuse=origMtl.diffuseMap
--		if 
--		showTextureMap $.material $.material.diffuseMap true
		)
	r.texmap_diffuse_on=origMtl.diffuseMapEnable
	r.texmap_diffuse_multiplier=origMtl.diffuseMapAmount
	
	-- bump map
	r.texmap_bump=origMtl.bumpMap
	r.texmap_bump_on=origMtl.bumpMapEnable
	r.texmap_bump_multiplier=origMtl.bumpMapAmount
	
	-- displacement
	r.texmap_displacement=origMtl.displacementMap
	r.texmap_displacement_on=origMtl.displacementMapEnable
	r.texmap_displacement_multiplier=origMtl.displacementMapAmount
	
	-- reflection
	if origMtl.reflectionMap!=undefined then
		(
		if (origMtl.reflectionMapEnable!=false) then
			(
			r.reflection=std_refl origMtl.reflectionMapAmount
			)
		else
			(
			r.texmap_reflection_multiplier=origMtl.reflectionMapAmount
			)
		cls_reflmap=classof origMtl.reflectionMap
		if (cls_reflmap == Bitmaptexture) then
			(
			r.texmap_environment=origMtl.reflectionMap
			r.texmap_environment_on=origMtl.reflectionMapEnable
			)
		else if (cls_reflmap == Mask) then
			(
			if (origMtl.reflectionMap.map!=undefined) then
				(
				if (classof origMtl.reflectionMap.map!=VRayMap) then
					(
					r.texmap_environment=origMtl.reflectionMap.map
					r.texmap_environment_on=origMtl.reflectionMap.mapEnabled
					)
				else 
					(
					if origMtl.reflectionMap.map.EnvironmentMap!=undefined then
						(
						r.texmap_environment=origMtl.reflectionMap.map.EnvironmentMap
						r.texmap_environment_on=origMtl.reflectionMap.map.EnvironmentMapOn
						)
					r.option_reflectOnBack = origMtl.reflectionMap.map.reflect_backSide
					)
				)
			if (origMtl.reflectionMap.Mask!=undefined) then
				(
				r.texmap_reflection=origMtl.reflectionMap.Mask
				)
			)
		else if (cls_reflmap == falloff) then
			(
			r.texmap_reflection=origMtl.reflectionMap
			r.texmap_reflection_on=origMtl.reflectionMapEnable
			r.texmap_reflection.map1 = undefined
			r.texmap_reflection.map2 = undefined
			)
		else if (cls_reflmap == VRayMap) then
			(
			if (origMtl.reflectionMap.environmentMap != undefined) then
				(
				r.texmap_environment=origMtl.reflectionMap.environmentMap
				r.texmap_environment_on=origMtl.reflectionMapEnable
				)
			r.option_reflectOnBack = origMtl.reflectionMap.reflect_backSide
			r.refraction_maxDepth=origMtl.reflectionMap.reflect_maxDepth
			)
		r.texmap_reflection_on=origMtl.reflectionMapEnable
		)
--	else r.option_traceReflection = off
	-- Hilight Glossines
	if origMtl.glossinessMap!=undefined then
		(
		if newVRay==true then
			(
			r.texmap_hilightGlossiness=origMtl.glossinessMap
			r.texmap_hilightGlossiness_on=origMtl.glossinessMapEnable
			r.texmap_hilightGlossiness_multiplier=origMtl.glossinessMapAmount
			r.reflection_lockGlossiness = off
	--		else r.hilight_glossiness=(origMtl.glossiness/100)*1.5
			)
		else
			(
			r.texmap_reflectionGlossiness=origMtl.glossinessMap
			r.texmap_reflectionGlossiness_on=origMtl.glossinessMapEnable
			r.texmap_reflectionGlossiness_multiplier=origMtl.glossinessMapAmount
			)
		)
		
	-- refraction and opacity
	std_refr = ((100 - origMtl.opacity)*0.01)*255
	
	if (origMtl.refractionMap!=undefined) and (origMtl.refractionMapEnable!=false) then
		(
		if (classof origMtl.refractionMap==VRayMap) or (classof origMtl.refractionMap==Reflect_Refract) then
			(
			srefr_vr=(origMtl.refractionMapAmount/100*255) as integer
			r.refraction = color srefr_vr srefr_vr srefr_vr
			)
		)
	else r.refraction = color std_refr std_refr std_refr
	
	if (origMtl.opacityMap!=undefined) then
		(
		if newVRay==true then
			(
			r.texmap_opacity=origMtl.opacityMap
			r.texmap_opacity_on=origMtl.opacityMapEnable
			r.texmap_opacity_multiplier=origMtl.opacityMapAmount
			r.refraction_ior=1.0
			r.refraction_affectShadows = on
			r.reflection_fresnel = off
			)
		else
			(
			cls_refrmap=classof origMtl.opacityMap
			
			r.texmap_refraction=origMtl.opacityMap
			r.texmap_refraction_on=origMtl.opacityMapEnable
			r.texmap_refraction_multiplier=origMtl.opacityMapAmount
			r.refraction_ior=1.0
			r.refraction_affectShadows = on
			r.reflection_fresnel = off
			if (cls_refrmap == Bitmaptexture) then
				(
				if (origMtl.opacityMap.output.Invert == true) then
					(
					r.texmap_refraction.output.Invert = off
					)
				else
					(
					r.texmap_refraction.output.Invert = on
					)
				)
			if (cls_refrmap == Bricks) then
				(
				swap r.texmap_refraction.Brick_Color r.texmap_refraction.Mortar_Color
				)
			)
		)
	else
		(
		if (origMtl.opacity < 100) then
			(
			r.refraction_ior=origMtl.ior
			r.refraction_affectShadows = on
			r.reflection_fresnel = on
			)
		)
	return r
)

--------------------------
-- From Raytrace
--------------------------

fn convertFrom_Raytrace origMtl = 
	(
	local r=VRayMtl()
	r.name=origMtl.name

	r.effectsChannel=origMtl.effectsChannel
	
	r.option_doubleSided = origMtl.twoSided
	
	-- Shader if possible
	if origMtl.shaderType == 2 then 
	(
	r.brdf_type = 2
	)
	if origMtl.shaderType == 1 then 
	(
	r.brdf_type = 0
	)
	if origMtl.shaderType == 3 then 
	(
	r.brdf_type = 1
	)
	if origMtl.shaderType == 5 then 
	(
	r.brdf_type = 0
	r.anisotropy = 0.5
	)

	-- diffuse
	OrigRGB_value=origMtl.diffuse.v
	if OrigRGB_value==255 then
		(
		d_color=origMtl.diffuse
		d_color.v=max_value
		r.diffuse=d_color
		)
	else r.diffuse=origMtl.Diffuse
	r.texmap_diffuse_on=origMtl.diffuseMapEnable
	r.texmap_diffuse_multiplier=origMtl.diffuseMapAmount
	if origMtl.diffuseMap!=undefined then r.texmap_diffuse=origMtl.diffuseMap
	
	-- bump map
	r.texmap_bump_on=origMtl.bumpMapEnable
	r.texmap_bump_multiplier=origMtl.bumpMapAmount
	if origMtl.bumpMap!=undefined then r.texmap_bump=origMtl.bumpMap
	
	-- displacement
	r.texmap_displacement_on=origMtl.displacementMapEnable
	r.texmap_displacement_multiplier=origMtl.displacementMapAmount
	if origMtl.displacementMap!=undefined then r.texmap_displacement=origMtl.displacementMap
	
	-- reflection
	if (origMtl.Reflect_Color_On==0) then
		(
		if origMtl.Reflect_Amount==0 then
			(
			ref_clr_enbl=false
--			r.option_traceReflection = off
			)
		else
			(
			ref_clr_enbl=true
			ray_refl=(origMtl.Reflect_Amount/100*255) as integer
			r.reflection=color ray_refl ray_refl ray_refl
			)
		)
	if (origMtl.Reflect_Color_On==1) then 
		(
		if origMtl.Reflect.v==0 then
			(
			ref_clr_enbl=false
--			r.option_traceReflection = off
			)
		else 
			(
			ref_clr_enbl=true
			r.reflection=origMtl.Reflect
			)
		)
	if (origMtl.Reflect_Color_On==2) then
		(
		if origMtl.Reflect.v==0 then
			(
			ref_clr_enbl=false
--			r.option_traceReflection = off
			)
		else
			(
			ref_clr_enbl=true
			r.reflection=origMtl.Reflect
			r.reflection_fresnel = on
			)
		)
--	if (ref_clr_enbl!=true) or (origMtl.reflectionMap==undefined) then r.option_traceReflection = off
	if (ref_clr_enbl!=false) then
		(
		if (origMtl.environmentMap!=undefined) then
			(
			r.texmap_environment=origMtl.environmentMap
			r.texmap_environment_on=origMtl.environmentMapEnable
			)
		)
		
	if origMtl.reflectionMap!=undefined then
		(
		cls_reflmap=classof origMtl.reflectionMap
--		r.texmap_reflection=origMtl.reflectionMap
--		r.texmap_reflection_on=origMtl.reflectionMapEnable
		if (origMtl.reflectionMapEnable!=false) then
			(
			r.reflection=std_refl origMtl.reflectionMapAmount
			)
		else
			(
			r.texmap_reflection_multiplier=origMtl.reflectionMapAmount
			)
		if (cls_reflmap == Bitmaptexture) then
			(
			if (origMtl.environmentMap==undefined) then
				(
				r.texmap_environment=origMtl.reflectionMap
				r.texmap_environment_on=origMtl.reflectionMapEnable
				)
			)
		else if (cls_reflmap == Mask) then
			(
			if (origMtl.reflectionMap.map!=undefined) then
				(
				if (origMtl.environmentMap==undefined) then
					(
					if (classof origMtl.reflectionMap.map!=VRayMap) then
						(
						r.texmap_environment=origMtl.reflectionMap.map
						r.texmap_environment_on=origMtl.reflectionMap.mapEnabled
						)
					else 
						(
						if origMtl.reflectionMap.map.EnvironmentMap!=undefined then
							(
							r.texmap_environment=origMtl.reflectionMap.map.EnvironmentMap
							)
						r.option_reflectOnBack = origMtl.reflectionMap.map.reflect_backSide
						)
					)
				)
			if (origMtl.reflectionMap.Mask!=undefined) then
				(
				r.texmap_reflection=origMtl.reflectionMap.Mask
				)
			)
		else if (cls_reflmap == falloff) then
			(
			r.texmap_reflection=origMtl.reflectionMap
			r.texmap_reflection.map1 = undefined
			r.texmap_reflection.map2 = undefined
			)
		)

	-- Hilight Glossines
	if origMtl.glossinessMap!=undefined then
		(
		if newVRay==true then
			(
			r.texmap_hilightGlossiness=origMtl.glossinessMap
			r.texmap_hilightGlossiness_on=origMtl.glossinessMapEnable
			r.texmap_hilightGlossiness_multiplier=origMtl.glossinessMapAmount
			r.reflection_lockGlossiness = off
			)
		else
			(
			r.texmap_reflectionGlossiness=origMtl.glossinessMap
			r.texmap_reflectionGlossiness_on=origMtl.glossinessMapEnable
			r.texmap_reflectionGlossiness_multiplier=origMtl.glossinessMapAmount
			)
		)
		
	-- refraction and opasity
	r.refraction_affectShadows = on
		
	cls_transmap=classof origMtl.transparencyMap
		
	if (origMtl.transparencyMap!=undefined) and (origMtl.transparencyMapEnable!=false) then
		(
		if newVRay==true then
			(
			r.refraction_ior=1.0
			r.reflection_fresnel = off
			r.texmap_opacity_on=origMtl.transparencyMapEnable
			r.texmap_opacity_multiplier=origMtl.transparencyMapAmount
			r.texmap_opacity=origMtl.transparencyMap
			)
		else
			(
			r.refraction_ior=1.0
			r.reflection_fresnel = off
			r.texmap_refraction_on=origMtl.transparencyMapEnable
			r.texmap_refraction_multiplier=origMtl.transparencyMapAmount
			r.texmap_refraction=origMtl.transparencyMap
			)
		)
	else
		(
		if (origMtl.Transparency_Color_On==true) then 
			(
			r.refraction=origMtl.Transparecy
			)
		else
			(
			ray_refr=(origMtl.Transparency_Amount/100*255) as integer
			ray_refr_vol=color ray_refr ray_refr ray_refr
			r.refraction=ray_refr_vol
			)
		r.refraction_ior=origMtl.Index_of_Refraction
		r.texmap_refractionIOR_on=origMtl.iorMapEnable
		r.texmap_refractionIOR_multiplier=origMtl.iorMapAmount
		if origMtl.iorMap!=undefined then r.texmap_refractionIOR=origMtl.iorMap
		if (r.refraction!=black) then
			(
			r.reflection_fresnel = on
			)
		)
	return r
)

---------------------------------------------
-- From Brazill -----------------------------
---------------------------------------------
fn convertFrom_Brazil origMtl = 
(
local r=VRayMtl()
r.name=origMtl.name

--Transfer Basic Attributes--
	r.effectsChannel=origMtl.effectsChannel
	r.option_doubleSided = origMtl.bmtl_twosided_on
	r.refraction_affectShadows = origMtl.bmtl_transp_shadows_on
	-- diffuse
	OrigRGB_value=origMtl.bmtl_Cs_color.v
	if OrigRGB_value==255 then
		(
		d_color=origMtl.bmtl_Cs_color
		d_color.v=max_value
		r.diffuse=d_color
		)
	else r.diffuse=origMtl.bmtl_Cs_color
	if origMtl.bmtl_Cs_map!=undefined then r.texmap_diffuse=origMtl.bmtl_Cs_map
	r.texmap_diffuse_multiplier = origMtl.bmtl_Cs_map_amt*100
	r.texmap_diffuse_on = origMtl.bmtl_Cs_map_on
	-- bump map
	if origMtl.bmtl_bump_map!=undefined then r.texmap_bump=origMtl.bmtl_bump_map
	r.texmap_bump_multiplier = origMtl.bmtl_bump_map_amt*100
	r.texmap_bump_on = origMtl.bmtl_bump_map_on
	-- displacement
	if origMtl.bmtl_disp_map!=undefined then r.texmap_displacement=origMtl.bmtl_disp_map 
	r.texmap_displacement_multiplier = origMtl.bmtl_disp_map_amt*100
	r.texmap_displacement_on = origMtl.bmtl_disp_map_on
	
	
--Transfer Highlight Attributes
	
	-- Shader if possible
	if origMtl[16] as string == "SubAnim:Phong_Highlight" then 
	(
	r.brdf_type = 0
	)
	if origMtl[16] as string == "SubAnim:Blinn_Highlight" then 
	(
	r.brdf_type = 1
	)
	if origMtl[16] as string == "SubAnim:Sheen_Highlight" then 
	(
	r.brdf_type = 2
	)

	-- Hilight Glossines
	if newVRay==true then
		(
		if (origMtl.texmapmgr[11].map!=undefined) then
			(
			r.reflection_lockGlossiness = off
			r.texmap_hilightGlossiness=origMtl.texmapmgr[11].map
			r.texmap_hilightGlossiness_on=origMtl.texmapmgr[11].map_on
			r.texmap_hilightGlossiness_multiplier=origMtl.texmapmgr[11].map_amount*100
			)
		)
	
	if (origMtl.texmapmgr[12].map!=undefined) then
		(
		r.texmap_reflectionGlossiness = origMtl.texmapmgr[12].map
		r.texmap_reflectionGlossiness_multiplier = origMtl.texmapmgr[12].map_amount*100
		r.texmap_reflectionGlossiness_on = origMtl.texmapmgr[12].map_on
		)

--Transfer Reflection Effects
	if origMtl.brazil_default.reflect.v != 0 or origMtl.texmapmgr[6].map != undefined then 
		(
		r.reflection = origMtl.brazil_default.reflect
		if origMtl.texmapmgr[6].map != undefined then
			(
			r.texmap_reflection = origMtl.texmapmgr[6].map
			r.texmap_reflection_multiplier = origMtl.texmapmgr[6].Map_Amount*100
			r.texmap_reflection_on = origMtl.texmapmgr[6].Map_On
			)
		if origMtl.bmtl_env_map!=undefined then 
			(
			r.texmap_environment = origMtl.bmtl_env_map
			r.texmap_environment_on	= origMtl.bmtl_env_map_on
			)
		)

--Transfer Reflection and Opasity Effects
	origMtl_op = origMtl.bmtl_Os_color
	origMtl_op.v = 255-origMtl.bmtl_Os_color.v
	r.refraction = origMtl_op
	
	cls_transmap=classof origMtl.bmtl_Os_map
	
	if (origMtl.bmtl_Os_map != undefined) then
		(
		r.refraction_ior=1.0
		r.reflection_fresnel = off
		if newVRay==true then
			(
			r.texmap_opacity = origMtl.bmtl_Os_map
			r.texmap_opacity_multiplier = origMtl.bmtl_Os_map_amt*100
			r.texmap_opacity_on = origMtl.bmtl_Os_map_on
			)
		else
			(
			r.texmap_refraction = origMtl.bmtl_Os_map
			r.texmap_refraction_multiplier = origMtl.bmtl_Os_map_amt*100
			r.texmap_refraction_on = origMtl.bmtl_Os_map_on
			if (cls_transmap == Bitmaptexture) then
				(
				if (origMtl.bmtl_Os_map.output.Invert == true) then
					(
					r.texmap_refraction.output.Invert = off
					)
				else
					(
					r.texmap_refraction.output.Invert = on
					)
				)
			if (cls_transmap == Bricks) then
				(
				swap r.texmap_refraction.Brick_Color r.texmap_refraction.Mortar_Color
				)
			)
		)
	else
		(
		r.refraction_ior = origMtl.brazil_default.ior
		if origMtl.bmtl_Os_color.v != 255 and origMtl.brazil_default.ior != 1 then
			(
			r.reflection_fresnel = on
			)
		)

--Return new material		
	return r
)

---------------------------------------------
-- From fR-stade 1 --------------------------
---------------------------------------------
fn convertFrom_FR origMtl = 
	(
	local r=VRayMtl()
	r.name=origMtl.name
	
	r.effectsChannel=origMtl.effectsChannel
	
	r.option_doubleSided = origMtl.twoSided
	
	-- Shader if possible
	if origMtl.shadingType == 0 then 
	(
	r.brdf_type = 0
	)
	if origMtl.shadingType == 1 then 
	(
	r.brdf_type = 1
	)
	if origMtl.shadingType == 2 then 
	(
	r.brdf_type = 2
	)
	
	
	-- diffuse
	OrigRGB_value=origMtl.diffuseColor.v
	if OrigRGB_value==255 then
		(
		d_color=origMtl.diffuseColor
		d_color.v=max_value
		r.diffuse=d_color
		)
	else r.diffuse=origMtl.diffuseColor
	if origMtl.diffuseMap!=undefined then r.texmap_diffuse=origMtl.diffuseMap
	r.texmap_diffuse_on=origMtl.useDiffuseMap
	r.texmap_diffuse_multiplier=origMtl.diffuseMapAmount
	
	-- bump map
	if origMtl.bumpMap!=undefined then r.texmap_bump=origMtl.bumpMap
	r.texmap_bump_on=origMtl.useBumpMap
	r.texmap_bump_multiplier=origMtl.bumpMapAmount
	
	-- displacement
	if origMtl.MTDMap!=undefined then r.texmap_displacement=origMtl.MTDMap
	r.texmap_displacement_on=origMtl.useMTDMap
	r.texmap_displacement_multiplier=origMtl.MTDMapAmount

	-- reflection
	r.reflection=origMtl.reflectFilter
	if origMtl.reflectMap!=undefined then r.texmap_reflection=origMtl.reflectMap
	r.texmap_reflection_on=origMtl.useReflectMap
	r.texmap_reflection_multiplier=origMtl.reflectMapAmount
	
	r.reflection_glossiness=1.0-sqr (origMtl.reflectGlossy*0.01)
	if origMtl.reflectBlurryMap!=undefined then r.texmap_reflectionGlossiness=origMtl.reflectBlurryMap
	r.texmap_reflectionGlossiness_on=origMtl.useReflectBlurryMap
	r.texmap_reflectionGlossiness_multiplier=origMtl.reflectBlurryMapAmount
	
	r.reflection_subdivs=sqrt origMtl.reflectSamples
	r.reflection_fresnel=origMtl.useFresnel
	r.reflection_useInterpolation=origMtl.useReflBlur
	
	if origMtl.reflectIOR!=origMtl.refractIOR then
		(
		r.reflection_lockIOR = off
		r.reflection_ior=origMtl.reflectIOR
		)
	if origMtl.reflectIORMap!=undefined then r.texmap_reflectionIOR=origMtl.reflectIORMap
	r.texmap_reflectionIOR_on=origMtl.useReflectIORMap
	r.texmap_reflectionIOR_multiplier=origMtl.reflectIORMapAmount
	
	-- refraction
	r.refraction=origMtl.refractFilter
	r.texmap_refraction=origMtl.refractMap
	r.texmap_refraction_on=origMtl.useRefractMap
	r.texmap_refraction_multiplier=origMtl.refractMapAmount
	
	r.refraction_ior=origMtl.refractIOR
	if origMtl.refractIORMap!=undefined then r.texmap_refractionIOR=origMtl.refractIORMap
	r.texmap_refractionIOR_on=origMtl.useRefractIORMap
	r.texmap_refractionIOR_multiplier=origMtl.refractIORMapAmount
	
	r.refraction_glossiness=1.0-sqr (origMtl.refractGlossy*0.01)
	if origMtl.refractBlurryMap!=undefined then r.texmap_refractionGlossiness=origMtl.refractBlurryMap
	r.texmap_refractionGlossiness_on=origMtl.useRefractBlurryMap
	r.texmap_refractionGlossiness_multiplier=origMtl.refractBlurryMapAmount
	
	r.refraction_subdivs=sqrt origMtl.refractSamples
	r.refraction_useInterpolation=origMtl.useRefrBlur
	
	-- opasity
	if newVRay==true then
		(
		if origMtl.opacityMap!=undefined then r.texmap_opacity=origMtl.opacityMap
		r.texmap_opacity_on=origMtl.useOpacityMap
		r.texmap_opacity_multiplier=origMtl.opacityMapAmount
		)
		
	-- environment
	if (origMtl.environmentMode == 2) then
		(
		r.texmap_environment_on = true
		if origMtl.environmentMap!=undefined then r.texmap_environment=origMtl.environmentMap
		)
	
	return r
)

-- convert to Standard process --
fn vray_refl x = (
		vrefl=(x/255*100) as integer
		return vrefl
		)

fn vrayMtl2Standard origMtl = (
	local r=Standardmaterial()
	r.name=origMtl.name
	
	r.effectsChannel=origMtl.effectsChannel
	
	r.twoSided = origMtl.option_doubleSided
	
	-- Shader if possible
	if origMtl.brdf_type == 0 then 
	(
	r.shadertype = 5
	)
	if origMtl.brdf_type == 1 then 
	(
	r.shadertype = 1
	)
	if origMtl.brdf_type == 2 then 
	(
	r.shadertype = 1
	)
	
	-- diffuse
	-- 有贴图优先使用贴图
	if origMtl.texmap_diffuse!=undefined then r.diffuseMap=origMtl.texmap_diffuse
	else r.Diffuse=origMtl.Diffuse
	r.diffuseMapEnable=origMtl.texmap_diffuse_on
	r.diffuseMapAmount=origMtl.texmap_diffuse_multiplier
	
	-- bump map
	r.bumpMap=origMtl.texmap_bump
	r.bumpMapEnable=origMtl.texmap_bump_on
	r.bumpMapAmount=origMtl.texmap_bump_multiplier
	
	-- displacement
	r.displacementMap=origMtl.texmap_displacement
	r.displacementMapEnable=origMtl.texmap_displacement_on
	r.displacementMapAmount=origMtl.texmap_displacement_multiplier
	
	-- reflection
	cls_envmap=classof origMtl.texmap_environment
	cls_reflmap=classof origMtl.texmap_reflection
	r.reflectionMapEnable=origMtl.texmap_reflection_on
	if (origMtl.texmap_reflection != undefined) then
		(
		if (cls_reflmap == falloff) then
			(
			r.reflectionMap = origMtl.texmap_reflection
			r.reflectionMap.map2 = Raytrace()
			r.reflectionMap.map2On=origMtl.texmap_reflection.map2On
			)
		else
			(
			r.reflectionMap = Mask()
			r.reflectionMap.map = Raytrace()
			r.reflectionMap.mask = origMtl.texmap_reflection
			)
		r.reflectionMapAmount=origMtl.texmap_reflection_multiplier
		r.reflectionMapEnable=origMtl.texmap_reflection_on
		)
	else 
		(
		if (origMtl.reflection.v > 0) then
			(
			r.reflectionMap = Raytrace()
			r.reflectionMapAmount=vray_refl origMtl.reflection.v
			r.reflectionMapEnable=true
			)
		--保留高光反射颜色
		r.specular = origMtl.reflection
		)
	
		
	-- Hilight Glossines
	if newVRay==true then
		(
		if (origMtl.texmap_hilightGlossiness!=undefined) then
			(
			r.GlossinessMap=origMtl.texmap_hilightGlossiness
			r.GlossinessMapEnable=origMtl.texmap_hilightGlossiness_on
			r.GlossinessMapAmount=origMtl.texmap_hilightGlossiness_multiplier
			)
		r.glossiness=(origMtl.hilight_glossiness*100)/2
		)
	else
		(
		if (origMtl.texmap_reflectionGlossiness!=undefined) then
			(
			r.GlossinessMap=origMtl.texmap_reflectionGlossiness
			r.GlossinessMapEnable=origMtl.texmap_reflectionGlossiness_on
			r.GlossinessMapAmount=origMtl.texmap_reflectionGlossiness_multiplier
			)
		r.glossiness=(origMtl.reflection_glossiness*100)/2
		)

	-- opacity
	r.opacity = (100-(((origMtl.Refraction.v)/255)*100)) as integer
	r.ior=origMtl.refraction_ior
	
	if newVRay==true then
		(
		if (origMtl.texmap_opacity!=undefined) then
			(
			r.opacityMap=origMtl.texmap_opacity
			r.opacityMapEnable=origMtl.texmap_opacity_on
			r.opacityMapAmount=origMtl.texmap_opacity_multiplier
			)
		)
	else
		(
		cls_refrmap=classof origMtl.texmap_refraction
		
		if (origMtl.texmap_refraction!=undefined) then
			(
			r.opacityMap=origMtl.texmap_refraction
			if (cls_refrmap == Bitmaptexture) then
				(
				if (origMtl.texmap_refraction.output.Invert == true) then
					(
					r.opacityMap.output.Invert = off
					)
				else
					(
					r.opacityMap.output.Invert = on
					)
				)
			if (cls_refrmap == Bricks) then
				(
				swap r.opacityMap.Brick_Color r.opacityMap.Mortar_Color
				)
			r.opacityMapEnable=origMtl.texmap_refraction_on
			r.opacityMapAmount=origMtl.texmap_refraction_multiplier
			)
		)

	--selfIllum Map
	r.selfIllumMap = origMtl.texmap_self_illumination
	r.selfIllumMapEnable = origMtl.texmap_self_illumination_on
	r.selfIllumMapAmount = origMtl.texmap_self_illumination_multiplier*5

	return r

)

fn vraylightMtl2Standard origMtl = (
	local r=Standardmaterial()
	r.name=origMtl.name
	
	r.useSelfIllumColor = true
	r.selfIllumColor = origMtl.color
	r.selfIllumAmount = origMtl.multiplier*5

	return r
)

-- create VRayMtl process --		
fn F_CreateVRayMtl origMtl = (
	cls=classof origMtl
	if (cls==Multimaterial) then (
		for i=1 to origMtl.count do origMtl[i]=F_CreateVRayMtl origMtl[i]
		return origMtl
	)
	if (cls==compositematerial) then (
		for i=1 to origMtl.materialList.count do origMtl.materialList[i]=F_CreateVRayMtl origMtl.materialList[i]
		return origMtl
	)
	if (cls==VRayMtlWrapper) then
		(
		neworigMtl=origMtl
		origMtl=F_CreateVRayMtl origMtl.BaseMtl
		neworigMtl.BaseMtl = origMtl
		return neworigMtl
		)
	if (cls==Blend) then 
		(
		neworigMtl=origMtl
		origMtl1=F_CreateVRayMtl origMtl.map1
		origMtl2=F_CreateVRayMtl origMtl.map2
		neworigMtl.map1 = origMtl1
		neworigMtl.map2 = origMtl2
		return neworigMtl
		)
	if (cls==Shellac) then 
		(
		neworigMtl=origMtl
		origMtl1=F_CreateVRayMtl origMtl.shellacMtl1
		origMtl2=F_CreateVRayMtl origMtl.shellacMtl2
		neworigMtl.shellacMtl1 = origMtl1
		neworigMtl.shellacMtl2 = origMtl2
		return neworigMtl
		)
	
	if (cls==VRayMtl) then 
		(
		return origMtl
		)
	
	if (cls==Standardmaterial) then 
		(
		append conv_mat origMtl
		return convertFrom_Standard origMtl
		)
	if (cls==RaytraceMaterial) then 
		(
		append conv_mat origMtl
		return convertFrom_Raytrace origMtl
		)
	if (cls==Brazil_Advanced) then 
		(
		if (origMtl[10] as string == "SubAnim:Brazil_Default") then 
			(
			append conv_mat origMtl
			return convertFrom_Brazil origMtl
			)
		)
	if (cls==fR_Advanced) then 
		(
		append conv_mat origMtl
		return convertFrom_FR origMtl
		)
	
	-- unrecognized material, leave as it is
	print " Unrecognized material"
	return origMtl
)

-- create Standard Mat process --		
fn F_CreateOtherMtl origMtl = (
	cls=classof origMtl
	if (cls==Multimaterial) then
	(
		for i=1 to origMtl.count do
		(
			origMtl[i]=F_CreateOtherMtl origMtl[i]
		)
		
		return origMtl
	)
	if (cls==compositematerial) then
	(
		for i=1 to origMtl.materialList.count do
		(
			origMtl.materialList[i]=F_CreateOtherMtl origMtl.materialList[i]
		)
		
		return origMtl
	)
	if (cls==Standardmaterial) then 
		(
		append conv_mat origMtl
		return convertFrom_Standard origMtl
		)
	if (cls==RaytraceMaterial) then 
		(
		append conv_mat origMtl
		return convertFrom_Raytrace origMtl
		)
	if (cls==Blend) then 
		(
		--FBX不支持Blend材质
		--neworigMtl=origMtl
		--origMtl1=F_CreateOtherMtl origMtl.map1
		--origMtl2=F_CreateOtherMtl origMtl.map2
		--neworigMtl.map1 = origMtl1
		--neworigMtl.map2 = origMtl2
		--return neworigMtl
		
		--返回第一个材质
		return F_CreateOtherMtl origMtl.map1
		)
	if (cls==Shellac) then 
		(
		neworigMtl=origMtl
		origMtl1=F_CreateOtherMtl origMtl.shellacMtl1
		origMtl2=F_CreateOtherMtl origMtl.shellacMtl2
		neworigMtl.shellacMtl1 = origMtl1
		neworigMtl.shellacMtl2 = origMtl2
		return neworigMtl
		)
	if (cls==VRayMtlWrapper) then 
		(
		append conv_mat origMtl
		origMtl=F_CreateOtherMtl origMtl.BaseMtl
		return origMtl
		)
	--
	if (cls==VRayBlendMtl) then
		(		
		append conv_mat origMtl
		origMtl=F_CreateOtherMtl origMtl.BaseMtl
		return origMtl
		)
	--
	if (cls==VRayOverrideMtl) then
		(
		append conv_mat origMtl
		origMtl=F_CreateOtherMtl origMtl.BaseMtl
		return origMtl
		)
	--
	if (cls==VRay2SidedMtl) then
		(
		append conv_mat origMtl
		origMtl=F_CreateOtherMtl origMtl.frontMtl
		return origMtl
		)
	if (cls==VrayLightMtl) then
		(
		append conv_mat origMtl
		return origMtl
		--return vrayLightMtl2Standard origMtl
		)
	if (cls==VRayMtl) then
		(
		append conv_mat origMtl
		return origMtl
		--return vrayMtl2Standard origMtl
		)
	else 
		(
		return origMtl
		)
	
	-- unrecognized material, leave as it is
	print " Unrecognized material"
	return origMtl
)

-- replace Mtl process--
fn F_ReplaceMtlNode node origMtl newMtl = (
	if ((isValidNode node) and (node.material==origMtl)) then (
		node.material = newMtl
		nodemat_cls = classof newMtl
	)
	for i=1 to node.children.count do (
		F_ReplaceMtlNode node.children[i] origMtl newMtl
	)
)

fn F_ReplaceMtl origMtl newMtl = (
	if (origMtl != newMtl) then F_ReplaceMtlNode rootNode origMtl newMtl
)

-- convert choisen materials to VRay materials process --
fn F_MtlsToVRay = (
	mat_array = F_GetMatArray()
	matstd_count=0
	print "...Converting to VRayMtl..."
	for i in mat_array do
		(
		matstd_count += 1
		origMtl=i
		print "Converting - " + origMtl.name
		
		-- create a new VRay material out of this one
		vrMtl = F_CreateVRayMtl origMtl
		
		-- Apply Crop\Clamp for Bitmap's
		-- F_Clamper vrMtl

		-- put the material back to the scene
		F_ReplaceMtl origMtl vrMtl
		)
	if conv_mat.count==0 then 
		(
		print (cur_proc2_text1 + "Converting!")
		)
	else
		(
		print ((conv_mat.count as string) + cur_proc2_text2 + "Converted.")
		)
)

fn F_MtlsFromVRay = (
	mat_array = F_GetMatArray()
	matvr_count=0
	print "...Converting to Standard..."
	for i in mat_array do
		(
		matvr_count += 1
		origMtl=i
		print "Converting - " + origMtl.name
		
		-- create a new Standard material out of this one
		stMtl = F_CreateOtherMtl origMtl
		
		-- put the material back to the scene
		F_ReplaceMtl origMtl stMtl
		)
	if conv_mat.count==0 then 
		(
		print (cur_proc2_text1 + "Converting!\n")
		)
	else 
		(
		print ((conv_mat.count as string) + cur_proc2_text2 + "Converted.")
		)
)

fn F_Clamper mat = 
	(
	if mat.numsubs != 0 then
		(
		for i = 1 to mat.numsubs do
			(
			if mat[i] != undefined then
				(
				try
					(
					if iskindof mat[i] texturemap == true then  mat[i].output.clamp = true
					if iskindof mat[i] bitmaptexture == true then mat[i].apply = true
					)
				catch()
				)
			F_Clamper mat[i]
			)
		)
	)

fn F_ShadowsNode node = (
	if (isValidNode node) then (
		scls=superclassof node
		if (scls==light) then (
			try (
				olds=node.shadowGenerator
				cls=classof olds
				node.shadowGenerator=ShadowMap()
			)
			catch ()
		)
	)
	
	for i=1 to node.children.count do (
		F_ShadowsNode node.children[i]
	)
)

fn F_ShadowsConv = (
	F_ShadowsNode rootNode
)

fn F_ConvertToVRay = (
	F_MtlsToVRay()
	--Convert Light's Shadow Types
	--F_ShadowsConv()
	)

fn F_ConvertFromVray = (
	F_MtlsFromVRay()
	--Convert Light's Shadow Types
	--F_ShadowsConv()
	)
	
--分离多维材质
fn F_DetachMultiMaterial obj =
(
	local detachedObjArray = #()
	if GeometryClass != (superClassOf obj) do
	(
		return detachedObjArray
	)
	
	if Multimaterial != (classOf obj.material) do
	(
		return detachedObjArray
	)
	
	try
	(
		convertToPoly obj
	)
	catch
	(
		print "Error Occured on convertToPoly"
		return detachedObjArray
	)
	
	if Editable_Poly != (classOf obj) do
	(
		return detachedObjArray
	)
	
	local theMat = obj.material
	for id in theMat.materialIDList do
	(
		if Material != (superClassOf theMat[id]) do
		(
			continue
		)
		obj.selectByMaterial id
		theFaces = getFaceSelection obj
		
		theName = uniquename (obj.name)
		
		polyOp.detachFaces obj theFaces asNode:true name:theName
		theDetachedObj = (getNodeByName theName)
		
		if false == (isValidNode theDetachedObj) do
		(
			continue
		)
		
		--防止子材质重名
		uuid = ((dotNetClass "System.Guid").NewGuid()).ToString()
		theMat[id].name = uuid
		theDetachedObj.material = theMat[id]
		
		if Multimaterial == (classOf theDetachedObj.material) then
		(
			format "嵌套多维材质 %\n" theDetachedObj.material
			
			local detachedSubObjArray = F_DetachMultiMaterial theDetachedObj
			for theDetachedSubObj in detachedSubObjArray do
			(
				append detachedObjArray theDetachedSubObj
			)
		)
		else
		(
			append detachedObjArray theDetachedObj
		)
	)
	
	return detachedObjArray
)

fn F_CropImage bitImg clipX clipY clipW clipH =
(
	xB = clipX
	yB = clipY
	xE = clipX + clipW
	yE = clipY + clipH
	
	theImg = bitmap clipW clipH color:white
	pasteBitmap bitImg theImg (box2 xB yB xE yE) [0, 0]
	
	return theImg
)

fn F_SaveImage bitImg filePath =
(
	bitImg.filename = filePath
	save bitImg
	close bitImg
	--gc()
)

fn F_ClipBitmaptexture theMtlMap =
(
	print theMtlMap
	if (undefined == theMtlMap) do
	(
		return theMtlMap
	)

	if (0.000001 > theMtlMap.clipu)	\
		AND (0.000001 > theMtlMap.clipv)	\
		AND (0.000001 > abs (1.0 - theMtlMap.clipw))	\
		AND (0.000001 > abs (1.0 - theMtlMap.cliph))	\
	do
	(
		return theMtlMap
	)
	
	srcFileName = theMtlMap.fileName
	srcBitImg = openBitMap srcFileName
	if undefined == srcBitImg do
	(
		return theMtlMap
	)
	
	clipX = srcBitImg.width * theMtlMap.clipu
	clipY = srcBitImg.height * theMtlMap.clipv
	clipW = srcBitImg.width * theMtlMap.clipw
	clipH = srcBitImg.height * theMtlMap.cliph
	
	tgtBitImg = F_CropImage srcBitImg clipX clipY clipW clipH
	
	uuid = ((dotNetClass "System.Guid").NewGuid()).ToString()
	fileType = getFilenameType srcFileName
	tgtFileName = maxFilePath + uuid + fileType
	
	F_SaveImage tgtBitImg tgtFileName
	
	theMtlMap.fileName = tgtFileName
	theMtlMap.clipu = 0
	theMtlMap.clipv = 0
	theMtlMap.clipw = 1
	theMtlMap.clipH = 1
	
	theMtlMap.reload()
	
	return theMtlMap
)

--裁剪贴图位图
fn F_ClipMtlTexmap theMtl =
(
	if Material != (superClassOf theMtl) do
	(
		return false
	)
	
	--if StandardMaterial != (classOf theMtl) do
	if VRayMtl != (classOf theMtl) do
	(
		return false
	)
	
	if Bitmaptexture == (classOf theMtl.texmap_diffuse) do (theMtl.texmap_diffuse = F_ClipBitmaptexture theMtl.texmap_diffuse)
	if Bitmaptexture == (classOf theMtl.texmap_reflection) do (theMtl.texmap_reflection = F_ClipBitmaptexture theMtl.texmap_reflection)
	if Bitmaptexture == (classOf theMtl.texmap_refraction) do (theMtl.texmap_bump = F_ClipBitmaptexture theMtl.texmap_bump)
	if Bitmaptexture == (classOf theMtl.texmap_bump) do (theMtl.texmap_bump = F_ClipBitmaptexture theMtl.texmap_bump)
	if Bitmaptexture == (classOf theMtl.texmap_reflectionGlossiness) do (theMtl.texmap_reflectionGlossiness = F_ClipBitmaptexture theMtl.texmap_reflectionGlossiness)
	if Bitmaptexture == (classOf theMtl.texmap_refractionGlossiness) do (theMtl.texmap_refractionGlossiness = F_ClipBitmaptexture theMtl.texmap_refractionGlossiness)
	if Bitmaptexture == (classOf theMtl.texmap_refractionIOR) do (theMtl.texmap_refractionIOR = F_ClipBitmaptexture theMtl.texmap_refractionIOR)
	if Bitmaptexture == (classOf theMtl.texmap_displacement) do (theMtl.texmap_displacement = F_ClipBitmaptexture theMtl.texmap_displacement)
	if Bitmaptexture == (classOf theMtl.texmap_translucent) do (theMtl.texmap_translucent = F_ClipBitmaptexture theMtl.texmap_translucent)
	if Bitmaptexture == (classOf theMtl.texmap_environment) do (theMtl.texmap_environment = F_ClipBitmaptexture theMtl.texmap_environment)
	if Bitmaptexture == (classOf theMtl.texmap_hilightGlossiness) do (theMtl.texmap_hilightGlossiness = F_ClipBitmaptexture theMtl.texmap_hilightGlossiness)
	if Bitmaptexture == (classOf theMtl.texmap_reflectionIOR) do (theMtl.texmap_reflectionIOR = F_ClipBitmaptexture theMtl.texmap_reflectionIOR)
	if Bitmaptexture == (classOf theMtl.texmap_opacity) do (theMtl.texmap_opacity = F_ClipBitmaptexture theMtl.texmap_opacity)
	if Bitmaptexture == (classOf theMtl.texmap_roughness) do (theMtl.texmap_roughness = F_ClipBitmaptexture theMtl.texmap_roughness)
	if Bitmaptexture == (classOf theMtl.texmap_anisotropy) do (theMtl.texmap_anisotropy = F_ClipBitmaptexture theMtl.texmap_anisotropy)
	if Bitmaptexture == (classOf theMtl.texmap_anisotropy_rotation) do (theMtl.texmap_anisotropy_rotation = F_ClipBitmaptexture theMtl.texmap_anisotropy_rotation)
	if Bitmaptexture == (classOf theMtl.texmap_refraction_fog) do (theMtl.texmap_refraction_fog = F_ClipBitmaptexture theMtl.texmap_refraction_fog)
	if Bitmaptexture == (classOf theMtl.texmap_self_illumination) do (theMtl.texmap_self_illumination = F_ClipBitmaptexture theMtl.texmap_self_illumination)
	
	return true
)

fn F_RenderTexmap theMtlMap =
(
	if UndefinedClass == (classOf theMtlMap) do
	(
		return theMtlMap
	)
	
	if VRayDirt == (classOf theMtlMap) do
	(
		format "丢弃VRayDirt贴图\n"
		return undefined
	)
	
	format "Before:%\n" theMtlMap
	
	local srcMtlMap = theMtlMap
	
	uuid = ((dotNetClass "System.Guid").NewGuid()).ToString()
	filePath = maxFilePath + uuid + ".jpg"
	print filePath
	
	bitImg = rendermap srcMtlMap size:[480,480] filename:filePath
	save bitImg
	free bitImg
	
	local tgtMtlMap = bitmaptexture()
	tgtMtlMap.fileName = filePath
	tgtMtlMap.reload()
	
	theMtlMap = tgtMtlMap
	theMtlMap.reload()
	
	format "After:%\n" theMtlMap
	return theMtlMap
)

--通过渲染贴图更新贴图
fn F_UpdateMapsWithRenderMap theMtl =
(
	if Material != (superClassOf theMtl) do
	(
		return theMtl
	)
	
	--if StandardMaterial != (classOf theMtl) do
	if VRayMtl != (classOf theMtl) do
	(
		return theMtl
	)
	
	if Bitmaptexture != (classOf theMtl.texmap_diffuse) do (theMtl.texmap_diffuse = F_RenderTexmap theMtl.texmap_diffuse)
	if Bitmaptexture != (classOf theMtl.texmap_reflection) do (theMtl.texmap_reflection = F_RenderTexmap theMtl.texmap_reflection)
	if Bitmaptexture != (classOf theMtl.texmap_refraction) do (theMtl.texmap_bump = F_RenderTexmap theMtl.texmap_bump)
	if Bitmaptexture != (classOf theMtl.texmap_bump) do (theMtl.texmap_bump = F_RenderTexmap theMtl.texmap_bump)
	if Bitmaptexture != (classOf theMtl.texmap_reflectionGlossiness) do (theMtl.texmap_reflectionGlossiness = F_RenderTexmap theMtl.texmap_reflectionGlossiness)
	if Bitmaptexture != (classOf theMtl.texmap_refractionGlossiness) do (theMtl.texmap_refractionGlossiness = F_RenderTexmap theMtl.texmap_refractionGlossiness)
	if Bitmaptexture != (classOf theMtl.texmap_refractionIOR) do (theMtl.texmap_refractionIOR = F_RenderTexmap theMtl.texmap_refractionIOR)
	if Bitmaptexture != (classOf theMtl.texmap_displacement) do (theMtl.texmap_displacement = F_RenderTexmap theMtl.texmap_displacement)
	if Bitmaptexture != (classOf theMtl.texmap_translucent) do (theMtl.texmap_translucent = F_RenderTexmap theMtl.texmap_translucent)
	if Bitmaptexture != (classOf theMtl.texmap_environment) do (theMtl.texmap_environment = F_RenderTexmap theMtl.texmap_environment)
	if Bitmaptexture != (classOf theMtl.texmap_hilightGlossiness) do (theMtl.texmap_hilightGlossiness = F_RenderTexmap theMtl.texmap_hilightGlossiness)
	if Bitmaptexture != (classOf theMtl.texmap_reflectionIOR) do (theMtl.texmap_reflectionIOR = F_RenderTexmap theMtl.texmap_reflectionIOR)
	if Bitmaptexture != (classOf theMtl.texmap_opacity) do (theMtl.texmap_opacity = F_RenderTexmap theMtl.texmap_opacity)
	if Bitmaptexture != (classOf theMtl.texmap_roughness) do (theMtl.texmap_roughness = F_RenderTexmap theMtl.texmap_roughness)
	if Bitmaptexture != (classOf theMtl.texmap_anisotropy) do (theMtl.texmap_anisotropy = F_RenderTexmap theMtl.texmap_anisotropy)
	if Bitmaptexture != (classOf theMtl.texmap_anisotropy_rotation) do (theMtl.texmap_anisotropy_rotation = F_RenderTexmap theMtl.texmap_anisotropy_rotation)
	if Bitmaptexture != (classOf theMtl.texmap_refraction_fog) do (theMtl.texmap_refraction_fog = F_RenderTexmap theMtl.texmap_refraction_fog)
	if Bitmaptexture != (classOf theMtl.texmap_self_illumination) do (theMtl.texmap_self_illumination = F_RenderTexmap theMtl.texmap_self_illumination)
	
	return theMtl
)

--视口配置
viewport.numViewEx()
viewport.activeViewportEx 1
viewport.setLayout #layout_1
viewport.setGridVisibility #all false

vs = NitrousGraphicsManager.GetActiveViewportSetting()
vs.VisualStyleMode = #Shaded 
vs.UseTextureEnabled = true
vs.TransparencyEnabled = true
vs.ShowSelectionBracketsEnabled = false
vs.SelectedEdgedFacesEnabled = false
vs.ShadeSelectedFacesEnabled = false
vs.ShadeSelectedObjectsEnabled = false
vs.LightOption = #DefaultLight
vs.DefaultLightMode = #TwoLights
vs.DefaultLightsFollowCamera = true
vs.ShowHighlightEnabled = true
vs.AutoDisplaySelectedLightEnabled = false
vs.LightingAndShadowQualityMode = 6 --2.0X
vs.UseSimplifiedSkyLight = false
vs.ShadowsEnabled = false
vs.AmbientOcclusionEnabled  = true
vs.AmbientOcclusionIntensity = 1.0
vs.AmbientOcclusionRadius = 30
vs.ProgressiveFadingFactor = 4
--开启后台渐进渲染
NitrousGraphicsManager.BackgroundProgressiveRenderingEnabled = true

--不显示安全框
displaySafeFrames = false

fn F_ShowMaterialInViewPort =
(
	for obj in $* do
	(
		if obj.material == undefined do
		(
			continue
		)
		
		local theM = obj.material
		if (isKindOf theM Multimaterial) then
		(
			local subMaterialList = theM.materialList
			for subM in subMaterialList do
			(
				if undefined == subM do
				(
					continue
				)

				subM.showInViewport = on
			)
		)
		if (isKindOf theM Blend) then
		(
			theM.map1.showInViewport = on
			theM.map2.showInViewport = on
		)
		else
		(
			theM.showInViewport = on
		)
		--showTextureMap obj.material on
	)
)

fn F_CaptureViewPortDib =
(
	local x = y = 0, v
	local s = getViewSize()
	local k = getRendImageAspect()

	if k < (s.x/s.y) then
	(
		v = [s.y*k, s.y]
		x = (s.x-v.x)/2
	)
	else
	(
		v = [s.x, s.x/k]
		y = (s.y-v.y)/2
	)
	
	bitImg = bitmap s.x s.y
	--pasteBitmap (gw.getViewportDib()) bitImg (box2 x y v.x v.y) [0,0]
	pasteBitmap (gw.getViewportDib()) bitImg (box2 0 0 s.x s.y) [0,0]
	return bitImg
)

fn F_WriteFile filePath fileData =
(
	fs = openFile filePath mode:"w+"

	format "%" fileData to:fs
	flush fs
	close fs
)

fn F_RotatePivotOnly obj rotation =
(
	local rotValInv = inverse (rotation as quat)
	animate off in coordsys local obj.rotation *= rotValInv
	obj.objectoffsetpos *= rotValInv
	obj.objectoffsetrot *= rotValInv
)

fn RotatePivotOnlyAbsolute obj rotation = 
(
	--capture current rotation
	currentRot=obj.objectoffsetrot as eulerangles
	--not performance oriented readablity step
	wishRot=rotation
	--calculate the difference
	needRot=(currentRot as quat - wishRot as quat) as eulerangles
	--rotate only the pivot
	F_RotatePivotOnly obj needRot
)

fn F_AvgColor bitImg =
(
	local fSumR = 0.0
	local fSumG = 0.0
	local fSumB = 0.0
	
	local nW = bitImg.width
	local nH = bitImg.height
	
	local iCount = 0
	for r=0 to nH-1 do
	(
		local pixels = getpixels bitImg [0,r] nW
		
		for c=1 to nW do
		(
			local p = pixels[c]
			
			--alpha权重
			local iWeight = (p.a / 255)
			
			fSumR += p.r * iWeight
			fSumG += p.g * iWeight
			fSumB += p.b * iWeight
			
			iCount += iWeight
		)
	)
	
	local fAvgR = fSumR / iCount
	local fAvgG = fSumG / iCount
	local fAvgB = fSumB / iCount
	
	return (color fAvgR fAvgG fAvgB)
)

--
max zoomext sel all
completeredraw()

GetRenderers()
print full_vrstr
print "Convert All Scene Materials"
print "Don't Convert light's Shadow Types"
print "Don't Apply Crop/Clamp for Bitmap's"
F_ListMat()
print (sc_mat as string + " Materials in Scene")

--其他材质转VRayMtl
F_ConvertFromVray()

--分离多维材质
(
	--备份原组的pivot
	local backupPivot = [0, 0, 0]
	
	--炸开原组
	for obj in $* do
	(
		if isGroupHead obj do
		(
			backupPivot = obj.pivot
			
			format "GroupHeader % \n" obj
			explodeGroup obj
		)
	)

	--分离
	local deleteObjArray =#()
	for obj in $* do
	(
		format "对象% 材质%\n" obj.name obj.material
		if Multimaterial != (classOf obj.material) do
		(
			continue
		)
		
		F_DetachMultiMaterial obj
		
		append deleteObjArray obj
	)
	delete deleteObjArray

	--重新组合
	group $* select:true
	
	--还原新组的pivot
	for obj in $* do
	(
		if isGroupHead obj do
		(
			obj.pivot = backupPivot
		)
	)
)

--裁剪贴图位图
(
	for theMat in sceneMaterials do
	(
		F_ClipMtlTexmap theMat
	)
)

--处理Mask/Falloff/Gradient这些贴图
(
	for theMtl in sceneMaterials do
	(
		print theMtl
		newMtl = F_UpdateMapsWithRenderMap theMtl
		
		F_ReplaceMtl theMtl newMtl
	)
)

--
F_ShowMaterialInViewPort()

--设置focus到世界坐标原点
--viewport.ZoomToBounds true (point3 0 0 0) (point3 0 0 0)


--转换为SBS材质
for theMtl in sceneMaterials do
(
	if VRayMtl != (ClassOf theMtl) do
	(
		continue
	)
	local theMat = copy theMtl
	
	print theMat.name
	format "reflection_glossiness %\n" theMat.reflection_glossiness
	format "hilight_glossiness %\n" theMat.hilight_glossiness
	format "reflection_lockGlossiness %\n" theMat.reflection_lockGlossiness
	format "diffuse %\n" theMat.diffuse
	format "reflection %\n" theMat.reflection
	format "reflection_fresnel %\n" theMat.reflection_ior
	
	if (undefined != theMat.texmap_reflection) do
	(
		reflectIntensity = theMat.texmap_reflection_multiplier / 100
		
		texmapImg = openBitMap theMat.texmap_reflection.fileName
		theMat.reflection = F_AvgColor texmapImg
		theMat.reflection *= reflectIntensity
		
		format "使用贴图反射 reflection %\n" theMat.reflection
	)
	
	if false == theMat.option_traceReflection do
	(
		theMat.reflection = Color 0 0 0 1
		format "关闭追踪反射 reflection %\n" theMat.reflection
	)
	
	if true == theMat.reflection_fresnel	\
		AND 0 < theMat.reflection.v			\
	do
	(
		local rgba = Color theMat.reflection.r theMat.reflection.g theMat.reflection.b theMat.reflection.a
		
		format "菲涅尔反射前 reflection %\n" theMat.reflection
		if 1.0 > theMat.reflection_ior do
		(
			x = theMat.reflection_ior
			--y = -367.65x6 + 509.99x5 + 798.93x4 - 2214.9x3 + 2064.7x2 - 1046.3x + 255.16
			rgba.v = -367.65*x*x*x*x*x*x + 509.99*x*x*x*x*x + 798.93*x*x*x*x - 2214.9*x*x*x + 2064.7*x*x - 1046.3*x + 255.16
		)
		if 0.001 > abs (1.0 - theMat.reflection_ior) do
		(
			rgba.v = 0.0
		)
		if 1.0 < theMat.reflection_ior do
		(
			x = theMat.reflection_ior
			--y = 56.389ln(x) - 0.1197
			rgba.v = 56.389*(log x) - 0.1197
		)
		
		if 0 > rgba.v do
		(
			rgba.v  = 0
		)
		if 255 < rgba.v do
		(
			rgba.v  = 255
		)
		
		theMat.reflection = rgba
		format "菲涅尔反射后 reflection %\n" theMat.reflection
	)
	
	basecolorBitmap = ""
	if (undefined != theMat.texmap_diffuse) do
	(
		basecolorBitmap = theMat.texmap_diffuse.fileName
	)
	basecolorColor = ""
	(		
		--按照漫反射与反射的亮度 进行混合
		fA_diffuse =  theMat.diffuse.v / (theMat.diffuse.v + theMat.reflection.v)
		fA_reflection = theMat.reflection.v / (theMat.diffuse.v + theMat.reflection.v)
		format "fA_diffuse:%\n" fA_diffuse
		format "fA_reflection:%\n" fA_reflection
		
		theR = theMat.reflection.r * fA_reflection + theMat.diffuse.r * fA_diffuse
		theG = theMat.reflection.g * fA_reflection + theMat.diffuse.g * fA_diffuse
		theB = theMat.reflection.b * fA_reflection + theMat.diffuse.b * fA_diffuse
		
		format "% % %\n" (theR) (theG) (theB)
		
		ss = stringstream ""
		format "% % % %" (theR/256) (theG/256) (theB/256) (1.0) to:ss
		basecolorColor = ss as string
	)
	
	normalBitmap = ""
	if (undefined != theMat.texmap_bump) do
	(
		normalBitmap = theMat.texmap_bump.fileName
	)
	normalIntensity = theMat.texmap_bump_multiplier / 100
	
	roughnessColor = ""
	(
		--roughnessColorR = (1.0 - theMat.reflection.r/255) * theMat.reflection_glossiness
		--roughnessColorG = (1.0 - theMat.reflection.g/255) * theMat.reflection_glossiness
		--roughnessColorB = (1.0 - theMat.reflection.b/255) * theMat.reflection_glossiness
		--roughnessColorA = (theMat.reflection.a/255)
		
		z = 1.0
		if 0 < theMat.reflection.v then
		(
			--反射越强 粗糙度越低
			--反射模糊越弱（reflection_glossiness = 1.0 完全光滑） 粗糙度越低
			--roughnessColorR = (1.0 - theMat.reflection.r/255) * (1.0 - theMat.reflection_glossiness)
			--roughnessColorG = (1.0 - theMat.reflection.g/255) * (1.0 - theMat.reflection_glossiness)
			--roughnessColorB = (1.0 - theMat.reflection.b/255) * (1.0 - theMat.reflection_glossiness)
			--roughnessColorA = (1.0 - theMat.reflection.a/255)
			z = (1.0 - theMat.reflection_glossiness)*0.7 + (1.0 - theMat.reflection.r/255)*0.3
		)
		
		ss = stringstream ""
		format "% % % %" (z) (z) (z) (1.0) to:ss
		roughnessColor = ss as string
	)
	
	metallicColor = ""
	(
		if true == theMat.reflection_lockGlossiness do
		(
			theMat.hilight_glossiness = theMat.reflection_glossiness
		)
	
		--高光值计算
		--x = theMat.hilight_glossiness
		--y = theMat.reflection.v
		--w = (3716.3x6 - 6158.9x5 + 3765.6x4 - 976.22x3 + 89.229x2 + 3.6218x) * y
		x = theMat.hilight_glossiness
		y = theMat.reflection.v
		w = (3716.3*x*x*x*x*x*x - 6158.9*x*x*x*x*x + 3765.6*x*x*x*x - 976.22*x*x*x + 89.229*x*x + 3.6218*x) * y
		if w > 255 then w = 255.0
		
		--金属值计算
		--theMat.reflection.v > theMat.diffuse.v 反射增强本色 --->金属感强
		--theMat.reflection.v > theMat.diffuse.v 反射削弱本色 --->金属感弱
		z = (w*0.01 + theMat.reflection.v*0.95) * (theMat.reflection.v / theMat.diffuse.v)
		if z > 255 then z = 255.0
		
		format "高光色% 金属色%\n" w z
		
		ss = stringstream ""
		format "% % % %" (z/255) (z/255) (z/255) (1.0) to:ss
		metallicColor = ss as string
	)

	--
	ss = stringstream ""
	
	format  "{
				\"name\": \"%\",
				\"basecolor\": {
					\"bitmap\": \"%\",
					\"color\": \"%\"
				},
				\"normal\": {
					\"bitmap\": \"%\",
					\"intensity\": \"%\"
				},
				\"roughness\": {
					\"color\": \"%\"
				},
				\"metallic\": {
					\"color\": \"%\"
				}
			}"	\
			theMat.name basecolorBitmap basecolorColor normalBitmap normalIntensity roughnessColor metallicColor to:ss
	
	fileData = substituteString (ss as string) "\\" "/"
	print fileData

	--写入转换参数到文件
	filePath = maxFilePath + theMat.name + ".json"
	F_WriteFile filePath fileData
	
	cvtDir = getFilenamePath(getSourceFileName())
	cvtTools = "VRAY2SBS.exe"
	
	cvtTemplate = cvtDir + "template.sbs"
	cvtParameter = substituteString (filePath as string) "\\" "/"
	
	ss = stringstream ""
	format  "%/% \"%\" \"%\"" cvtDir cvtTools cvtTemplate cvtParameter to:ss
	theCmd = ss as string
	
	print theCmd
	
	HiddenDOSCommand theCmd donotwait:false
)

--转换为可编辑多边形
--防止导入Unity出现UV问题
for m in objects do
(
	converttopoly m
)

--Y轴朝上
(
	--备份原组的pivot
	local backupPivot = [0, 0, 0]
	
	for obj in $* do
	(
		if isGroupHead obj do
		(
			backupPivot = obj.pivot
			format "backupPivot %\n" backupPivot
			
			--旋转
			rotate obj (EulerAngles -90 0 0)
			
			--炸开
			explodeGroup obj
		)
	)
	
	--组成员
	/*for obj in $* do
	(
		if isGroupHead obj do
		(
			continue
		)
		
		ResetXForm obj
		ResetScale obj
		
		--锚点居中到对象
		CenterPivot obj
		--锚点对齐到对象
		AlignPivot obj
	)*/

	--重新组合
	group $* select:true
	
	for obj in $* do
	(
		if isGroupHead obj do
		(
			obj.pivot = backupPivot
			F_RotatePivotOnly obj (EulerAngles 90 0 0)
		)
	)
	
	--组成员
	/*for obj in $* do
	(
		if isGroupHead obj do
		(
			continue
		)
		
		RotatePivotOnlyAbsolute obj (EulerAngles 0 0 0)
	)*/
)

--导出FBX
(
	tokens = filterString maxFileName "."
	fbxFileName = tokens[tokens.Count - 1] + ".fbx"
	fbxFilePath = (substituteString (maxFilePath) "\\" "/")

	exportFile (fbxFilePath + fbxFileName) #noPrompt using:FBXEXP
)

--退出
quitMAX #noPrompt
