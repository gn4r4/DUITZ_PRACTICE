#include "model.h"

Model::Model() {}

void Model::init() {}

void Model::render(Shader shader) {

	if (meshes.empty()) return;

	for (Mesh& mesh : meshes) {
		mesh.render(shader);
	}
}

void Model::cleanup() {

	if (meshes.empty()) return;

	for (Mesh& mesh : meshes) {
		mesh.cleanup();
	}
}