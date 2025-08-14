#include <string.h>
#include <stdlib.h>
#include <math.h>
#define CGLTF_IMPLEMENTATION
#include "cgltf.hpp"

cgltf_data *loadGLTF(const char *fileName) { // glt/glb
  cgltf_options options;
  cgltf_data *data;
  memset(&options,0,sizeof(cgltf_options));
  cgltf_result result = cgltf_parse_file(&options, fileName, &data);
  if (result == cgltf_result_success) {
    result = cgltf_load_buffers(&options, data, fileName);
    if (result == cgltf_result_success) {
      result = cgltf_validate(data);
    }
  }
  return result == cgltf_result_success ? data : NULL;
}

int indent = 0;
void logIndent() {
  for (int i = 0; i < indent; i++) printf(".");
}

void logFileType(cgltf_file_type p) {
  const char *s = "[]";
  switch(p) {
  case cgltf_file_type_invalid: {s = "invalid";} break;
  case cgltf_file_type_gltf: {s = "gltf";} break;
  case cgltf_file_type_glb: {s = "glb";} break;
  }
  logIndent();printf("file_type:(%d)->%s\n",p,s);
}

void logPtr(const char *name, void *ptr) {
  logIndent();printf("%s #%08x\n",name,(unsigned int)ptr);
}

void logCountPtr(const char *name, int c, void *ptr) {
  logIndent();printf("%s[%d] #%08x\n",name,c,(unsigned int)ptr);
}

void logPtrN(const char *name, void *ptr) {
  printf("%s #%08x;",name,(unsigned int)ptr);
}

void logCount(const char *name, int count) {
  logIndent();printf("%s count:%d\n",name,count);
}

void logCountN(const char *name, int count) {
  printf("%s count:%d;",name,count);
}

void logPtr2(const char *name, void *ptr, int count) {
  logIndent();printf("[%d] %s #%08x\n",count,name,(unsigned int)ptr);
}

void log(const char *v) {
  logIndent();printf("%s\n",v);
}

void log2(const char *v, void *k) {
  logIndent();printf("%s #%08x\n",v,(int)k);
}

void logN(const char *v) {
  printf("%s;",v);
}

void logN2(const char *v, void *k) {
  printf("%s #%08x;",v,(int)k);
}

void logLF() {
  printf("\n");
}

void logString(const char *n, const char *v) {
  logIndent();printf("%s: %s\n",n,v!=NULL?v:"<NULL>");
}

void logString2(const char *n, const char *v, void *k) {
  logIndent();printf("%s: %s #%08x\n",n,v!=NULL?v:"<NULL>",(int)k);
}

void logString3(const char *n, const char *v) {
  logIndent();printf("%s: %s #%08x\n",v,n,(int)0);
}

void logBool(const char *n, const bool v) {
  logIndent();printf("%s: %s\n",n,v?"yes":"no");
}

void logInt(const char *n, const int v) {
  logIndent();printf("%s: %d\n",n,v);
}

void logFloat(const char *n, const float v) {
  logIndent();printf("%s: %f\n",n,v);
}

void logCountString(const char *n, int c, const char *v) {
  logIndent();printf(n,c);printf("%s\n",v!=NULL?v:"<NULL>");
}

void logExtras(cgltf_extras *v) {
  indent++;
  logIndent();
  logN2("extras-begin {",v);
  indent++;
  logCountN("start_offset",v->start_offset);
  logCountN("end_offset",v->end_offset);
  logPtrN("data",v->data);
  indent--;
  logN("} extras-end");
  logLF();
  indent--;
}

void logAsset(cgltf_asset *v) {
  indent++;
  log2("asset-begin {",v);
  indent++;
  logString("copyright",v->copyright);
  logString("generator",v->generator);
  logString("version",v->version);
  logString("min_version",v->min_version);
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  log("} asset-end");
  indent--;
}

void logMemoryOptions(cgltf_memory_options *v) {
  indent++;
  log2("memory_options-begin {",v);
  indent++;
  logPtr("alloc_func",v->alloc_func);
  logPtr("free_func",v->free_func);
  logPtr("user_data",v->user_data);
  indent--;
  log("} memory_options-end");
  indent--;
}

void logFileOptions(cgltf_file_options *v) {
  indent++;
  log2("file_options-begin {",v);
  indent++;
  logPtr("read",v->read);
  logPtr("release",v->release);
  logPtr("user_data",v->user_data);
  indent--;
  log("} file_options-end");
  indent--;
}

void logData(cgltf_data *data) {
  indent++;
  logFileType(data->file_type);
  logPtr("file_data",data->file_data);
  logAsset(&data->asset);
  logPtr2("meshes",data->meshes,data->meshes_count);
  logPtr2("materials",data->materials,data->materials_count);
  logPtr2("accessors",data->accessors,data->accessors_count);
  logPtr2("buffer_views",data->buffer_views,data->buffer_views_count);
  logPtr2("buffers",data->buffers,data->buffers_count);
  logPtr2("images",data->images,data->images_count);
  logPtr2("textures",data->textures,data->textures_count);
  logPtr2("samplers",data->samplers,data->samplers_count);
  logPtr2("skins",data->skins,data->skins_count);
  logPtr2("cameras",data->cameras,data->cameras_count);
  logPtr2("lights",data->lights,data->lights_count);
  logPtr2("nodes",data->nodes,data->nodes_count);
  logPtr2("scenes",data->scenes,data->scenes_count);
  logPtr("scene",data->scene);
  logPtr2("animations",data->animations,data->animations_count);
  logPtr2("variants",data->animations,data->variants_count);
  logExtras(&data->extras);
  logPtr2("data_extensions",data->data_extensions,data->data_extensions_count);
  logPtr2("extensions_used",(void*)data->extensions_used,data->extensions_used_count);
  logPtr2("extensions_required",(void*)data->extensions_required,data->extensions_required_count);
  logPtr2("json",(void*)data->json,data->json_size);
  logPtr2("bin",(void*)data->bin,data->bin_size);
  logMemoryOptions(&data->memory);
  logFileOptions(&data->file);
  indent--;
}

void logPrimitiveType(cgltf_primitive_type p) {
  const char *s = "[]";
  switch(p) {
  case cgltf_primitive_type_invalid: {s = "invalid";} break;
  case cgltf_primitive_type_points: {s = "points";} break;
  case cgltf_primitive_type_lines: {s = "lines";} break;
  case cgltf_primitive_type_line_loop: {s = "line_loop";} break;
  case cgltf_primitive_type_line_strip: {s = "line_strip";} break;
  case cgltf_primitive_type_triangles: {s = "triangles";} break;
  case cgltf_primitive_type_triangle_strip: {s = "triangle_strip";} break;
  case cgltf_primitive_type_triangle_fan: {s = "triangle_fan";} break;
  }
  logIndent();printf("primitive_type:(%d)->%s\n",p,s);
}

void logComponentType(const char *name, cgltf_component_type p) {
  const char *s = "[]";
  switch(p) {
  case cgltf_component_type_invalid: {s = "invalid";} break;
  case cgltf_component_type_r_8: {s = "s8";} break;
  case cgltf_component_type_r_8u: {s = "u8";} break;
  case cgltf_component_type_r_16: {s = "s16";} break;
  case cgltf_component_type_r_16u: {s = "u16";} break;
  case cgltf_component_type_r_32u: {s = "u32";} break;
  case cgltf_component_type_r_32f: {s = "f32";} break;
  }
  logIndent();printf("%s:(%d)->%s\n",name,p,s);
}

void logType(const char *name, cgltf_type p) {
  const char *s = "[]";
  switch(p) {
  case cgltf_type_invalid: {s = "invalid";} break;
  case cgltf_type_scalar: {s = "scalar";} break;
  case cgltf_type_vec2: {s = "vec2";} break;
  case cgltf_type_vec3: {s = "vec3";} break;
  case cgltf_type_vec4: {s = "vec4";} break;
  case cgltf_type_mat2: {s = "mat2";} break;
  case cgltf_type_mat3: {s = "mat3";} break;
  case cgltf_type_mat4: {s = "mat4";} break;
  }
  logIndent();printf("%s:(%d)->%s\n",name,p,s);
}

void logBufferViewType(const char *name, cgltf_buffer_view_type p) {
  const char *s = "[]";
  switch(p) {
  case cgltf_buffer_view_type_invalid: {s = "invalid";} break;
  case cgltf_buffer_view_type_indices: {s = "indices";} break;
  case cgltf_buffer_view_type_vertices: {s = "vertices";} break;
  }
  logIndent();printf("%s:(%d)->%s\n",name,p,s);
}

void logDataFreeMethod(const char *name, cgltf_data_free_method p) {
  const char *s = "[]";
  switch(p) {
  case cgltf_data_free_method_none: {s = "none";} break;
  case cgltf_data_free_method_file_release: {s = "file_release";} break;
  case cgltf_data_free_method_memory_free: {s = "memory_free";} break;
  }
  logIndent();printf("%s:(%d)->%s\n",name,p,s);
}

void logBuffer(const char *name, cgltf_buffer *v) {
  indent++;
  logString2("buffer-begin {",name,v);
  indent++;
  logString("name",v->name);
  logInt("size",v->size);
  logString("uri",v->uri);
  logPtr("void *data",v->data);
  logDataFreeMethod("data_free_method",v->data_free_method);
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  logString("} buffer-end",name);
  indent--;
}

void logBufferView(const char *name, cgltf_buffer_view *v) {
  indent++;
  logString2("bufferview-begin {",name,v);
  indent++;
  logString("name",v->name);
  logBuffer("buffer",v->buffer);
  logInt("offset",v->offset);
  logInt("size",v->size);
  logInt("stride",v->stride);
  logBufferViewType("type",v->type);
  logPtr("void *data",v->data);
  logBool("has_meshopt_comperssion",v->has_meshopt_compression);
  logPtr("meshopt_compression",&v->meshopt_compression);
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  logString("} bufferview-end",name);
  indent--;
}

void logAccessor(const char *name, cgltf_accessor *v) {
  indent++;
  logString2("------- accessor-begin {",name,v);
  indent++;
  logString("name",v->name);
  logComponentType("component_type",v->component_type);
  logBool("normalized", v->normalized);
  logType("type", v->type);
  logInt("offset", v->offset);
  logInt("count", v->count);
  logInt("stride", v->stride);
  logBufferView("buffer_view",v->buffer_view);
  logBool("has_min", v->has_min);  
  logPtr("min[16]",v->min);
  logBool("has_max", v->has_max);
  logPtr("max[16]",v->max);
  logBool("is_sparse", v->is_sparse);
  logPtr("accessor_sparse",&v->sparse);
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  logString("} ------- accessor-end",name);
  indent--;
}

void logImage(const char *name, cgltf_image *v) {
  if (v == NULL) {logString3("image-empty",name);return; }
  indent++;
  logString2("image-begin {",name,v);
  indent++;
  logString("name",v->name);
  logString("uri",v->uri);
  logBufferView("buffer_view",v->buffer_view);
  logString("mime_type",v->mime_type);
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  logString("} image-end",name);
  indent--;
}

void logFilterType(const char *name, cgltf_filter_type p) {
  const char *s = "[]";
  switch(p) {
  case cgltf_filter_type_undefined: {s = "undefined";} break;
  case cgltf_filter_type_nearest: {s = "nearest";} break;
  case cgltf_filter_type_linear: {s = "linear";} break;
  case cgltf_filter_type_nearest_mipmap_nearest: {s = "nearest_mipmap_nearest";} break;
  case cgltf_filter_type_linear_mipmap_nearest: {s = "linear_mipmap_nearest";} break;
  case cgltf_filter_type_nearest_mipmap_linear: {s = "nearest_mipmap_linear";} break;
  case cgltf_filter_type_linear_mipmap_linear: {s = "linear_mipmap_linear";} break;
  }
  logIndent();printf("%s:(%d)->%s\n",name,p,s);
}

void logWrapMode(const char *name, cgltf_wrap_mode p) {
  const char *s = "[]";
  switch(p) {
  case cgltf_wrap_mode_clamp_to_edge: {s = "clamp_to_edge";} break;
  case cgltf_wrap_mode_mirrored_repeat: {s = "mirrored_repeat";} break;
  case cgltf_wrap_mode_repeat: {s = "repeat";} break;
  }
  logIndent();printf("%s:(%d)->%s\n",name,p,s);
}

void logSampler(const char *name, cgltf_sampler *v) {
  if (v == NULL) {logString3("sampler-empty",name);return; }
  indent++;
  logString2("sampler-begin {",name,v);
  indent++;
  logString("name",v->name);
  logFilterType("mag_filter",v->mag_filter);
  logFilterType("min_filter",v->min_filter);
  logWrapMode("wrap_s",v->wrap_s);
  logWrapMode("wrap_t",v->wrap_t);
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  logString("} sampler-end",name);
  indent--;
}

void logTexture(const char *name, cgltf_texture *v) {
  if (v == NULL) {logString3("texture-empty",name);return; }
  indent++;
  logString2("texture-begin {",name,v);
  indent++;
  logString("name",v->name);
  logImage("image",v->image);
  logSampler("sampler",v->sampler);
  logBool("hasBasisU",v->has_basisu);
  logImage("basisuImage",v->basisu_image);
  logBool("hasWebP",v->has_webp);
  logImage("webPImage",v->webp_image);
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  logString("} texture-end",name);
  indent--;
}

void logTextureTransform(const char *name, cgltf_texture_transform *v) {
  indent++;
  logString2("textureTransform-begin {",name,v);
  indent++;
  logFloat("offset[0]",v->offset[0]);
  logFloat("offset[1]",v->offset[1]);
  logFloat("rotation",v->rotation);
  logBool("has_texcoord",v->has_texcoord);
  logInt("texcoord",v->texcoord);
  indent--;
  logString("} textureTransform-end",name);
  indent--;
}

void logTextureView(const char *name,cgltf_texture_view *v) {
  indent++;
  logString2("textureView-begin {",name,v);
  indent++;
  logTexture("texture",v->texture);
  logInt("texcoord",v->texcoord);
  logFloat("scale",v->scale);
  logBool("has_transform",v->has_transform);
  logTextureTransform("transform",&v->transform);
  indent--;
  logString("} textureView-end",name);
  indent--;
}

void logMetallicRoughness(const char *name,cgltf_pbr_metallic_roughness *v) {
  indent++;
  logString2("pbr_metallic_roughness-begin {",name,v);
  indent++;
  logTextureView("base_color_texture",&v->base_color_texture);
  logTextureView("metallic_roughness_texture",&v->metallic_roughness_texture);
  logFloat("base_color_factor[0]",v->base_color_factor[0]);
  logFloat("base_color_factor[1]",v->base_color_factor[1]);
  logFloat("base_color_factor[2]",v->base_color_factor[2]);
  logFloat("base_color_factor[3]",v->base_color_factor[3]);
  logFloat("metallic_factor",v->metallic_factor);
  logFloat("roughness_factor",v->roughness_factor);
  indent--;
  logString("} pbr_metallic_roughness-end",name);
  indent--;
}

void logSpecularGlossiness(const char *name,cgltf_pbr_specular_glossiness *v) {
  indent++;
  logString2("pbr_specular_glossiness-begin {",name,v);
  indent++;
  logTextureView("diffuse_texture",&v->diffuse_texture);
  logTextureView("specular_glossiness_texture",&v->specular_glossiness_texture);
  logFloat("diffuse_factor[0]",v->diffuse_factor[0]);
  logFloat("diffuse_factor[1]",v->diffuse_factor[1]);
  logFloat("diffuse_factor[2]",v->diffuse_factor[2]);
  logFloat("diffuse_factor[3]",v->diffuse_factor[3]);
  logFloat("specular_factor[0]",v->specular_factor[0]);
  logFloat("specular_factor[1]",v->specular_factor[1]);
  logFloat("specular_factor[2]",v->specular_factor[2]);
  logFloat("glossiness_factor",v->glossiness_factor);
  indent--;
  logString("} pbr_specular_glossiness-end",name);
  indent--;
}

void logClearCoat(const char *name, cgltf_clearcoat *v) {
  indent++;
  logString2("clearcoat-begin {",name,v);
  indent++;
  logTextureView("clearcoat_texture",&v->clearcoat_texture);
  logTextureView("clearcoat_roughness_texture",&v->clearcoat_roughness_texture);
  logTextureView("clearcoat_normal_texture",&v->clearcoat_normal_texture);
  logFloat("clearcoat_factor",v->clearcoat_factor);
  logFloat("clearcoat_roughness_factor",v->clearcoat_roughness_factor);
  indent--;
  logString("} clearcoat-end",name);
  indent--;
}

void logIor(const char *name, cgltf_ior *v) {
  logFloat(name,v->ior);
}

void logSpecular(const char *name, cgltf_specular *v) {
  indent++;
  logString2("specular-begin {",name,v);
  indent++;
  logTextureView("specular_texture",&v->specular_texture);
  logTextureView("specular_color_texture",&v->specular_color_texture);
  logFloat("specular_color_factor[0]",v->specular_color_factor[0]);
  logFloat("specular_color_factor[1]",v->specular_color_factor[1]);
  logFloat("specular_color_factor[2]",v->specular_color_factor[2]);
  logFloat("specular_factor",v->specular_factor);
  indent--;
  logString("} specular-end",name);
  indent--;
}

void logSheen(const char *name, cgltf_sheen *v) {
  indent++;
  logString2("sheen-begin {",name,v);
  indent++;
  logTextureView("sheen_color_texture",&v->sheen_color_texture);
  logFloat("sheen_color_factor[0]",v->sheen_color_factor[0]);
  logFloat("sheen_color_factor[1]",v->sheen_color_factor[1]);
  logFloat("sheen_color_factor[2]",v->sheen_color_factor[2]);
  logTextureView("sheen_roughness_texture",&v->sheen_roughness_texture);
  logFloat("sheen_roughness_factor",v->sheen_roughness_factor);
  indent--;
  logString("} sheen-end",name);
  indent--;
}

void logTransmission(const char *name, cgltf_transmission *v) {
  indent++;
  logString2("transmission-begin {",name,v);
  indent++;
  logTextureView("transmission_texture",&v->transmission_texture);
  logFloat("transmission_factor",v->transmission_factor);
  indent--;
  logString("} transmission-end",name);
  indent--;
}

void logVolume(const char *name, cgltf_volume *v) {
  indent++;
  logString2("volume-begin {",name,v);
  indent++;
  logTextureView("thickness_texture",&v->thickness_texture);
  logFloat("thickness_factor",v->thickness_factor);
  logFloat("attenuation_color[0]",v->attenuation_color[0]);
  logFloat("attenuation_color[1]",v->attenuation_color[1]);
  logFloat("attenuation_color[2]",v->attenuation_color[2]);
  logFloat("attenuation_distance",v->attenuation_distance);
  indent--;
  logString("} volume-end",name);
  indent--;
}

void logEmissiveStrength(const char *name, cgltf_emissive_strength *v) {
  logFloat(name, v->emissive_strength);
}

void logIridescence(const char *name, cgltf_iridescence *v) {
  indent++;
  logString2("iridiscence-begin {",name,v);
  indent++;
  logFloat("iridescence_factor",v->iridescence_factor);
  logTextureView("iridescence_texture",&v->iridescence_texture);
  logFloat("iridescence_ior",v->iridescence_ior);
  logFloat("iridescence_thickness_min",v->iridescence_thickness_min);
  logFloat("iridescence_thickness_max",v->iridescence_thickness_max);
  logTextureView("iridescence_thickness_texture",&v->iridescence_thickness_texture);
  indent--;
  logString("} iridiscence-end",name);
  indent--;
}

void logDiffuseTransmission(const char *name, cgltf_diffuse_transmission *v) {
  indent++;
  logString2("diffuseTransmission-begin {",name,v);
  indent++;
  logTextureView("diffuse_transmission_texture",&v->diffuse_transmission_texture);
  logFloat("diffuse_transmission_factor",v->diffuse_transmission_factor);
  logFloat("diffuse_transmission_color_factor[0]",v->diffuse_transmission_color_factor[0]);
  logFloat("diffuse_transmission_color_factor[1]",v->diffuse_transmission_color_factor[1]);
  logFloat("diffuse_transmission_color_factor[2]",v->diffuse_transmission_color_factor[2]);
  logTextureView("diffuse_transmission_color_texture",&v->diffuse_transmission_color_texture);
  indent--;
  logString("} diffuseTransmission-end",name);
  indent--;
}

void logAnisotropy(const char *name, cgltf_anisotropy *v) {
  indent++;
  logString2("anisotropy-begin {",name,v);
  indent++;
  logFloat("anisotropy_strength",v->anisotropy_strength);
  logFloat("anisotropy_rotation",v->anisotropy_rotation);
  logTextureView("anisotropy_texture",&v->anisotropy_texture);
  indent--;
  logString("} anisotropy-end",name);
  indent--;
}

void logDispersion(const char *name, cgltf_dispersion *v) {
  logFloat(name,v->dispersion);
}

void logEmissiveFactor(const char *name, cgltf_float v[3]) {
  logFloat("emissive_factor[0]",v[0]);
  logFloat("emissive_factor[1]",v[1]);
  logFloat("emissive_factor[2]",v[2]);
}

void logAlphaMode(const char *name, cgltf_alpha_mode p) {
  const char *s = "[]";
  switch(p) {
  case cgltf_alpha_mode_opaque: {s = "opaque";} break;
  case cgltf_alpha_mode_mask: {s = "mask";} break;
  case cgltf_alpha_mode_blend: {s = "blend";} break;
  }
  logIndent();printf("%s:(%d)->%s\n",name,p,s);
}

void logMaterial(const char *name, cgltf_material *v) {
  indent++;
  logString2("material-begin {",name,v);
  log("-----------------------------------");
  indent++;
  logString("name",v->name);
  logBool("has_pbr_metallic_roughness",v->has_pbr_metallic_roughness);
  logBool("has_pbr_specular_glossiness",v->has_pbr_specular_glossiness);
  logBool("has_clearcoat",v->has_clearcoat);
  logBool("has_transmission",v->has_transmission);
  logBool("has_volume",v->has_volume);
  logBool("has_ior",v->has_ior);
  logBool("has_specular",v->has_specular);
  logBool("has_sheen",v->has_sheen);
  logBool("has_emissive_strength",v->has_emissive_strength);
  logBool("has_iridescence",v->has_iridescence);
  logBool("has_diffuse_transmission",v->has_diffuse_transmission);
  logBool("has_anisotropy",v->has_anisotropy);
  logBool("has_dispersion",v->has_dispersion);
  logMetallicRoughness("pbr_metallic_roughness",&v->pbr_metallic_roughness);
  logSpecularGlossiness("pbr_specular_glossiness",&v->pbr_specular_glossiness);
  logClearCoat("clearcoat",&v->clearcoat);
  logIor("ior",&v->ior);
  logSpecular("specular",&v->specular);
  logSheen("sheen",&v->sheen);
  logTransmission("transmission",&v->transmission);
  logVolume("volume",&v->volume);
  logEmissiveStrength("emissive_strength",&v->emissive_strength);
  logIridescence("iridescence",&v->iridescence);
  logDiffuseTransmission("diffuseTransmission",&v->diffuse_transmission);
  logAnisotropy("anisotropy",&v->anisotropy);
  logDispersion("dispersion",&v->dispersion);
  logTextureView("normal_texture",&v->normal_texture);
  logTextureView("occlusion_texture",&v->occlusion_texture);
  logTextureView("emissive_texture",&v->emissive_texture);
  logEmissiveFactor("emissive_factor",v->emissive_factor);
  logAlphaMode("alpha_mode",v->alpha_mode);
  logFloat("alpha_cutoff",v->alpha_cutoff);
  logBool("double_sided",v->double_sided);
  logBool("unlit",v->unlit);
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  log("-----------------------------------");
  logString("} material-end",name);
  indent--;
}

void logAttributeType(const char *name, cgltf_attribute_type p) {
  const char *s = "[]";
  switch(p) {
  case cgltf_attribute_type_invalid: {s = "invalid";} break;
  case cgltf_attribute_type_position: {s = "position";} break;
  case cgltf_attribute_type_normal: {s = "normal";} break;
  case cgltf_attribute_type_tangent: {s = "tangent";} break;
  case cgltf_attribute_type_texcoord: {s = "texcoord";} break;
  case cgltf_attribute_type_color: {s = "color";} break;
  case cgltf_attribute_type_joints: {s = "joints";} break;
  case cgltf_attribute_type_weights: {s = "weights";} break;
  case cgltf_attribute_type_custom: {s = "custom";} break;
  }
  logIndent();printf("%s:(%d)->%s\n",name,p,s);
}


void logAttribute(cgltf_attribute *v) {
  logString("name",v->name);
  logAttributeType("type",v->type);
  logInt("index",v->index);
  logAccessor("data",v->data);
}


void logMapping(cgltf_material_mapping *v) {
  logInt("variant",v->variant);
  logMaterial("material",v->material);
  logExtras(&v->extras);
}

void logPrimitives(cgltf_primitive *primitives, int primitive_count) {
  indent++;
  for (int i = 0; i < primitive_count; i++) {
    logCountString("primitive[%d] {",i,"");
    indent++;
    cgltf_primitive *v = &primitives[i];
    logPrimitiveType(v->type);
    logAccessor("indices",v->indices);
    logMaterial("material",v->material);

    indent++;
    log("attributes-begin {");
    indent++;
    {for (int j = 0; j < v->attributes_count;j++) {
      logInt("attribute ------------------------------- Nr:",j);
      indent++;
      logAttribute(&v->attributes[j]);
      indent--;
    }}
    indent--;
    log("} attributes-end");
    indent--;

    indent++;
    log("morph_targets-begin {");
    indent++;
    {for (int j = 0; j < v->targets_count;j++) {
      
      logInt("morph_target ------------------------------- Nr:",j);
      cgltf_morph_target *w = &v->targets[j];
      logInt("attributes_count", w->attributes_count);
      indent++;
      for (int k = 0; k < w->attributes_count; k++) {
        logAttribute(&w->attributes[k]);
      }
      indent--;
    }}
    indent--;
    log("} morph_targets-end");
    indent--;


    logExtras(&v->extras);
    logBool("has_draco_mesh_compression",v->has_draco_mesh_compression);

    indent++;
    log("material_mappings-begin {");
    indent++;
    {for (int j = 0; j < v->mappings_count;j++) {
      logInt("mapping ------------------------------- Nr:",j);
      indent++;
      logMapping(&v->mappings[j]);
      indent--;
    }}
    indent--;
    log("} material_mappings-end");
    indent--;

    logPtr2("extensions",v->extensions,v->extensions_count);

    indent--;
    log("}");
  }
  indent--;
}

void logMeshes(cgltf_mesh *meshes, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_mesh *v = &meshes[i];
    logCountString("mesh[%d] name:",i,v->name);
    indent++;
    logPtr2("primitives",v->primitives,v->primitives_count);
    logPrimitives(v->primitives,v->primitives_count);
    logPtr2("weights",v->weights,v->weights_count);
    logPtr2("target_names",(void*)v->target_names,v->target_names_count);
    indent++;
    for (int j = 0; j < v->target_names_count; j++) {
      logCountString("name[%d]",j,v->target_names[j]);
    }
    indent--;
    logExtras(&v->extras);
    logPtr2("extensions",v->extensions,v->extensions_count);
    indent--;
  }
  indent--;
}

void logAccessors(cgltf_accessor *accessors, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_accessor *v = &accessors[i];
    logCountString("accessor[%d]",i,"");
    indent++;
    logAccessor("",v);
    indent--;
  }
  indent--;
}

void logBufferViews(cgltf_buffer_view *bufferViews, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_buffer_view *v = &bufferViews[i];
    logCountString("bufferView[%d]",i,"");
    indent++;
    logBufferView("",v);
    indent--;
  }
  indent--;
}

void logBuffers(cgltf_buffer *buffers, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_buffer *v = &buffers[i];
    logCountString("buffer[%d]",i,"");
    indent++;
    logBuffer("",v);
    indent--;
  }
  indent--;
}

void logImages(cgltf_image *images, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_image *v = &images[i];
    logCountString("image[%d]",i,"");
    indent++;
    logImage("",v);
    indent--;
  }
  indent--;
}

void logTextures(cgltf_texture *textures, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_texture *v = &textures[i];
    logCountString("texture[%d]",i,"");
    indent++;
    logTexture("",v);
    indent--;
  }
  indent--;
}

void logSamplers(cgltf_sampler *samplers, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_sampler *v = &samplers[i];
    logCountString("sampler[%d]",i,"");
    indent++;
    logSampler("",v);
    indent--;
  }
  indent--;
}

void logNode(const char *name, cgltf_node *v) {
  if (v == NULL) {logString3("node-empty:",name);return; }
  indent++;
  logString2("node-begin {",name,v);
  indent++;
  logString("name",v->name);
  logPtr("parent(node*)",v->parent);
  for (int i = 0; i < v->children_count; i++)
    logCountPtr("children",i,v->children[i]);
  logPtr("skin(skin*)",v->skin);
  logPtr("mesh(mesh*)",v->mesh);
  logPtr("camera(camera*)",v->camera);
  logPtr("camera(light*)",v->light);
  logPtr2("weights",v->weights,v->weights_count);
  logBool("has_translation",v->has_translation);
  logBool("has_rotation",v->has_rotation);
  logBool("has_scale",v->has_scale);
  logBool("has_matrix",v->has_matrix);
  logFloat("translation[0]",v->translation[0]);
  logFloat("translation[1]",v->translation[1]);
  logFloat("translation[2]",v->translation[2]);
  logFloat("rotation[0]",v->rotation[0]);
  logFloat("rotation[1]",v->rotation[1]);
  logFloat("rotation[2]",v->rotation[2]);
  logFloat("rotation[3]",v->rotation[3]);
  logFloat("scale[0]",v->scale[0]);
  logFloat("scale[1]",v->scale[1]);
  logFloat("scale[2]",v->scale[2]);
  logFloat("matrix[0]",v->matrix[0]);
  logBool("has_mesh_gpu_instancing",v->has_mesh_gpu_instancing);
  log("mesh_gpu_instancing (not logged)");
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  logString("} node-end",name);
  indent--;
}

void logJoints(const char *name, cgltf_node **joints, int count) {
  indent++;
  logString("joints-begin {",name);
  indent++;

  for (int i = 0; i < count; i++) {
    cgltf_node *v = joints[i];
    logCountString("joint[%d]",i,"");
    indent++;
    logNode("",v);
    indent--;
  }
  
  indent--;
  logString("} joints-end",name);
  indent--;
}

void logSkin(char *name, cgltf_skin *v) {
  indent++;
  logString2("skin-begin {",name,v);
  indent++;
  logJoints("joints", v->joints,v->joints_count);
  logNode("skeleton(node*)", v->skeleton);
  logAccessor("inverse_bind_matrices", v->inverse_bind_matrices);
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  logString("} skin-end",name);
  indent--;
}

void logSkins(cgltf_skin *skins, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_skin *v = &skins[i];
    logCountString("skin[%d]",i,"");
    indent++;
    logSkin("",v);
    indent--;
  }
  indent--;
}

void logCameraType(const char *name, cgltf_camera_type p) {
  const char *s = "[]";
  switch(p) {
  case cgltf_camera_type_invalid: {s = "invalid";} break;
  case cgltf_camera_type_perspective: {s = "perspective";} break;
  case cgltf_camera_type_orthographic: {s = "orthographic";} break;
  }
  logIndent();printf("%s:(%d)->%s\n",name,p,s);
}

void logCameraPerspective(cgltf_camera_perspective *v) {
  indent++;
  logString2("perspective","",v);
  logBool("has_aspect_ratio",v->has_aspect_ratio);
  logFloat("aspect_ratio",v->aspect_ratio);
  logFloat("yfov",v->yfov);
  logBool("has_zfar",v->has_zfar);
  logFloat("zfar",v->zfar);
  logFloat("znear",v->znear);
  logExtras(&v->extras);
  indent--;
}

void logCameraOrthographic(cgltf_camera_orthographic *v) {
  indent++;
  logString2("orthographics","",v);
  logFloat("xmag",v->xmag);
  logFloat("ymag",v->ymag);
  logFloat("zfar",v->zfar);
  logFloat("znear",v->znear);
  logExtras(&v->extras);
  indent--;
}

void logCamera(char *name, cgltf_camera *v) {
  indent++;
  logString2("camera-begin {",name,v);
  indent++;
  logString("name",v->name);
  logCameraType("type",v->type);
  logCameraPerspective(&v->data.perspective);
  logCameraOrthographic(&v->data.orthographic);
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  logString("} camera-end",name);
  indent--;
}

void logCameras(cgltf_camera *cameras, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_camera *v = &cameras[i];
    logCountString("camera[%d]",i,"");
    indent++;
    logCamera("",v);
    indent--;
  }
  indent--;
}

void logLightType(const char *name, cgltf_light_type p) {
  const char *s = "[]";
  switch(p) {
  case cgltf_light_type_invalid: {s = "invalid";} break;
  case cgltf_light_type_directional: {s = "directional";} break;
  case cgltf_light_type_point: {s = "point";} break;
  case cgltf_light_type_spot: {s = "spot";} break;
  }
  logIndent();printf("%s:(%d)->%s\n",name,p,s);
}

void logLight(char *name, cgltf_light *v) {
  indent++;
  logString2("light-begin {",name,v);
  indent++;
  logString("name",v->name);
  logFloat("color[0]",v->color[0]);
  logFloat("color[1]",v->color[1]);
  logFloat("color[2]",v->color[2]);
  logFloat("intensity",v->intensity);
  logLightType("type",v->type);
  logFloat("range",v->range);
  logFloat("spot_inner_cone_angle",v->spot_inner_cone_angle);
  logFloat("spot_outer_cone_angle",v->spot_outer_cone_angle);
  logExtras(&v->extras);
  indent--;
  logString("} light-end",name);
  indent--;
}

void logLights(cgltf_light *lights, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_light *v = &lights[i];
    logCountString("light[%d]",i,"");
    indent++;
    logLight("",v);
    indent--;
  }
  indent--;
}

void logNodes(cgltf_node *nodes, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_node *v = &nodes[i];
    logCountString("node[%d]",i,"");
    indent++;
    logNode("",v);
    indent--;
  }
  indent--;
}

void logNodes2(cgltf_node **nodes, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_node *v = nodes[i];
    logCountString("node[%d]",i,"");
    indent++;
    logNode("",v);
    indent--;
  }
  indent--;
}

void logScene(char *name, cgltf_scene *v) {
  indent++;
  logString2("scene-begin {",name,v);
  indent++;
  logString("name",v->name);
  logNodes2(v->nodes, v->nodes_count);
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  logString("} scene-end",name);
  indent--;
}

void logScenes(cgltf_scene *scenes, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_scene *v = &scenes[i];
    logCountString("scene[%d]",i,"");
    indent++;
    logScene("",v);
    indent--;
  }
  indent--;
}

void logInterpolationType(const char *name, cgltf_interpolation_type p) {
  const char *s = "[]";
  switch(p) {
  case cgltf_interpolation_type_linear: {s = "linear";} break;
  case cgltf_interpolation_type_step: {s = "step";} break;
  case cgltf_interpolation_type_cubic_spline: {s = "cubic_spline";} break;
  }
  logIndent();printf("%s:(%d)->%s\n",name,p,s);
}

void logAnimationSampler(const char *name,cgltf_animation_sampler *v) {
  indent++;
  logString2("animation_sampler-begin {",name,v);
  indent++;
  logAccessor("input",v->input);
  logAccessor("output",v->output);
  logInterpolationType("interpolation",v->interpolation);
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  logString("} animation_sampler-end",name);
  indent--;
}

void logAnimationSamplers(const char *name,cgltf_animation_sampler *samplers, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_animation_sampler *v = &samplers[i];
    logCountString("sampler[%d]",i,"");
    indent++;
    logAnimationSampler("",v);
    indent--;
  }
  indent--;
}

void logAnimationPathType(const char *name, cgltf_animation_path_type p) {
  const char *s = "[]";
  switch(p) {
  case cgltf_animation_path_type_invalid: {s = "invalid";} break;
  case cgltf_animation_path_type_translation: {s = "translation";} break;
  case cgltf_animation_path_type_rotation: {s = "rotation";} break;
  case cgltf_animation_path_type_scale: {s = "scale";} break;
  case cgltf_animation_path_type_weights: {s = "weights";} break;
  }
  logIndent();printf("%s:(%d)->%s\n",name,p,s);
}

void logAnimationChannel(const char *name,cgltf_animation_channel *v) {
  indent++;
  logString2("animation_channel-begin {",name,v);
  indent++;
  logAnimationSampler("sampler",v->sampler);
  logNode("target_node",v->target_node);
  logAnimationPathType("animation_path_type", v->target_path);
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  logString("} animation_channel-end",name);
  indent--;
}

void logAnimationChannels(const char *name,cgltf_animation_channel *channels, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_animation_channel *v = &channels[i];
    logCountString("channel[%d]",i,"");
    indent++;
    logAnimationChannel("",v);
    indent--;
  }
  indent--;
}

void logAnimation(char *name, cgltf_animation *v) {
  indent++;
  logString2("animation-begin {",name,v);
  indent++;
  logString("name",v->name);
  logAnimationSamplers("samplers",v->samplers,v->samplers_count);
  logAnimationChannels("channels",v->channels,v->channels_count);
  logExtras(&v->extras);
  logPtr2("extensions",v->extensions,v->extensions_count);
  indent--;
  logString("} animation-end",name);
  indent--;
}

void logAnimations(cgltf_animation *animations, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_animation *v = &animations[i];
    logCountString("animation[%d]",i,"");
    indent++;
    logAnimation("",v);
    indent--;
  }
  indent--;
}

void logMaterialVariant(char *name, cgltf_material_variant *v) {
  indent++;
  logString2("material-variant-begin {",name,v);
  indent++;
  logString("name",v->name);
  logExtras(&v->extras);
  indent--;
  logString("} material-variant-end",name);
  indent--;
}


void logMaterialVariants(cgltf_material_variant *variants, int count) {
  indent++;
  for (int i = 0; i < count; i++) {
    cgltf_material_variant *v = &variants[i];
    logCountString("variant[%d]",i,"");
    indent++;
    logMaterialVariant("",v);
    indent--;
  }
  indent--;
}


int main(int argc, const char *argv[]) {

  printf("gltf-logger\n");
  printf("------------------------\n");
  if (argc != 2) {
    printf("usage: gltflog.exe <glt file>or<glb file>\n");
    exit(0);
  }
  const char *fileName = argv[1];
  printf("trying to load:\"%s\"\n",fileName);
  cgltf_data *data = loadGLTF(fileName);
  if (data==NULL) {printf("failed to load:\"%s\"\n", fileName); exit(0);}
  printf("success\n");
  log("------------------------");
  log("..... cgltf_data");
  log("------------------------");
  logData(data);
  log("------------------------");
  log("..... cgltf_meshes");
  log("------------------------");
  logMeshes(data->meshes, data->meshes_count);
  log("------------------------");
  log("..... cgltf_accessors");
  log("------------------------");
  log("not logged");
  //logAccessors(data->accessors, data->accessors_count);
  log("------------------------");
  log("..... cgltf_buffer_views");
  log("------------------------");
  log("not logged");
  //logBufferViews(data->buffer_views, data->buffer_views_count);
  log("------------------------");
  log("..... cgltf_buffers");
  log("------------------------");
  logBuffers(data->buffers, data->buffers_count);
  log("------------------------");
  log("..... cgltf_images");
  log("------------------------");
  logImages(data->images, data->images_count);
  log("------------------------");
  log("..... cgltf_textures");
  log("------------------------");
  logTextures(data->textures, data->textures_count);
  log("------------------------");
  log("..... cgltf_samplers");
  log("------------------------");
  logSamplers(data->samplers, data->samplers_count);
  log("------------------------");
  log("..... cgltf_skins");
  log("------------------------");
  logSkins(data->skins, data->skins_count);
  log("------------------------");
  log("..... cgltf_cameras");
  log("------------------------");
  logCameras(data->cameras, data->cameras_count);
  log("------------------------");
  log("..... cgltf_lights");
  log("------------------------");
  logLights(data->lights, data->lights_count);
  log("------------------------");
  log("..... cgltf_nodes");
  log("------------------------");
  logNodes(data->nodes, data->nodes_count);
  log("------------------------");
  log("..... cgltf_scenes");
  log("------------------------");
  logScenes(data->scenes, data->scenes_count);
  log("------------------------");
  log("..... cgltf_scene");
  log("------------------------");
  logScene("scene",data->scene);
  log("------------------------");
  log("..... cgltf_animations");
  log("------------------------");
  log("not logged");
  //logAnimations(data->animations, data->animations_count);
  log("------------------------");
  log("..... cgltf_material_variants");
  log("------------------------");
  logMaterialVariants(data->variants,data->variants_count);

  return 0;
}
